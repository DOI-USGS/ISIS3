#include "Isis.h"
#include "ProcessImportVicar.h"
#include "Application.h"
#include "Cube.h"
#include "Statistics.h"

using namespace std;
void IsisMain() {

  Isis::Preference::Preferences(true);

  Isis::ProcessImportVicar p;
  Isis::Pvl vlab;
  p.SetVicarFile("unitTest.img",vlab);
  p.SetOutputCube("TO");
  p.StartProcess();
  p.EndProcess();

  cout << vlab << endl;
  Isis::Process p2;
  Isis::CubeAttributeInput att;
  string file = Isis::Application::GetUserInterface().GetFilename("TO");
  Isis::Cube *icube = p2.SetInputCube(file,att);
  Isis::Statistics *stat = icube->Statistics();
  cout << stat->Average() << endl;
  cout << stat->Variance() << endl; 
  p2.EndProcess();
  remove(file.c_str());
}
