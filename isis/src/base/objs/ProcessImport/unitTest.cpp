#include "Isis.h"

#include <QFile>

#include "ProcessImport.h"
#include "Cube.h"
#include "Application.h"
#include "Preference.h"
#include "Statistics.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  Preference::Preferences(true);

  cout << "Testing ProcessImport Class ... " << endl;

  Preference::Preferences(true);

  ProcessImport p;
  p.SetInputFile("$base/testData/isisTruth.dat");
  p.SetBase(0.0);
  p.SetMultiplier(1.0);
  p.SetDataHeaderBytes(0);
  p.SetDataPrefixBytes(0);
  p.SetDataSuffixBytes(0);
  p.SetDataTrailerBytes(0);
  p.SetDimensions(126, 126, 1);
  p.SetFileHeaderBytes(16384);
  p.SetOrganization(ProcessImport::BSQ);
  p.SetPixelType(Real);
  p.SetByteOrder(Lsb);
  p.SetOutputCube("TO");
  p.StartProcess();
  p.EndProcess();

  Process p2;
  CubeAttributeInput att;
  QString file = Application::GetUserInterface().GetFileName("TO");
  Cube *icube = p2.SetInputCube(file, att);
  Statistics *stat = icube->statistics();
  cout << endl << "Average: " << stat->Average() << endl;
  cout << endl << "Variance: " << stat->Variance() << endl;
  p2.EndProcess();
  cout << endl;

  //Checks the setting of special pixel ranges

  cout << "Check the settings of the special pixel ranges" << endl;

  ProcessImport pNull;
  pNull.SetNull(0.0, 45.0);
  try { // Should NOT throw an error
    pNull.SetNull(0.0, 45.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pNull.SetLRS(35.0, 55.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should NOT throw an error
    pNull.SetLIS(50.0, 52.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pNull.SetHRS(-10.0, 5.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;

  ProcessImport pLRS;
  pLRS.SetLRS(10.0, 145.0);
  try { // Should throw an error
    pLRS.SetNull(35.0, 55.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pNull.SetLIS(0.0, 15.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should throw an error
    pLRS.SetHIS(-10.0, 155.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;
  try { // Should NOT throw an error
    pLRS.SetHIS(145.0, 155.0);
  }
  catch(IException e) {
    cout << e.toString() << endl;
  }
  cout << endl;

  cout << "Testing ProcessBil()" << endl;
  ProcessImport p3;
  p3.SetInputFile("$base/testData/isisTruth.dat");
  p3.SetBase(0.0);
  p3.SetMultiplier(1.0);
  p3.SetDataHeaderBytes(0);
  p3.SetDataPrefixBytes(0);
  p3.SetDataSuffixBytes(0);
  p3.SetDataTrailerBytes(0);
  p3.SetDimensions(126, 126, 1);
  p3.SetFileHeaderBytes(16384);
  p3.SetOrganization(ProcessImport::BIL);
  p3.SetPixelType(Real);
  p3.SetByteOrder(Lsb);
  p3.SetOutputCube("TO");
  p3.StartProcess();
  p3.EndProcess();

  cout << endl << "Testing ProcessBip()" << endl;
  ProcessImport p4;
  p4.SetInputFile("$base/testData/isisTruth.dat");
  p4.SetBase(0.0);
  p4.SetMultiplier(1.0);
  p4.SetDataHeaderBytes(0);
  p4.SetDataPrefixBytes(0);
  p4.SetDataSuffixBytes(0);
  p4.SetDataTrailerBytes(0);
  p4.SetDimensions(126, 126, 1);
  p4.SetFileHeaderBytes(16384);
  p4.SetOrganization(ProcessImport::BIP);
  p4.SetPixelType(Real);
  p4.SetByteOrder(Lsb);
  p4.SetOutputCube("TO");
  p4.StartProcess();
  p4.EndProcess();

  QFile::remove(file);
}
