#include "ControlNet.h"
#include "ControlNetFilter.h"
#include "ControlNetStatistics.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Isis.h"
#include "Progress.h"
#include "Pvl.h"

using namespace Isis;
using namespace std;

void ReadDefFile(ControlNetFilter & pcNetFilter, Pvl & pvlDefFile);

void (ControlNetFilter::*GetPtr2Filter(const string sFilter)) (const PvlGroup & pvlGrp, bool pbLastFilter);

void IsisMain() {

  try {
    UserInterface &ui = Application::GetUserInterface();
    string sSerialNumFile = ui.GetFilename("FROMLIST");
    
    // Get the original control net internalized
    Progress progress;
    ControlNet origNet(ui.GetFilename("CNET"), &progress);
    
    // Copy of the Control Network - preserve the original
    ControlNet newNet(origNet);
    
    // Get the output file and the format
    string sFileType = ui.GetString("TYPE") ;
    bool bPvl = true;
    if (sFileType == "CVS") {
      bPvl = false;
    }

    Progress statsProgress;
    ControlNetFilter cNetFilter(&newNet, sSerialNumFile, &statsProgress);
    
    // Get the DefFile
    string sDefFile = "";
    string sOutFile = "";
    if (ui.WasEntered("DEFFILE")) {
      sDefFile = ui.GetFilename("DEFFILE");
      sOutFile = ui.GetFilename("FLATFILE");
      
      cNetFilter.SetOutputFile(sOutFile, bPvl);
      Pvl pvlDefFile(sDefFile);
      ReadDefFile(cNetFilter, pvlDefFile);
    }
    
    // Get the Image Stats File
    string sImageFile= "";
    if (ui.WasEntered("CREATE_IMAGE_STATS") && ui.GetBoolean("CREATE_IMAGE_STATS")) {
      sImageFile = ui.GetFilename("IMAGE_STATS_FILE");
      cNetFilter.GenerateImageStats();
      cNetFilter.PrintImageStats(sImageFile);
    }
    
    // Get the Point Stats File
    string sPointFile="";
    if (ui.WasEntered("CREATE_POINT_STATS") && ui.GetBoolean("CREATE_POINT_STATS")) {
      sPointFile = ui.GetFilename("POINT_STATS_FILE");
      cNetFilter.GeneratePointStats(sPointFile);
    }
    
    // Log the summary of Control Network
    PvlGroup statsGrp;
    cNetFilter.GenerateControlNetStats(statsGrp);
    Application::Log(statsGrp);
     
  } // REFORMAT THESE ERRORS INTO ISIS TYPES AND RETHROW
  catch(Isis::iException &e) {
    throw;
  }
}

/**
 * Reads the DefFile having info about the different filters to 
 * be used on the Control Network.
 * 
 * @author Sharmila Prasad (9/7/2010)
 * 
 * @param pcNetFilter 
 * @param pvlDefFile 
 */
void ReadDefFile(ControlNetFilter & pcNetFilter, Pvl & pvlDefFile)
{  
  // prototype to ControlNetFilter member function
  void (ControlNetFilter::*pt2Filter)(const PvlGroup & pvlGrp, bool pbLastFilter);
  
  // Parse the Groups in Point Object
  PvlObject filtersObj = pvlDefFile.FindObject("Filters", Pvl::Traverse);
  int iNumGroups = filtersObj.Groups();

  for (int i=0; i<iNumGroups; i++) {
    PvlGroup pvlGrp = filtersObj.Group(i);
    // Get the pointer to ControlNetFilter member function based on Group name
    pt2Filter=GetPtr2Filter(pvlGrp.Name());
    if (pt2Filter != NULL) {
      (pcNetFilter.*pt2Filter)(pvlGrp, ((i==(iNumGroups-1)) ? true : false));
    }
  }
}

/**
 * Returns the pointer to filter function based on the Group name string
 * 
 * @author Sharmila Prasad (8/11/2010)
 * 
 * @return void(ControlNetFilter::*GetPtr2Filter)(const PvlGroup&pvlGrp) 
 */
void (ControlNetFilter::*GetPtr2Filter(const string psFilter)) (const PvlGroup & pvlGrp, bool pbPvl)
{  
  // Point Filters
  if(psFilter == "Point_ErrorMagnitude")
    return &ControlNetFilter::PointErrorFilter;

  if (psFilter == "Point_IdExpression") 
    return &ControlNetFilter::PointIDFilter; 
  
  if (psFilter == "Point_NumMeasures")
    return &ControlNetFilter::PointMeasuresFilter;
  
  if (psFilter == "Point_Properties") {
    return &ControlNetFilter::PointPropertiesFilter;
  }
  
  if (psFilter == "Point_LatLon") {
    return &ControlNetFilter::PointLatLonFilter;
  } 
  
  if (psFilter == "Point_Distance") {
    return &ControlNetFilter::PointDistanceFilter;
  } 
  
  if (psFilter == "Point_MeasureProperties") {
    return &ControlNetFilter::PointMeasurePropertiesFilter;
  }
  
  if (psFilter == "Point_GoodnessOfFit") {
    return &ControlNetFilter::PointGoodnessOfFitFilter;
  }

  if (psFilter == "Point_CubeNames") {
    return &ControlNetFilter::PointCubeNamesFilter;
  }
  
  // Cube Filters
  if (psFilter == "Cube_NameExpression") {
    return &ControlNetFilter::CubeNameExpressionFilter;
  }
  
  if (psFilter == "Cube_NumPoints") {
    return &ControlNetFilter::CubeNumPointsFilter;
  }
  
  if (psFilter == "Cube_Distance") {
    return &ControlNetFilter::CubeDistanceFilter;
  }
  
  return NULL;
}


