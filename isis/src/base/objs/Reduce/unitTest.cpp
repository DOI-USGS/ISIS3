#include "Isis.h"
#include "iString.h"
#include "ProcessByLine.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "PvlGroup.h"
#include "Reduce.h"
#include "UserInterface.h"
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

void IsisMain() {
  Isis::Preference::Preferences(true);
  Isis::ProcessByLine p;
  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  vector<string> bands;
  
  p.SetInputCube("FROM");
  Isis::Cube icube;

  Isis::CubeAttributeInput cai(ui.GetAsString("FROM"));
  bands = cai.Bands();

  icube.setVirtualBands(bands);
  icube.open(ui.GetFileName("FROM"));
  
  double sscale = 3;
  double lscale = 4;
  int ons = (int)ceil((double)icube.getSampleCount() / sscale);
  int onl = (int)ceil((double)icube.getLineCount() / lscale);
    
  // Reduce by "Near"
  Isis::Cube *ocube = p.SetOutputCube("TO", ons, onl, icube.getBandCount());
  Isis::Nearest near(&icube, sscale, lscale);
  p.ClearInputCubes();
  cout << "Reduce by Near\n";
  p.ProcessCubeInPlace(near, false);
  Isis::PvlGroup results = near.UpdateOutputLabel(ocube);
  p.EndProcess();
  cout << results << endl;
  
  // Reduce by "Average"
  p.SetInputCube("FROM");
  ocube=p.SetOutputCube("TO2", ons, onl, icube.getBandCount());
  p.ClearInputCubes();
  Isis::Average avg(&icube, sscale, lscale, 0.5, "scale");
  cout << "\nReduce by Average\n";
  p.ProcessCubeInPlace(avg, false);
  results = avg.UpdateOutputLabel(ocube);
  cout << results << endl;

  p.EndProcess();
  icube.close();
  remove(ui.GetAsString("TO").c_str());
  remove(ui.GetAsString("TO2").c_str());
}
