#include "Isis.h"
#include "ControlNet.h"
#include "SerialNumberList.h"
#include "ControlPointList.h"

using namespace std;
using namespace Isis;

void ProcessControlPoints  (std::string psFileName, ControlNet & pcCnet, bool pbDelete, Pvl & pcPvlLog, bool pbPreserve);  
void ProcessControlMeasures(std::string psFileName, ControlNet & pcCnet, bool pbDelete, bool pbPreserve); 

static int giPointsDeleted=0, giMeasuresDeleted=0;

static bool gbLog;

// Main program
void IsisMain() {

  // Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  ControlNet cnet( ui.GetFilename("CNET") );      
  bool bDelete   = ui.GetBoolean("DELETE");
  bool bPreserve = ui.GetBoolean("PRESERVE");
  bool bPointList=false, bCubeList=false;  
  Pvl pvlLog;

  gbLog=false;
     
  std::string sPointsFile, sMeasuresFile, sLogfile;

  //Get the Log file
  if ( ui.WasEntered("LOG") ) {
    sLogfile = ui.GetFilename("LOG");
    gbLog=true;
  }
  
  //List has Points Ids
  if ( ui.WasEntered("POINTLIST") ){   
    sPointsFile = ui.GetFilename("POINTLIST");        
    ProcessControlPoints(sPointsFile, cnet, bDelete, pvlLog, bPreserve);      
  }  

  //List has Cube file names
  if ( ui.WasEntered("FROMLIST") ) {       
    sMeasuresFile = ui.GetFilename("FROMLIST");     
    ProcessControlMeasures(sMeasuresFile, cnet, bDelete, bPreserve);  
  }  
  
  //No List files - only Delete option was chosen 
  if ( bDelete && !bPointList && !bCubeList ){           
    for ( int cp = cnet.Size()-1; cp >= 0; cp -- ) {   
      if ( cnet[cp].Ignore() ) {
        giMeasuresDeleted += cnet[cp].Size();
        cnet.Delete(cp);
        giPointsDeleted++;
      }
      else {        
        for ( int cm = cnet[cp].Size()-1; cm >= 0; cm -- ) {
          if ( cnet[cp][cm].Ignore() ) {
            cnet[cp].Delete(cm);
            giMeasuresDeleted++;
          }
          // Check if the number of measures in the point is zero or 
          // there are few measures in the point and preserve flag is false.          
          if ( ( (cnet[cp].Size() < 2 && !bPreserve) && cnet[cp].Type() != ControlPoint::Ground )
                                   || cnet[cp].Size() == 0 ) {
            giMeasuresDeleted += cnet[cp].Size();
            cnet.Delete(cp);
            giPointsDeleted++;
          }

        }
      }
    }
  }

  //log statistics
  if ( gbLog ) {              
    pvlLog += Isis::PvlKeyword("PointsDeleted", giPointsDeleted);
    pvlLog += Isis::PvlKeyword("MeasuresDeleted", giMeasuresDeleted);         

    pvlLog.Write(sLogfile);
  }
  
  cnet.Write( ui.GetFilename("ONET") );
}


  /**
   * Reads the Control Points list and matches with the control 
   * network. If match was successful, ignore the point. If 
   * Delete option was chosen, delete the point 
   * 
   * @param psFileName - Filename with Control Points
   * @param pcCnet     - holds the input Control Network 
   * @param pbDelete   - Delete option (true/false) 
   * @param pcPvlLog   - Pvl for which control points stats have 
   *                     to be added
   * @param pbPreserve - Preserve Control Points with Measures 
   *                     equal to one. (true/false)
   *  
   * @return none
   */
void ProcessControlPoints(std::string psFileName, ControlNet & pcCnet, bool pbDelete, Pvl& pcPvlLog, bool pbPreserve)
{
  ControlPointList cpList(psFileName);  
    
  for ( int cp = pcCnet.Size()-1; cp >= 0; cp -- ) {
    
    // Compare each Point Id listed with the Point in the
    // Control Network for according exclusion  
    if ( cpList.HasControlPoint(pcCnet[cp].Id()) ) {
      pcCnet[cp].SetIgnore(true);      
    }
        
    if ( pbDelete ){
      //look for previously ignored control points
      if (pcCnet[cp].Ignore()){
        giMeasuresDeleted += pcCnet[cp].Size();
        pcCnet.Delete(cp);
        giPointsDeleted++;
      }
      else{        
        //look for previously ignored control measures
        for ( int cm = pcCnet[cp].Size()-1; cm >= 0 ; cm-- ) {
          if (pcCnet[cp][cm].Ignore() && pbDelete) {
            pcCnet[cp].Delete(cm);
            giMeasuresDeleted++;
          }
        }
        // Check if there are too few measures in the point or the point was previously ignored
        if (( (pcCnet[cp].Size() < 2 && !pbPreserve) && pcCnet[cp].Type() != ControlPoint::Ground )
         || pcCnet[cp].Size() == 0 || (pcCnet[cp].Ignore() && pbDelete)) {
            giMeasuresDeleted += pcCnet[cp].Size();
            pcCnet.Delete(cp);
            giPointsDeleted++;
        }
      }
    }
  }
  if ( gbLog ) {
      cpList.RegisterStatistics(pcPvlLog);              
  }
}

  /**
   * Reads the Cube file list and creates the serial number of the
   * Cubes. If Control Measure serial# matches with the control 
   * network,ignore the point. If Delete option was chosen, delete 
   * the Measure 
   * 
   * @param psFileName - Filename with Cube File names
   * @param pcCnet     - holds the input Control Network 
   * @param pbDelete   - Delete option (true/false) 
   * @param pbPreserve - Preserve Control Points with Measures 
   *                     equal to one. (true/false)
   *  
   * @return none
   */
void ProcessControlMeasures(std::string psFileName, ControlNet & pcCnet, bool pbDelete, bool pbPreserve)
{  
  SerialNumberList snl = psFileName;
  
  for ( int cp = pcCnet.Size()-1; cp >= 0; cp -- ) {

    // Compare each Serial Number listed with the serial number in the
    // Control Measure for according exclusion  
    for ( int cm = pcCnet[cp].Size()-1; cm >= 0 ; cm-- ) {
      if ( snl.HasSerialNumber(pcCnet[cp][cm].CubeSerialNumber()) ) {
          pcCnet[cp][cm].SetIgnore( true );           
      }
      //also look for previously ignored control measures
      if ( pbDelete && pcCnet[cp][cm].Ignore() ) {
        pcCnet[cp].Delete(cm);
        giMeasuresDeleted++;
      }
    }
    // Check if there are too few measures in the point or the point was previously ignored
    if (( (pcCnet[cp].Size() < 2 && !pbPreserve) && pcCnet[cp].Type() != ControlPoint::Ground )
         || pcCnet[cp].Size() == 0 || (pcCnet[cp].Ignore() && pbDelete)) {
            giMeasuresDeleted += pcCnet[cp].Size();
            pcCnet.Delete(cp);
            giPointsDeleted++;
    } 
  }
}

