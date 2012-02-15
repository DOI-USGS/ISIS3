#include "Isis.h"

#include <iostream>
#include <sstream>
#include <string>

#include <QList>
#include <QMap>
#include <QSet>
#include <QString>

#include "Filename.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TextFile.h"

using namespace Isis;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Fetch the DEM DB file
  Filename dbFilename;
  if (ui.WasEntered("FROM")) {
    dbFilename = ui.GetFilename("FROM");
  }
  else {
    // If not provided, assume the latest DB file in the data area
    string demFile("$base/dems/kernels.????.db");
    dbFilename = demFile;
    dbFilename.HighestVersion();
  }

  Pvl dems(dbFilename.Expanded());
  PvlObject &demObject = dems.FindObject("Dem");

  QMap< QString, QList<QString> > demMap;
  for (int i = 0; i < demObject.Groups(); i++) {
    PvlGroup &group = demObject.Group(i);

    if (group.IsNamed("Selection")) {
      PvlKeyword &match = group.FindKeyword("Match");
      PvlKeyword &file = group.FindKeyword("File");

      // The third element in the Match keyword describes the DEM target (e.g.
      // Mars)
      QString target = QString::fromStdString(match[2]);

      // The first element of the File keyword gives the "mission" associated
      // with the keyword (currently, always "base").  The second element gives
      // the path from "base" to the actual DEM cube.
      string mission = file[0];
      string area = "$" + file[0];
      string pattern = file[1];

      // Some DEMs are hardcoded, but others are versioned.  If we see any "?"
      // symbols in the filename, treat them as version placeholders to get the
      // latest DEM.
      Filename demFilename(area + "/" + pattern);
      if (demFilename.Expanded().find('?') != string::npos)
        demFilename.HighestVersion();

      // Find the corresponding Isis Preference if one exists
      if (Preference::Preferences().HasGroup("DataDirectory")) {
        PvlGroup &dataDir =
          Preference::Preferences().FindGroup("DataDirectory");

        // If the mission name maps to a data area in the Isis Preferences file,
        // then replace the variable $MISSION with the path to that area
        if (dataDir.HasKeyword(mission)) {
          area = (string) dataDir[mission];
        }
      }

      // Construct the relative path with environment variable placeholders in
      // tact for outputting to XML
      string filePath = area + "/dems/" + demFilename.Name();

      // Add this filename to the list of DEMs corresponding to its target.  If
      // that list doesn't already exist, create it first.
      if (!demMap.contains(target)) demMap.insert(target, QList<QString>());
      demMap[target].append(QString::fromStdString(filePath));
    }
  }

  // Prepare to write out the output XML.
  Filename outFile = ui.GetFilename("TO");
  ofstream os;
  os.open(outFile.Expanded().c_str(), ios::out);

  // Write the installation XML
  os << "<packs>";
  QList<QString> keys = demMap.keys();
  for (int i = 0; i < keys.size(); i++) {
    os << "<pack name=\"" << keys[i].toStdString() << "\">";
    QList<QString> fileList = demMap[keys[i]];
    for (int j = 0; j < fileList.size(); j++) {
      QString izpackFile = fileList[j].replace("ISIS3DATA", "{ENV[ISIS3DATA]}");
      os << "<file src=\"" << izpackFile.toStdString() << "\" />";
    }
    os << "</pack>";
  }
  os << "</packs>" << endl;
}

