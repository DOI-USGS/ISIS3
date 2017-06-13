#include "Isis.h"

#include <cstdio>

#include <QFile>
#include <QString>
#include <vector>
#include "stdlib.h"

#include "ProcessImportPds.h"
#include "ProcessBySample.h"
#include "UserInterface.h"
#include "FileName.h"
#include "IString.h"

using namespace std;
using namespace Isis;

void flipbyline(Buffer &in, Buffer &out);

void IsisMain() {
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");
  QString instid;
  QString missid;

  try {
    Pvl lab(inFile.expanded());
    instid = (QString) lab.findKeyword("INSTRUMENT_ID");
    missid = (QString) lab.findKeyword("MISSION_ID");
  }
  catch(IException &e) {
    QString msg = "Unable to read [INSTRUMENT_ID] or [MISSION_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  instid = instid.simplified().trimmed();
  missid = missid.simplified().trimmed();
  if(missid != "ROSETTA" && instid != "OSINAC" && instid != "OSIWAC") {
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
                 "an OSIRIS Wide Angle Camera (WAC) or Narrow Angle Camera (NAC) file.";
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
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["rosetta"] + "/translations/";

  // Create a PVL to store the translated labels in
  Pvl outLabel;

  // Translate the Archive group
  FileName transFile(transDir + "osirisArchive.trn");
  PvlToPvlTranslationManager archiveXlater(labelPvl, transFile.expanded());
  archiveXlater.Auto(outLabel);

  // Translate the BandBin group
  transFile = transDir + "osirisBandBin.trn";
  PvlToPvlTranslationManager bandBinXlater(labelPvl, transFile.expanded());
  bandBinXlater.Auto(outLabel);

  // Translate the Instrument group
  transFile = transDir + "osirisInstrument.trn";
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
  QStringList filterNames=combFilterName.split("_");
  vector<int> filterIds (2,0);
  vector<double> filterWidths (2,0.0);
  vector<double> filterCenters (2,0.0);

  // OSIRIS NAC and WAC have different filters 
  if (instid == "OSINAC") {
    for (int i = 0; i < filterNames.size(); i++) {
      if(filterNames[i] == "FFP-UV") {
        //filterIds[i] = 1;
        filterCenters[i] = 600;
        filterWidths[i] = 600;
      }
      else if(filterNames[i] == "FFP-IR") {
        //filterIds[i] = 2;
        filterCenters[i] = 600;
        filterWidths[i] = 600;
      }
      else if(filterNames[i] == "Neutral") {
        //filterIds[i] = 3;
        filterCenters[i] = 640;
        filterWidths[i] = 520;
      }
      else if(filterNames[i] == "NFP-Vis") {
        //filterIds[i] = 4;
        filterCenters[i] = 600;
        filterWidths[i] = 600;
      }
      else if(filterNames[i] == "Far-UV") {
        //filterIds[i] = 5;
        filterCenters[i] = 269.3;
        filterWidths[i] = 53.6;
      }
      else if(filterNames[i] == "Near-UV") {
        //filterIds[i] = 6;
        filterCenters[i] = 360.0;
        filterWidths[i] = 51.1;
      }
      else if(filterNames[i] == "Blue") {
        //filterIds[i] = 1;
        filterCenters[i] = 480.7;
        filterWidths[i] = 74.9;
      }
      else if(filterNames[i] == "Green") {
        //filterIds[i] = 2;
        filterCenters[i] = 535.7;
        filterWidths[i] = 62.4;
      }
      else if(filterNames[i] == "FFP-Vis") {
        //filterIds[i] = 3;
        filterCenters[i] = 600;
        filterWidths[i] = 600;
      }
      else if(filterNames[i] == "Orange") {
        //filterIds[i] = 4;
        filterCenters[i] = 649.2;
        filterWidths[i] = 84.5;
      }
      else if(filterNames[i] == "Hydra") {
        //filterIds[i] = 5;
        filterCenters[i] = 701.2;
        filterWidths[i] = 22.1;
      }
      else if(filterNames[i] == "Red") {
        //filterIds[i] = 6;
        filterCenters[i] = 743.7;
        filterWidths[i] = 64.1;
      }
      else if(filterNames[i] == "Ortho") {
        //filterIds[i] = 1;
        filterCenters[i] = 805.3;
        filterWidths[i] = 40.5;
      }
      else if(filterNames[i] == "Near-IR") {
        //filterIds[i] = 2;
        filterCenters[i] = 882.1;
        filterWidths[i] = 65.9;
      }
      else if(filterNames[i] == "Fe203")   {
        //filterIds[i] = 3;
        filterCenters[i] = 931.9;
        filterWidths[i] = 34.9;
      }
      else if(filterNames[i] == "IR") {
        //filterIds[i] = 4;
        filterCenters[i] = 989.3;
        filterWidths[i] = 38.2;
      }
      else {
        QString msg = "Input file [" + inFile.expanded() + "] appears to have an invalid " +
                      "FilterName.";
        throw IException(IException::Io, msg, _FILEINFO_);
      }  
    }
  } 
  else if (instid == "OSIWAC") {
    for (int i = 0; i < filterNames.size(); i++) {
      if(filterNames[i] == "Empty") {
        filterCenters[i] = 0; 
        filterWidths[i] = 0;
      }
      else if(filterNames[i] == "UV245") {
        filterCenters[i] = 246.2;
        filterWidths[i] = 14.1;
      }
      else if(filterNames[i] == "CS") {
        filterCenters[i] = 259.0;
        filterWidths[i] = 5.6;
      }
      else if(filterNames[i] == "UV295") {
        filterCenters[i] = 295.9;
        filterWidths[i] = 10.9;
      }
      else if(filterNames[i] == "OH-WAC") {
        filterCenters[i] = 309.7;
        filterWidths[i] = 4.1;
      }
      else if(filterNames[i] == "UV325") {
        filterCenters[i] = 325.8;
        filterWidths[i] = 10.7;
      }
      else if(filterNames[i] == "NH") {
        filterCenters[i] = 335.9;
        filterWidths[i] = 4.1;
      }
      else if(filterNames[i] == "UV375") {
        filterCenters[i] = 375.6;
        filterWidths[i] = 9.8; 
      }
      else if(filterNames[i] == "CN") {
        filterCenters[i] = 388.4;
        filterWidths[i] = 5.2;
      }
      else if(filterNames[i] == "Green") {
        filterCenters[i] = 537.2;
        filterWidths[i] = 63.2;
      }
      else if(filterNames[i] == "NH2") {
        filterCenters[i] = 572.1;
        filterWidths[i] = 11.5;
      }
      else if(filterNames[i] == "Na") {
        filterCenters[i] = 590.7;
        filterWidths[i] = 4.7;
      }
      else if(filterNames[i] == "VIS610") {
        filterCenters[i] = 612.6;
        filterWidths[i] = 9.8;
      }
      else if(filterNames[i] == "OI") {
        filterCenters[i] = 631.6;
        filterWidths[i] = 4.0;
      }
      else if(filterNames[i] == "Red") {
        filterCenters[i] = 629.8;
        filterWidths[i] = 156.8;
      }
      else {
        QString msg = "Input file [" + inFile.expanded() + "] appears to have an invalid " +
                      "FilterName.";
        throw IException(IException::Io, msg, _FILEINFO_);
      }  
    }
  } 
  else { 
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
                  "an OSIRIS Wide Angle Camera (WAC) or Narrow Angle Camera (NAC) file.";
    throw IException(IException::Io, msg, _FILEINFO_);
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
  if(instid == "OSINAC") {
    kerns += PvlKeyword("NaifFrameCode", toString(-226111)); //should I add [-filtno] directly after the number?  That's what Dawn did
  }
  else if(instid == "OSIWAC") {
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
  for(int i = 0; i < in.size(); i++) {
    out[i] = in[index - i];
  }
}
