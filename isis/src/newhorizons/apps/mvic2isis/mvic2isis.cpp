#include "Isis.h"

#include <fstream>
#include <iostream>

#include <QString>

#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "LineManager.h"
#include "NaifStatus.h"
#include "OriginalLabel.h"
#include "ProcessBySample.h"
#include "ProcessImportFits.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void translateLabels(Pvl &fitsLabel, Cube *ocube);
void flip(Buffer &in);

void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();

  ProcessImportFits importFits;
  importFits.setFitsFile(FileName(ui.GetFileName("FROM")));

  // Get the primary FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsLabel(0));

  // Make sure this is a New Horizons MVIC image FITS formatted file
  bool mvic = true;
  if (fitsLabel.hasKeyword("MISSION", Pvl::Traverse)) {
    QString mission = fitsLabel.findKeyword("MISSION", Pvl::Traverse);
    if (!mission.contains("New Horizons")) mvic = false;
    if (fitsLabel.hasKeyword("INSTRU", Pvl::Traverse)) {
      QString instrument = fitsLabel.findKeyword("INSTRU", Pvl::Traverse);
      if (!instrument.contains("mvi")) mvic = false;
    }
    else {
      mvic = false;
    }
  }
  else {
    mvic = false;
  }

  if (!mvic) {
    FileName in = ui.GetFileName("FROM");
    QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
                  "in New Horizons/MVIC FITS format.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (!fitsLabel.hasKeyword("BITPIX", Pvl::Traverse)) {
    FileName in = ui.GetFileName("FROM");
    QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
                  "in New Horizons/MVIC FITS format. BITPIX keyword is missing.";
    throw IException(IException::User, msg, _FILEINFO_);
  }


  // Check to see if the undistorted image was requested from the FITS file and it has the 
  // corresponding extension and keywords
  if (ui.WasEntered("UNDISTORTED")) {
    PvlGroup undistortedLabel = importFits.fitsLabel(1);
    if (!undistortedLabel.hasKeyword("XTENSION") || !undistortedLabel.hasKeyword("COMMENT") ||
        undistortedLabel["XTENSION"][0] != "IMAGE" || 
        !undistortedLabel["COMMENT"][0].startsWith("This is the bias-subtracted, flattened, distortion-removed image cube.")) {

      QString msg = QObject::tr("Input file [%1] does not appear to contain an MVIC Undistorted image. "
                                "FITS label value for EXTNAME is [%2]").
                             arg(ui.GetFileName("FROM")).arg(undistortedLabel["EXTNAME"][0]);
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (!undistortedLabel.hasKeyword("BITPIX")) {
      FileName in = ui.GetFileName("FROM");
      QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
                    "in New Horizons/MVIC FITS format. BITPIX keyword is missing from label [1].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  // Check to see if the error image was requested from the FITS file and it has the 
  // corresponding extension and keywords
  if (ui.WasEntered("ERROR")) {
    PvlGroup errorLabel = importFits.fitsLabel(2);
    if (!errorLabel.hasKeyword("XTENSION") || !errorLabel.hasKeyword("COMMENT") ||
        errorLabel["XTENSION"][0] != "IMAGE" || 
        errorLabel["COMMENT"][0] != "1-sigma error per pixel for the image in extension 1.") {

      QString msg = QObject::tr("Input file [%1] does not appear to contain an MVIC Error image. "
                                "FITS label value for EXTNAME is [%2]").
                    arg(ui.GetFileName("FROM")).arg(errorLabel["EXTNAME"][0]);
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (!errorLabel.hasKeyword("BITPIX")) {
      FileName in = ui.GetFileName("FROM");
      QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
                    "in New Horizons/MVIC FITS format. BITPIX keyword is missing from label [2].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  // Check to see if the quality image was requested from the FITS file and it has the 
  // corresponding extension and keywords
  if (ui.WasEntered("QUALITY")) {
    PvlGroup qualityLabel = importFits.fitsLabel(3);
    if (!qualityLabel.hasKeyword("XTENSION") || !qualityLabel.hasKeyword("COMMENT") ||
        qualityLabel["XTENSION"][0] != "IMAGE" || 
        qualityLabel["COMMENT"][0] != "Data quality flag for the image in extension 1.") {

      QString msg = QObject::tr("Input file [%1] does not appear to contain an MVIC Quality image. "
                                "FITS label value for EXTNAME is [%2]").
                    arg(ui.GetFileName("FROM")).arg(qualityLabel["EXTNAME"][0]);
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (!qualityLabel.hasKeyword("BITPIX")) {
      FileName in = ui.GetFileName("FROM");
      QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
                    "in New Horizons/MVIC FITS format. BITPIX keyword is missing from label [3].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }


  // Start converting images

  QString bitpix = fitsLabel.findKeyword("BITPIX", Pvl::Traverse);
  int bytesPerPix = abs(toInt(bitpix)) / 8;
  importFits.SetDataPrefixBytes(bytesPerPix * 12);
  importFits.SetDataSuffixBytes(bytesPerPix * 12);
  importFits.setProcessFileStructure(0);

  // Set up the output cube
  Cube *output = importFits.SetOutputCube("TO");

  translateLabels(fitsLabel, output); // Results are put directly into the cube label (output)

  // Save the input FITS label in the output cube original labels
  OriginalLabel originals(fitsLabel);
  output->write(originals);

  // Import the file
  importFits.Progress()->SetText("Importing main MVIC image");
  importFits.StartProcess();
  importFits.EndProcess();


  // Convert the bias-subtracted, flattened, distortion removed image. It is currently assumed 
  // to be the 2rd image in the FITS file (i.e., 1st extension)
  if (ui.WasEntered("UNDISTORTED")) {
    PvlGroup undistortedLabel = importFits.fitsLabel(1);
//    if (!undistortedLabel.hasKeyword("XTENSION") || !undistortedLabel.hasKeyword("COMMENT") ||
//        undistortedLabel["XTENSION"][0] != "IMAGE" || 
//        !undistortedLabel["COMMENT"][0].startsWith("This is the bias-subtracted, flattened, distortion-removed image cube.")) {

//      QString msg = QObject::tr("Input file [%1] does not appear to contain an MVIC Undistorted image "
//                                "Input file label value for EXTNAME is [%2]").
//                             arg(ui.GetFileName("FROM")).arg(undistortedLabel["COMMENT"][0]);
//      throw IException(IException::User, msg, _FILEINFO_);
//    }

//    if (!undistortedLabel.hasKeyword("BITPIX")) {
//      FileName in = ui.GetFileName("FROM");
//      QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
//                    "in New Horizons/MVIC FITS format. BITPIX keyword is missing.";
//      throw IException(IException::User, msg, _FILEINFO_);
//    }

    QString bitpix = undistortedLabel.findKeyword("BITPIX");
    int bytesPerPix = abs(toInt(bitpix)) / 8;
    importFits.SetDataPrefixBytes(bytesPerPix * 12);
    importFits.SetDataSuffixBytes(bytesPerPix * 12);
    importFits.setProcessFileStructure(0);
    importFits.setProcessFileStructure(1);

    Cube *outputError = importFits.SetOutputCube("UNDISTORTED");

    // Save the input FITS label in the Cube original labels
    Pvl pvlError;
    pvlError += undistortedLabel;
    OriginalLabel originals(pvlError);
    outputError->write(originals);

    // Convert the main image data
    importFits.Progress()->SetText("Importing MVIC Undistorted image");
    importFits.StartProcess();
    importFits.ClearCubes();
  }


  // Convert the Error image. It is currently assumed to be the 3rd image in the FITS file
  if (ui.WasEntered("ERROR")) {
    // Get the label of the Error image and make sure this is a New Horizons MVIC Error image
    PvlGroup errorLabel = importFits.fitsLabel(2);
//    if (!errorLabel.hasKeyword("XTENSION") || !errorLabel.hasKeyword("COMMENT") ||
//        errorLabel["XTENSION"][0] != "IMAGE" || 
//        errorLabel["COMMENT"][0] != "1-sigma error per pixel for the image in extension 1.") {

//      QString msg = QObject::tr("Input file [%1] does not appear to contain an MVIC Error image "
//                                "Input file label value for EXTNAME is [%2]").
//                    arg(ui.GetFileName("FROM")).arg(errorLabel["COMMENT"][0]);
//      throw IException(IException::User, msg, _FILEINFO_);
//    }

//    if (!errorLabel.hasKeyword("BITPIX")) {
//      FileName in = ui.GetFileName("FROM");
//      QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
//                    "in New Horizons/MVIC FITS format. BITPIX keyword is missing.";
//      throw IException(IException::User, msg, _FILEINFO_);
//    }
    QString bitpix = errorLabel.findKeyword("BITPIX");
    int bytesPerPix = abs(toInt(bitpix)) / 8;
    importFits.SetDataPrefixBytes(bytesPerPix * 12);
    importFits.SetDataSuffixBytes(bytesPerPix * 12);
    importFits.setProcessFileStructure(0);
    importFits.setProcessFileStructure(2);

    Cube *outputError = importFits.SetOutputCube("ERROR");

    // Save the input FITS label in the Cube original labels
    Pvl pvlError;
    pvlError += errorLabel;
    OriginalLabel originals(pvlError);
    outputError->write(originals);

    // Convert the main image data
    importFits.Progress()->SetText("Importing MVIC Error image");
    importFits.StartProcess();
    importFits.ClearCubes();
  }


  // Convert the Quality image. It is currently assumed to be the 4th image in the FITS file
  if (ui.WasEntered("QUALITY")) {
    // Get the label of the Error image and make sure this is a New Horizons MVIC Quality image
    PvlGroup qualityLabel = importFits.fitsLabel(3);
//    if (!qualityLabel.hasKeyword("XTENSION") || !qualityLabel.hasKeyword("COMMENT") ||
//        qualityLabel["XTENSION"][0] != "IMAGE" || 
//        qualityLabel["COMMENT"][0] != "Data quality flag for the image in extension 1.") {

//      QString msg = QObject::tr("Input file [%1] does not appear to contain an MVIC Quality image "
//                                "Input file label value for EXTNAME is [%2]").
//                    arg(ui.GetFileName("FROM")).arg(qualityLabel["COMMENT"][0]);
//      throw IException(IException::User, msg, _FILEINFO_);
//    }

//    if (!qualityLabel.hasKeyword("BITPIX")) {
//      FileName in = ui.GetFileName("FROM");
//      QString msg = "Input file [" + in.expanded() + "] does not appear to be " +
//                    "in New Horizons/MVIC FITS format. BITPIX keyword is missing.";
//      throw IException(IException::User, msg, _FILEINFO_);
//    }
    QString bitpix = qualityLabel.findKeyword("BITPIX");
    int bytesPerPix = abs(toInt(bitpix)) / 8;
    importFits.SetDataPrefixBytes(bytesPerPix * 12);
    importFits.SetDataSuffixBytes(bytesPerPix * 12);
    importFits.setProcessFileStructure(0);
    importFits.setProcessFileStructure(3);

    Cube *outputError = importFits.SetOutputCube("QUALITY");

    // Save the input FITS label in the Cube original labels
    Pvl pvlError;
    pvlError += qualityLabel;
    OriginalLabel originals(pvlError);
    outputError->write(originals);

    // Convert the main image data
    importFits.Progress()->SetText("Importing MVIC Quality image");
    importFits.StartProcess();
    importFits.ClearCubes();
  }
}



void translateLabels(Pvl &fitsLabel, Cube *ocube) {

  // Get the directory where the New Horizons translation tables are.
  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["NewHorizons"] + "/translations/";

  Pvl *isisLabel = ocube->label();
  // Create an Instrument group
  FileName insTransFile(transDir + "mvicInstrument_fit.trn");
  PvlTranslationManager insXlater(fitsLabel, insTransFile.expanded());
  insXlater.Auto(*(isisLabel));

  // Modify/add Instument group keywords not handled by the translater
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);

  QString instId = (QString)inst["InstrumentId"];
  QString scanType = (QString)inst["ScanType"];
  instId = instId + "_" + scanType;
  inst.addKeyword(PvlKeyword("InstrumentId", instId), PvlGroup::Replace);

  QString target = (QString)inst["TargetName"];
  if (target.startsWith("RADEC=")) {
    inst.addKeyword(PvlKeyword("TargetName", "Sky"), PvlGroup::Replace);
  }

  if (inst.hasKeyword("TdiRate")) {
    inst.findKeyword("TdiRate").setUnits("hz");
  }

  //  Create StartTime (UTC) from the SpacecraftClockStartCount.  Need to load the leapsecond
  //  and spacecraft clock kernels to calculate time.
  NaifStatus::CheckErrors();
  // Leapsecond kernel
  QString lsk = "$ISIS3DATA/base/kernels/lsk/naif????.tls";
  FileName lskName(lsk);
  lskName = lskName.highestVersion();
  furnsh_c(lskName.expanded().toAscii().data());

  // Spacecraft clock kernel
  QString sclk = "$ISIS3DATA/newhorizons/kernels/sclk/new_horizons_???.tsc";
  FileName sclkName(sclk);
  sclkName = sclkName.highestVersion();
  furnsh_c(sclkName.expanded().toAscii().data());

  SpiceInt sclkCode;
  if (fitsLabel.hasKeyword("SPCSCID", Pvl::Traverse)) {
    sclkCode = fitsLabel.findKeyword("SPCSCID", Pvl::Traverse);
  }
  else {
    QString msg = "Input file is missing the spacecraft Naif Id.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  QString scTime = inst["SpacecraftClockStartCount"];
  double et;
  scs2e_c(sclkCode, scTime.toAscii().data(), &et);
  SpiceChar utc[30];
  et2utc_c(et, "ISOC", 3, 30, utc);
  inst.addKeyword(PvlKeyword("StartTime", QString(utc)));
  // Create a Band Bin group
  FileName bandTransFile(transDir + "mvicBandBin_fit.trn");
  PvlTranslationManager bandBinXlater(fitsLabel, bandTransFile.expanded());
  bandBinXlater.Auto(*(isisLabel));
  // Add units and OriginalBand keyword
  PvlGroup &bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  bandBin.addKeyword(PvlKeyword("OriginalBand", "1"));
  PvlKeyword &center = bandBin.findKeyword("Center");
  center.setUnits("nanometers");
  PvlKeyword &width = bandBin.findKeyword("Width");
  width.setUnits("nanometers");

  // If image is in the framing mode, the BandBin keywords will need to be duplicated for each
  // band.  Also, add BandStartTime
  if (scanType.contains("FRAMING")) {
    PvlGroup &bandBin = isisLabel->findGroup("BandBin", Pvl::Traverse);
    QString name = bandBin.findKeyword("Name")[0];
    QString center = QString(bandBin.findKeyword("Center")[0]);
    QString width = QString(bandBin.findKeyword("Width")[0]);
    for (int i = 1; i < ocube->bandCount(); i++) {
      bandBin.findKeyword("Name").addValue(name);
      bandBin.findKeyword("Center").addValue(center, "nanometers");
      bandBin.findKeyword("Width").addValue(width, "nanometers");
      bandBin.findKeyword("OriginalBand").addValue(QString::number(i+1));
      QString fitsKey = QString("UTCMID%1").arg(i, 2, 10, QChar('0'));
      QString fitsVal = fitsLabel.findKeyword(fitsKey, Pvl::Traverse);
      bandBin.findKeyword("UtcTime").addValue(fitsVal);
    }
  }

  // Create an Archive group
  FileName archiveTransFile(transDir + "mvicArchive_fit.trn");
  PvlTranslationManager archiveXlater(fitsLabel, archiveTransFile.expanded());
  archiveXlater.Auto(*(isisLabel));

  // Create a Kernels group
  FileName kernelsTransFile(transDir + "mvicKernels_fit.trn");
  PvlTranslationManager kernelsXlater(fitsLabel, kernelsTransFile.expanded());
  kernelsXlater.Auto(*(isisLabel));
  
  // If Level 2 product, Create a RadiometricCalibration group
  if (fitsLabel.hasKeyword("SOCL2VER", Pvl::Traverse)) {
    FileName calibrationTransFile(transDir + "mvicCalibration_fit.trn"); 
    PvlTranslationManager calibrationXlater(fitsLabel, calibrationTransFile.expanded());
    calibrationXlater.Auto(*(isisLabel));

    //  Add comments to calibration keywords.  This is done by hand because the translation tables
    //  did not handle comments at the time this was written.
    PvlGroup &calibration = isisLabel->findGroup("RadiometricCalibration", Pvl::Traverse);
    if (calibration.hasKeyword("PixelSize")) {
      calibration.findKeyword("PixelSize").setUnits("microns");
    }
    if (calibration.hasKeyword("PixelFov")) {
      calibration.findKeyword("PixelFov").setUnits("microrad/pix");
    }
    if (calibration.hasKeyword("Gain")) {
      calibration.findKeyword("Gain").setUnits("electrons/DN");
    }
    if (calibration.hasKeyword("ReadNoise")) {
      calibration.findKeyword("ReadNoise").setUnits("electrons");
    }
    if (calibration.hasKeyword("TdiRate")) {
      calibration.findKeyword("TdiRate").setUnits("hz");
    }
    if (calibration.hasKeyword("SolarSpectrumResolved")) {
      calibration.findKeyword("SolarSpectrumResolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("SolarSpectrumUnresolved")) {
      calibration.findKeyword("SolarSpectrumUnresolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("PholusSpectrumResolved")) {
      calibration.findKeyword("PholusSpectrumResolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("PholusSpectrumUnresolved")) {
      calibration.findKeyword("PholusSpectrumUnresolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("CharonSpectrumResolved")) {
      calibration.findKeyword("CharonSpectrumResolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("CharonSpectrumUnresolved")) {
      calibration.findKeyword("CharonSpectrumUnresolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("JupiterSpectrumResolved")) {
      calibration.findKeyword("JupiterSpectrumResolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("JupiterSpectrumUnresolved")) {
      calibration.findKeyword("JupiterSpectrumUnresolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("PlutoSpectrumResolved")) {
      calibration.findKeyword("PlutoSpectrumResolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("PlutoSpectrumUnresolved")) {
      calibration.findKeyword("PlutoSpectrumUnresolved").setUnits("(erg/cm^2/s/sr)/(DN/s/pix)");
    }
    if (calibration.hasKeyword("SolarPivotWavelength")) {
      calibration.findKeyword("SolarPivotWavelength").setUnits("cm");
    }
    if (calibration.hasKeyword("JupiterPivotWavelength")) {
      calibration.findKeyword("JupiterPivotWavelength").setUnits("cm");
    }
    if (calibration.hasKeyword("PholusPivotWavelength")) {
      calibration.findKeyword("PholusPivotWavelength").setUnits("cm");
    }
    if (calibration.hasKeyword("PlutoPivotWavelength")) {
      calibration.findKeyword("PlutoPivotWavelength").setUnits("cm");
    }
    if (calibration.hasKeyword("CharonPivotWavelength")) {
      calibration.findKeyword("CharonPivotWavelength").setUnits("cm");
    }
  }
}


void flip(Buffer &in) {
  for(int i = 0; i < in.size() / 2; i++) {
    swap(in[i], in[in.size() - i - 1]);
  }
}

