#include "Isis.h"
#include "ProcessImport.h"
#include "Cube.h"
#include "Application.h"
#include "Preference.h"
#include "Statistics.h"

using namespace std;

void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessImport Class ... " << endl;

  Isis::Preference::Preferences(true);

  Isis::ProcessImport p;
  p.SetInputFile("$base/testData/isisTruth.dat");
  p.SetBase(0.0);
  p.SetMultiplier(1.0);
  p.SetDataHeaderBytes(0);
  p.SetDataPrefixBytes(0);
  p.SetDataSuffixBytes(0);
  p.SetDataTrailerBytes(0);
  p.SetDimensions(126, 126, 1);
  p.SetFileHeaderBytes(16384);
  p.SetOrganization(Isis::ProcessImport::BSQ);
  p.SetPixelType(Isis::Real);
  p.SetByteOrder(Isis::Lsb);
  p.SetOutputCube("TO");
  p.StartProcess();
  p.EndProcess();

  Isis::Process p2;
  Isis::CubeAttributeInput att;
  string file = Isis::Application::GetUserInterface().GetFileName("TO");
  Isis::Cube *icube = p2.SetInputCube(file, att);
  Isis::Statistics *stat = icube->getStatistics();
  cout << endl << "Average: " << stat->Average() << endl;
  cout << endl << "Variance: " << stat->Variance() << endl;
  p2.EndProcess();
  remove(file.c_str());
  cout << endl;

  //Checks the setting of special pixel ranges

  cout << "Check the settings of the special pixel ranges" << endl;

  Isis::ProcessImport pNull;
  pNull.SetNull(0.0, 45.0);
  try { // Should NOT throw an error
    pNull.SetNull(0.0, 45.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pNull.SetLRS(35.0, 55.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should NOT throw an error
    pNull.SetLIS(50.0, 52.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pNull.SetHRS(-10.0, 5.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;

  Isis::ProcessImport pLRS;
  pLRS.SetLRS(10.0, 145.0);
  try { // Should throw an error
    pLRS.SetNull(35.0, 55.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pNull.SetLIS(0.0, 15.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pLRS.SetHIS(-10.0, 155.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should NOT throw an error
    pLRS.SetHIS(145.0, 155.0);
  }
  catch(Isis::IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;

  cout << "Testing ProcessBil()" << endl;
  Isis::ProcessImport p3;
  p3.SetInputFile("$base/testData/isisTruth.dat");
  p3.SetBase(0.0);
  p3.SetMultiplier(1.0);
  p3.SetDataHeaderBytes(0);
  p3.SetDataPrefixBytes(0);
  p3.SetDataSuffixBytes(0);
  p3.SetDataTrailerBytes(0);
  p3.SetDimensions(126, 126, 1);
  p3.SetFileHeaderBytes(16384);
  p3.SetOrganization(Isis::ProcessImport::BIL);
  p3.SetPixelType(Isis::Real);
  p3.SetByteOrder(Isis::Lsb);
  p3.SetOutputCube("TO");
  p3.StartProcess();
  p3.EndProcess();

  cout << endl << "Testing ProcessBip()" << endl;
  Isis::ProcessImport p4;
  p4.SetInputFile("$base/testData/isisTruth.dat");
  p4.SetBase(0.0);
  p4.SetMultiplier(1.0);
  p4.SetDataHeaderBytes(0);
  p4.SetDataPrefixBytes(0);
  p4.SetDataSuffixBytes(0);
  p4.SetDataTrailerBytes(0);
  p4.SetDimensions(126, 126, 1);
  p4.SetFileHeaderBytes(16384);
  p4.SetOrganization(Isis::ProcessImport::BIP);
  p4.SetPixelType(Isis::Real);
  p4.SetByteOrder(Isis::Lsb);
  p4.SetOutputCube("TO");
  p4.StartProcess();
  p4.EndProcess();
}
