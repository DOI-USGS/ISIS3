#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"

#include "UserInterface.h"
#include "FileName.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl label;
  UserInterface &ui = Application::GetUserInterface();

  string labelFile = ui.GetFileName("FROM");
  FileName inFile = ui.GetFileName("FROM");
  iString id;
  Pvl lab(inFile.expanded());

  try {
    id = (string) lab.FindKeyword("DATA_SET_ID");
  }
  catch(IException &e) {
    string msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  id.ConvertWhiteSpace();
  id.Compress();
  id.Trim(" ");
  if(id != "CHAN1-L-MRFFR-5-CDR-MAP-V1.0" && id != "CHAN1-L-MRFFR-4-CDR-V1.0" &&
      id != "CH1-ORB-L-MRFFR-4-CDR-V1.0" && id != "CH1-ORB-L-MRFFR-5-CDR-MAP-V1.0" &&
      id != "CH1-ORB-L-MRFFR-5-CDR-MOSAIC-V1.0" &&
      id != "LRO-L-MRFLRO-3-CDR-V1.0" && id != "LRO-L-MRFLRO-5-CDR-MAP-V1.0" &&
      id != "LRO-L-MRFLRO-4-CDR-V1.0" && id != "LRO-L-MRFLRO-5-CDR-MOSAIC-V1.0") {
    string msg = "Input file [" + inFile.expanded() + "] does not appear to be " +
                 "in CHANDRAYAAN-1 MINI-RF FORERUNNER level 1 or level 2 format " +
                 "or in LUNAR RECONNAISSANCE ORBITER MINI-RF LRO level 1 or " +
                 "level 2 format. " +
                 "DATA_SET_ID is [" + id + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  p.SetPdsFile(labelFile, "", label);
  Cube *outcube = p.SetOutputCube("TO");

  iString bandorder;
  bandorder = (string) lab.FindObject("IMAGE").FindKeyword("BAND_STORAGE_TYPE");
  bandorder.UpCase();
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
    string msg = "Input file [" + inFile.expanded() + "] has an invalid " +
                 "band storage type. BAND_STORAGE_TYPE is [" + bandorder + "]";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
  p.StartProcess();

  // Get the mapping labels
  Pvl otherLabels;
  p.TranslatePdsProjection(otherLabels);

  // Get the directory where the MiniRF level 2 translation tables are.
  PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
  iString transDir = (string) dataDir["Lro"] + "/translations/";

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
        string msg = "Input file [" + inFile.expanded() + "] does not have valid " +
                     "ScaledPixelHeight and ScaledPixelWidth values. These values " +
                     "must be equivalent or the image is considered to be invalid.";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }

    // Set the frequency based on the InstrumentModeId. This has to
    // be done manually, because the frequency information was not
    // put in the PDS labels.
    if(!(instGrp.HasKeyword("Frequency"))) {
      string instmodeid = instGrp["InstrumentModeId"];
      double frequency;
      if(instmodeid.compare(0, 10, "BASELINE_S") == 0 ||
          instmodeid.compare(0, 6, "ZOOM_S") == 0) {
        frequency = 2379305000.0;
      }
      else {   // BASELINE_X or ZOOM_X
        frequency = 7140000000.0;
      }
      instGrp.AddKeyword(PvlKeyword("Frequency", frequency));
      outcube->putGroup(instGrp);
    }
    PvlGroup kerns("Kernels");
    if(id.compare(0, 5, "CHAN1") == 0 || id.compare(0, 3, "CH1") == 0) {
      kerns += PvlKeyword("NaifFrameCode", -86001);
    }
    else {   // LRO
      kerns += PvlKeyword("NaifFrameCode", -85700);
    }
    outcube->putGroup(kerns);
  }

  p.EndProcess();
}
