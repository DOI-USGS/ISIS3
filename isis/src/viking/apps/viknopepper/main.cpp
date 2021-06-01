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
  Pipeline pipeline("viknopepper");

  bool rmv = ui.GetBoolean("REMOVE");

  // Run a standard deviation filter on the cube
  pipeline.SetInputFile("FROM");
  pipeline.SetOutputFile("TO");
  pipeline.KeepTemporaryFiles(!rmv);

  pipeline.AddToPipeline("noisefilter", "noisefilter1");
  pipeline.Application("noisefilter1").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter1").SetOutputParameter("TO", "step1");
  pipeline.Application("noisefilter1").AddConstParameter("toldef", "stddev");
  pipeline.Application("noisefilter1").AddConstParameter("flattol", "10");
  pipeline.Application("noisefilter1").AddConstParameter("line", "9");
  pipeline.Application("noisefilter1").AddConstParameter("samp", "9");
  pipeline.Application("noisefilter1").AddConstParameter("minimum", "9");
  pipeline.Application("noisefilter1").AddConstParameter("tolmin", "4.0");
  pipeline.Application("noisefilter1").AddConstParameter("tolmax", "4.0");
  pipeline.Application("noisefilter1").AddConstParameter("replace", "null");

  // Run a standard deviation filter on the cube
  pipeline.AddToPipeline("noisefilter", "noisefilter2");
  pipeline.Application("noisefilter2").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter2").SetOutputParameter("TO", "step2");
  pipeline.Application("noisefilter2").AddConstParameter("toldef", "stddev");
  pipeline.Application("noisefilter2").AddConstParameter("flattol", "10");
  pipeline.Application("noisefilter2").AddConstParameter("line", "3");
  pipeline.Application("noisefilter2").AddConstParameter("samp", "3");
  pipeline.Application("noisefilter2").AddConstParameter("minimum", "3");
  pipeline.Application("noisefilter2").AddConstParameter("tolmin", "3.5");
  pipeline.Application("noisefilter2").AddConstParameter("tolmax", "3.5");

  // Run a standard deviation filter on the cube
  pipeline.AddToPipeline("noisefilter", "noisefilter3");
  pipeline.Application("noisefilter3").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter3").SetOutputParameter("TO", "step3");
  pipeline.Application("noisefilter3").AddConstParameter("toldef", "stddev");
  pipeline.Application("noisefilter3").AddConstParameter("flattol", "10");
  pipeline.Application("noisefilter3").AddConstParameter("line", "9");
  pipeline.Application("noisefilter3").AddConstParameter("samp", "9");
  pipeline.Application("noisefilter3").AddConstParameter("minimum", "9");
  pipeline.Application("noisefilter3").AddConstParameter("tolmin", "4.0");
  pipeline.Application("noisefilter3").AddConstParameter("tolmax", "4.0");
  pipeline.Application("noisefilter3").AddConstParameter("replace", "null");

  // Run a standard deviation filter on the cube
  pipeline.AddToPipeline("noisefilter", "noisefilter4");
  pipeline.Application("noisefilter4").SetInputParameter("FROM", false);
  pipeline.Application("noisefilter4").SetOutputParameter("TO", "step4");
  pipeline.Application("noisefilter4").AddConstParameter("toldef", "stddev");
  pipeline.Application("noisefilter4").AddConstParameter("line", "3");
  pipeline.Application("noisefilter4").AddConstParameter("samp", "3");
  pipeline.Application("noisefilter4").AddConstParameter("minimum", "3");
  pipeline.Application("noisefilter4").AddConstParameter("tolmin", "3.5");
  pipeline.Application("noisefilter4").AddConstParameter("tolmax", "3.5");


  // Run a lowpass filter on the cube
  pipeline.AddToPipeline("lowpass");
  pipeline.Application("lowpass").SetInputParameter("FROM", false);
  pipeline.Application("lowpass").SetOutputParameter("TO", "", "cub");
  pipeline.Application("lowpass").AddConstParameter("filter", "outside");
  pipeline.Application("lowpass").AddConstParameter("samp", "3");
  pipeline.Application("lowpass").AddConstParameter("line", "3");
  pipeline.Application("lowpass").AddConstParameter("minimum", "5");
  pipeline.Application("lowpass").AddConstParameter("replacement", "null");

  pipeline.Run();
}
