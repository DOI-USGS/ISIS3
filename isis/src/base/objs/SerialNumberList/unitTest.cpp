#include <iostream>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "SerialNumberList.h"

using namespace Isis;
using namespace std;

/**
 * Unit Test for SerialNumberList 
 *  
 * @internal
 *   @history 2015-12-22 Jeannie Backer - Improved test coverage.
 *  
 *   @todo Test SerialNumberList(QString list, bool checkTarget, NULL);
 *   @todo Test SerialNumberList(QString list, bool checkTarget, non-NULL Progress);
 *   @todo Test error throws from SerialNumberList(QString, bool, Progress) :
 *         "Can't open or invalid file list."
 *   @todo Test error throws from Add(const QString &filename, bool def2filename) :
 *         "Invalid serial number [Unknown]"
 *         "Duplicate, serial number from files"
 *   @todo Test error throws from Add(const QString &serialNumber, const QString &filename) :
 *         "Invalid serial number [Unknown] from file"
 *         "Duplicate, serial number from files"
 *         "Unable to find Instrument group needed for performing bundle adjustment"
 *         "Unable to find SpacecraftName or InstrumentId keywords needed for performing bundle adjustment"
 */
int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  try {
    SerialNumberList snl(false);
    cout << endl << endl;
    // add filename, def2filename=false
    snl.Add("$mgs/testData/ab102401.cub");
    snl.Add("$mgs/testData/m0402852.cub");
    snl.Add("$lo/testData/3133_h1.cub");
    // add filename, def2filename=true
    snl.Add("$mgs/testData/ab102402.lev2.cub",true);
    // Test adding by giving a serial number and filename
    snl.Add("m0402852.cub", "$mgs/testData/m0402852.cub");

    cout << "SerialNumberList size = " << snl.Size() << endl;
    cout << "SerialNumberList has SerialNumber XYZ?          "
         << snl.HasSerialNumber(QString("XYZ")) << endl;
    cout << "SerialNumberList has SerialNumber m0402852.cub? "
         << snl.HasSerialNumber(QString("m0402852.cub")) << endl;

    for(int i = 0; i < snl.Size(); i++) {
      cout << toString(i+1) << endl;
      QString file = snl.FileName(i);
      QString sn = snl.SerialNumber(i);
      cout << "  FileName from index                  = " << FileName(file).name() << endl;
      cout << "  FileName from SerialNumber           = " << FileName(snl.FileName(sn)).name() << endl;
      cout << "  FileName index from FileName         = " << snl.FileNameIndex(file) << endl;
      cout << "  SerialNumber from index              = " << sn << endl;
      cout << "  SerialNumber from FileName           = " << snl.SerialNumber(file) << endl;
      cout << "  SerialNumber index from SerialNumber = " << snl.SerialNumberIndex(sn) << endl;
      cout << "  Observation number from index        = " << snl.ObservationNumber(i) << endl;
      cout << "  Spacecraft Instrument ID from index  = " << snl.SpacecraftInstrumentId(i) << endl;
      cout << "  Spacecraft ID from SerialNumber      = " << snl.SpacecraftInstrumentId(sn) << endl;
    }

    cout << endl << endl;
    cout << "Deleting first SerialNumber in the list..." << endl;
    snl.Delete(snl.SerialNumber(0));
    cout << "new list size = " << snl.Size() << endl;
    cout << "new list serial numbers: " << endl;
    for(int i = 0; i < snl.Size(); i++) {
      QString sn = snl.SerialNumber(i);
      cout << "  " << sn << endl;
    }

    cout << endl << endl;
    for (int i = 0; i < snl.Size(); i++) {
      QString obsNum = snl.ObservationNumber(i);
      cout << "Possible SerialNumbers for Observation Number: " << obsNum << endl;
      std::vector<QString> possibles = snl.PossibleSerialNumbers(obsNum);
      for (unsigned int j = 0; j < possibles.size(); j++) {
        cout << "  " << possibles[j] << endl;
      }
    }

  }
  catch(IException &e) {
    e.print();
  }

  SerialNumberList snl;
  try {
    cout << endl << endl;
    // Test to make sure all targets being the same works
    snl.Add("$mgs/testData/ab102401.cub", true);
    snl.Add("$base/testData/blobTruth.cub");
    snl.Add("$lo/testData/3133_h1.cub");
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }
  try {
    cout << endl << endl;
    // no inst group, def2filename, no mapping
    QString filename("$base/testData/isisTruth.cub");
    snl.Add(filename, true);
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }
  try {
    cout << endl << endl;
    // no inst group, no def2filename
    QString filename("$base/testData/isisTruth.cub");
    snl.Add(filename, false);
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }


  try {
    cout << endl << endl;
    // Test to make sure all targets being the same works
    snl.Add("sn1", "$mgs/testData/ab102401.cub");
    snl.Add("sn2", "$base/testData/blobTruth.cub");
    snl.Add("sn3", "$lo/testData/3133_h1.cub");
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }
  try {
    cout << endl << endl;
    // no inst group, def2filename, no mapping
    QString filename("$base/testData/isisTruth.cub");
    snl.Add("sn1", filename);
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }


  try {
    cout << endl << endl;
    snl.FileName("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.SerialNumber("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.SerialNumber(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.ObservationNumber(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.SerialNumberIndex("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.FileNameIndex("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.FileName(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.SpacecraftInstrumentId(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.SpacecraftInstrumentId("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.PossibleSerialNumbers("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }

}
