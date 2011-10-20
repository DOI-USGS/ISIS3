#include "Isis.h"

#include <iostream>
#include <sstream>
#include <string>

#include <QHash>
#include <QList>
#include <QString>

#include "Pvl.h"
#include "PvlGroup.h"
#include "Filename.h"
#include "TextFile.h"

using namespace Isis;
using std::string;


void IsisMain() {
  // Open the input file from the GUI or find the latest version of the DB file
  UserInterface &ui = Application::GetUserInterface();

  string inDBfile;
  if (ui.WasEntered("FROM")) {
    inDBfile = ui.GetFilename("FROM");
  }
  else {
    // Stores highest version
    string exDBfile("$cassini/kernels/spk/kernels.????.db");
    Filename exDBfilenm(exDBfile);
    exDBfilenm.HighestVersion();
    inDBfile = exDBfilenm.Expanded();
  }

  Filename basepckPath("$base/kernels/pck/kernels.????.db");
  basepckPath.HighestVersion();

  // Read SPK DB file into a PVL
  Pvl spkdb(inDBfile);
  Pvl basepck(basepckPath.Expanded());
  PvlKeyword basefile = basepck.FindObject("TargetAttitudeShape").Group(0)["File"];

  //Search PVL for main object
  PvlObject &mainob = spkdb.FindObject("SpacecraftPosition");

  // Search for the Selection Groups, and the based on the File Keyword, add the
  // appropriate pck file keyword.  First check to see if the input file has
  // already been updated from an old version of the program.
  QHash<QString, int> spkGroups;
  for (int grpIndex = 0; grpIndex < mainob.Groups(); grpIndex++) {
    PvlGroup &grp = mainob.Group(grpIndex);

    if (grp.IsNamed("Selection")) {
      int count = 0;
      for(int keyIndex = 0; keyIndex < grp.Keywords(); keyIndex++) {
        if (grp[keyIndex].IsNamed("File")) {
          count++;

          // Older versions of this program added the file to the SPK kernels DB
          // file instead of creating a new one in the PCK directory. Check for
          // this.
          if (count > 1) {
            string msg = "This file has already been updated [";
            msg += iString(inDBfile) + "] by an old version of this program.";
            msg += " This is not a valid input.";
            throw iException::Message(iException::User, msg, _FILEINFO_);
          }
        }
      }

      // Get the basename of the SPK file so it references 1:1 with an entry in
      // the pairing file
      string value = (string)grp["File"];
      Filename fnm(value);
      string basename = fnm.Basename();

      // Add an entry in our hash for the current SPK filename for quick lookup
      // of the group later
      spkGroups.insert(QString::fromStdString(basename), grpIndex);
    }
  }

  // Fetch the pairing file
  Filename pckFilename;
  if (ui.WasEntered("PAIRING")) {
    pckFilename = ui.GetFilename("PAIRING");
  }
  else {
    // If not provided, assume the latest pairing file in the data area
    string pckfile("$cassini/kernels/pck/pck2spk_????.map");
    pckFilename = pckfile;
    pckFilename.HighestVersion();
  }

  TextFile txt(pckFilename.Expanded());

  // Store the file into a list so we can iterate over it in reverse order
  QList<QString> rawLines;
  iString rawLine;
  while (txt.GetLine(rawLine)) {
    rawLines.append(QString::fromStdString(rawLine));
  }

  // Begin building up our output PVL
  PvlObject targetAttitudeShape("TargetAttitudeShape");

  PvlKeyword &runTime = mainob.FindKeyword("RunTime");
  targetAttitudeShape.AddKeyword(runTime);

  PvlGroup &dependencies = mainob.FindGroup("Dependencies");
  targetAttitudeShape.AddGroup(dependencies);

  // Loop over the pairing file in reverse so the output PCK DB file will be
  // ordered from oldest date to most recent (the pairing file is the opposite)
  for (int i = rawLines.size() - 1; i >= 0; i--) {
    iString line = rawLines[i];

    // Split the line around the command and strip the extraneous characters
    string unwantedChars = "\n\r\t\v\f ";
    iString pck = line.Token(",");
    pck.Trim(unwantedChars);
    iString spk = line;
    spk.Trim(unwantedChars);

    if (!spkGroups.contains(spk)) {
      // Every pair in the pairing file must have a corresponding SPK in the DB
      // file
      string msg = "Spk [" + spk + "] does not exist in [" + inDBfile + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    else {
      // Create the PCK Selection group from data in the mapped SPK
      PvlGroup selection("Selection");

      PvlGroup &grp = mainob.Group(spkGroups[spk]);
      selection.AddKeyword(grp.FindKeyword("Time"));
      selection.AddKeyword(basefile);

      PvlKeyword newfile("File", "$cassini/kernels/pck/" + pck);
      selection.AddKeyword(newfile);

      targetAttitudeShape.AddGroup(selection);
    }
  }

  // Make a new PVL file so we can write out all the PCK DB data
  Pvl outPvl;
  outPvl.AddObject(targetAttitudeShape);

  // Create new DB file with the updated contents of the PVL
  Filename outDBfile;
  if (ui.WasEntered("TO")) {
    outDBfile = ui.GetFilename("TO");
  }
  else {
    outDBfile = "$cassini/kernels/pck/kernels.????.db";
    outDBfile.NewVersion();
  }

  outPvl.Write(outDBfile.Expanded());
}

