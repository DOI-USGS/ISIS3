#include "Isis.h"
#include "Application.h"
#include "FileList.h"
#include "Cube.h"
#include "Preference.h"

#include <iostream>
#include <fstream>

using namespace std; 
using namespace Isis;

void IsisMain() {

  //Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileList cubes;
  cubes.Read(ui.GetFilename("FROMLIST"));

  int hns = ui.GetInteger("HNS");
  int hnl = ui.GetInteger("HNL");
  int lns = ui.GetInteger("LNS");
  int lnl = ui.GetInteger("LNL");
  string match = ui.GetAsString("MATCHBANDBIN");
  
  //Sets upt the pathName to be used for most application calls
  Filename inFile = Isis::Filename( cubes[0] );

  Pvl &pref = Preference::Preferences();
  string pathName = (string)pref.FindGroup("DataDirectory")["Temporary"] + "/";

  /** 
   * Creates a mosaic from the original images.  It is placed here 
   * so that the failure MATCHBANDBIN causes does not leave 
   * highpasses cubes lying around!
  */
  string parameters = "FROMLIST=" + ui.GetFilename("FROMLIST") +
        " MOSAIC=" + pathName + "OriginalMosaic.cub" +
        " MATCHBANDBIN=" + match;
  Isis::iApp ->Exec("automos",parameters);

  //Creates the highpass cubes from the cubes FileList
  std::ofstream highPassList;
  highPassList.open( "HighPassList.lis" );
  for ( unsigned i=0; i<cubes.size(); i++ ) {
    inFile = Isis::Filename( cubes[i] );
    string outParam = pathName + inFile.Basename() + "_highpass.cub";
    parameters = "FROM=" + inFile.Expanded() +
          " TO=" + outParam
          + " SAMPLES=" + iString(hns) + " LINES=" + iString(hnl);
    Isis::iApp ->Exec("highpass",parameters);
    //Reads the just created highpass cube into a list file for automos
    highPassList << outParam << endl;
  }
  highPassList.close();
  
  //Makes a mosaic out of the highpass cube filelist
  parameters = "FROMLIST=HighPassList.lis MOSAIC=" + pathName + "HighpassMosaic.cub"
            + " MATCHBANDBIN=" + match;
  Isis::iApp ->Exec("automos",parameters);

  //Does a lowpass on the original mosaic
  parameters = "FROM=" + pathName + "OriginalMosaic.cub"
        + " TO=" + pathName + "LowpassMosaic.cub"
        + " SAMPLES=" + iString(lns) + " LINES=" + iString(lnl);
  Isis::iApp ->Exec("lowpass",parameters);
  
  //Finally combines the highpass and lowpass mosaics
  parameters = "FROM1=" + pathName + "HighpassMosaic.cub" +
        " FROM2=" + pathName + "LowpassMosaic.cub" +
        " TO=" + ui.GetFilename("TO") +
        " OPERATOR= add";
  Isis::iApp ->Exec("algebra",parameters);
  
  //Will remove all of the temp files by default
  if ( ui.GetBoolean("REMOVETEMP") ) {
    string file("HighPassList.lis");
    remove( file.c_str() );
    file = pathName+"HighpassMosaic.cub";
    remove( file.c_str() );
    file = pathName+"LowpassMosaic.cub";
    remove( file.c_str() );
    file = pathName+"OriginalMosaic.cub";
    remove( file.c_str() );

    for ( unsigned i=0; i<cubes.size(); i++ ) {
      inFile = Isis::Filename( cubes[i] );
      file = pathName + inFile.Basename() + "_highpass.cub";
      remove( file.c_str() );
    }
  }

}
