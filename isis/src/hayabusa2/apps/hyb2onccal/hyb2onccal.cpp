
#include <vector>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cmath>
#include <cfloat>

#include <QDebug>
#include <QFile>
#include <QString>
#include <QScopedPointer>
#include <QTemporaryFile>
#include <QVector>

#include "Application.h"
#include "AlphaCube.h"
#include "Buffer.h"
#include "FileName.h"
#include "hyb2onccal.h"
#include "Hyb2OncCalUtils.h"
#include "IException.h"
#include "iTime.h"
#include "LineManager.h"
#include "Pixel.h"
#include "ProcessByLine.h"
#include "ProcessBySample.h"
#include "ProcessByBrick.h"
#include "ProcessByBoxcar.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Spice.h"
#include "Statistics.h"
#include "TextFile.h"

using namespace std;

namespace Isis {

  void hyb2onccal(UserInterface &ui, Pvl *log) {
    Cube icube;
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube.setVirtualBands(inAtt.bands());
    }
    icube.open(ui.GetFileName("FROM"));
    hyb2onccal(&icube, ui, log);
  }

  void hyb2onccal(Cube *icube, UserInterface &ui, Pvl *log) {
    g_calStep = ui.GetString("UNITS");

    const QString hyb2cal_program = "hyb2onccal";
    const QString hyb2cal_version = "1.1";
    const QString hyb2cal_revision = "$Revision$";
    QString hyb2cal_runtime = Application::DateTime();

    ProcessBySample p;
    p.SetInputCube(icube);

    // Basic assurances...
    if (icube->bandCount() != 1) {
      throw IException(IException::User,
        "ONC images may only contain one band", _FILEINFO_);
      }

    PvlGroup &inst = icube->group("Instrument");
    PvlGroup &bandbin = icube->group("BandBin");

    try {
      g_filter = bandbin["FilterName"][0];
    }
    catch(IException &e) {
      QString msg = "Unable to read [FilterName] keyword in the BandBin group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    QString instrument("");

    try {
      instrument = inst["InstrumentId"][0];
    }
    catch(IException &e) {
      QString msg = "Unable to read [InstrumentId] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    if (instrument == "ONC-W1") {
      g_instrument = InstrumentType::ONCW1;
    }
    else if (instrument == "ONC-W2") {
      g_instrument = InstrumentType::ONCW2;
    }
    else if (instrument == "ONC-T") {
      g_instrument = InstrumentType::ONCT;
    }
    else {
      QString msg = "Unidentified instrument key in the "
                    "InstrumentId key of the Instrument Pvl group.";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    //Set up binning and image subarea mapping
    binning = inst["Binning"];
    int startLine = inst["SelectedImageAreaY1"];
    int startSample = inst["SelectedImageAreaX1"];
    int lastLine = inst["SelectedImageAreaY2"];
    int lastSample = inst["SelectedImageAreaX2"];

    AlphaCube myAlpha(1024, 1024, icube->sampleCount(), icube->lineCount(),
                      startSample, startLine, lastSample, lastLine);

    try {
      g_bitDepth = inst["BitDepth"];
    }
    catch (IException &e) {
      QString msg = "Unable to read [BitDepth] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      g_bitDepth = 12;

    }

    if (g_bitDepth < 0) {
      g_bitDepth = 12;  //Correpsonds to no correction being done for bit depth
    }

    alpha = &myAlpha;

    try {
      g_texp = inst["ExposureDuration"][0].toDouble();
    }
    catch(IException &e) {
      QString msg = "Unable to read [ExposureDuration] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    try {
      g_AEtemperature = inst["ONCAETemperature"][0].toDouble();
    }
    catch(IException &e) {
      QString msg = "Unable to read [ONCAETemperature] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    try {
      g_CCD_T_temperature = inst["ONCTCCDTemperature"][0].toDouble();
    }
    catch(IException &e) {
      QString msg = "Unable to read [ONCTCCDTemperature] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    try {
      g_ECT_T_temperature = inst["ONCTElectricCircuitTemperature"][0].toDouble();
    }
    catch(IException &e) {
      QString msg = "Unable to read [ONCTElectricCircuitTemperature] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    try {
      g_startTime = inst["SpacecraftClockStartCount"][0];
    }
    catch (IException &e) {
      QString msg = "Unable to read [SpacecraftClockStartCount] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    try {
      // Convert from KM to AU
      QString solarDistance = inst["SolarDistance"][0].replace('D', 'e');
      g_solarDist = solarDistance.toDouble() * (1.0 / 49598073.0);
    }
    catch(IException &e) {
      QString msg = "Unable to read [SolarDistance] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    QString smearCorrection;
    try {
      smearCorrection = inst["SmearCorrection"][0];
    }
    catch (IException &e) {
      QString msg = "Unable to read [SmearCorrection] keyword in the Instrument group "
                    "from input file [" + icube->fileName() + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    if (smearCorrection == "ONBOARD") {
      g_onBoardSmearCorrection = true;
    }

    QString compmode = inst["Compression"];
    // TODO: verify that the compression factor/scale is actually 16 for compressed Hayabusa2 images.
    g_compfactor = ("lossy" == compmode.toLower()) ? 16.0 : 1.0;

    QString target = inst["TargetName"];
    g_target = target;

    // NOTE we do not have a valid flat-field for the W1 or W2 images.
    FileName flatfile = DetermineFlatFieldFile(g_filter);
    PvlGroup alphaCube;

    QScopedPointer<Cube, TemporaryCubeDeleter> flatcube;
    QString reducedFlat(flatfile.expanded());

    try {
      alphaCube = icube->group("AlphaCube");
    }

   catch(IException &e) {
     g_cropped = false;
    }

    // Image is not cropped
    if (!g_cropped) {
      // Determine if we need to subsample the flat field if pixel binning occurred
      // TODO: test a binned image (add test case).
      if (binning > 1) {
        QString scale(toString(binning));
        FileName newflat = FileName::createTempFile("$TEMPORARY/" +
        flatfile.baseName() + "_reduced.cub");
        reducedFlat = newflat.expanded();
        QString parameters = "FROM=" + flatfile.expanded() +
                             " TO="   + newflat.expanded() +
                             " MODE=SCALE" +
                             " LSCALE=" + scale +
                             " SSCALE=" + scale;

        try {
          ProgramLauncher::RunIsisProgram("reduce", parameters);
        }
        catch (IException& ie) {
          remove(reducedFlat.toLatin1().data());
          throw ie;
        }
      }

      // Set up processing for flat field as a second input file
      CubeAttributeInput att;
      p.SetInputCube(reducedFlat, att);
    }
    // The image was cropped, so pull the same subarea from the flat file into a temp file
    else {
      FileName transFlat =
      FileName::createTempFile("$TEMPORARY/" + flatfile.baseName() + "_translated.cub");

      Cube *flatOriginal = new Cube(flatfile.expanded() );

      double alphaStartSample = alphaCube["AlphaStartingSample"][0].toDouble();
      double alphaStartLine = alphaCube["AlphaStartingLine"][0].toDouble();
      double alphaEndSample = alphaCube["AlphaEndingSample"][0].toDouble();
      double alphaEndLine = alphaCube["AlphaEndingLine"][0].toDouble();

      double transform[5] = {(double)binning, alphaStartSample, alphaStartLine, alphaEndSample, alphaEndLine};

      // Translates and scales the flatfield image.  Scaling
      // might be necessary in the event that the raw image was also binned.
      translate(flatOriginal,transform,transFlat.expanded());

      QScopedPointer<Cube, TemporaryCubeDeleter> translated(new Cube(transFlat.expanded(), "r"));
      flatcube.swap(translated);

      CubeAttributeInput att;
      p.SetInputCube(transFlat.expanded(),att);

    }  //Finished setting flatfield file

    Cube *ocube = p.SetOutputCube(ui.GetFileName("TO"), ui.GetOutputAttribute("TO"), 
                                  icube->sampleCount(), icube->lineCount(), icube->bandCount());
    QString calfile = loadCalibrationVariables(ui.GetAsString("CONFIG"));
    g_timeRatio = g_Tvct/(g_texp + g_Tvct);

    // Output units of DN
    QString units = "DN";

    // Output units of radiance
    if (g_calStep == "RADIANCE") {
      // Units of RADIANCE
      g_calibrationScale = 1.0 / (g_texp * g_sensitivity);
      units = "W / (m**2 micrometer sr)";
    }

    // Output units of I/F
    else if (g_calStep == "IOF") {
      // Convert to radiance
      g_calibrationScale = 1.0 / (g_texp * g_sensitivity);
      // Convert to reflectance
      g_calibrationScale *= (Isis::PI * (g_solarDist * g_solarDist)) / g_solarFlux;
      units = "I over F";
    }

    // Calibrate!
    try {
      p.Progress()->SetText("Calibrating Hayabusa2 Cube");
      p.StartProcess(Calibrate);
    }
    catch (IException &ie) {
      throw IException(ie, IException::Programmer,
        "Radiometric calibration failed!", _FILEINFO_);
    }

    // Log calibration activity performed so far
    PvlGroup calibrationLog("RadiometricCalibration");
    calibrationLog.addKeyword(PvlKeyword("SoftwareName", hyb2cal_program));
    calibrationLog.addKeyword(PvlKeyword("SoftwareVersion", hyb2cal_version));
    calibrationLog.addKeyword(PvlKeyword("ProcessDate", hyb2cal_runtime));
    calibrationLog.addKeyword(PvlKeyword("CalibrationFile", calfile));
    calibrationLog.addKeyword(PvlKeyword("FlatFieldFile", flatfile.originalPath()
    + "/" + flatfile.name()));

    PvlKeyword sensitivityFactor("SensitivityFactor");
    sensitivityFactor.addValue(toString(g_sensitivity, 16));
    calibrationLog.addKeyword(sensitivityFactor);

    // Parameters
    PvlKeyword bn("Bias_Bn");
    bn.addValue(toString(g_b0, 8));
    bn.addValue(toString(g_b1, 8));
    bn.addValue(toString(g_b2, 8));
    calibrationLog.addKeyword(bn);

    PvlKeyword bnae("Bias_AECorrection");
    bnae.addValue(toString(g_bae0, 8));
    bnae.addValue(toString(g_bae1, 8));
    calibrationLog.addKeyword(bnae);  
    calibrationLog.addKeyword(PvlKeyword("Bias_AETemp", toString(g_AEtemperature, 16)));

    switch (g_instrument) {
      case InstrumentType::ONCT:
         calibrationLog.addKeyword(PvlKeyword("Bias_CCDTemp", toString(g_CCD_T_temperature, 16)));
         calibrationLog.addKeyword(PvlKeyword("Bias_ECTTemp", toString(g_ECT_T_temperature, 16)));
      break;

      case InstrumentType::ONCW1:
        calibrationLog.addKeyword(PvlKeyword("Bias_CCDTemp", toString(g_CCD_W1_temperature, 16)));
        calibrationLog.addKeyword(PvlKeyword("Bias_ECTTemp", toString(g_ECT_W1_temperature, 16)));
        break;

      case InstrumentType::ONCW2:
        calibrationLog.addKeyword(PvlKeyword("Bias_CCDTemp", toString(g_CCD_W2_temperature, 16)));
        calibrationLog.addKeyword(PvlKeyword("Bias_ECTTemp", toString(g_ECT_W2_temperature, 16)));
        break;
    }

    calibrationLog.addKeyword(PvlKeyword("Bias", toString(g_bias, 16), "DN"));
    calibrationLog.addKeyword(PvlKeyword("Smear_Tvct", toString(g_Tvct, 16)));
    calibrationLog.addKeyword(PvlKeyword("Smear_texp", toString(g_texp, 16)));

    calibrationLog.addKeyword(PvlKeyword("CalibrationUnits", g_calStep));

    calibrationLog.addKeyword(PvlKeyword("RadianceScaleFactor", toString(g_iofScale, 16)));
    calibrationLog.addKeyword(PvlKeyword("SolarFlux", toString(g_solarFlux, 16)));  
    calibrationLog.addKeyword(PvlKeyword("Units", units));
    PvlKeyword linearityCoefs("LinearityCoefficients");
    linearityCoefs.addValue(toString(g_L[0], 16));
    linearityCoefs.addValue(toString(g_L[1], 16));
    linearityCoefs.addValue(toString(g_L[2], 16));
    calibrationLog.addKeyword(linearityCoefs);

    PvlKeyword darkCurrentCoefs("DarkCurrentCoefficients");
    darkCurrentCoefs.addValue(toString(g_d0, 16));
    darkCurrentCoefs.addValue(toString(g_d1, 16));
    calibrationLog.addKeyword(darkCurrentCoefs);

    PvlKeyword darkCurrent("DarkCurrent");
    darkCurrent.addValue(toString(g_darkCurrent, 16));
    calibrationLog.addKeyword(darkCurrent);

    // Write Calibration group to output file
    ocube->putGroup(calibrationLog);
    log->addGroup(calibrationLog);
    p.EndProcess();

  }
}
