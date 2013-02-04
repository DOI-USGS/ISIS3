#include <iostream>
#include "IException.h"
#include "KernelDb.h"
#include "Preference.h"
#include "Pvl.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  KernelDb kdb("test.db", 15);

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
  std::vector<QString> temp;

  Pvl lab2;
  PvlObject obj2("IsisCube");
  group.AddKeyword(endKey2, Pvl::Replace);
  obj2.AddGroup(group);
  obj2.AddGroup(group2);
  lab2.AddObject(obj2);

  temp.clear();
  temp = kdb.LeapSecond(lab).kernels;
  cout << "LeapSecond: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.TargetAttitudeShape(lab).kernels;
  cout << endl << "TargetAttitudeShape: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.TargetPosition(lab).kernels;
  cout << endl << "TargetPosition: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.SpacecraftPointing(lab).top().kernels;
  cout << endl << "SpacecraftPointing: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.SpacecraftPointing(lab2).top().kernels;
  cout << endl << "SpacecraftPointing 2: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.Instrument(lab).kernels;
  cout << endl << "Instrument: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.SpacecraftClock(lab).kernels;
  cout << endl << "SpacecraftClock: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.SpacecraftPosition(lab).kernels;
  cout << endl << "SpacecraftPosition: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.Frame(lab).kernels;
  cout << endl << "Frame: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.InstrumentAddendum(lab).kernels;
  cout << endl << "InstrumentAddendum: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }

  temp.clear();
  temp = kdb.Dem(lab).kernels;
  cout << endl << "Dem: " << endl;
  for(unsigned int i = 0; i < temp.size(); i++) {
    cout << temp[i] << endl;
  }
}
