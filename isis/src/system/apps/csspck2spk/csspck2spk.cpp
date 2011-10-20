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
using namespace std;


void IsisMain() {
  /* Open the input file from the guior find
     the latest version of the db file */
  UserInterface &ui = Application::GetUserInterface();
  string inDBfile;
  if(ui.WasEntered("FROM")) {
    inDBfile = ui.GetFilename("FROM");
  }
  else {
    string exDBfile("$cassini/kernels/spk/kernels.????.db");
    Filename exDBfilenm(exDBfile);
    exDBfilenm.HighestVersion();      // stores highest version and returns a void
    inDBfile = exDBfilenm.Expanded();
  }

  Filename basepckPath("$base/kernels/pck/kernels.????.db");
  basepckPath.HighestVersion();

  // Read SPK db file into a PVL
  Pvl spkdb(inDBfile);
  Pvl basepck(basepckPath.Expanded());
  PvlKeyword basefile = basepck.FindObject("TargetAttitudeShape").Group(0)["File"];

  //Search PVL for main object
  PvlObject &mainob = spkdb.FindObject("SpacecraftPosition");

  /* Search for the Selection Groups, and the based on the File Keyword, add
     the appropriate pck file keyword.  First check to see if the input file has
     already been updated from an old version of the program. */
  QHash<QString, int> spkGroups;
  for(int grpIndex = 0; grpIndex < mainob.Groups(); grpIndex ++) {
    PvlGroup &grp = mainob.Group(grpIndex);
    if(grp.IsNamed("Selection")) {

      int count = 0;
      for(int keyIndex = 0; keyIndex < grp.Keywords(); keyIndex ++) {
        if(grp[keyIndex].IsNamed("File")) {
          count ++;

          // Older versions of this program added the file to the SPK kernels DB file instead
          //   of creating a new one in the PCK directory. Check for this.
          if(count > 1) {
            string msg = "This file has already been updated [";
            msg += iString(inDBfile) + "] by an old version of this program.";
            msg += " This is not a valid input.";
            throw iException::Message(iException::User, msg, _FILEINFO_);
          }
        }
      }

      string value = (string)grp["File"];
      Filename fnm(value);
      string basename = fnm.Basename();

      spkGroups.insert(QString::fromStdString(basename), grpIndex);
    }
  }

  /* Ingesting the pck2spk into mapped key/values; first grap latest file name,
     and then the individual lines for parsing into the map */
  Filename pckFilename;
  if(ui.WasEntered("PAIRING")) {
    pckFilename = ui.GetFilename("PAIRING");
  }
  else {
    string pckfile("$cassini/kernels/pck/pck2spk_????.map");
    pckFilename = pckfile;
    pckFilename.HighestVersion();
  }

  TextFile txt(pckFilename.Expanded());

  QList<QString> rawLines;
  iString rawLine;
  while(txt.GetLine(rawLine)) {
    rawLines.append(QString::fromStdString(rawLine));
  }

  PvlObject targetAttitudeShape("TargetAttitudeShape");

  PvlKeyword &runTime = mainob.FindKeyword("RunTime");
  targetAttitudeShape.AddKeyword(runTime);

  PvlGroup &dependencies = mainob.FindGroup("Dependencies");
  targetAttitudeShape.AddGroup(dependencies);

  for (int i = rawLines.size() - 1; i >= 0; i--) {
    iString line = rawLines[i];

    iString pck = line.Token(",");
    pck.Trim("\n\r\t\v\f ");        // stripping the extraneous characters
    iString spk = line;
    spk.Trim("\n\r\t\v\f ");

    if(!spkGroups.contains(spk)) {
      std::string msg = "Spk [" + spk + "] does not exist in [" + inDBfile + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    else {
      PvlGroup selection("Selection");

      PvlGroup &grp = mainob.Group(spkGroups[spk]);
      selection.AddKeyword(grp.FindKeyword("Time"));
      selection.AddKeyword(basefile);

      PvlKeyword newfile("File", "$cassini/kernels/pck/" + pck);
      selection.AddKeyword(newfile);

      targetAttitudeShape.AddGroup(selection);
    }
  }

  Pvl outPvl;
  outPvl.AddObject(targetAttitudeShape);

  // Create new db file with the updated contents of the Pvl
  Filename outDBfile;
  if(ui.WasEntered("TO")) {
    outDBfile = ui.GetFilename("TO");
  }
  else {
    outDBfile = "$cassini/kernels/pck/kernels.????.db";
    outDBfile.NewVersion();
  }

  outPvl.Write(outDBfile.Expanded());
}

