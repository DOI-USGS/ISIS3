#include <iostream>
#include <sstream>

#include <QList>
#include <QString>
#include <QStringList>

#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Kernel.h"
#include "KernelDb.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

void testBetter(KernelDb kdb, QString conditions);
void testLoadSystemDb(KernelDb &kdb, const QString &mission, const Pvl &lab);
void testKernelAccessors(KernelDb &kdb, Pvl &lab, bool timeBasedKernelsOnly);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  Pvl lab;
  PvlObject obj("IsisCube");
  PvlGroup group("Instrument");
  PvlGroup group2("TestGroup");
  PvlKeyword keyword("TestKeyword", "TestValue");
  PvlKeyword startKey("StartTime", "2005 JUN 15 12:00:00.000 TDB");
  PvlKeyword spacecraft("SpacecraftName", "IdealSpacecraft");
  PvlKeyword instrument("InstrumentId", "IdealCamera");
  PvlKeyword endKey("StopTime", "2005 DEC 15 12:00:00.000 TDB");
  PvlKeyword endKey2("StopTime", "2005 JUN 15 12:14:00.000 TDB");
  group2.AddKeyword(keyword);
  group.AddKeyword(startKey);
  group.AddKeyword(endKey);
  group.AddKeyword(spacecraft);
  group.AddKeyword(instrument);
  obj.AddGroup(group);
  obj.AddGroup(group2);
  lab.AddObject(obj);

  Pvl lab2;
  PvlObject obj2("IsisCube");
  group.AddKeyword(endKey2, Pvl::Replace);
  obj2.AddGroup(group);
  obj2.AddGroup(group2);
  lab2.AddObject(obj2);

  cout << "Pvl Label 1: " << endl;
  cout << lab << endl;
  cout << endl;
  KernelDb kdb("test.db", 15);
  testKernelAccessors(kdb, lab, false);
  cout << "/---------------------------------------/" << endl;
  cout << endl << endl;
  cout << "Pvl Label 2: " << endl;
  cout << lab2 << endl;
  cout << endl;
  testKernelAccessors(kdb, lab2, false);

  // Test loadSystemDb

  // In the following tests, we test 
  // 
  // (1) No config file exists, use kernel.????.db file (this is true for all
  // but ck directories
  // 
  // (2) A config file exists with two match cases and no default (MDIS)
  //    (a) test for the correct match to NAC
  //    (b) test for the correct match to WAC
  // (3) A config file exists with one match case and a default (Mro)
  //    (a) test for default instrument settings
  //    (b) test for the correct match to CRISM
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("StartTime").SetValue("2008 JAN 12 00:00:00.0");
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("StopTime").SetValue("2008 JAN 12 00:00:00.0"); 

  // Note: The following is temporarily commented out since currently there is
  // no conf file for Messenger ISS NAC and WAC
  #if 0
  // (1) and (2a)
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("InstrumentId").SetValue("MDIS-NAC");
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("SpacecraftName").SetValue("Messenger");
  cout << "/---------------------------------------/" << endl;
  cout << endl << endl;
  cout << "Messenger NAC Label: " << endl;
  cout << lab << endl;
  KernelDb messengerMatchNac(13);
  testLoadSystemDb(messengerMatchNac, "Messenger", lab);
  testKernelAccessors(messengerMatchNac, lab, true);
  
  // (1) and (2b)
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("InstrumentId").SetValue("MDIS-WAC");
  cout << "/---------------------------------------/" << endl;
  cout << endl << endl;
  cout << "Messenger WAC Label: " << endl;
  cout << lab << endl;
  KernelDb messengerMatchWac(4);
  testLoadSystemDb(messengerMatchWac, "Messenger", lab);
  testKernelAccessors(messengerMatchWac, lab, true);
  #endif

  // (1) and (3a)
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("SpacecraftName").SetValue("MarsReconnaissanceOrbiter");
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("InstrumentId").SetValue("HiRISE");
  cout << "/---------------------------------------/" << endl;
  cout << endl << endl;
  cout << "Mro (non-CRISM) Instrument Label: " << endl;
  cout << lab << endl;
  KernelDb defaultMroInstrument(4);
  testLoadSystemDb(defaultMroInstrument, "Mro", lab);
  testKernelAccessors(defaultMroInstrument, lab, true);

  // (1) and (3b)
  lab.FindObject("IsisCube").FindGroup("Instrument")
     .FindKeyword("InstrumentId").SetValue("CRISM");
  cout << "/---------------------------------------/" << endl;
  cout << endl << endl;
  cout << "Mro CRISM Label: " << endl;
  cout << lab << endl;
  KernelDb mroMatchCrism(13);
  testLoadSystemDb(mroMatchCrism, "Mro", lab);
  testKernelAccessors(mroMatchCrism, lab, true);

  testBetter(kdb, "When all kernel types are allowed");
  testBetter(mroMatchCrism, "When Nadir is not allowed");

  // Not yet tested:

  // constructor stream db file, not label
  
  // Not able to test the following from "loadSystemDb()"
  // since these are in system config file:
  // 
  // Obj = Instrument has group != Selection.
  // pvl.Read(kernelDbFileName.expanded())

  // findLast() and findAll() not tested:
  // 
  // priority queue has only empty Kernel (i.e. no matches found)
  // kerneldb file doesn't have specified kernel file "entry" name
  // Label InstrumentGroup does not have StopTime
  // group != Selection.
  // one group has keyword "Type" and another doesn't
  // different "Type" values for allowed groups
  
  // files()
  // 
  // Test Selection group has File keyword with no value
  // Test Selection group has File keyword with 1 value that is versioned

  cout << "/---------------------------------------/" << endl;
  cout << endl << endl;
  Pvl kernelData;
  // this object's selection group has no kernel files
  PvlObject lsk("LeapSecond");
  PvlGroup selGrpLsk("Selection");
  lsk.AddGroup(selGrpLsk);
  kernelData.AddObject(lsk);
  // this object has a group that is not named Selection
  PvlObject pck("TargetAttitudeShape");
  PvlGroup selGrpPck("Selection");
  PvlGroup noSelGrpPck("NotSelectionGroup");
  PvlKeyword filePck("File");
  pck.AddGroup(selGrpPck);
  filePck.AddValue("pckTest????");
  selGrpPck.AddKeyword(filePck);
  noSelGrpPck.AddKeyword(filePck);
  pck.AddGroup(selGrpPck);
  pck.AddGroup(noSelGrpPck);
  kernelData.AddObject(pck);
  // this selection group has different quality types allowed
  PvlObject spk("TargetPosition");
  PvlGroup selGrp1("Selection");
  PvlKeyword file1("File");
  file1.AddValue("missionName");
  file1.AddValue("spkPredict");
  PvlKeyword type1("Type");
  type1.AddValue("Predicted");
  selGrp1.AddKeyword(file1);
  PvlGroup selGrp2("Selection");
  PvlKeyword file2("File");
  file2.AddValue("missionName");
  file2.AddValue("spkRecon");
  PvlKeyword type2("Type");
  type2.AddValue("Reconstructed");
  selGrp2.AddKeyword(file2);
  PvlGroup selGrp3("Selection");
  PvlKeyword file3("File");
  file3.AddValue("missionName");
  file3.AddValue("spkSmith");
  PvlKeyword type3("Type");
  type3.AddValue("Smithed");
  selGrp3.AddKeyword(file3);
  spk.AddGroup(selGrp1);
  spk.AddGroup(selGrp2);
  spk.AddGroup(selGrp3);
  kernelData.AddObject(spk);
  stringstream istrm;
  istrm << kernelData;
  KernelDb kdb2(istrm,  5);
  lab.FindObject("IsisCube").FindGroup("Instrument").DeleteKeyword("StopTime");
  cout << "Label, no StopTime: " << endl;
  cout << lab << endl << endl;
  testKernelAccessors(kdb2, lab, false);
  return 0;
}

/**
 * Method that prints a table of results that compare all Kernel::Types 
 * with for the given KernelDb using the better() method. This table will 
 * depend on the kernel types allowed. This should be described in the 
 * conditions parameter. 
 * 
 * @param kdb KernelDb object
 * @param conditions QString describing which Kernel::Types are allowed
 */
void testBetter(KernelDb kdb, QString conditions) {
  cout << endl; 
  cout << endl; 
  cout << endl; 
  cout << "Testing better(row, column) method ..." << endl; 
  cout << endl; 
  cout << "\t" << conditions << ", is row better than col?" << endl; 
  cout << "\t\t\tUnknown\tPredicted\tNadir\tRecon\tSmithed" << endl; 
  cout << "Unknown\t\t\t" << kdb.better("Unknown","Unknown") << "\t\t"
                          << kdb.better("Unknown","Predicted") << "\t\t"
                          << kdb.better("Unknown","Nadir") << "\t\t"
                          << kdb.better("Unknown","Reconstructed") << "\t\t" 
                          << kdb.better("Unknown","Smithed") << endl; 
  cout << "Predicted\t\t" << kdb.better("Predicted","Unknown") << "\t\t"
                          << kdb.better("Predicted","Predicted") << "\t\t" 
                          << kdb.better("Predicted","Nadir") << "\t\t" 
                          << kdb.better("Predicted","Reconstructed") << "\t\t" 
                          << kdb.better("Predicted","Smithed") << endl; 
  cout << "Nadir\t\t\t" << kdb.better("Nadir","Unknown") << "\t\t" 
                        << kdb.better("Nadir","Predicted") << "\t\t" 
                        << kdb.better("Nadir","Nadir") << "\t\t" 
                        << kdb.better("Nadir","Reconstructed") << "\t\t" 
                        << kdb.better("Nadir","Smithed") << endl; 
  cout << "Reconstructed\t" << kdb.better("Reconstructed","Unknown") << "\t\t" 
                            << kdb.better("Reconstructed","Predicted") << "\t\t" 
                            << kdb.better("Reconstructed","Nadir") << "\t\t" 
                            << kdb.better("Reconstructed","Reconstructed") << "\t\t" 
                            << kdb.better("Reconstructed","Smithed") << endl; 
  cout << "Smithed\t\t\t"   << kdb.better("Smithed","Unknown") << "\t\t" 
                            << kdb.better("Smithed","Predicted") << "\t\t" 
                            << kdb.better("Smithed","Nadir") << "\t\t" 
                            << kdb.better("Smithed","Reconstructed") << "\t\t" 
                            << kdb.better("Smithed","Smithed") << endl; 

}

/**
 * Method that prints the kernel database files that are read in by the
 * loadSystemDb() method.  These file names have the version numbers 
 * replaced with ?.  Also, the begining of the paths is stripped off 
 * and we are left with $mission or $base. 
 * 
 * @param kdb KernelDb object
 * @param mission Mission to be loaded
 * @param lab Pvl containing the labels (the insturment id will be matched, if
 *            necessary)
 */
void testLoadSystemDb(KernelDb &kdb, const QString &mission, const Pvl &lab){
  kdb.loadSystemDb(mission, lab);
  cout << endl;
  cout << "Database Files read in by loadSystemDb()" << endl;
  QList<FileName> kdbFiles = kdb.kernelDbFiles();
  foreach (FileName kdbFile, kdbFiles) {
    QString file = (kdbFile.expanded()).replace(QRegExp("[0-9]"), "?");
    QString str = "";
    if (file.contains("/dems/")) {
      str = file.section("/", -3, -1);
    }
    else {
      str = file.section("/", -4, -1);
    }
    str = "$" + str;
    cout << str << endl;
  }
  return;
}

/**
 * Method that prints the files that are returned by the following 
 * accessor methods: 
 *  
 * <ul> 
 *   <li>leapSecond()
 *   <li>targetAttitudeShape()
 *   <li>targetPosition()
 *   <li>spacecraftPointing()
 *   <li>instrument()
 *   <li>spacecraftClock()
 *   <li>spacecraftPosition()
 *   <li>frame()
 *   <li>instrumentAddendum()
 *   <li>dem()
 * </ul>
 * 
 * @param kdb KernelDb object
 * @param lab Pvl containing the labels (the insturment id will be matched, if
 *            necessary)
 * @param timeBasedKernelsOnly If true, only the accessors that find
 *                             kernels based on the start and end times
 *                             in the labels will be checked:
 *                             targetPosition(), spacecraftPointing()
 *                             and spacecraftPosition().
 */
void testKernelAccessors(KernelDb &kdb, Pvl &lab, bool timeBasedKernelsOnly){
  QStringList temp;
  if (!timeBasedKernelsOnly) {
    try {
      cout << "LeapSecond Kernels: " << endl;
      temp.clear();
      temp = kdb.leapSecond(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No LeapSecond Kernels" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }

    try {
      cout << endl << "TargetAttitudeShape Kernels: " << endl;
      temp.clear();
      temp = kdb.targetAttitudeShape(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No TargetAttitudeShape Kernels" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }
  }

  try {
    cout << endl << "TargetPosition Kernels: " << endl;
    temp.clear();
    temp = kdb.targetPosition(lab).kernels();
    for (int i = 0; i < temp.size(); i++) {
      cout << temp[i] << endl;
    }
    if (temp.size() == 0) {
      cout << "No TargetPosition Kernels" << endl;
    }
  }
  catch (IException &e) {
    e.print();
  }

  try {
    cout << endl << "SpacecraftPointing Kernels: " << endl;
    temp.clear();
    QList< priority_queue<Kernel> > cks = kdb.spacecraftPointing(lab); 
    for (int i = 0; i < cks.size(); i++) {
      priority_queue<Kernel> ck_queue = cks[i];
      if (ck_queue.size() > 0) {
        Kernel ck = ck_queue.top();
        temp = ck.kernels();
        for (int j = 0; j < temp.size(); j++) {
          cout << temp[j] << endl;
        }
      }
      else {
        cout << "No SpacecraftPointing Kernels for queue " << i + 1 << endl;
      }
    }
  }
  catch (IException &e) {
    e.print();
  }

  if (!timeBasedKernelsOnly) {
    try {
      cout << endl << "Instrument Kernels: " << endl;
      temp.clear();
      temp = kdb.instrument(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No Instrument Kernels" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }

    try {
      cout << endl << "SpacecraftClock Kernels: " << endl;
      temp.clear();
      temp = kdb.spacecraftClock(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No SpacecraftClock Kernels" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }
  }

  try {
    cout << endl << "SpacecraftPosition Kernels: " << endl;
    temp.clear();
    temp = kdb.spacecraftPosition(lab).kernels();
    for (int i = 0; i < temp.size(); i++) {
      cout << temp[i] << endl;
    }
    if (temp.size() == 0) {
      cout << "No SpacecraftPosition Kernels" << endl;
    }
  }
  catch (IException &e) {
    e.print();
  }

  if (!timeBasedKernelsOnly) {
    try {
      cout << endl << "Frame Kernels: " << endl;
      temp.clear();
      temp = kdb.frame(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No Frame Kernels" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }

    try {
      cout << endl << "InstrumentAddendum Kernels: " << endl;
      temp.clear();
      temp = kdb.instrumentAddendum(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No InstrumentAddendum Kernels" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }

    try {
      cout << endl << "Dems: " << endl;
      temp.clear();
      temp = kdb.dem(lab).kernels();
      for (int i = 0; i < temp.size(); i++) {
        cout << temp[i] << endl;
      }
      if (temp.size() == 0) {
        cout << "No DEMs" << endl;
      }
    }
    catch (IException &e) {
      e.print();
    }
  }
  return;
}
