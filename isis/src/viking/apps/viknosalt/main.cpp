/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include "Pipeline.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  bool rmv = ui.GetBoolean("REMOVE");

  Pipeline pipeline("viknosalt");
  pipeline.SetInputFile("FROM");
  pipeline.SetOutputFile("TO");
  pipeline.KeepTemporaryFiles(!rmv);

  // Trim the edges of the cube
  pipeline.AddToPipeline("trim");
  pipeline.Application("trim").SetInputParameter("FROM", false);
  pipeline.Application("trim").SetOutputParameter("TO", "step1");
  pipeline.Application("trim").AddConstParameter("top", "1");
  pipeline.Application("trim").AddConstParameter("left", "1");
  pipeline.Application("trim").AddConstParameter("right", "1");

  // Run a trimfilter on the cube
  pipeline.AddToPipeline("trimfilter");
  pipeline.Application("trimfilter").SetInputParameter("FROM", false);
  pipeline.Application("trimfilter").SetOutputParameter("TO", "step2");
  pipeline.Application("trimfilter").AddConstParameter("samp", "3");
  pipeline.Application("trimfilter").AddConstParameter("line", "3");
  pipeline.Application("trimfilter").AddConstParameter("minimum", "3");

  // Run a standard deviation filter on the cube
  pipeline.AddToPipeline("noisefilter", "stddev1");
  pipeline.Application("stddev1").SetInputParameter("FROM", false);
  pipeline.Application("stddev1").SetOutputParameter("TO", "step3");
  pipeline.Application("stddev1").AddConstParameter("toldef", "stddev");
  pipeline.Application("stddev1").AddConstParameter("tolmin", "100");
  pipeline.Application("stddev1").AddConstParameter("tolmax", "3.0");
  pipeline.Application("stddev1").AddConstParameter("samp", "3");
  pipeline.Application("stddev1").AddConstParameter("line", "3");
  pipeline.Application("stddev1").AddConstParameter("minimum", "3");

  // Run a noise filter on the cube
  pipeline.AddToPipeline("noisefilter", "noisefilter1");
  pipeline.Application("noisefilter1").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter1").SetOutputParameter("TO", "step4");
  pipeline.Application("noisefilter1").AddConstParameter("tolmin", "300");
  pipeline.Application("noisefilter1").AddConstParameter("tolmax", "100");
  pipeline.Application("noisefilter1").AddConstParameter("samp", "3");
  pipeline.Application("noisefilter1").AddConstParameter("line", "3");
  pipeline.Application("noisefilter1").AddConstParameter("minimum", "2");

  // Run a second noise filter on the cube
  pipeline.AddToPipeline("noisefilter", "noisefilter2");
  pipeline.Application("noisefilter2").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter2").SetOutputParameter("TO", "step5");
  pipeline.Application("noisefilter2").AddConstParameter("tolmin", "300");
  pipeline.Application("noisefilter2").AddConstParameter("tolmax", "60");
  pipeline.Application("noisefilter2").AddConstParameter("samp", "3");
  pipeline.Application("noisefilter2").AddConstParameter("line", "3");
  pipeline.Application("noisefilter2").AddConstParameter("minimum", "2");

  // Run a second standard deviation filter on the cube
  pipeline.AddToPipeline("noisefilter", "stddev2");
  pipeline.Application("stddev2").SetInputParameter("FROM", false);
  pipeline.Application("stddev2").SetOutputParameter("TO", "step6");
  pipeline.Application("stddev2").AddConstParameter("toldef", "stddev");
  pipeline.Application("stddev2").AddConstParameter("tolmin", "100");
  pipeline.Application("stddev2").AddConstParameter("tolmax", "2.0");
  pipeline.Application("stddev2").AddConstParameter("samp", "3");
  pipeline.Application("stddev2").AddConstParameter("line", "3");
  pipeline.Application("stddev2").AddConstParameter("minimum", "7");

  // Run a third noise filter on the cube
  pipeline.AddToPipeline("noisefilter", "noisefilter3");
  pipeline.Application("noisefilter3").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter3").SetOutputParameter("TO", "step7");
  pipeline.Application("noisefilter3").AddConstParameter("tolmin", "300");
  pipeline.Application("noisefilter3").AddConstParameter("tolmax", "46");
  pipeline.Application("noisefilter3").AddConstParameter("samp", "3");
  pipeline.Application("noisefilter3").AddConstParameter("line", "3");
  pipeline.Application("noisefilter3").AddConstParameter("minimum", "7");

  // Run a low pass filter on the invalid data in the cube
  pipeline.AddToPipeline("lowpass");
  pipeline.Application("lowpass").SetInputParameter("FROM", false);
  pipeline.Application("lowpass").SetOutputParameter("TO", "", "cub");
  pipeline.Application("lowpass").AddConstParameter("samp", "3");
  pipeline.Application("lowpass").AddConstParameter("line", "3");
  pipeline.Application("lowpass").AddConstParameter("minimum", "2");
  pipeline.Application("lowpass").AddConstParameter("filter", "outside");
  pipeline.Application("lowpass").AddConstParameter("null", "true");
  pipeline.Application("lowpass").AddConstParameter("lis", "true");
  pipeline.Application("lowpass").AddConstParameter("his", "true");
  pipeline.Application("lowpass").AddConstParameter("lrs", "true");

  pipeline.Run();
}
