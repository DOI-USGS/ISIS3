#include "Isis.h"

#include "Pipeline.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  Pipeline p("mar10clean");
  p.SetInputFile("FROM");
  p.SetOutputFile("TO");
  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  // Run mar10nonoise to remove noise
  p.AddToPipeline("mar10nonoise");
  p.Application("mar10nonoise").SetInputParameter("FROM", true);
  p.Application("mar10nonoise").SetOutputParameter("TO", "mar10nonoise");

  // Run findrx on the cube to find the actual position of the reseaus
  p.AddToPipeline("findrx");
  p.Application("findrx").SetInputParameter("FROM", true);

  // Run remrx on the cube to remove the reseaus
  p.AddToPipeline("remrx");
  p.Application("remrx").SetInputParameter("FROM", true);
  p.Application("remrx").SetOutputParameter("TO", "remrx");
  p.Application("remrx").AddParameter("SDIM", "SDIM");
  p.Application("remrx").AddParameter("LDIM", "LDIM");

  // Run a low pass filter on the null data in the cube
  p.AddToPipeline("lowpass", "pass1");
  p.Application("pass1").SetInputParameter("FROM", true);
  p.Application("pass1").SetOutputParameter("TO", "lowpass1");
  p.Application("pass1").AddConstParameter("SAMP", "3");
  p.Application("pass1").AddConstParameter("LINE", "3");
  p.Application("pass1").AddConstParameter("MINIMUM", "4");
  p.Application("pass1").AddConstParameter("FILTER", "outside");

  // Run a low pass filter on the null data in the cube
  p.AddToPipeline("lowpass", "pass2");
  p.Application("pass2").SetInputParameter("FROM", true);
  p.Application("pass2").SetOutputParameter("TO", "lowpass2");
  p.Application("pass2").AddConstParameter("SAMP", "3");
  p.Application("pass2").AddConstParameter("LINE", "3");
  p.Application("pass2").AddConstParameter("MINIMUM", "4");
  p.Application("pass2").AddConstParameter("FILTER", "outside");

  // Run trim to remove data outside of visual frame
  p.AddToPipeline("trim");
  p.Application("trim").SetInputParameter("FROM", true);
  p.Application("trim").SetOutputParameter("TO", "trim");
  p.Application("trim").AddConstParameter("TOP", "5");
  p.Application("trim").AddConstParameter("LEFT", "11");
  p.Application("trim").AddConstParameter("RIGHT", "8");
  
  p.Run();
}

