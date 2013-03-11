#include "Isis.h"

#include <string>
#include <iostream>

#include "Cube.h"
#include "Process.h"
#include "Histogram.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Process p;

  // Get the histogram
  Cube *icube = p.SetInputCube("FROM");

  double validMin = Isis::ValidMinimum;
  double validMax = Isis::ValidMaximum;

  if(ui.WasEntered("VALIDMIN")) {
    validMin = ui.GetDouble("VALIDMIN");
  }

  if(ui.WasEntered("VALIDMAX")) {
    validMax = ui.GetDouble("VALIDMAX");
  }
  
  // Set a global Pvl for storing results
  Pvl mainpvl;
  
  // Get the number of bands to process
  int bandcount = icube->bandCount();
  
  for (int i = 1; i <= bandcount; i++) {
    Histogram *stats = icube->histogram(i, validMin, validMax);

    // Construct a label with the results
    PvlGroup results("Results");  
    results += PvlKeyword("From", icube->fileName());
    results += PvlKeyword("Band", toString(icube->physicalBand(i)));
    if(stats->ValidPixels() != 0) {
      results += PvlKeyword("Average", toString(stats->Average()));
      results += PvlKeyword("StandardDeviation", toString(stats->StandardDeviation()));
      results += PvlKeyword("Variance", toString(stats->Variance()));
      // These statistics only worked on a histogram
      results += PvlKeyword("Median", toString(stats->Median()));
      results += PvlKeyword("Mode", toString(stats->Mode()));
      results += PvlKeyword("Skew", toString(stats->Skew()));
      results += PvlKeyword("Minimum", toString(stats->Minimum()));
      results += PvlKeyword("Maximum", toString(stats->Maximum()));
      results += PvlKeyword("Sum", toString(stats->Sum()));
    }
    results += PvlKeyword("TotalPixels", toString(stats->TotalPixels()));
    results += PvlKeyword("ValidPixels", toString(stats->ValidPixels()));
    results += PvlKeyword("OverValidMaximumPixels", toString(stats->OverRangePixels()));
    results += PvlKeyword("UnderValidMinimumPixels", toString(stats->UnderRangePixels()));
    results += PvlKeyword("NullPixels", toString(stats->NullPixels()));
    results += PvlKeyword("LisPixels", toString(stats->LisPixels()));
    results += PvlKeyword("LrsPixels", toString(stats->LrsPixels()));
    results += PvlKeyword("HisPixels", toString(stats->HisPixels()));
    results += PvlKeyword("HrsPixels", toString(stats->HrsPixels()));
    
    mainpvl.addGroup(results);
    
    delete stats;
    // Write the results to the log
    Application::Log(results);
  }
  
  // Write the results to the output file if the user specified one
  if(ui.WasEntered("TO")) {
    QString outFile = FileName(ui.GetFileName("TO")).expanded();
    bool exists = FileName(outFile).fileExists();
    bool append = ui.GetBoolean("APPEND");
    ofstream os;
    bool writeHeader = false;
    //write the results in the requested format.
    if(ui.GetString("FORMAT") == "PVL") {
      if(append) {
        mainpvl.append(outFile);
      }
      else {
        mainpvl.write(outFile);
      }
    }
    else {
      //if the format was not PVL, write out a flat file.
      if(append) {
        os.open(outFile.toAscii().data(), ios::app);
        if(!exists) {
          writeHeader = true;
        }
      }
      else {
        os.open(outFile.toAscii().data(), ios::out);
        writeHeader = true;
      }

      if(writeHeader) {
        for(int i = 0; i < mainpvl.group(0).keywords(); i++) {
          os << mainpvl.group(0)[i].name();
          if( i < mainpvl.group(0).keywords() - 1 ) {
            os << ",";
          }
        }
        os << endl;
      }
      
      for(int i = 0; i < mainpvl.groups(); i++) {
        for (int j = 0; j < mainpvl.group(i).keywords(); j++) {
          os << (QString)mainpvl.group(i)[j];
          if(j < mainpvl.group(i).keywords() - 1) {
            os << ",";
          }
        }
        os << endl;
      }
    }
  }
}
