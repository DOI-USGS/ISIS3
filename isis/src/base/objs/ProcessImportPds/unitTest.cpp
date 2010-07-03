#include "Isis.h"
#include "ProcessImportPds.h"
#include "Application.h"
#include "OriginalLabel.h"
#include "Statistics.h"

using namespace std;
void IsisMain() {

  Isis::Preference::Preferences(true);

  // Test an IMAGE file
  try {
    cout << "Testing PDS file containing an ^IMAGE pointer" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("unitTest.img", "unitTest.img", plab);
    p.SetOutputCube("TO");
    p.StartProcess();
    p.EndProcess();

    cout << plab << endl;
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    string file = Isis::Application::GetUserInterface().GetFilename("TO");
    Isis::Cube *cube = p2.SetInputCube(file, att);
    Isis::Statistics *stat = cube->Statistics();
    cout << stat->Average() << endl;
    cout << stat->Variance() << endl; 
    p2.EndProcess();
    Isis::OriginalLabel ol(file);
    Isis::Pvl label = ol.ReturnLabels();
    cout << label << endl;
    remove(file.c_str());
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  // Test a QUBE file
  try {
    cout << endl;
    cout << "Testing PDS file containing a ^QUBE pointer" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("unitTest.lab", "", plab);
    p.SetOutputCube("TO");
    p.StartProcess();
    p.OmitOriginalLabel();
    p.EndProcess();
  
    cout << plab << endl;
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    string file = Isis::Application::GetUserInterface().GetFilename("TO");
    Isis::Cube *cube = p2.SetInputCube(file, att);
    Isis::Statistics *stat = cube->Statistics();
    cout << stat->Average() << endl;
    cout << stat->Variance() << endl; 
    p2.EndProcess();
    try {
      Isis::OriginalLabel ol(file);
    }
    catch (Isis::iException &e) {
      e.Report(false);
    }
    remove(file.c_str());
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  // Test an Isis2 file
  try {
    cout << endl;
    cout << "Testing Isis2 file" << endl;
    Isis::ProcessImportPds p;
    Isis::Pvl plab;
    p.SetPdsFile("clemuvvis.cub", "clemuvvis.cub", plab);
    p.SetOutputCube("TO");
    p.StartProcess();
    Isis::Pvl ilab;
    p.TranslateIsis2Labels (ilab);
    p.EndProcess();

    cout << ilab << endl;
    string file = Isis::Application::GetUserInterface().GetFilename("TO");
    remove(file.c_str());
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}
