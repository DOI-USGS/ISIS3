#include "Isis.h"

#include <fstream>
#include <iostream>

#include <QString>

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

using namespace std;
using namespace Isis;

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  ProcessImportFits importFits;

  importFits.setFitsFile(FileName(ui.GetFileName("FROM")));

  importFits.setProcessFileStructure(0);

  Cube *output = importFits.SetOutputCube("TO");

  // Get the directory where the OSIRIS-REx translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["OsirisRex"] + "/translations/";

  // Temp storage of translated labels
  Pvl outLabel;

  // Get the FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsImageLabel(0));

  // Create an Instrument group
  FileName insTransFile(transDir + "ocamsInstrument_fit.trn");
  PvlToPvlTranslationManager insXlater(fitsLabel, insTransFile.expanded());
  insXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("Instrument", Pvl::Traverse));


  // Create a Band Bin group
  FileName bandTransFile(transDir + "ocamsBandBin_fit.trn");
  PvlToPvlTranslationManager bandBinXlater(fitsLabel, bandTransFile.expanded());
  bandBinXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("BandBin", Pvl::Traverse));

  // Create a Kernels group
  FileName kernelsTransFile(transDir + "ocamsKernels_fit.trn");
  PvlToPvlTranslationManager kernelsXlater(fitsLabel, kernelsTransFile.expanded());
  kernelsXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("Kernels", Pvl::Traverse));

  // Save the input FITS label in the Cube original labels
  Pvl pvl;
  pvl += importFits.fitsImageLabel(0);
  OriginalLabel originals(pvl);
  output->write(originals);

  // Convert the image data
  importFits.StartProcess();
  importFits.Finalize();
}
