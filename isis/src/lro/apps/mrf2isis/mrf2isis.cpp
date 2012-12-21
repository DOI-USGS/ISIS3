#include "Isis.h"

#include <cstdio>
#include <QString>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "FileName.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  QString labelFile = ui.GetFileName("FROM");
  FileName inFile = ui.GetFileName("FROM");
  QString id;
  Pvl lab(inFile.expanded());

  try {
    id = (QString) lab.FindKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    QString msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  id = id.simplified().trimmed();
  if(id != "CHAN1-L-MRFFR-5-CDR-MAP-V1.0" && id != "CHAN1-L-MRFFR-4-CDR-V1.0" &&
      id != "CH1-ORB-L-MRFFR-4-CDR-V1.0" && id != "CH1-ORB-L-MRFFR-5-CDR-MAP-V1.0" &&
      id != "CH1-ORB-L-MRFFR-5-CDR-MOSAIC-V1.0" &&
      id != "LRO-L-MRFLRO-3-CDR-V1.0" && id != "LRO-L-MRFLRO-5-CDR-MAP-V1.0" &&
      id != "LRO-L-MRFLRO-4-CDR-V1.0" && id != "LRO-L-MRFLRO-5-CDR-MOSAIC-V1.0") {
    QString msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
                 "in CHANDRAYAAN-1 MINI-RF FORERUNNER level 1 or level 2 format " +
                 "or in LUNAR RECONNAISSANCE ORBITER MINI-RF LRO level 1 or " +
                 "level 2 format. " +
                 "DATA_SET_ID is [" + id + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  p.SetPdsFile(labelFile, "", label);
  Cube *outcube = p.SetOutputCube("TO");

  QString bandorder;
  bandorder = (QString) lab.FindObject("IMAGE").FindKeyword("BAND_STORAGE_TYPE");
  bandorder = bandorder.toUpper();
  if(bandorder == "BAND_SEQUENTIAL") {
    p.SetOrganization(Isis::ProcessImport::BSQ);
  }
  else if(bandorder == "SAMPLE_INTERLEAVED") {
    p.SetOrganization(Isis::ProcessImport::BIP);
  }
  else if(bandorder == "LINE_INTERLEAVED") {
    p.SetOrganization(Isis::ProcessImport::BIL);
  }
  else {
    QString msg = "Input file [" + inFile.expanded() + "] has an invalid " +
                 "band storage type. BAND_STORAGE_TYPE is [" + bandorder + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  p.StartProcess();

  // Get the mapping labels
  Pvl otherLabels;
  p.TranslatePdsProjection(otherLabels);

  // Get the directory where the MiniRF level 2 translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  QString transDir = (QString) dataDir["Lro"] + "/translations/";

  if(id == "CHAN1-L-MRFFR-5-CDR-MAP-V1.0" || id == "LRO-L-MRFLRO-5-CDR-MAP-V1.0") {
    // Translate the BandBin group
    FileName transFile(transDir + "mrflev2BandBin.trn");
    PvlTranslationManager bandBinXlater(label, transFile.expanded());
    bandBinXlater.Auto(otherLabels);

    // Translate the Archive group
    transFile = transDir + "mrflev2Archive.trn";
    PvlTranslationManager archiveXlater(label, transFile.expanded());
    archiveXlater.Auto(otherLabels);

    // Write the BandBin, Archive, and Mapping groups to the output cube label
    outcube->putGroup(otherLabels.FindGroup("BandBin"));
    outcube->putGroup(otherLabels.FindGroup("Mapping"));
    outcube->putGroup(otherLabels.FindGroup("Archive"));
  }
  else {
    // Translate the BandBin group
    FileName transFile(transDir + "mrflev1BandBin.trn");
    PvlTranslationManager bandBinXlater(label, transFile.expanded());
    bandBinXlater.Auto(otherLabels);

    // Translate the Archive group
    transFile = transDir + "mrflev1Archive.trn";
    PvlTranslationManager archiveXlater(label, transFile.expanded());
    archiveXlater.Auto(otherLabels);

    // Translate the Instrument group
    transFile = transDir + "mrflev1Instrument.trn";
    PvlTranslationManager instrumentXlater(label, transFile.expanded());
    instrumentXlater.Auto(otherLabels);

    // Translate the Image group
    transFile = transDir + "mrflev1Image.trn";
    PvlTranslationManager imageXlater(label, transFile.expanded());
    imageXlater.Auto(otherLabels);

    // Write the BandBin, Archive, Instrument, and ImageInfo groups
    // to the output cube label
    outcube->putGroup(otherLabels.FindGroup("BandBin"));
    outcube->putGroup(otherLabels.FindGroup("Archive"));
    outcube->putGroup(otherLabels.FindGroup("Instrument"));
    outcube->putGroup(otherLabels.FindGroup("ImageInfo"));

    // Make sure the ScaledPixelHeight and ScaledPixelWidth are the same
    PvlGroup &instGrp(otherLabels.FindGroup("Instrument", Pvl::Traverse));
    if(instGrp.HasKeyword("ScaledPixelHeight") &&
        instGrp.HasKeyword("ScaledPixelWidth")) {
      double pheight = instGrp["ScaledPixelHeight"];
      double pwidth = instGrp["ScaledPixelWidth"];
      if(pheight != pwidth) {
        QString msg = "Input file [" + inFile.expanded() + "] does not have valid " +
                     "ScaledPixelHeight and ScaledPixelWidth values. These values " +
                     "must be equivalent or the image is considered to be invalid.";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }

    // Set the frequency based on the InstrumentModeId. This has to
    // be done manually, because the frequency information was not
    // put in the PDS labels.
    if(!(instGrp.HasKeyword("Frequency"))) {
      QString instmodeid = instGrp["InstrumentModeId"];
      double frequency;
      if(instmodeid.startsWith("BASELINE_S") ||
          instmodeid.startsWith("ZOOM_S")) {
        frequency = 2379305000.0;
      }
      else {   // BASELINE_X or ZOOM_X
        frequency = 7140000000.0;
      }
      instGrp.AddKeyword(PvlKeyword("Frequency", toString(frequency)));
      outcube->putGroup(instGrp);
    }
    PvlGroup kerns("Kernels");
    if(id.startsWith("CHAN1") || id.startsWith("CH1")) {
      kerns += PvlKeyword("NaifFrameCode", toString(-86001));
    }
    else {   // LRO
      kerns += PvlKeyword("NaifFrameCode", toString(-85700));
    }
    outcube->putGroup(kerns);
  }

  p.EndProcess();
}
