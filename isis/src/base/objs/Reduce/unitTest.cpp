#include "Isis.h"
#include "iString.h"
#include "ProcessByLine.h"
#include "Cube.h"
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
  vector<string> bands;
  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  
  p.SetInputCube("FROM");
  Isis::Cube *icube=new Isis::Cube;
  icube->Open(ui.GetFilename("FROM"));
  // Get input bands
  int inb = icube->Bands();
  for(int i = 1; i <= inb; i++) {
    bands.push_back((Isis::iString)i);
  }
  double sscale = 3;
  double lscale = 4;
  int ons = (int)ceil((double)icube->Samples() / sscale);
  int onl = (int)ceil((double)icube->Lines() / lscale);
    
  // Reduce by "Near"
  Isis::Cube *ocube = p.SetOutputCube("TO", ons, onl, icube->Bands());
  Isis::Nearest near(icube, bands, sscale, lscale);
  p.ClearInputCubes();
  cout << "Reduce by Near\n";
  p.StartProcessInPlace(near);
  Isis::PvlGroup results = near.UpdateOutputLabel(ocube);
  p.EndProcess();
  cout << results << endl;
  
  // Reduce by "Average"
  p.SetInputCube("FROM");
  ocube=p.SetOutputCube("TO2", ons, onl, icube->Bands());
  p.ClearInputCubes();
  Isis::Average avg(icube, bands, sscale, lscale, 0.5, "scale");
  cout << "\nReduce by Average\n";
  p.StartProcessInPlace(avg);
  results = avg.UpdateOutputLabel(ocube);
  cout << results << endl;
  
  p.EndProcess();
  icube->Close();
  ocube->Close();
  remove(ui.GetAsString("TO").c_str());
  remove(ui.GetAsString("TO2").c_str());
}
