#include "Isis.h"

#include <iostream>
#include <fstream>

#include "Application.h"
#include "Cube.h"
#include "FileList.h"
#include "ProgramLauncher.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  //Get user parameters
  UserInterface &ui = Application::GetUserInterface();
  FileList cubes;
  cubes.Read(ui.GetFilename("FROMLIST"));
  string PRIORITY = ui.GetString("PRIORITY");
  string HNS1 = ui.GetAsString("HNS1");
  string HNL1 = ui.GetAsString("HNL1");
  string HNS2 = ui.GetAsString("HNS2");
  string HNL2 = ui.GetAsString("HNL2");
  string LNS = ui.GetAsString("LNS");
  string LNL = ui.GetAsString("LNL");
  string GRANGE = ui.GetString("GRANGE");
  string MINLAT, MAXLAT, MINLON, MAXLON;
  if(GRANGE == "USER") {
    MINLAT = ui.GetAsString("MINLAT");
    MAXLAT = ui.GetAsString("MAXLAT");
    MINLON = ui.GetAsString("MINLON");
    MAXLON = ui.GetAsString("MAXLON");
  }
  string MATCHBANDBIN = ui.GetAsString("MATCHBANDBIN");

  // Sets up the pathName to be used for most application calls
  string pathName = Filename("$TEMPORARY/").Path() + "/";
  string cubeListBaseName = pathName + Filename(ui.GetFilename("FROMLIST")).Basename();

  //Creates the first highpass cubes
  std::ofstream firstHighPassList;
  string firstHighPass(cubeListBaseName + "_FirstHighPassList.lis");
  firstHighPassList.open(firstHighPass.c_str());
  for(unsigned i = 0; i < cubes.size(); i++) {
    Filename inFile = Isis::Filename(cubes[i]);
    string outParam = pathName + inFile.Basename() + "_hpffirst.cub";
    string parameters = "FROM=" + inFile.Expanded()
                        + " TO=" + outParam
                        + " SAMPLES= " + HNS1
                        + " LINES= " + HNL1;
    ProgramLauncher::RunIsisProgram("highpass", parameters);
    //Reads the just created highpass cube into a list file for automos
    firstHighPassList << outParam << endl;
  }
  firstHighPassList.close();

  //Creates the second highpass cubes
  std::ofstream secondHighPassList;
  string secondHighPass(cubeListBaseName + "_SecondHighPassList.lis");
  secondHighPassList.open(secondHighPass.c_str());
  for(unsigned i = 0; i < cubes.size(); i++) {
    Filename inFile = Isis::Filename(cubes[i]);
    string outParam = pathName + inFile.Basename() + "_hpfsecond.cub";
    string parameters = "FROM=" + inFile.Expanded()
                        + " TO=" + outParam
                        + " SAMPLES= " + HNS2
                        + " LINES= " + HNL2;
    ProgramLauncher::RunIsisProgram("highpass", parameters);
    //Reads the just created highpass cube into a list file for automos
    secondHighPassList << outParam << endl;
  }
  secondHighPassList.close();

  //Makes a mosaic out of the first highpass cube filelist
  string parameters = "FROM= " + cubeListBaseName + "_FirstHighPassList.lis MOSAIC="
                      + cubeListBaseName + "_newmosFirst.cub"
                      + " MATCHBANDBIN=" + MATCHBANDBIN
                      + " GRANGE= " + GRANGE;
  if(PRIORITY == "BENEATH") {
    parameters += " PRIORITY=beneath";
  }
  if(GRANGE == "USER") {
    parameters += " MINLAT= " + MINLAT
                  + " MINLON= " + MINLON
                  + " MAXLAT= " + MAXLAT
                  + " MAXLON= " + MAXLON;
  }
  ProgramLauncher::RunIsisProgram("automos", parameters);

  //Makes a mosaic out of the second highpass cube filelist
  parameters = "FROM= " + cubeListBaseName + "_SecondHighPassList.lis MOSAIC="
               + cubeListBaseName + "_newmosSecond.cub"
               + " MATCHBANDBIN=" + MATCHBANDBIN
               + " GRANGE= " + GRANGE;
  if(PRIORITY == "BENEATH") {
    parameters += " PRIORITY=beneath";
  }
  if(GRANGE == "USER") {
    parameters += " MINLAT= " + MINLAT
                  + " MINLON= " + MINLON
                  + " MAXLAT= " + MAXLAT
                  + " MAXLON= " + MAXLON;
  }
  ProgramLauncher::RunIsisProgram("automos", parameters);

  //Does a lowpass on the Second highpass
  parameters = "FROM=" + cubeListBaseName + "_newmosSecond.cub"
               + " TO=" + cubeListBaseName + "_lpfmos.cub"
               + " SAMPLES=" + LNS
               + " LINES=" + LNL
               + " FILTER=inside";
  ProgramLauncher::RunIsisProgram("lowpass", parameters);

  //Finally combines the first highpass and lowpass mosaics
  parameters = "FROM1=" + cubeListBaseName + "_newmosFirst.cub" +
               " FROM2=" + cubeListBaseName + "_lpfmos.cub" +
               " TO= " + cubeListBaseName + "_untrimmedmoc.cub" +
               " OPERATOR= add";
  ProgramLauncher::RunIsisProgram("algebra", parameters);

  //Concludes with a maptrim of the final product
  parameters = "FROM=" + cubeListBaseName + "_untrimmedmoc.cub" +
               " TO=" + ui.GetAsString("TO");
  if(GRANGE == "USER") {
    parameters += " MINLAT= " + MINLAT
                  + " MINLON= " + MINLON
                  + " MAXLAT= " + MAXLAT
                  + " MAXLON= " + MAXLON;
  }
  ProgramLauncher::RunIsisProgram("maptrim", parameters);


  //Will remove all of the temp files by default
  if(ui.GetBoolean("REMOVETEMP")) {
    string newmosFirst(cubeListBaseName + "_newmosFirst.cub");
    string newmosSecond(cubeListBaseName + "_newmosSecond.cub");
    string lpfmos(cubeListBaseName + "_lpfmos.cub");
    string untrimmedmoc(cubeListBaseName + "_untrimmedmoc.cub");
    remove(firstHighPass.c_str());
    remove(secondHighPass.c_str());
    remove(newmosFirst.c_str());
    remove(newmosSecond.c_str());
    remove(lpfmos.c_str());
    remove(untrimmedmoc.c_str());
    for(unsigned i = 0; i < cubes.size(); i++) {
      Filename inFile = Isis::Filename(cubes[i]);
      string hpffirst(pathName + inFile.Basename() + "_hpffirst.cub");
      string hpfsecond(pathName + inFile.Basename() + "_hpfsecond.cub");
      remove(hpffirst.c_str());
      remove(hpfsecond.c_str());
    }
  }


}
