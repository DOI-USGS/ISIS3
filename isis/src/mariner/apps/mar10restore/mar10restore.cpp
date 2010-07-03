#include "Isis.h"
#include "Chip.h"
#include "Cube.h"
#include "iException.h"
#include "iString.h"
#include "Pipeline.h"
#include "Statistics.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  Cube cube;
  cube.Open(ui.GetFilename("FROM"));

  // Check that the cube actually needs reconstruction
  Chip cp(5,5);
  cp.TackCube(25,25);
  cp.Load(cube);
  Statistics *stats = cp.Statistics();
  if (stats->ValidPixels() > 8) {
    string msg = "The cube [" + ui.GetFilename("FROM") + "] does not need reconstruction";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  delete stats;
  stats = NULL;

  // Open the input cube
  Pipeline p("mar10restore");
  p.SetInputFile("FROM");
  p.SetOutputFile("TO");
  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  // Run a standard deviation filter on the cube
  p.AddToPipeline("noisefilter", "noise1");
  p.Application("noise1").SetInputParameter("FROM", true);
  p.Application("noise1").SetOutputParameter("TO", "noise1");
  p.Application("noise1").AddConstParameter("TOLDEF", "stddev");
  p.Application("noise1").AddConstParameter("FLATTOL", "10");
  p.Application("noise1").AddConstParameter("SAMP", "5");
  p.Application("noise1").AddConstParameter("LINE", "5");
  p.Application("noise1").AddConstParameter("MINIMUM", "4");
  p.Application("noise1").AddConstParameter("TOLMIN", "2.0");
  p.Application("noise1").AddConstParameter("TOLMAX", "1.5");
  p.Application("noise1").AddConstParameter("REPLACE", "null");

  // run a standard deviation filter on the cube
  p.AddToPipeline("noisefilter", "noise2");
  p.Application("noise2").SetInputParameter("FROM", true);
  p.Application("noise2").SetOutputParameter("TO", "noise2");
  p.Application("noise2").AddConstParameter("TOLDEF", "stddev");
  p.Application("noise2").AddConstParameter("FLATTOL", "10");
  p.Application("noise2").AddConstParameter("SAMP", "11");
  p.Application("noise2").AddConstParameter("LINE", "11");
  p.Application("noise2").AddConstParameter("MINIMUM", "9");
  p.Application("noise2").AddConstParameter("TOLMIN", "100");
  p.Application("noise2").AddConstParameter("TOLMAX", "2.0");
  p.Application("noise2").AddConstParameter("REPLACE", "null");

  // Run a standard deviation filter on the cube
  p.AddToPipeline("noisefilter", "noise3");
  p.Application("noise3").SetInputParameter("FROM", true);
  p.Application("noise3").SetOutputParameter("TO", "noise3");
  p.Application("noise3").AddConstParameter("TOLDEF", "stddev");
  p.Application("noise3").AddConstParameter("FLATTOL", "10");
  p.Application("noise3").AddConstParameter("SAMP", "7");
  p.Application("noise3").AddConstParameter("LINE", "7");
  p.Application("noise3").AddConstParameter("MINIMUM", "4");
  p.Application("noise3").AddConstParameter("TOLMIN", "100");
  p.Application("noise3").AddConstParameter("TOLMAX", "1.5");
  p.Application("noise3").AddConstParameter("REPLACE", "null");

  // Run a low pass filter on the invalid data in the cube
  p.AddToPipeline("lowpass", "lowpass1");
  p.Application("lowpass1").SetInputParameter("FROM", true);
  p.Application("lowpass1").SetOutputParameter("TO", "lp1");
  p.Application("lowpass1").AddConstParameter("SAMP", "3");
  p.Application("lowpass1").AddConstParameter("LINE", "3");
  p.Application("lowpass1").AddConstParameter("MINIMUM", "2");
  p.Application("lowpass1").AddConstParameter("FILTER", "outside");
  p.Application("lowpass1").AddConstParameter("NULL", "true");
  p.Application("lowpass1").AddConstParameter("LIS", "true");
  p.Application("lowpass1").AddConstParameter("HIS", "true");
  p.Application("lowpass1").AddConstParameter("LRS", "true"); 

  // Run a low pass filter on the invalid data in the cube
  p.AddToPipeline("lowpass", "lowpass2");
  p.Application("lowpass2").SetInputParameter("FROM", true);
  p.Application("lowpass2").SetOutputParameter("TO", "lp2");
  p.Application("lowpass2").AddConstParameter("SAMP", "3");
  p.Application("lowpass2").AddConstParameter("LINE", "3");
  p.Application("lowpass2").AddConstParameter("MINIMUM", "2");
  p.Application("lowpass2").AddConstParameter("FILTER", "outside");
  p.Application("lowpass2").AddConstParameter("NULL", "true");
  p.Application("lowpass2").AddConstParameter("LIS", "true");
  p.Application("lowpass2").AddConstParameter("HIS", "true");
  p.Application("lowpass2").AddConstParameter("LRS", "true"); 

  // Run a low pass filter on the invalid data in the cube
  p.AddToPipeline("lowpass", "lowpass3");
  p.Application("lowpass3").SetInputParameter("FROM", true);
  p.Application("lowpass3").SetOutputParameter("TO", "lp3");
  p.Application("lowpass3").AddConstParameter("SAMP", "3");
  p.Application("lowpass3").AddConstParameter("LINE", "3");
  p.Application("lowpass3").AddConstParameter("MINIMUM", "2");
  p.Application("lowpass3").AddConstParameter("FILTER", "outside");
  p.Application("lowpass3").AddConstParameter("NULL", "true");
  p.Application("lowpass3").AddConstParameter("LIS", "true");
  p.Application("lowpass3").AddConstParameter("HIS", "true");
  p.Application("lowpass3").AddConstParameter("LRS", "true");

  p.AddToPipeline("trim");
  p.Application("trim").SetInputParameter("FROM", true);
  p.Application("trim").SetOutputParameter("TO", "trim");
  p.Application("trim").AddConstParameter("LEFT", 15);
  p.Application("trim").AddConstParameter("RIGHT", 5);
  p.Application("trim").AddConstParameter("BOTTOM", 0);
  p.Application("trim").AddConstParameter("TOP", 5);

  p.Run();
}


