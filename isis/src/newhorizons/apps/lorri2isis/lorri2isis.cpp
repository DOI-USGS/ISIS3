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

void flip(Buffer &in);


void IsisMain() {

  // NOTE: 
  // Still to be done/considered
  //   Process the other FITS channels. One of them contains pixel quality info
  //   May want to set special pixel values using this channel

  UserInterface &ui = Application::GetUserInterface();

  ProcessImportFits importFits;

  importFits.setFitsFile(FileName(ui.GetFileName("FROM")));

  Cube *output = importFits.SetOutputCube("TO");

  // Get the directory where the New Horizons translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["NewHorizons"] + "/translations/";

  // Temp storage of translated labels
  Pvl outLabel;

  // Get the FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsLabel());

  // Check to make sure this looks like a New Horizons LORRI image


  // Create an Instrument group
  FileName insTransFile(transDir + "lorriInstrument_fit.trn");
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
  FileName bandTransFile(transDir + "lorriBandBin_fit.trn");
  PvlTranslationManager bandBinXlater(fitsLabel, bandTransFile.expanded());
  bandBinXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("BandBin", Pvl::Traverse));

  // Create an Archive group
  FileName archiveTransFile(transDir + "lorriArchive_fit.trn");
  PvlTranslationManager archiveXlater(fitsLabel, archiveTransFile.expanded());
  archiveXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("Archive", Pvl::Traverse));

  // Create a Kernels group
  FileName kernelsTransFile(transDir + "lorriKernels_fit.trn");
  PvlTranslationManager kernelsXlater(fitsLabel, kernelsTransFile.expanded());
  kernelsXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("Kernels", Pvl::Traverse));

  // Save the input FITS label in the Cube original labels
  Pvl pvl;
  pvl += importFits.fitsLabel();
  OriginalLabel originals(pvl);
  output->write(originals);

  // Convert the image data
  importFits.StartProcess();
  importFits.Finalize();

  // The images need to be flipped from top to bottom to put the origin in the upper left for ISIS
// Commented out because we don't need to do this. If we find later that we do, remove comments
//  ProcessBySample flipLines;
//  CubeAttributeInput inAttribute;
//  Cube *cube = flipLines.SetInputCube(ui.GetFileName("TO"), inAttribute);
//  cube->reopen("rw");
//  flipLines.Progress()->SetText("Flipping top to bottom");
//  flipLines.ProcessCubeInPlace(flip, false);
}


void flip(Buffer &in) {
  for(int i = 0; i < in.size() / 2; i++) {
    swap(in[i], in[in.size() - i - 1]);
  }
}

