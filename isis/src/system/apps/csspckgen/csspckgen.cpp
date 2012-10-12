#include "Isis.h"

#include "FileName.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iTime.h"

using namespace Isis;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Open the input file from the GUI or find the latest version of the DB file
  FileName inputName;
  if (ui.WasEntered("FROM")) {
    inputName = ui.GetFileName("FROM");
  }
  else {
    // Stores highest version
    inputName = "$cassini/kernels/pck/kernels.????.db";
    inputName = inputName.highestVersion();
  }

  // Read PCK DB file into a PVL and search the PVL for the main object
  Pvl db(inputName.expanded());
  PvlObject &main = db.FindObject("TargetAttitudeShape");

  // Add a timestamp for when this file was created
  PvlObject latestMain("TargetAttitudeShape");
  latestMain += PvlKeyword("RunTime", iTime::CurrentLocalTime());

  // Add our dependencies, only the Leapsecond Kernel
  PvlGroup dependencies("Dependencies");
  FileName lskName("$base/kernels/lsk/naif????.tls");
  lskName = lskName.highestVersion();
  IString lskString = lskName.originalPath() + "/" + lskName.name();
  dependencies += PvlKeyword("LeapsecondKernel", lskString);
  latestMain += dependencies;

  for (int g = 0; g < main.Groups(); g++) {
    PvlGroup &group = main.Group(g);

    // Look for Selection groups with date-versioned filenames
    if (group.IsNamed("Selection")) {
      // Copy the Selection group in case we need to replace one or more
      // date-versioned filenames
      PvlGroup latestGroup = group;

      bool hasDateVersioning = false;
      for (int k = 0; k < group.Keywords(); k++) {
        PvlKeyword &keyword = group[k];
        if (keyword.IsNamed("File")) {
          FileName pckName(keyword[0]);
          if (pckName.isDateVersioned()) {
            pckName = pckName.highestVersion();
            IString latestPck = pckName.originalPath() + "/" + pckName.name();

            // Replace the date-versioned filename with the direct path to the
            // latest PCK
            PvlKeyword &latestKeyword = latestGroup[k];
            latestKeyword[0] = latestPck;

            hasDateVersioning = true;
          }
        }
      }

      // When we find date versioning, we need to add the a group without it to
      // the PVL
      if (hasDateVersioning) {
        // Add back the date-versioned path for new versions of Isis
        latestMain.AddGroup(group);

        // Add comment specifying that this PCK is hardcoded for legacy support
        IString comment = "This PCK is hardcoded to support versions of "
          "Isis prior to v3.3.2";
        latestGroup.AddComment(comment);

        // Add the direct path to the DB file to support older versions of Isis
        // that do not support date-versioned filenames
        latestMain.AddGroup(latestGroup);
      }
    }
  }

  // Determine whether we want to update the data area directly or write to a
  // user-specified location
  FileName outputName;
  if (ui.WasEntered("TO")) {
    outputName = ui.GetFileName("TO");
  }
  else {
    outputName = "$cassini/kernels/pck/kernels.????.db";
    outputName = outputName.newVersion();
  }

  // Write the updated PVL as the new PCK DB file
  Pvl latestDb;
  latestDb.AddObject(latestMain);
  latestDb.Write(outputName.expanded());
}

