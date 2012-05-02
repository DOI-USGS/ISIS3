#include "Isis.h"

#include <iostream>
#include <sstream>
#include <string>

#include <QFile>
#include <QHash>
#include <QList>
#include <QSet>
#include <QString>

#include "FileName.h"
#include "iTime.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"


using namespace Isis;
using std::string;


void updatePointing(PvlGroup &ckGroup,
    PvlObject &pivotPointing, PvlObject &atthistPointing);
PvlGroup* insertGroup(PvlObject &object, PvlGroup &group, int index);


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Convert between integer representations of months to abbreviated strings,
  // as used in the TDB time format of the kernel date ranges
  QHash<int, QString> MONTH_TO_STRING;
  MONTH_TO_STRING[1] = "JAN";
  MONTH_TO_STRING[2] = "FEB";
  MONTH_TO_STRING[3] = "MAR";
  MONTH_TO_STRING[4] = "APR";
  MONTH_TO_STRING[5] = "MAY";
  MONTH_TO_STRING[6] = "JUN";
  MONTH_TO_STRING[7] = "JUL";
  MONTH_TO_STRING[8] = "AUG";
  MONTH_TO_STRING[9] = "SEP";
  MONTH_TO_STRING[10] = "OCT";
  MONTH_TO_STRING[11] = "NOV";
  MONTH_TO_STRING[12] = "DEC";

  // Fetch the pivot file
  FileName pivotFileName;
  if (ui.WasEntered("PIVOT")) {
    pivotFileName = ui.GetFileName("PIVOT");
  }
  else {
    // If not provided, assume the latest pivot file in the data area
    string pivotString("$messenger/kernels/ck/pivot_kernels.????.db");
    pivotFileName = FileName(pivotString).highestVersion();
  }
  Pvl pivot(pivotFileName.expanded());

  // Fetch the atthist file
  FileName atthistFileName;
  if (ui.WasEntered("ATTHIST")) {
    atthistFileName = ui.GetFileName("ATTHIST");
  }
  else {
    // If not provided, assume the latest atthist file in the data area
    string atthistString("$messenger/kernels/ck/atthist_kernels.????.db");
    atthistFileName = FileName(atthistString).highestVersion();
  }
  Pvl atthist(atthistFileName.expanded());

  // Open the input file from the GUI or find the latest version of the DB file
  FileName dbFileName;
  if (ui.WasEntered("FROM")) {
    dbFileName = ui.GetFileName("FROM");
  }
  else {
    string dbString("$messenger/kernels/ck/kernels.????.db");
    dbFileName = FileName(dbString).highestVersion();
  }
  Pvl kernelDb(dbFileName.expanded());

  PvlObject &pointing = kernelDb.FindObject("SpacecraftPointing");
  PvlObject &pivotPointing = pivot.FindObject("SpacecraftPointing");
  PvlObject &atthistPointing = atthist.FindObject("SpacecraftPointing");

  PvlKeyword &runtime = pointing.FindKeyword("Runtime");
  runtime[0] = pivotPointing.FindKeyword("Runtime")[0];

  PvlKeyword &clock = pointing.FindKeyword(
      "SpacecraftClockKernel", Pvl::Traverse);
  clock[0] = pivotPointing.FindKeyword(
      "SpacecraftClockKernel", Pvl::Traverse)[0];

  PvlKeyword &leapsecond = pointing.FindKeyword(
      "LeapsecondKernel", Pvl::Traverse);
  leapsecond[0] = pivotPointing.FindKeyword(
      "LeapsecondKernel", Pvl::Traverse)[0];

  bool foundMapping = false;
  for (int i = 0; i < pointing.Groups(); i++) {
    PvlGroup &ckGroup = pointing.Group(i);

    if (ckGroup.IsNamed("Selection")) {

      // We've already found the mapping section, so just update the pivot and
      // atthist files for all remaining selection groups
      if (foundMapping)
        updatePointing(ckGroup, pivotPointing, atthistPointing);

      // We're looking for the group with a comment that says MAPPING,
      // signifying the beginning of the section we wish to update
      for (int j = 0; j < ckGroup.Comments(); j++) {
        QString comment = QString::fromStdString(ckGroup.Comment(j));
        if (comment.contains("MAPPING")) {
          foundMapping = true;
          updatePointing(ckGroup, pivotPointing, atthistPointing);

          PvlGroup &pivotSelection = pivotPointing.FindGroup("Selection");

          // Find end time, if it's a week past the current start date, or more,
          // then create a new selection group.  Otherwise, add new kernel
          // entries to the existing group.
          QString pivotEndRaw;
          for (int k = pivotSelection.Keywords() - 1; k >= 0; k--) {
            PvlKeyword &keyword = pivotSelection[k];
            if (keyword.IsNamed("Time")) {
              pivotEndRaw = QString::fromStdString(keyword[1]);
              break;
            }
          }

          // Remove the trailing " TDB" as it confuses the time conversion
          string newEnd = pivotEndRaw.toStdString();
          pivotEndRaw.remove(QRegExp(" TDB$"));
          string pivotEnd = pivotEndRaw.toStdString();

          PvlKeyword &time = ckGroup.FindKeyword("Time");
          QString currentStartRaw = QString::fromStdString(time[0]);
          currentStartRaw.remove(QRegExp(" TDB$"));
          string currentStart = currentStartRaw.toStdString();

          // Add 7 days (in units of seconds) to the current start time to
          // signify a week's time from the start time
          iTime weekFromStart(currentStart);
          weekFromStart += 7 * 24 * 3600 + 1;

          // See if a week has passed from the start time to the pivot end time
          iTime pivotEndTime(pivotEnd);
          time[1] = newEnd;

          PvlGroup *currentGroup = &ckGroup;

          // Add a second to adjust for midnight conversion
          iTime coveredTime(currentStart);
          coveredTime += 1;
          while (coveredTime <= pivotEndTime) {
            // Keep adding a new file for every day that doesn't have coverage
            // in the DB file, but is covered by the pivot and atthist files
            PvlObject::PvlKeywordIterator itr = currentGroup->Begin();
            itr++;

            // Until our covered time has exceeded a week past the current
            // group's start time, add new files to the current group
            while (coveredTime <= weekFromStart && coveredTime <= pivotEndTime) {
              // Construct the string used to identify the day's BC file
              string year = coveredTime.YearString();
              string month = coveredTime.MonthString();
              if (month.size() < 2) month = "0" + month;
              string day = coveredTime.DayString();
              if (day.size() < 2) day = "0" + day;

              string bcFileName = "$messenger/kernels/ck/";
              bcFileName += "msgr" + year + month + day + ".bc";

              // Check that the current day's BC file exists
              string bcExpanded = FileName(bcFileName).expanded();
              if (!QFile(QString::fromStdString(bcExpanded)).exists()) {
                string msg = "The BC file [" + bcExpanded + "] does not exist";
                throw IException(IException::User, msg, _FILEINFO_);
              }

              // If the current day's file isn't already present in the group,
              // then go ahead and add it
              if ((*itr)[0] != bcFileName) {
                PvlKeyword bcKeyword("File", bcFileName);
                itr = currentGroup->AddKeyword(bcKeyword, itr);
              }
              itr++;

              // Move forward a day's time (in seconds) so we can do the same
              // for the next day
              coveredTime += 24 * 3600;
            }

            if (coveredTime <= pivotEndTime) {
              // Set the end of the previous range and the beginning of the new
              // range to a week past the previous beginning
              iString newEndTime = weekFromStart.YearString() + " ";
              newEndTime += MONTH_TO_STRING[weekFromStart.Month()].toStdString() + " ";
              newEndTime += weekFromStart.DayString() + " ";
              newEndTime += "00:00:00.000 TDB";

              // Add another week's time
              coveredTime = weekFromStart;
              weekFromStart += 7 * 24 * 3600;

              PvlKeyword &currentTime = currentGroup->FindKeyword("Time");
              currentTime[1] = newEndTime;
              PvlKeyword latestTime(currentTime);
              latestTime[0] = newEndTime;
              latestTime[1] = newEnd;

              PvlGroup *latestGroup = new PvlGroup("Selection");
              latestGroup->AddKeyword(latestTime);

              PvlKeyword atthistPlaceholder("File");
              atthistPlaceholder += "";
              PvlKeyword pivotPlaceholder("File");
              pivotPlaceholder.AddComment("Regular pivot angle CK");
              pivotPlaceholder += "";

              latestGroup->AddKeyword(atthistPlaceholder);
              latestGroup->AddKeyword(pivotPlaceholder);

              latestGroup->AddKeyword(currentGroup->FindKeyword("Type"));

              updatePointing(*latestGroup, pivotPointing, atthistPointing);
              currentGroup = insertGroup(pointing, *latestGroup, i);
            }
          }
        }
      }
    }
  }

  // Get the output filename, either user-specified or the latest version for
  // the kernels area (as run by makedb)
  FileName outDBfile;
  if (ui.WasEntered("TO")) {
    outDBfile = ui.GetFileName("TO");
  }
  else {
    outDBfile = FileName("$messenger/kernels/ck/kernels.????.db").newVersion();
  }

  // Write the updated PVL as the new CK DB file
  kernelDb.Write(outDBfile.expanded());
}


void updatePointing(PvlGroup &ckGroup,
    PvlObject &pivotPointing, PvlObject &atthistPointing) {

  bool foundPivot = false;
  for (int k = ckGroup.Keywords() - 1; k >= 0; k--) {
    PvlKeyword &keyword = ckGroup[k];
    if (keyword.IsNamed("File")) {
      if (!foundPivot) {
        // Last file in the list is the pivot file
        PvlGroup &pivotSelection = pivotPointing.FindGroup("Selection");
        keyword[0] = pivotSelection.FindKeyword("File")[0];
        foundPivot = true;
      }
      else {
        // Atthist file comes just before the pivot file in the
        // MAPPING group
        PvlGroup &atthistSelection = atthistPointing.FindGroup("Selection");
        keyword[0] = atthistSelection.FindKeyword("File")[0];
        break;
      }
    }
  }
}


PvlGroup* insertGroup(PvlObject &object, PvlGroup &group, int index) {
  // Keep track of the comments signifying the beginning of the mapping section,
  // as it will need to be moved to the most recent date range
  QList<QString> mappingComments;

  // Add a copy of the last group to the end so we can begin shifting all our
  // mapping selection groups down
  object.AddGroup(object.Group(object.Groups() - 1));
  for (int i = object.Groups() - 2; i > index; i--) {
    // Shift groups down until we reach the new beginning of the mapping section
    object.Group(i) = object.Group(i - 1);

    // See if we've found the mapping comments yet
    if (mappingComments.size() == 0) {
      // If not, let's get the comments from the current group and check them
      // against our criteria for the mapping comments
      PvlGroup &currentGroup = object.Group(i);
      bool foundMapping = false;
      for (int j = 0; j < currentGroup.Comments(); j++) {
        QString comment = QString::fromStdString(currentGroup.Comment(j));
        mappingComments.append(comment);

        if (comment.contains("MAPPING"))
          foundMapping = true;
      }

      if (mappingComments.size() > 0 && !foundMapping) {
        // Still haven't found the mapping comments, so clear away whatever
        // comments we did find
        mappingComments.clear();
      }
      else {
        // We found the mapping comments, so now that we've extracted them we
        // can remove them from the former latest selection group
        currentGroup.GetNameKeyword().ClearComments();
      }
    }
  }

  // Add the new group
  object.Group(index) = group;

  // Add all the mapping comments
  PvlGroup &currentGroup = object.Group(index);
  for (int i = 0; i < mappingComments.size(); i++)
    currentGroup.AddComment(mappingComments[i].toStdString());

  // Return the location of the new group
  return &currentGroup;
}

