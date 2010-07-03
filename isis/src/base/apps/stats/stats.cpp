#include "Isis.h"

#include <string>

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

  Histogram *stats = icube->Histogram(1, validMin, validMax);

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword ("From", icube->Filename());
  if (stats->ValidPixels() != 0) {
    results += PvlKeyword("Average",stats->Average());
    results += PvlKeyword ("StandardDeviation", stats->StandardDeviation());
    results += PvlKeyword ("Variance", stats->Variance());
    // These statistics only worked on a histogram
    results += PvlKeyword ("Median", stats->Median());
    results += PvlKeyword ("Mode", stats->Mode());
    results += PvlKeyword ("Skew", stats->Skew());
    results += PvlKeyword ("Minimum", stats->Minimum());
    results += PvlKeyword ("Maximum", stats->Maximum());
    results += PvlKeyword ("Sum", stats->Sum());
  }
  results += PvlKeyword ("TotalPixels", stats->TotalPixels());
  results += PvlKeyword ("ValidPixels", stats->ValidPixels());
  results += PvlKeyword ("OverValidMaximumPixels", stats->OverRangePixels());
  results += PvlKeyword ("UnderValidMinimumPixels", stats->UnderRangePixels());
  results += PvlKeyword ("NullPixels", stats->NullPixels());
  results += PvlKeyword ("LisPixels", stats->LisPixels());
  results += PvlKeyword ("LrsPixels", stats->LrsPixels());
  results += PvlKeyword ("HisPixels", stats->HisPixels());
  results += PvlKeyword ("HrsPixels", stats->HrsPixels());

  // Write the results to the output file if the user specified one
  if (ui.WasEntered("TO")) {
    string outFile = Filename(ui.GetFilename("TO")).Expanded();
    bool exists = Filename(outFile).Exists();
    bool append = ui.GetBoolean("APPEND");
    ofstream os;
    bool writeHeader = false;
    //write the results in the requested format.
    if (ui.GetString("FORMAT") == "PVL") {
      Pvl temp;
      temp.AddGroup(results);
      if (append) {
        temp.Append(outFile);
      }
      else {
        temp.Write(outFile);
      }
    } else {
      //if the format was not PVL, write out a flat file.
      if (append) {
        os.open(outFile.c_str(),ios::app);
        if (!exists) {
          writeHeader = true;
        }
      }else {
        os.open(outFile.c_str(),ios::out);
        writeHeader = true;
      } 
    } 
    if(writeHeader) {
      for(int i = 0; i < results.Keywords(); i++) {
        os << results[i].Name();
        if(i < results.Keywords()-1) {
          os << ",";
        }
      }
      os << endl;
    }
    for(int i = 0; i < results.Keywords(); i++) {
      os << (string)results[i];
      if(i < results.Keywords()-1) {
        os << ",";
      }
    }
    os << endl;
  }

  delete stats;
  // Write the results to the log
  Application::Log(results);
}
