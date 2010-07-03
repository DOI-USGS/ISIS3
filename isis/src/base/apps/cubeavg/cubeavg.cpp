#include "Isis.h"
#include "ProcessBySpectra.h"
#include "Statistics.h"
#include "Application.h"
#include "PvlGroup.h"
#include "PvlSequence.h"

using namespace std; 
using namespace Isis;

void cubeavg (vector<Buffer *> &in,
              vector<Buffer *> &out);

void removekeywords ( PvlGroup & pvlg );

void compute (vector<double> centers,
              vector<double> widths,
              Cube *ocube );

void IsisMain() {
  ProcessBySpectra p;
  p.SetType( ProcessBySpectra::PerPixel );
  Cube *icube = p.SetInputCube("FROM");
  Cube *ocube = p.SetOutputCube("TO", icube->Samples(), icube->Lines(), 1);

  //Get user parameters and sets outputcube's BandBin
  UserInterface &ui = Application::GetUserInterface();
  if(ui.GetString("BANDBIN") == "COMPUTE") {
    if( icube->HasGroup("BandBin") ) {
      PvlGroup &pvlg = icube->GetGroup("BandBin");
      removekeywords( pvlg );
      if(pvlg.HasKeyword("Center")) {
        bool hasWidth = pvlg.HasKeyword("Width");
        PvlKeyword &pvlCenter = pvlg.FindKeyword("Center");
        PvlKeyword * pvlWidth = NULL;
        if(hasWidth) {
          pvlWidth = & pvlg.FindKeyword("Width");
        }
        std::vector<double> centers;
        centers.resize(icube->Bands());
        std::vector<double> widths;
        widths.resize(icube->Bands());
        for( int i=0; i<pvlCenter.Size(); i++ ) {
          centers[i] = pvlCenter[i];
          if( hasWidth )
            widths[i] = (*pvlWidth)[i];
          else
            widths[i] = 0.0;
        }
        compute(centers, widths, ocube);
      }
      else {
        string message = "The BandBin in your input cube does not have a Center value.";
        throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
      }
    }
    else{
      string message = "There is not a BandBin Group in the input cube.";
      throw Isis::iException::Message(Isis::iException::User,message,_FILEINFO_);
    }
  }

  else if(ui.GetString("BANDBIN") == "USER") {
    PvlGroup pvlg;
    if(!icube->HasGroup("BandBin")) {
      pvlg = PvlGroup("BandBin");
      icube->PutGroup(pvlg);
    }
    else {
      pvlg = ocube->GetGroup("BandBin");
      removekeywords(pvlg);
    }
    string Units = "";
    PvlKeyword pvlCenter;
    if(pvlg.HasKeyword("Center")) {
      pvlCenter = pvlg.FindKeyword("Center");
      Units = pvlCenter.Unit();
      pvlg.DeleteKeyword("Center");
    }

    pvlCenter = PvlKeyword("Center");
    pvlCenter.SetValue(ui.GetAsString("CENTER"), Units);
    pvlg.AddKeyword(pvlCenter);
    PvlKeyword pvlWidth;
    if( pvlg.HasKeyword("Width") ) {
      pvlWidth = pvlg.FindKeyword("Width");
      Units = pvlWidth.Unit();
      pvlg.DeleteKeyword("Width");
    }

    pvlWidth = PvlKeyword("Width");
    pvlWidth.SetValue(ui.GetAsString("WIDTH"), Units);
    pvlg.AddKeyword(pvlWidth);
    //Destroys the old and adds the new BandBin Group
    if( ocube->HasGroup("BandBin") ) {
      ocube->DeleteGroup("BandBin");
    }
    ocube->PutGroup(pvlg);
  }

  else if(ui.GetString("BANDBIN") == "DELETE") {
    if(ocube->HasGroup("BandBin")) {
      ocube->DeleteGroup("BandBin");
    }
  }

  p.StartProcess(cubeavg);
  p.EndProcess();
}

// Band processing routine
void cubeavg ( vector<Buffer *> &in, vector<Buffer *> &out ) {
  Statistics sts;
  sts.AddData( (*in[0]).DoubleBuffer() , (*in[0]).size() );
  (*out[0]) = sts.Average();
}

/** 
 * Removes the PvlKeywords that can't be processed
 * 
 * @param pvlg the group from which the keywords are removed
 */
void removekeywords ( PvlGroup & pvlg ) {
  if( pvlg.HasKeyword("OriginalBand") ) {
    pvlg.DeleteKeyword("OriginalBand");
  }
  if( pvlg.HasKeyword("Name") ) {
    pvlg.DeleteKeyword("Name");
  }
}

//BandBin Computeing
void compute( vector<double> centers, vector<double> widths,
              Cube *ocube ) {
  PvlGroup &pvlg = ocube->GetGroup("BandBin");
  PvlKeyword &pvlCenter = pvlg.FindKeyword("Center");
  string centerUnit = pvlCenter.Unit();
  bool hasWidth  = pvlg.HasKeyword("Width");
  double large = centers[0] + widths[0]/2;
  double small = centers[0] - widths[0]/2;
  for( int i=1; i<pvlCenter.Size(); i++ ) {
    if( large < (double)centers[i] + (double)widths[i]/2.0 ) {
      large = (double)centers[i] + (double)widths[i]/2.0;
    }
    if( small > (double)centers[i] - (double)widths[i]/2.0 ) {
      small = (double)centers[i] - (double)widths[i]/2.0;
    }
  }
  pvlCenter.SetValue( iString( (large-small)/2 + small ), centerUnit );
  if( hasWidth ) {
    PvlKeyword &pvlWidth  = pvlg.FindKeyword("Width");
    pvlWidth.SetValue( large-small, pvlWidth.Unit() );
  }
  else {
    PvlKeyword pvlWidth = PvlKeyword("Width");
    pvlWidth.SetValue( large-small, centerUnit );
    pvlg.AddKeyword(pvlWidth);
  }
  
}
