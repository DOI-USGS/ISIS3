#include "Isis.h"

#include <cmath>
#include <float.h>

#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "iException.h"
#include "PvlContainer.h"
#include "PvlGroup.h"



using namespace std;
using namespace Isis;

bool filesMatch;
iString differenceReason;
PvlGroup tolerances;
PvlGroup ignorekeys;

void CompareKeywords(const PvlKeyword &pvl1, const PvlKeyword &pvl2);
void CompareGroups(const PvlContainer &pvl1, const PvlContainer &pvl2);
void Compare(const ControlPoint &point1, const ControlPoint &point2);
void Compare(ControlNet net1, ControlNet net2);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  tolerances = PvlGroup();
  ignorekeys = PvlGroup();

  differenceReason = "";
  filesMatch = true;

  const ControlNet net1(ui.GetFilename("FROM"));
  const ControlNet net2(ui.GetFilename("FROM2"));

  if(ui.WasEntered("DIFF")) {
    Pvl diffFile(ui.GetFilename("DIFF"));

    if(diffFile.HasGroup("Tolerances")) {
      tolerances = diffFile.FindGroup("Tolerances");
    }

    if(diffFile.HasGroup("IgnoreKeys")) {
      ignorekeys = diffFile.FindGroup("IgnoreKeys");
    }
  }

  // Don't want to consider the DateTime of a Point or Measure was set by
  // default.
  if (!ignorekeys.HasKeyword("DateTime")) {
    ignorekeys += PvlKeyword("DateTime", "true");
  }

  Compare(net1, net2);

  PvlGroup differences("Results");
  if(filesMatch) {
    differences += PvlKeyword("Compare", "Identical");
  }
  else {
    differences += PvlKeyword("Compare", "Different");
    differences += PvlKeyword("Reason", differenceReason);
  }

  Application::Log(differences);

  if(ui.WasEntered("TO")) {
    Pvl out;
    out.AddGroup(differences);
    out.Write(ui.GetFilename("TO"));
  }

  differenceReason = "";
}

void Compare(ControlNet net1, ControlNet net2) {
  if(net1.Size() != net2.Size()) {
    differenceReason = "The number of control points in the networks, [" +
        iString(net1.Size()) + "] and [" + iString(net2.Size()) + ", differ";
    filesMatch = false;
    return;
  }

  if(net1.NetworkId() != net2.NetworkId()) {
    differenceReason = "The network IDs, [" +
        iString(net1.NetworkId()) + "] and [" + iString(net2.NetworkId()) + 
        " differ";
    filesMatch = false;
    return;
  }

  if(net1.NetworkId() != net2.NetworkId()) {
    differenceReason = "The targets, [" +
        iString(net1.Target()) + "] and [" + iString(net2.Target()) + 
        " differ";
    filesMatch = false;
    return;
  }

  net1.SortControlNet();
  net2.SortControlNet();

  for(int cpIndex = 0; cpIndex < net1.Size(); cpIndex ++) {
    const ControlPoint &net1Point = net1[cpIndex];
    const ControlPoint &net2Point = net2[cpIndex];

    Compare(net1Point, net2Point);

    if(!filesMatch) {
      return;
    }
  }
}

void Compare(const ControlPoint &point1, const ControlPoint &point2) {
  PvlObject point1Pvl = point1.ToPvlObject();
  PvlObject point2Pvl = point2.ToPvlObject();

  // both names must be at least equal, should be named ControlPoint
  if(point1Pvl.Name() != point2Pvl.Name()) {
    iString msg = "The control points' CreatePvlOject method returned an "
        "unexpected result";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }
  
  if(point1Pvl.Groups() != point2Pvl.Groups()) {
    filesMatch = false;
    differenceReason = "The number of control measures, [" + 
        iString(point1Pvl.Groups()) + "] and [" + iString(point2Pvl.Groups()) +
        "] does not match";
  }

  // Start by comparing top level control point keywords.
  if(filesMatch) CompareGroups(point1Pvl, point2Pvl);

  // Now compare each measure
  for(int cmIndex = 0; filesMatch && cmIndex < point1Pvl.Groups(); cmIndex ++) {
    PvlGroup &measure1 = point1Pvl.Group(cmIndex);
    PvlGroup &measure2 = point2Pvl.Group(cmIndex);

    CompareGroups(measure1, measure2);

    if(!filesMatch) {
      differenceReason = "Control Measure for Cube [" +
          measure1["SerialNumber"][0] + "] " + differenceReason;
    }
  }

  if(!filesMatch) {
    differenceReason = "Control Point [" + point1.GetId() +
        "] " + differenceReason;
  }
}


void CompareGroups(const PvlContainer &pvl1, const PvlContainer &pvl2) {
  // Create equivalent PvlGroups that can easily be compared to each other
  PvlGroup point1FullKeys;
  PvlGroup point2FullKeys;

  for(int keywordIndex = 0; keywordIndex < pvl1.Keywords(); keywordIndex++) {
    PvlKeyword thisKey = pvl1[keywordIndex]; 
    point1FullKeys += thisKey;

    if(!pvl2.HasKeyword(thisKey.Name())) {
      point2FullKeys += PvlKeyword(thisKey.Name(), "");
    }
  }

  for(int keywordIndex = 0; keywordIndex < pvl2.Keywords(); keywordIndex++) {
    PvlKeyword thisKey = pvl2[keywordIndex]; 
    point2FullKeys += thisKey;

    if(!pvl1.HasKeyword(thisKey.Name())) {
      point1FullKeys += PvlKeyword(thisKey.Name(), "");
    }
  }

  // Now compare the PvlGroups
  for(int keywordIndex = 0;
      keywordIndex < point1FullKeys.Keywords();
      keywordIndex++) {
    PvlKeyword key1 = point1FullKeys[keywordIndex];
    PvlKeyword key2 = point2FullKeys[key1.Name()];
    CompareKeywords(key1, key2);
  }
}



void CompareKeywords(const PvlKeyword &pvl1, const PvlKeyword &pvl2) {
  if(pvl1.Name().compare(pvl2.Name()) != 0) {
    iString msg = "CompareKeywords should always be called with keywords that "
        "have the same name";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }

  if(pvl1.Size() != pvl2.Size()) {
    filesMatch = false;
    differenceReason = "Value '" + pvl1.Name() + "' array size does not match.";
    return;
  }

  if(tolerances.HasKeyword(pvl1.Name()) &&
      tolerances[pvl1.Name()].Size() > 1 &&
      pvl1.Size() != tolerances[pvl1.Name()].Size()) {
    string msg = "Size of value '" + pvl1.Name() + "' does not match with ";
    msg += "its number of tolerances in the DIFF file.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  if(ignorekeys.HasKeyword(pvl1.Name()) &&
      ignorekeys[pvl1.Name()].Size() > 1 &&
      pvl1.Size() != ignorekeys[pvl1.Name()].Size()) {
    string msg = "Size of value '" + pvl1.Name() + "' does not match with ";
    msg += "its number of ignore keys in the DIFF file.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  for(int i = 0; i < pvl1.Size() && filesMatch; i++) {
    iString val1 = pvl1[i];
    iString val2 = pvl2[i];
    iString unit1 = pvl1.Unit(i);
    iString unit2 = pvl2.Unit(i);

    int ignoreIndex = 0;
    if(ignorekeys.HasKeyword(pvl1.Name()) && ignorekeys[pvl1.Name()].Size() > 1) {
      ignoreIndex = i;
    }

    try {
      if(!ignorekeys.HasKeyword(pvl1.Name()) ||
          ignorekeys[pvl1.Name()][ignoreIndex] == "false") {

        if(!unit1.Equal(unit2)) {
          filesMatch = false;
          differenceReason = "Value '" + pvl1.Name() + "': units do not match.";
          return;
        }

        double tolerance = 0.0;
        double difference = abs((double)val1 - (double)val2);

        if(tolerances.HasKeyword(pvl1.Name())) {
          tolerance = (tolerances[pvl1.Name()].Size() == 1) ?
                      tolerances[pvl1.Name()][0] : tolerances[pvl1.Name()][i];
        }

        if(difference > tolerance) {
          filesMatch = false;
          if(pvl1.Size() == 1) {
            differenceReason = "Value [" + pvl1.Name() + "] difference is " +
                               iString(difference);
          }
          else {
            differenceReason = "Value [" + pvl1.Name() + "] at index " +
                               iString(i) + ": difference is " + iString(difference);
          }
          differenceReason += " (values are [" + iString(val1) + "] and [" + 
              iString(val2) + "], tolerance is [" + iString(tolerance) + "])";
        }
      }
    }
    catch(iException e) {
      iException::Clear();

      if(!val1.Equal(val2)) {
        filesMatch = false;
        differenceReason = "Value '" + pvl1.Name() + "': values do not match.";
      }
    }
  }
}

