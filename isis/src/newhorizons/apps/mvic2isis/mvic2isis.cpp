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

  // NOTE: 
  // Still to be done/considered
  //   Process the other FITS channels. One of them contains pixel quality info
  //   May want to set special pixel values using this channel

  UserInterface &ui = Application::GetUserInterface();

  ProcessImportFits importFits;
  importFits.setFitsFile(FileName(ui.GetFileName("FROM")));

  // Get the FITS label
  Pvl fitsLabel;
  fitsLabel.addGroup(importFits.fitsLabel(0));

  // Make sure this is a New Horizons MVIC image Fits formatted file
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

  importFits.setProcessFileStructure(0);

  // Set up the output cube
  Cube *output = importFits.SetOutputCube("TO");

  Pvl isisLabel;
  translateLabels(fitsLabel, output);

  // Save the input FITS label in the Cube original labels
  OriginalLabel originals(fitsLabel);
  output->write(originals);

  // Import the file and then translate labels
  importFits.StartProcess();
  importFits.EndProcess();
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

