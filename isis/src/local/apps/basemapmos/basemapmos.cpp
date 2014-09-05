#include "Isis.h"

#include <iostream>
#include <fstream>

#include <QFile>

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
  cubes.read(ui.GetFileName("FROMLIST"));
  QString PRIORITY = ui.GetString("PRIORITY");
  QString HNS1 = ui.GetAsString("HNS1");
  QString HNL1 = ui.GetAsString("HNL1");
  QString HNS2 = ui.GetAsString("HNS2");
  QString HNL2 = ui.GetAsString("HNL2");
  QString LNS = ui.GetAsString("LNS");
  QString LNL = ui.GetAsString("LNL");
  QString GRANGE = ui.GetString("GRANGE");
  QString MINLAT, MAXLAT, MINLON, MAXLON;
  if(GRANGE == "USER") {
    MINLAT = ui.GetAsString("MINLAT");
    MAXLAT = ui.GetAsString("MAXLAT");
    MINLON = ui.GetAsString("MINLON");
    MAXLON = ui.GetAsString("MAXLON");
  }
  QString MATCHBANDBIN = ui.GetAsString("MATCHBANDBIN");

  // Sets up the pathName to be used for most application calls
  QString pathName = FileName("$TEMPORARY/").path() + "/";
  QString cubeListBaseName = pathName + FileName(ui.GetFileName("FROMLIST")).baseName();

  //Creates the first highpass cubes
  std::ofstream firstHighPassList;
  QString firstHighPass(cubeListBaseName + "_FirstHighPassList.lis");
  firstHighPassList.open(firstHighPass.toAscii().data());
  for (int i = 0; i < cubes.size(); i++) {
    FileName inFile = cubes[i];
    QString outParam = pathName + inFile.baseName() + "_hpffirst.cub";
    QString parameters = "FROM=" + inFile.expanded()
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
  QString secondHighPass(cubeListBaseName + "_SecondHighPassList.lis");
  secondHighPassList.open(secondHighPass.toAscii().data());
  for(int i = 0; i < cubes.size(); i++) {
    FileName inFile = cubes[i];
    QString outParam = pathName + inFile.baseName() + "_hpfsecond.cub";
    QString parameters = "FROM=" + inFile.expanded()
                        + " TO=" + outParam
                        + " SAMPLES= " + HNS2
                        + " LINES= " + HNL2;
    ProgramLauncher::RunIsisProgram("highpass", parameters);
    //Reads the just created highpass cube into a list file for automos
    secondHighPassList << outParam << endl;
  }
  secondHighPassList.close();

  //Makes a mosaic out of the first highpass cube filelist
  QString parameters = "FROM= " + cubeListBaseName + "_FirstHighPassList.lis MOSAIC="
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
  parameters = "FROM=" + cubeListBaseName + "_newmosFirst.cub" +
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
    QString newmosFirst(cubeListBaseName + "_newmosFirst.cub");
    QString newmosSecond(cubeListBaseName + "_newmosSecond.cub");
    QString lpfmos(cubeListBaseName + "_lpfmos.cub");
    QString untrimmedmoc(cubeListBaseName + "_untrimmedmoc.cub");
    QFile::remove(firstHighPass);
    QFile::remove(secondHighPass);
    QFile::remove(newmosFirst);
    QFile::remove(newmosSecond);
    QFile::remove(lpfmos);
    QFile::remove(untrimmedmoc);
    for(int i = 0; i < cubes.size(); i++) {
      FileName inFile = cubes[i];
      QString hpffirst(pathName + inFile.baseName() + "_hpffirst.cub");
      QString hpfsecond(pathName + inFile.baseName() + "_hpfsecond.cub");
      QFile::remove(hpffirst);
      QFile::remove(hpfsecond);
    }
  }


}
