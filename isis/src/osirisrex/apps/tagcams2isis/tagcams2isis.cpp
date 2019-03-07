#include "Isis.h"

#include <fstream>
#include <iostream>
#include <string>
#include <bitset>

#include <QString>
#include <QObject>

#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "ITime.h"
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

  FileName from(ui.GetFileName("FROM"));
  importFits.setFitsFile(from);

  // Get the primary FITS label so we can confirm its the proper format
  // and get some values for processing
  Pvl flabel;
  flabel.addGroup(importFits.fitsImageLabel(0));

  // Do we remove image boundary pixels?
  bool removeCal = ui.GetBoolean("REMOVECALPIXELS");

  // Collect some raw label values. If these fail, then its likely
  // this is not a TAGCAMS image
  int rawSamples, rawLines, summing, binning, rawcamT, tcmode;
  try {
    rawSamples = (int)flabel.findKeyword("NAXIS1", Pvl::Traverse);
    rawLines = (int) flabel.findKeyword("NAXIS2", Pvl::Traverse);
    summing  = (int) flabel.findKeyword("TCSUM", Pvl::Traverse);
    binning   = (int) flabel.findKeyword("TCSSMPL", Pvl::Traverse);
    rawcamT  = (int) flabel.findKeyword("TCCHTEMP", Pvl::Traverse);
    tcmode  = (int) flabel.findKeyword("TCMODE", Pvl::Traverse);
  } 
  catch (IException &ie) {
    QString msg = QObject::tr("Unable to retrieve expected TAGCAMS keywords."
                              "The file provided in FROM is likely not a TAGCAMS image.");
    throw IException(ie, IException::User, msg, _FILEINFO_);
  }

  // Check for summing conditions
  summing = ( summing <= 1 )  ?  1 : 2;
  if (binning == 1) {
    binning = 2;
  }
  else if (binning == 4 ) {
    binning = 4;
  }
  else if (binning >= 16) {
    binning = binning - 14;
  }
  else { // ( binning <= 0)  || binning is defined
    binning = 1;
  }
 
  // True pixel scaling
  int pixScale = summing * binning;

  // Was boundary data included? Bit 4 is set if so...
  std::bitset<8> darkbit(std::string("00010000"));
  bool hasDark = darkbit.to_ulong() & (unsigned long) tcmode;
  // cout << "HasDark: " << hasDark << "\n";

  // Full input frame size before dark removal (if present/requested)
  int ns(rawSamples), nl(rawLines), nb(1);

   // Compute the boundary pixels if present and requested.
  if ( removeCal && hasDark ) {
      QString bitpix = flabel.findKeyword("BITPIX", Pvl::Traverse);
      int bytesPerPix = abs(toInt(bitpix)) / 8;
      importFits.SetDataHeaderBytes(bytesPerPix * ((54 * rawSamples) / pixScale));
      importFits.SetDataPrefixBytes(bytesPerPix * (144 / pixScale));
      importFits.SetDataSuffixBytes(bytesPerPix * (16  / pixScale));
      // importFits.SetDataTrailerBytes(bytesPerPix * ((6 * (rawSamples-16))  / pixScale));

      // Adjust the number of pipels removed by binning (NOTE this may not
      // be correct as we don't have any images to test that has any
      // summing or binning)
      ns -= (144 + 16) / pixScale;
      nl -= (54 + 6) / pixScale;
  }

  // Now set the output file characteristics
  importFits.setProcessFileStructure(0);
  importFits.SetDimensions(ns,nl,nb);
  Cube *output = importFits.SetOutputCube("TO");


  QString target;
  if(ui.WasEntered("TARGET")) {
    target = ui.GetString("TARGET");
  }

  // Get the directory where the OSIRIS-REx translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["OsirisRex"] + "/translations/";

  // Temp storage of translated labels
  Pvl outLabel;

  // Get the FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsImageLabel(0));

  // Create an Instrument group
  FileName insTransFile(transDir + "tagcamsInstrument_fit.trn");
  PvlToPvlTranslationManager insXlater(fitsLabel, insTransFile.expanded());
  insXlater.Auto(outLabel);
  PvlGroup &instGrp(outLabel.findGroup("Instrument", Pvl::Traverse));

    // Create an Archive group
  FileName archTransFile(transDir + "tagcamsArchive_fit.trn");
  PvlToPvlTranslationManager archXlater(fitsLabel, archTransFile.expanded());
  archXlater.Auto(outLabel);
  PvlGroup &archiveGrp(outLabel.findGroup("Archive", Pvl::Traverse));

  // Add product id which is just the filename base
  QString prodid = from.baseName();
  archiveGrp.addKeyword(PvlKeyword("SourceProductId", prodid), archiveGrp.begin());

 //  Create YearDoy keyword in Archive group
  iTime stime(instGrp["StartTime"][0]);
  PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
  archiveGrp.addKeyword(yeardoy);

  output->putGroup(archiveGrp);

  //  If the user specifed the target explicitly or it doesn't exist, create
  //  something so the camera will always work
  if(instGrp.findKeyword("TargetName").isNull() || (!target.isEmpty())) {
    if(!target.isEmpty()) {
      instGrp["TargetName"] = QString(target);
    }
    else {
      instGrp["TargetName"] = QString("Sky");
    }
  }

  // Convert raw temp DN to celcius - from UA-SIS-9.4.4-322, Rev. 3.0
  double camHeadTempC = 0.15259 * ((double) rawcamT) + (-273.14);
  instGrp.addKeyword(PvlKeyword("CameraHeadTemperature", toString(camHeadTempC), "celsius"));
  output->putGroup(instGrp);


  // Create a Band Bin group
  FileName bandTransFile(transDir + "tagcamsBandBin_fit.trn");
  PvlToPvlTranslationManager bandBinXlater(fitsLabel, bandTransFile.expanded());
  bandBinXlater.Auto(outLabel);
  output->putGroup(outLabel.findGroup("BandBin", Pvl::Traverse));

  // Create a Kernels group
  FileName kernelsTransFile(transDir + "tagcamsKernels_fit.trn");
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
