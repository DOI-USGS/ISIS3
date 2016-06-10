#include <iostream>

#include <QFile>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "Progress.h"
#include "SerialNumberList.h"

using namespace Isis;
using namespace std;

/**
 * Unit Test for SerialNumberList 
 *  
 * @internal
 *   @history 2015-12-22 Jeannie Backer - Improved test coverage.
 *   @history 2016-06-02 Ian Humphrey - Improved test coverage.
 *  
 *   @todo Test error throws from add(const QString &filename, bool def2filename) :
 *         "Invalid serial number [Unknown]"
 */

void printSerialNumberList(SerialNumberList snl);

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

    printSerialNumberList(snl);

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
  

  cout << endl << endl;

  // Test SerialNumberList(QString, bool, Progress)
  cout << "Creating SerialNumberList(QString, bool, Progress)" << endl;

  // Setup temp file - Need to cleanup later!
  FileName temp("$temporary/templist.txt");
  QFile tempFile(temp.expanded());
  tempFile.open(QIODevice::WriteOnly | QIODevice::Text);
  tempFile.write("$mgs/testData/ab102401.cub\n");
  tempFile.write("$mgs/testData/m0402852.cub\n");
  tempFile.close();

  Progress p;
  SerialNumberList snlProgress(temp.expanded(), false, &p);
  printSerialNumberList(snlProgress);

  // Test SerialNumberList(QString, bool, Progress=NULL)
  cout << endl << "Creating SerialNumberList(QString, bool, Progress=NULL)" << endl;
  SerialNumberList snlProgressNull(temp.expanded(), true, NULL);
  printSerialNumberList(snlProgressNull);

  cout << endl << endl;


  // Exceptions


  // Test SerialNumberList(QString, bool, Progress) with invalid file
  try {
    SerialNumberList("DNEFile", true, NULL);
  }
  catch (IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }


  SerialNumberList snl;
  // Test add(QString, bool)
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
    // adding a duplicate filename
    snl.add("$mgs/testData/ab102401.cub");
  }
  catch (IException &e) {
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


  SerialNumberList snlNoTarget(false);
  // Test add(QString, QString)
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
    // adding an unknown sn
    snlNoTarget.add(QString("Unknown"), QString("$base/testData/blobTruth.cub"));
  }
  catch (IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }

  try {
    cout << endl << endl;
    // adding a duplicate sn
    snlNoTarget.add("$mgs/testData/ab102401.cub");
    QString sn = snlProgress.serialNumber("$mgs/testData/ab102401.cub");
    snlNoTarget.add(sn, QString("$base/testData/blobTruth.cub"));
  }
  catch (IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }

  try {
    cout << endl << endl;
    // adding a file without an Instrument group
    QString filename("$base/testData/isisTruth.cub");
    snlNoTarget.add("sn1", filename);
  }
  catch (IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }

  try {
    cout << endl << endl;
    // adding a file without a SpacecraftName keyword
    snlNoTarget.add("sn2", QString("$base/testData/isisTruthNoSpacecraftName.cub"));
  }
  catch (IException &e) {
    QString error = e.toString().replace(QRegExp("(\\[[^\\]]*/)([^\\]]*)"), "[.../\\2");
    cerr << error.toStdString() << endl;
  }

  try {
    cout << endl << endl;
    // adding a file without an InstrumentId keyword
    snlNoTarget.add("sn3", QString("$base/testData/isisTruthNoInstrumentId.cub"));
  }
  catch (IException &e) {
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


  // Cleanup the temp file
  tempFile.remove();

}


/**
 * Prints the information contained in the SerialNumberList for verification
 *
 * @param snl SerialNumberList to print
 *
 * @internal
 *   @history 2016-06-02 Ian Humphrey - Added this method, adapted from code in unit test.
 */
void printSerialNumberList(SerialNumberList snl) {
  for (int i = 0; i < snl.size(); i++) {
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
}
