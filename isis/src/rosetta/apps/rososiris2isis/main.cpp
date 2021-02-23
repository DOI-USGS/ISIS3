/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"

#include <QDebug>
#include <QFile>
#include <QString>

#include <cstdio>
#include <vector>
#include <cstdlib>

#include "FileName.h"
#include "IString.h"
#include "ProcessBySample.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void flipbyline(Buffer &in, Buffer &out);

void IsisMain() {
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");
  QString instId;
  QString missionId;

  try {
    Pvl lab(inFile.expanded());
    instId = (QString) lab.findKeyword("INSTRUMENT_ID");
    missionId = (QString) lab.findKeyword("MISSION_ID");
  }
  catch (IException &e) {
    QString msg = "Unable to read [INSTRUMENT_ID] or [MISSION_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(e, IException::Io, msg, _FILEINFO_);
  }

  instId = instId.simplified().trimmed();
  missionId = missionId.simplified().trimmed();
  if (missionId.compare("ROSETTA", Qt::CaseInsensitive) != 0
     && instId.compare("OSINAC", Qt::CaseInsensitive) != 0
     && instId.compare("OSIWAC", Qt::CaseInsensitive) != 0) {
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
                  "a Rosetta OSIRIS Wide Angle Camera (WAC) or Narrow Angle Camera (NAC) file.";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  p.SetPdsFile(inFile.expanded(), "", pdsLabel);
  p.SetOrganization(Isis::ProcessImport::BSQ);
  QString tmpName = "$TEMPORARY/" + inFile.baseName() + ".tmp.cub";
  FileName tmpFile(tmpName);
  CubeAttributeOutput outatt = CubeAttributeOutput("+Real");
  p.SetOutputCube(tmpFile.expanded(), outatt);
  p.SaveFileHeader();

  Pvl labelPvl(inFile.expanded());

  p.StartProcess();
  p.EndProcess();

  ProcessBySample p2;
  CubeAttributeInput inatt;
  p2.SetInputCube(tmpFile.expanded(), inatt);
  Cube *outcube = p2.SetOutputCube("TO");

  // Get the directory where the OSIRIS translation tables are.
  QString transDir = "$ISISROOT/appdata/translations/";

  // Create a PVL to store the translated labels in
  Pvl outLabel;

  // Translate the Archive group
  FileName transFile(transDir + "RosettaOsirisArchive.trn");
  PvlToPvlTranslationManager archiveXlater(labelPvl, transFile.expanded());
  archiveXlater.Auto(outLabel);

  // Translate the BandBin group
  transFile = transDir + "RosettaOsirisBandBin.trn";
  PvlToPvlTranslationManager bandBinXlater(labelPvl, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Instrument group
  transFile = transDir + "RosettaOsirisInstrument.trn";
  PvlToPvlTranslationManager instrumentXlater(labelPvl, transFile.expanded());
  instrumentXlater.Auto(outLabel);

  // Write the BandBin, Archive, and Instrument groups
  // to the output cube label
  outcube->putGroup(outLabel.findGroup("BandBin", Pvl::Traverse));
  outcube->putGroup(outLabel.findGroup("Archive", Pvl::Traverse));
  outcube->putGroup(outLabel.findGroup("Instrument", Pvl::Traverse));

  // Set the BandBin filter name, center, and width values based on the
  // FilterNumber.  Note OSIRIS has 2 filter wheels, so information
  // must be looked up and stored for both.
  PvlGroup &bbGrp(outLabel.findGroup("BandBin", Pvl::Traverse));
  PvlGroup groupWithFilterInfo=pdsLabel.findGroup("SR_MECHANISM_STATUS");
  QString combFilterName = groupWithFilterInfo["FILTER_NAME"];
  bbGrp.addKeyword(PvlKeyword("CombinedFilterName", combFilterName));
  bbGrp.addKeyword(PvlKeyword("FilterId", (QString)groupWithFilterInfo["FILTER_NUMBER"]));
  QStringList filterNames = combFilterName.split("_");
  vector<int> filterIds(2,0);
  vector<double> filterWidths(2,0.0);
  vector<double> filterCenters(2,0.0);

  // OSIRIS NAC and WAC have different filters
  for (int i = 0; i < filterNames.size(); i++) {
    // Translate the Instrument group
    try {
      transFile = transDir + "RosettaOsirisFilters.trn";
      PvlTranslationTable filterTable(transFile.expanded());
      filterCenters[i] = toDouble(filterTable.Translate("FilterCenter_" + instId,
                                                        filterNames[i]));
      filterWidths[i] = toDouble(filterTable.Translate("FilterWidth_" + instId,
                                                       filterNames[i]));
    }
    catch (IException &e) {
      QString msg = "Input file [" + inFile.expanded()
                    + "] appears invalid. "
                    + "FilterName ["
                    + filterNames[i]
                    + "] for instrument ["
                    + instId
                    + "] not found in ["
                    + transFile.expanded() + "].";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }
  }
  // bandBin += PvlKeyword("FilterId", toString(filterId));
  bbGrp.addKeyword(PvlKeyword("FilterOneName", filterNames[0]));
  bbGrp.addKeyword(PvlKeyword("FilterOneCenter", toString(filterCenters[0]), "nanometers"));
  bbGrp.addKeyword(PvlKeyword("FilterOneWidth", toString(filterWidths[0]), "nanometers"));
  bbGrp.addKeyword(PvlKeyword("FilterTwoName", filterNames[1]));
  bbGrp.addKeyword(PvlKeyword("FilterTwoCenter", toString(filterCenters[1]), "nanometers"));
  bbGrp.addKeyword(PvlKeyword("FilterTwoWidth", toString(filterWidths[1]), "nanometers"));
  outcube->putGroup(bbGrp);

  PvlGroup kerns("Kernels");
  if (instId.compare("OSINAC", Qt::CaseInsensitive) == 0) {
    kerns += PvlKeyword("NaifFrameCode", toString(-226111)); //should I add [-filtno] directly after the number?  That's what Dawn did
  }
  else if (instId.compare("OSIWAC", Qt::CaseInsensitive) == 0) {
    kerns += PvlKeyword("NaifFrameCode", toString(-226112));  //should I add [-filtno] directly after the number?  That's what Dawn did
  }
  else {
    QString msg = "Input file [" + inFile.expanded() + "] has an invalid " +
                 "InstrumentId.";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  outcube->putGroup(kerns);

  p2.StartProcess(flipbyline);
  p2.EndProcess();

  QString tmp(tmpFile.expanded());
  QFile::remove(tmp);
}

// Flip image by line
void flipbyline(Buffer &in, Buffer &out) {
  int index = in.size() - 1;
  for (int i = 0; i < in.size(); i++) {
    out[i] = in[index - i];
  }
}
