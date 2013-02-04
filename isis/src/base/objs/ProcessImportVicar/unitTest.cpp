#include "Isis.h"

#include <QFile>

#include "ProcessImportVicar.h"
#include "Application.h"
#include "Cube.h"
#include "Statistics.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  Preference::Preferences(true);

  ProcessImportVicar p;
  Pvl vlab;
  p.SetVicarFile("unitTest.img", vlab);
  p.SetOutputCube("TO");
  p.StartProcess();
  p.EndProcess();

  cout << vlab << endl;
  Process p2;
  CubeAttributeInput att;
  QString file = Application::GetUserInterface().GetFileName("TO");
  Cube *icube = p2.SetInputCube(file, att);
  Statistics *stat = icube->statistics();
  cout << stat->Average() << endl;
  cout << stat->Variance() << endl;
  p2.EndProcess();
  QFile::remove(file);
}
