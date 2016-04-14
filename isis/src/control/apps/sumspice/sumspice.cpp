// $Id: sumspice.cpp 6565 2016-02-11 00:15:35Z kbecker@GS.DOI.NET $
#include "Isis.h"


#include <QDir>
#include <QFile>
#include <QScopedPointer>
#include <QtAlgorithms>
#include <QTextStream>

// boost library
#include <boost/foreach.hpp>

// The old original
#include "Application.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "FileList.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "IString.h"
#include "Kernels.h"
#include "NaifStatus.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "SumFile.h"
#include "Target.h"

using namespace std;
using namespace Isis;

void IsisMain() {

  //  Program constants
  const QString sumspice_program = "sumspice";
  const QString sumspice_version = "1.0";
  const QString sumspice_revision = "$Revision: 6565 $";
  const QString sumspice_runtime = Application::DateTime();
 
  UserInterface &ui = Application::GetUserInterface();

  // get the list of input cubes to be processed
  FileList cubeNameList;
  if ( ui.WasEntered("FROM") ) {
    cubeNameList.append(ui.GetFileName("FROM"));
  }
  else if ( ui.WasEntered("FROMLIST") ) {
    cubeNameList.read(ui.GetFileName("FROMLIST"));
  }
  else {
    QString message = "User must provide either an input cube file or an input cube file list.";
    throw IException(IException::User, message, _FILEINFO_);
  }

  // get the list of possible sum files to be applied
  FileList sumFileNameList;
  if ( ui.WasEntered("SUMFILE") )  {
    sumFileNameList.append(ui.GetFileName("SUMFILE"));
  }
  else if ( ui.WasEntered("SUMFILELIST") )  {
    sumFileNameList.read(ui.GetFileName("SUMFILELIST"));
  }
  else {
    QString message = "User must provide either a sum file or a sum file list.";
    throw IException(IException::User, message, _FILEINFO_);
  }


  // Load any meta kernels if provided by user
  Kernels meta;
  if ( ui.WasEntered("METAKERNEL") ) {
    QString metafile = ui.GetFileName("METAKERNEL");
    meta.Add(metafile);
    meta.Load();
  }

  // Load sumfiles
  SumFileList sumFiles = loadSumFiles(sumFileNameList);
  // Sort the sum file list in ascending order by ET
  qSort(sumFiles.begin(), sumFiles.end(), SortEtAscending());
  // check for uniqueness of sum files
  PvlGroup duplicates("SumFileWarnings");
  duplicates.addComment("First file will be used to update cube.");
  for (int sumIndex = 1; sumIndex < sumFiles.size(); sumIndex++) {
    if ( sumFiles[sumIndex]->et() == sumFiles[sumIndex-1]->et()) {
      PvlKeyword filePair("SumFilesWithDuplicateTimes", sumFiles[sumIndex-1]->name());
      filePair.addValue(sumFiles[sumIndex]->name());
      duplicates += filePair; 
    }
  }
  if (duplicates.keywords() != 0) {
    Application::Log(duplicates);
  }

  if (ui.GetString("MODE") == "UPDATETIMES") {
    PvlGroup warnings("UpdateStartClockWarnings");

    // loop through the input cubes
    Progress progress;
    progress.SetText("Updating Times in Cube List");
    progress.SetMaximumSteps(cubeNameList.size());
    double deltaTime = ui.GetDouble("TIMEDIFF");
    for (int cubeIndex = 0; cubeIndex < cubeNameList.size(); cubeIndex++) {
      progress.CheckStatus();

      Cube inputCube(cubeNameList[cubeIndex].expanded(), "rw");

      Kernels kernels(inputCube);
      kernels.Load();

      SharedSumFile sumFile = findSumFile(inputCube, sumFiles, deltaTime);
      // If findSumFile() returns a Null pointer, 
      // then no sum file within the given tolerance (deltaTime) was found.
      // Print a warning. 
      if ( sumFile.isNull() ) {// create cube file list containing file names that were updated.
        PvlKeyword file("CubeFileName", cubeNameList[cubeIndex].name());
        file.addComment("Clocks and times for this cube were NOT updated. "
                        "No sum file was found whose time was within a tolerance"
                        " of [" + Isis::toString(deltaTime, 10) + "] of the cube's start time.");
        warnings += file;

      }
      else {

        // If findSumFile() returned a non-null pointer, we will adjust the clock time
        // since findSumFile() has already verified that the new ET is within the given
        // tolerance (deltaTime) of the original ET
        //    1. copy original start/stop clock/time keyword values to Archive group
        //    2. replace these values in the instrument group with the spaceccraft clock time
        //       corresponding to the new time from the sum file.
        //    3. Clean Kernels group because output cube from this run must be re-spiceinited
        Pvl *cubeLabel = inputCube.label();
        PvlGroup &instGrp = cubeLabel->findGroup("Instrument", Pvl::Traverse);
        PvlKeyword origStartClock = instGrp["SpacecraftClockStartCount"];
        PvlKeyword origStopClock  = instGrp["SpacecraftClockStopCount"];
        PvlKeyword origStartTime  = instGrp["StartTime"];
        PvlKeyword origStopTime   = instGrp["StopTime"];

        PvlGroup &archGrp = cubeLabel->findGroup("Archive", Pvl::Traverse);
        // add the sum file name to the archive group
        PvlKeyword sumFileKeyword("SumFile", sumFile->name());
        sumFileKeyword.addComment("The sum file used to update the start count in the instrument group.");
        archGrp += sumFileKeyword;
        // add original time it to the archive group
        origStartClock.addComment("The original times, before sum file was applied.");
        archGrp += origStartClock;
        archGrp += origStopClock;
        archGrp += origStartTime;
        archGrp += origStopTime;

        // now that we have found the appropriate sum file and new ET, convert back to sclk ticks
        iTime newStartTime(sumFile->et());
        instGrp["StartTime"][0] = newStartTime.UTC();

        double exposureDuration = double(instGrp["ExposureDuration"]);
        QString units = instGrp["ExposureDuration"].unit();
        if (units.contains("m")) { //milliseconds, ms, millisecond, etc
          exposureDuration/=1000;
        }
        iTime newStopTime = newStartTime + exposureDuration;
        instGrp["StopTime"][0] = newStopTime.UTC();

        NaifStatus::CheckErrors();
        char newStartClock[80];
        sce2s_c(inputCube.camera()->naifSclkCode(), newStartTime.Et() , 80, newStartClock);
        NaifStatus::CheckErrors();
        instGrp["SpacecraftClockStartCount"][0] = newStartClock;

        char newStopClock[80];
        sce2s_c(inputCube.camera()->naifSclkCode(), newStopTime.Et() , 80, newStopClock);
        NaifStatus::CheckErrors();
        instGrp["SpacecraftClockStopCount"][0] = newStopClock;

        // force user to re-run spiceinit 
        // by removing everything from the Kernels Group except NAIF Frame code.
        PvlGroup kernels("Kernels");
        kernels += cubeLabel->findObject("IsisCube").findGroup("Kernels")["NaifFrameCode"];
        cubeLabel->findObject("IsisCube").deleteGroup("Kernels");
        cubeLabel->findObject("IsisCube") += kernels;
      }
    }
    if (warnings.keywords() != 0) {
      Application::Log(warnings);
    }
  }
  else { // ui.GetString("MODE") == "UPDATESPICETABLES"
    PvlGroup warnings("UpdateSpiceTablesWarnings");

    // time should already be adjusted, so time in the given sum file (list)
    // should have the exact time. However, due to rounding errors, we will
    // allow very small time tolerance.
    Progress progress;
    progress.SetText("Updating Tables in Cube List");
    progress.SetMaximumSteps(cubeNameList.size());
    for (int cubeIndex = 0; cubeIndex < cubeNameList.size(); cubeIndex++) {
      progress.CheckStatus();

      Cube inputCube(cubeNameList[cubeIndex].expanded(), "rw");

      PvlGroup &archGrp = inputCube.label()->findGroup("Archive", Pvl::Traverse);
      if (archGrp.hasKeyword("SumFile")) {
        PvlKeyword &sumFileKeyword = archGrp.findKeyword("SumFile");
        QString sumFileName = sumFileKeyword[0];
        SharedSumFile sumFile = findSumFile(inputCube, sumFiles, sumFileName);
        
        // If findSumFile() returns a Null pointer, 
        // then no sum file was found in the Archive group
        // Print a warning.
        if ( sumFile.isNull() ) {
          PvlKeyword file("CubeFileName", cubeNameList[cubeIndex].name());
          file.addComment("SPICE tables for this cube were NOT updated. "
                          "The sum file in the Archive group ["
                          + sumFileName + "] was not found in the given sum file list.");
          warnings += file;
        }
        else {
          // If findSumFile() returned a non-null pointer, we will adjust the ck and spk
          // tables in the cube's label
          Kernels kernels(inputCube);
          kernels.Load();
          QStringList missing = kernels.getMissingList();
          if (missing.size() != 0) {
            PvlKeyword file("CubeFileName", cubeNameList[cubeIndex].name());
            file.addComment("Missing kernel files. Have [" + Isis::toString(kernels.size())
                            + "] files loaded with " + Isis::toString(kernels.Missing()) 
                            + " missing (" + missing.join(",") + ").");
            warnings += file;
          }
          // update instrument pointing and position
          // Delete polygons if found in existing tables
          sumFile->update(inputCube);
          sumFileKeyword.addComment("The sum file used to update the SPICE tables.");
        }
      }
      else {
        PvlKeyword file("CubeFileName", cubeNameList[cubeIndex].name());
        file.addComment("SPICE tables for this cube were NOT updated. "
                        "No sum file was found in the Archive group.");
        warnings += file;
      }
    }
    if (warnings.keywords() != 0) {
      Application::Log(warnings);
    }
  }

  // Unload meta kernels - automatic, but done for completeness
  meta.UnLoad();
}
