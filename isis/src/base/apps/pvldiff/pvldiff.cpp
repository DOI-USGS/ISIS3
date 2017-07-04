#include "Isis.h"

#include <cmath>
#include <float.h>

#include "PvlContainer.h"
#include "Pvl.h"
#include "IException.h"
#include "QRegularExpression"


using namespace std;
using namespace Isis;

bool filesMatch;
QString differenceReason;
PvlGroup tolerances;
PvlGroup ignorekeys;
PvlGroup ignorefilepaths;

void CompareKeywords(const PvlKeyword &pvl1, const PvlKeyword &pvl2);
void CompareObjects(const PvlObject &pvl1, const PvlObject &pvl2);
void CompareGroups(const PvlGroup &pvl1, const PvlGroup &pvl2);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  tolerances = PvlGroup();
  ignorekeys = PvlGroup();
  ignorefilepaths = PvlGroup();
  differenceReason = "";
  filesMatch = true;

  const Pvl file1(ui.GetFileName("FROM"));
  const Pvl file2(ui.GetFileName("FROM2"));

  if(ui.WasEntered("DIFF")) {
    Pvl diffFile(ui.GetFileName("DIFF"));

    if(diffFile.hasGroup("Tolerances")) {
      tolerances = diffFile.findGroup("Tolerances");
    }

    if(diffFile.hasGroup("IgnoreKeys")) {
      ignorekeys = diffFile.findGroup("IgnoreKeys");
    }

    if(diffFile.hasGroup("IgnoreFilePaths")) {
      ignorefilepaths = diffFile.findGroup("IgnoreFilePaths");
    }
  }

  CompareObjects(file1, file2);

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
    out.addGroup(differences);
    out.write(ui.GetFileName("TO"));
  }

  differenceReason = "";
}

void CompareKeywords(const PvlKeyword &pvl1, const PvlKeyword &pvl2) {
  if(pvl1.name().compare(pvl2.name()) != 0) {
    filesMatch = false;
    differenceReason = "Keyword '" + pvl1.name() + "' does not match keyword '" + pvl2.name() + "'";
  }

  if(pvl1.size() != pvl2.size()) {
    filesMatch = false;
    differenceReason = "Keyword '" + pvl1.name() + "' size does not match.";
    return;
  }

  // Make sure that  Tolerances, IgnoreKeys and IgnoreFilePaths each have either one argument that
  // applies to all values in the keyword OR a one-to-one relationship between the two
  if(tolerances.hasKeyword(pvl1.name()) &&
      tolerances[pvl1.name()].size() > 1 &&
      pvl1.size() != tolerances[pvl1.name()].size()) {
    QString msg = "Size of keyword '" + pvl1.name() + "' does not match with ";
    msg += "its number of tolerances in the DIFF file.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(ignorekeys.hasKeyword(pvl1.name()) &&
      ignorekeys[pvl1.name()].size() > 1 &&
      pvl1.size() != ignorekeys[pvl1.name()].size()) {
    QString msg = "Size of keyword '" + pvl1.name() + "' does not match with ";
    msg += "its number of ignore keys in the DIFF file.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (ignorefilepaths.hasKeyword(pvl1.name()) &&
      ignorefilepaths[pvl1.name()].size() > 1 &&
      pvl1.size() != ignorefilepaths[pvl1.name()].size()) {
    QString msg = "Size of keyword '" + pvl1.name() + "' does not match with ";
    msg += "its number of filepath ignores in the DIFF file.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  for(int i = 0; i < pvl1.size() && filesMatch; i++) {
    QString val1 = pvl1[i];
    QString val2 = pvl2[i];

    QString unit1 = pvl1.unit(i);
    QString unit2 = pvl2.unit(i);

    int ignoreIndex = 0;
    if(ignorekeys.hasKeyword(pvl1.name()) && ignorekeys[pvl1.name()].size() > 1) {
      ignoreIndex = i;
    }

    // If we only have one value for the keyword, use it for all paths in the keyword's array,
    // otherwise use the corresponding IgnoreFilePaths value
    if (ignorefilepaths.hasKeyword(pvl1.name()) &&
      (((ignorefilepaths[pvl1.name()].size() > 1 && ignorefilepaths[pvl1.name()][i] == "true") ||
        (ignorefilepaths[pvl1.name()].size() == 1 && ignorefilepaths[pvl1.name()][0] == "true")))) {
          val1 =  val1.replace(QRegularExpression("(\\/[\\w\\-\\. ]*)+\\/"), "");
          val2 =  val2.replace(QRegularExpression("(\\/[\\w\\-\\. ]*)+\\/"), "");
    }


    try {
      if(!ignorekeys.hasKeyword(pvl1.name()) ||
          ignorekeys[pvl1.name()][ignoreIndex] == "false") {

        if(unit1.toLower() != unit2.toLower()) {
          filesMatch = false;
          differenceReason = "Keyword '" + pvl1.name() + "': units do not match.";
          return;
        }

        double tolerance = 0.0;
        double difference = abs(toDouble(val1) - toDouble(val2));

        if(tolerances.hasKeyword(pvl1.name())) {
          tolerance = toDouble(
              (tolerances[pvl1.name()].size() == 1) ?
                 tolerances[pvl1.name()][0] : tolerances[pvl1.name()][i]);
        }

        if(difference > tolerance) {
          filesMatch = false;
          if(pvl1.size() == 1) {
            differenceReason = "Keyword '" + pvl1.name() + "': difference is " +
                               toString(difference);
          }
          else {
            differenceReason = "Keyword '" + pvl1.name() + "' at index " +
                               toString(i) + ": difference is " + toString(difference);
          }
          differenceReason += " (tolerance is " + toString(tolerance) + ")";
        }
      }
    }
    catch(IException &) {
      if(val1.toLower() != val2.toLower()) {
        filesMatch = false;
        differenceReason = "Keyword '" + pvl1.name() + "': values do not "
            "match.";
      }
    }
  }
}

void CompareObjects(const PvlObject &pvl1, const PvlObject &pvl2) {
  if(pvl1.name().compare(pvl2.name()) != 0) {
    filesMatch = false;
    differenceReason = "Object " + pvl1.name() + " does not match " +
        pvl2.name();
  }

  if(pvl1.keywords() != pvl2.keywords()) {
    filesMatch = false;
    differenceReason = "Object " + pvl1.name() + " has varying keyword counts.";
  }

  if(pvl1.groups() != pvl2.groups()) {
    filesMatch = false;
    differenceReason = "Object " + pvl1.name() + " has varying group counts.";
  }

  if(pvl1.objects() != pvl2.objects()) {
    filesMatch = false;
    differenceReason = "Object " + pvl1.name() + " has varying object counts.";
  }

  if(!filesMatch) {
    return;
  }

  for(int keyword = 0; keyword < pvl1.keywords() && filesMatch; keyword++) {
    CompareKeywords(pvl1[keyword], pvl2[keyword]);
  }

  for(int object = 0; object < pvl1.objects() && filesMatch; object++) {
    CompareObjects(pvl1.object(object), pvl2.object(object));
  }

  for(int group = 0; group < pvl1.groups() && filesMatch; group++) {
    CompareGroups(pvl1.group(group), pvl2.group(group));
  }

  if(!filesMatch && pvl1.name().compare("Root") != 0) {
    differenceReason = "Object " + pvl1.name() + ": " + differenceReason;
  }
}

void CompareGroups(const PvlGroup &pvl1, const PvlGroup &pvl2) {
  if(pvl1.keywords() != pvl2.keywords()) {
    filesMatch = false;
    return;
  }

  for(int keyword = 0; keyword < pvl1.keywords() && filesMatch; keyword++) {
    CompareKeywords(pvl1[keyword], pvl2[keyword]);
  }

  if(!filesMatch) {
    differenceReason = "Group " + pvl1.name() + ": " + differenceReason;
  }
}
