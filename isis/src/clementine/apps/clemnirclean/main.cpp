/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include "Application.h"
#include "Pipeline.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Open the input cube and get filename of output cube
  UserInterface &ui = Application::GetUserInterface();

  Pipeline p("clemnirclean");
  p.SetInputFile("FROM");
  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));
  p.SetOutputFile("TO");

  // Run noisefilter on the cube and replace with nulls, 3x3 boxcar
  p.AddToPipeline("noisefilter", "noisefilter1");
  p.Application("noisefilter1").SetInputParameter("FROM", true);
  p.Application("noisefilter1").SetOutputParameter("TO", "box1");
  p.Application("noisefilter1").AddConstParameter("toldef", "stddev");
  p.Application("noisefilter1").AddConstParameter("tolmin", "1.25");
  p.Application("noisefilter1").AddConstParameter("tolmax", "1.25");
  p.Application("noisefilter1").AddConstParameter("samples", "3");
  p.Application("noisefilter1").AddConstParameter("lines", "3");
  p.Application("noisefilter1").AddConstParameter("replace", "null");

  // Run lowpass on the cube using outside filter, 3x3 boxcar
  p.AddToPipeline("lowpass", "lowpass1");
  p.Application("lowpass1").SetInputParameter("FROM", true);
  p.Application("lowpass1").SetOutputParameter("TO", "box2");
  p.Application("lowpass1").AddConstParameter("samples", "3");
  p.Application("lowpass1").AddConstParameter("lines", "3");
  p.Application("lowpass1").AddConstParameter("filter", "outside");

  // Run lowpass on the cube using outside filter, 3x3 boxcar
  p.AddToPipeline("lowpass", "lowpass2");
  p.Application("lowpass2").SetInputParameter("FROM", true);
  p.Application("lowpass2").SetOutputParameter("TO", "box3");
  p.Application("lowpass2").AddConstParameter("samples", "3");
  p.Application("lowpass2").AddConstParameter("lines", "3");
  p.Application("lowpass2").AddConstParameter("filter", "outside");

  // Run lowpass on the cube using outside filter, 3x3 boxcar
  p.AddToPipeline("noisefilter", "noisefilter2");
  p.Application("noisefilter2").SetInputParameter("FROM", true);
  p.Application("noisefilter2").SetOutputParameter("TO", "box4");
  p.Application("noisefilter2").AddConstParameter("toldef", "stddev");
  p.Application("noisefilter2").AddConstParameter("tolmin", "1.5");
  p.Application("noisefilter2").AddConstParameter("tolmax", "1.5");
  p.Application("noisefilter2").AddConstParameter("samples", "3");
  p.Application("noisefilter2").AddConstParameter("lines", "3");
  p.Application("noisefilter2").AddConstParameter("nullisnoise", "yes");

  // Run lowpass on the cube using outside filter, 3x3 boxcar
  p.AddToPipeline("lowpass", "lowpass3");
  p.Application("lowpass3").SetInputParameter("FROM", true);
  p.Application("lowpass3").SetOutputParameter("TO", "box5");
  p.Application("lowpass3").AddConstParameter("samples", "5");
  p.Application("lowpass3").AddConstParameter("lines", "5");
  p.Application("lowpass3").AddConstParameter("filter", "outside");

  p.Run();
}
