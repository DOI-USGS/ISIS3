#include "Isis.h"
#include "Pipeline.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  Pipeline p("vikclean");
  p.SetInputFile("FROM");
  p.SetOutputFile("TO");
  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  // Run viknosalt on the cube to remove the salt 
  p.AddToPipeline("viknosalt");
  p.Application("viknosalt").SetInputParameter("FROM", true);
  p.Application("viknosalt").SetOutputParameter("TO", "nosalt");

  // Run vikfixtrx on the cube to remove the tracks
  p.AddToPipeline("vikfixtrx");
  p.Application("vikfixtrx").SetInputParameter("FROM", true);
  p.Application("vikfixtrx").SetOutputParameter("TO", "fixtrx");

  // Run findrx on the cube to find the nominal position of the reseaus
  p.AddToPipeline("findrx");
  p.Application("findrx").SetInputParameter("FROM", true);

  // Run viknopepper on the cube to remove the pepper
  p.AddToPipeline("viknopepper");
  p.Application("viknopepper").SetInputParameter("FROM", true);
  p.Application("viknopepper").SetOutputParameter("TO", "nopepper");

  // Run remrx on the cube to remove the reseaus
  p.AddToPipeline("remrx");
  p.Application("remrx").SetInputParameter("FROM", true);
  p.Application("remrx").SetOutputParameter("TO", "remrx");
  p.Application("remrx").AddParameter("LDIM", "LDIM");
  p.Application("remrx").AddParameter("SDIM", "SDIM");

  // Run vikfixfly on the cube
  p.AddToPipeline("viknobutter");
  p.Application("viknobutter").SetInputParameter("FROM", true);
  p.Application("viknobutter").SetOutputParameter("TO", "");

  p.Run();
}

