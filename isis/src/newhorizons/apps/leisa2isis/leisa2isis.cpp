#include "Isis.h"

#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "OriginalLabel.h"
#include "ProcessBySample.h"
#include "ProcessImportFits.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

#include <fstream>
#include <iostream>
#include <QString>

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  ProcessImportFits importFits;

  importFits.setFitsFile(FileName(ui.GetFileName("FROM")));

  // Get the first label and make sure this is a New Horizons LEISA file
  PvlGroup mainLabel = importFits.fitsLabel(0);
  if (mainLabel["MISSION"][0] != "New Horizons" || mainLabel["INSTRU"][0] != "lei") {
    QString msg = QObject::tr("Input file [%1] does not appear to be a New Horizons LEISA FITS "
                              "file. Input file label value for MISSION is [%2], "
                              "INSTRU is [%3]").
                  arg(ui.GetFileName("FROM")).arg(mainLabel["MISSION"][0]).
                  arg(mainLabel["INSTRU"][0]);
    throw IException(IException::User, msg, _FILEINFO_);
  }

  //This is a seven-extension fits file. Only import the primary image for now.
  importFits.setProcessFileStructure(0);

  Cube *output = importFits.SetOutputCube("TO");

  // Get the directory where the New Horizons translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["NewHorizons"] + "/translations/";

  // Temp storage of translated labels
  Pvl outLabel;

  // Get the FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsLabel(0));

  // add in label processing later
  /*
  // Create an Instrument group
  FileName insTransFile(transDir + "leisaInstrument_fit.trn");// /s/lorri/leisa
  PvlTranslationManager insXlater(fitsLabel, insTransFile.expanded());
  insXlater.Auto(outLabel);

  // Modify/add Instument group keywords not handled by the translater
  PvlGroup &inst = outLabel.findGroup("Instrument", Pvl::Traverse);
  QString target = (QString)inst["TargetName"];
  if (target.startsWith("RADEC=")) {
    inst.addKeyword(PvlKeyword("TargetName", "Sky"), PvlGroup::Replace);
  }

  output->putGroup(outLabel.findGroup("Instrument", Pvl::Traverse));

  // Create a Band Bin group
  FileName bandTransFile(transDir + "leisaBandBin_fit.trn");
  PvlTranslationManager bandBinXlater(fitsLabel, bandTransFile.expanded());
  bandBinXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("BandBin", Pvl::Traverse));

  // Create an Archive group
  FileName archiveTransFile(transDir + "leisaArchive_fit.trn");
  PvlTranslationManager archiveXlater(fitsLabel, archiveTransFile.expanded());
  archiveXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("Archive", Pvl::Traverse));

  // Create a Kernels group
  FileName kernelsTransFile(transDir + "leisaKernels_fit.trn");
  PvlTranslationManager kernelsXlater(fitsLabel, kernelsTransFile.expanded());
  kernelsXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("Kernels", Pvl::Traverse));
  */

  // Save the input FITS label in the Cube original labels
  Pvl pvl;
  pvl += importFits.fitsLabel(0);
  OriginalLabel originals(pvl);
  output->write(originals);

  // Convert the main image data
  importFits.Progress()->SetText("Importing main LEISA image");
  importFits.StartProcess();
  importFits.ClearCubes();
  importFits.Finalize();
}



