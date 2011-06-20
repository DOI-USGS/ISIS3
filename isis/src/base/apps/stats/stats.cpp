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
  int bandcount = icube->getBandCount();
  
  for (int i = 1; i <= bandcount; i++) {
    Histogram *stats = icube->getHistogram(i, validMin, validMax);

    // Construct a label with the results
    PvlGroup results("Results");  
    results += PvlKeyword("From", icube->getFilename());
    results += PvlKeyword("Band", icube->getPhysicalBand(i));
    if(stats->ValidPixels() != 0) {
      results += PvlKeyword("Average", stats->Average());
      results += PvlKeyword("StandardDeviation", stats->StandardDeviation());
      results += PvlKeyword("Variance", stats->Variance());
      // These statistics only worked on a histogram
      results += PvlKeyword("Median", stats->Median());
      results += PvlKeyword("Mode", stats->Mode());
      results += PvlKeyword("Skew", stats->Skew());
      results += PvlKeyword("Minimum", stats->Minimum());
      results += PvlKeyword("Maximum", stats->Maximum());
      results += PvlKeyword("Sum", stats->Sum());
    }
    results += PvlKeyword("TotalPixels", stats->TotalPixels());
    results += PvlKeyword("ValidPixels", stats->ValidPixels());
    results += PvlKeyword("OverValidMaximumPixels", stats->OverRangePixels());
    results += PvlKeyword("UnderValidMinimumPixels", stats->UnderRangePixels());
    results += PvlKeyword("NullPixels", stats->NullPixels());
    results += PvlKeyword("LisPixels", stats->LisPixels());
    results += PvlKeyword("LrsPixels", stats->LrsPixels());
    results += PvlKeyword("HisPixels", stats->HisPixels());
    results += PvlKeyword("HrsPixels", stats->HrsPixels());
    
    mainpvl.AddGroup(results);
    
    delete stats;
    // Write the results to the log
    Application::Log(results);
  }
  
  // Write the results to the output file if the user specified one
  if(ui.WasEntered("TO")) {
    string outFile = Filename(ui.GetFilename("TO")).Expanded();
    bool exists = Filename(outFile).Exists();
    bool append = ui.GetBoolean("APPEND");
    ofstream os;
    bool writeHeader = false;
    //write the results in the requested format.
    if(ui.GetString("FORMAT") == "PVL") {
      if(append) {
        mainpvl.Append(outFile);
      }
      else {
        mainpvl.Write(outFile);
      }
    }
    else {
      //if the format was not PVL, write out a flat file.
      if(append) {
        os.open(outFile.c_str(), ios::app);
        if(!exists) {
          writeHeader = true;
        }
      }
      else {
        os.open(outFile.c_str(), ios::out);
        writeHeader = true;
      }

      if(writeHeader) {
        for(int i = 0; i < mainpvl.Group(0).Keywords(); i++) {
          os << mainpvl.Group(0)[i].Name();
          if( i < mainpvl.Group(0).Keywords() - 1 ) {
            os << ",";
          }
        }
        os << endl;
      }
      
      for(int i = 0; i < mainpvl.Groups(); i++) {
        for (int j = 0; j < mainpvl.Group(i).Keywords(); j++) {
          os << (string)mainpvl.Group(i)[j];
          if(j < mainpvl.Group(i).Keywords() - 1) {
            os << ",";
          }
        }
        os << endl;
      }
    }
  }
}
