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
 *   @todo Test error throws from add(const QString &filename, bool def2filename) :
 *         "Invalid serial number [Unknown]"
 *         "Duplicate, serial number from files"
 *   @todo Test error throws from add(const QString &serialNumber, const QString &filename) :
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
    snl.add("$mgs/testData/ab102401.cub");
    snl.add("$mgs/testData/m0402852.cub");
    snl.add("$lo/testData/3133_h1.cub");
    // add filename, def2filename=true
    snl.add("$mgs/testData/ab102402.lev2.cub",true);
    // Test adding by giving a serial number and filename
    snl.add("m0402852.cub", "$mgs/testData/m0402852.cub");

    cout << "SerialNumberList size = " << snl.size() << endl;
    cout << "SerialNumberList has SerialNumber XYZ?          "
         << snl.hasSerialNumber(QString("XYZ")) << endl;
    cout << "SerialNumberList has SerialNumber m0402852.cub? "
         << snl.hasSerialNumber(QString("m0402852.cub")) << endl;

    for(int i = 0; i < snl.size(); i++) {
      cout << toString(i+1) << endl;
      QString file = snl.fileName(i);
      QString sn = snl.serialNumber(i);
      cout << "  FileName from index                  = " << FileName(file).name() << endl;
      cout << "  FileName from SerialNumber           = " << FileName(snl.fileName(sn)).name() << endl;
      cout << "  FileName index from FileName         = " << snl.fileNameIndex(file) << endl;
      cout << "  SerialNumber from index              = " << sn << endl;
      cout << "  SerialNumber from FileName           = " << snl.serialNumber(file) << endl;
      cout << "  SerialNumber index from SerialNumber = " << snl.serialNumberIndex(sn) << endl;
      cout << "  Observation number from index        = " << snl.observationNumber(i) << endl;
      cout << "  Spacecraft Instrument ID from index  = " << snl.spacecraftInstrumentId(i) << endl;
      cout << "  Spacecraft ID from SerialNumber      = " << snl.spacecraftInstrumentId(sn) << endl;
    }

    cout << endl << endl;
    cout << "Deleting first SerialNumber in the list..." << endl;
    snl.Delete(snl.serialNumber(0));
    cout << "new list size = " << snl.size() << endl;
    cout << "new list serial numbers: " << endl;
    for(int i = 0; i < snl.size(); i++) {
      QString sn = snl.serialNumber(i);
      cout << "  " << sn << endl;
    }

    cout << endl << endl;
    for (int i = 0; i < snl.size(); i++) {
      QString obsNum = snl.observationNumber(i);
      cout << "Possible SerialNumbers for Observation Number: " << obsNum << endl;
      std::vector<QString> possibles = snl.possibleSerialNumbers(obsNum);
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
    snl.add("$mgs/testData/ab102401.cub", true);
    snl.add("$base/testData/blobTruth.cub");
    snl.add("$lo/testData/3133_h1.cub");
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }
  try {
    cout << endl << endl;
    // no inst group, def2filename, no mapping
    QString filename("$base/testData/isisTruth.cub");
    snl.add(filename, true);
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }
  try {
    cout << endl << endl;
    // no inst group, no def2filename
    QString filename("$base/testData/isisTruth.cub");
    snl.add(filename, false);
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }


  try {
    cout << endl << endl;
    // Test to make sure all targets being the same works
    snl.add("sn1", "$mgs/testData/ab102401.cub");
    snl.add("sn2", "$base/testData/blobTruth.cub");
    snl.add("sn3", "$lo/testData/3133_h1.cub");
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }
  try {
    cout << endl << endl;
    // no inst group, def2filename, no mapping
    QString filename("$base/testData/isisTruth.cub");
    snl.add("sn1", filename);
  }
  catch(IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }


  try {
    cout << endl << endl;
    snl.fileName("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.serialNumber("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.serialNumber(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.observationNumber(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.serialNumberIndex("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.fileNameIndex("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.fileName(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.spacecraftInstrumentId(17);
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.spacecraftInstrumentId("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }
  try {
    cout << endl << endl;
    snl.possibleSerialNumbers("Nonsense");
  }
  catch (IException &e) {
    e.print();
  }

}
