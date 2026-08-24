#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include <QDir>
#include <QRegExp>
#include <QString>

#include "Brick.h"
#include "Buffer.h"
#include "Camera.h"
#include "Constants.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "iTime.h"
#include "Message.h"
#include "NaifStatus.h"
#include "Preference.h"
#include "ProcessByBrick.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "spiceql.h"
#include "SpecialPixel.h"
#include "Statistics.h"

#include "lrowaccal.h"

namespace Isis {
  namespace LroWacCal {
    void CorrectRadiometric(Buffer &out, double exposure, double solarDistance, bool iof,
                            const std::vector<double> &iofResponsivity,
                            const std::vector<double> &radianceResponsivity) {
      for (int i = 0; i < out.size(); i++) {
        if (IsSpecial(out[i])) {
          out[i] = Isis::Null;
        }
        else {
          out[i] /= exposure;
          if (iof) {
            out[i] *= std::pow(solarDistance, 2) / iofResponsivity[out.Band(i) - 1];
          }
          else {
            out[i] /= radianceResponsivity[out.Band(i) - 1];
          }
        }
      }
    }

    void CorrectSpecialPixels(const Buffer &in, Buffer &out, int correctBand, int frame, int frameHeight,
                              int frameSize, Buffer *specpixCube) {
      for (int b = 0; b < in.BandDimension(); b++) {
        // We find the index of the corresponding specpix frame band as the offset
        int offset = 0;
        if (correctBand != -1) {
          offset = specpixCube->Index(1, frameHeight * std::min(frame, (specpixCube->LineDimension() - 1) / frameHeight) + 1, correctBand);
        }
        else {
          offset = specpixCube->Index(1, frameHeight * std::min(frame, (specpixCube->LineDimension() - 1) / frameHeight) + 1, b + 1);
        }

        for (int i = 0; i < frameSize; i++) {
          if (IsSpecial((*specpixCube)[offset + i])) {
            out[i + b * frameSize] = (*specpixCube)[offset + i];
          }
        }
      }
    }

    void CorrectTemperature(Buffer &out, int correctBand, double frameTemp,
                            const std::array<std::array<double, 2>, 7> &temperatureConstants) {
      for (int i = 0; i < out.size(); i++) {
        if (IsSpecial(out[i])) {
          out[i] = Isis::Null;
        }
        else {
          // Temperature Correction Formula
          //
          //       inputPixel
          //  ---------------------
          //    a*(frameTemp) + b
          //
          // Where:
          //  'a' and 'b' are band-dependent constants read in via a pvl file
          //
          //  AND
          //
          // frameTemp: (Pre-calculated as it is used in multiple places)
          //
          //    (WAC end temp - WAC start temp)
          //    -------------------------------   *   frame   +   WAC start temp
          //         (WAC num framelets)
          //
          //
          //
          if (correctBand != -1) {
            out[i] = out[i] / (temperatureConstants[correctBand - 1][0] * frameTemp + temperatureConstants[correctBand - 1][1]);
          }
          else {
            out[i] = out[i] / (temperatureConstants[out.Band(i) - 1][0] * frameTemp + temperatureConstants[out.Band(i) - 1][1]);
          }
        }
      }
    }
  }

  /**
   * @brief Calibrate a WAC cube. 
   *
   * This is the programmatic interface to the lrowaccal application.
   * 
   * @param ui the User Interface to parse the parameters from
   */
  void lrowaccal(UserInterface &ui) {
    std::unique_ptr<Cube> icube = std::make_unique<Cube>();
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube->setVirtualBands(inAtt.bands());
    }
    icube->open(ui.GetCubeName("FROM"));

    lrowaccal(icube.get(), ui);
  }

  /**
   * @brief Calibrate a WAC cube.
   *
   * This is the programmatic interface to the lrowaccal application.
   * 
   * @param cube input cube to be calibrated
   * @param ui the User Interface to parse the parameters from
   */
  void lrowaccal(Cube *icube, UserInterface &ui) {
    LroWacCal::CalParams calParams;
    std::vector<double> iofResponsivity;
    std::vector<double> radianceResponsivity;
    std::array<std::array<double, 2>, 7> temperatureConstants = {{{{0, 0}},
                                                                  {{0, 0}},
                                                                  {{0, 0}},
                                                                  {{0, 0}},
                                                                  {{0, 0}},
                                                                  {{0, 0}},
                                                                  {{0, 0}}}};

    double temp1 = 0.0;
    double temp2 = 0.0;

    int numFrames = 0;

    std::vector<int> bands;

    Buffer *darkCube1 = nullptr;
    Buffer *darkCube2 = nullptr;
    Buffer *flatCube = nullptr;
    Buffer *specpixCube = nullptr;

    calParams.dark = ui.GetBoolean("DARK");
    calParams.flatfield = ui.GetBoolean("FLATFIELD");
    calParams.radiometric = ui.GetBoolean("RADIOMETRIC");
    calParams.iof = (ui.GetString("RADIOMETRICTYPE") == "IOF");
    calParams.specpix = ui.GetBoolean("SPECIALPIXELS");
    calParams.temperature = ui.GetBoolean("TEMPERATURE");

    std::vector<QString> darkFiles;
    ui.GetAsString("DARKFILE", darkFiles);
    QString flatFile = ui.GetAsString("FLATFIELDFILE");
    QString radFile = ui.GetAsString("RADIOMETRICFILE");
    QString specpixFile = ui.GetAsString("SPECIALPIXELSFILE");
    QString tempFile = ui.GetAsString("TEMPERATUREFILE");

    //
    // Start processing code
    //
    ProcessByBrick p;
    p.SetInputCube(icube);

    // Make sure it is a WAC cube
    PvlGroup &inst = icube->label()->findGroup("Instrument", Pvl::Traverse);
    QString instId = (QString)inst["InstrumentId"];
    instId = instId.toUpper();
    if (instId != "WAC-VIS" && instId != "WAC-UV") {
      QString msg = "This program is intended for use on LROC WAC images only. [";
      msg += icube->fileName() + "] does not appear to be a WAC image.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // And check if it has already run through calibration
    if (icube->label()->findObject("IsisCube").hasGroup("Radiometry")) {
      QString msg = "This image has already been calibrated";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (icube->label()->findObject("IsisCube").hasGroup("AlphaCube")) {
      QString msg = "This application can not be run on any image that has been geometrically transformed (i.e. scaled, rotated, sheared, or reflected) or cropped.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Determine the dark/flat files to use
    QString offset = (QString)inst["BackgroundOffset"];
    QString mode = (QString)inst["Mode"];
    QString instModeId = (QString)inst["InstrumentModeId"];
    instModeId = instModeId.toUpper();

    if (instModeId == "COLOR" && (QString)inst["InstrumentId"] == "WAC-UV") {
      instModeId = "UV";
    }
    else if (instModeId == "VIS") {
      instModeId = "COLOR";
    }

    double startTemperature = (double)inst["BeginTemperatureFpa"];
    double endTemperature = (double)inst["EndTemperatureFpa"];

    numFrames = (int)inst["NumFramelets"];

    // Figure out which bands are input
    for (int i = 1; i <= icube->bandCount(); i++) {
      bands.push_back(icube->physicalBand(i));
    }
    if (bands.size() < 1 || bands.size() > 7) {
      QString msg = QString("This program is intended for use on LROC WAC images only.");
      msg += "Expected 1 to 7 bands, but got [" + toString(static_cast<int>(bands.size())) + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    PvlGroup &bandBin = icube->label()->findGroup("BandBin", Pvl::Traverse);
    QString filter = (QString)bandBin["Center"][0];
    QString filterNum = (QString)bandBin["FilterNumber"][0];
    // We have to pay special attention in case we are passed a
    // single band image that has been "exploded" from a multiband wac
    if (instModeId == "COLOR" && bands.size() == 1) {
      bands[0] = (toInt(filterNum) - 2);
    }
    else if (instModeId == "UV" && bands.size() == 1) {
      bands[0] = (toInt(filterNum));
    }

    if (calParams.dark) {
      if (darkFiles.size() == 0 || darkFiles[0] == "Default" || darkFiles[0].length() == 0) {
        darkFiles.resize(2);
        double temp = (double)inst["MiddleTemperatureFpa"];
        double time = iTime(inst["StartTime"][0]).Et();
        QString darkFile = LroWacCal::GetCalibrationDirectory("wac_darks") + "WAC_" + instModeId;
        if (instModeId == "BW") {
          darkFile += "_" + filter + "_Mode" + mode;
        }
        darkFile += "_Offset" + offset + "_*C_*T_Dark.????.cub";
        LroWacCal::GetDark(darkFile, temp, time, darkCube1, darkCube2, temp1,
                temp2, darkFiles[0], darkFiles[1]);
      }
      else if (darkFiles.size() == 1) {
        LroWacCal::CopyCubeIntoBuffer(darkFiles[0], darkCube1);
        temp1 = 0.0;
        darkCube2 = new Buffer(*darkCube1);
        temp2 = temp1;
      }
      else {
        LroWacCal::CopyCubeIntoBuffer(darkFiles[0], darkCube1);
        int index = darkFiles[0].lastIndexOf("_");
        temp1 = IString(darkFiles[0].mid(darkFiles[0].lastIndexOf("_", index - 1), index)).ToDouble();
        LroWacCal::CopyCubeIntoBuffer(darkFiles[1], darkCube2);
        index = darkFiles[1].lastIndexOf("_");
        temp2 = IString(darkFiles[1].mid(darkFiles[1].lastIndexOf("_", index - 1), index)).ToDouble();
      }
    }

    if (calParams.flatfield) {
      if (flatFile.toLower() == "default" || flatFile.length() == 0) {
        flatFile = LroWacCal::GetCalibrationDirectory("wac_flats") + "WAC_" + instModeId;
        if (instModeId == "BW")
          flatFile += "_" + filter + "_Mode" + mode;
        flatFile += "_Flatfield.????.cub";
      }
      LroWacCal::CopyCubeIntoBuffer(flatFile, flatCube);

      // invert the flat-field data here so we don't have to divide for every pixel of the wac
      for (int i = 0; i < flatCube->size(); i++) {
        (*flatCube)[i] = 1.0 / (*flatCube)[i];
      }
    }

    PvlKeyword responsivity;

    if (calParams.radiometric) {
      PvlKeyword &filterNumStrings = icube->label()->findGroup("BandBin", Pvl::Traverse).findKeyword("FilterNumber");

      if (radFile.toLower() == "default" || radFile.length() == 0)
        radFile = LroWacCal::GetCalibrationDirectory("") + "WAC_RadiometricResponsivity.????.pvl";

      FileName radFileName(radFile);
      if (radFileName.isVersioned()) {
        radFileName = radFileName.highestVersion();
      }
      if (!radFileName.fileExists()) {
        QString msg = radFile + " does not exist.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      Pvl radPvl(radFileName.expanded());

      if (calParams.iof) {
        responsivity = radPvl["IOF"];

        for (int i = 0; i < filterNumStrings.size(); i++) {
          iofResponsivity.push_back(toDouble(responsivity[toInt(filterNumStrings[i]) - 1]));
        }

        try {
          Camera *cam = nullptr;
          cam = icube->camera();
          iTime startTime((QString)inst["StartTime"]);
          cam->setTime(startTime);
          camParams.solarDistance = cam->sunToBodyDist() / KM_PER_AU;
        }
        catch(IException &e) {
          try {
            bool useWeb = Preference::Preferences().useWebSpice();

            iTime startTime((QString)inst["StartTime"]);
            double etStart = startTime.Et();
            // Get the distance between the Moon and the Sun at the given time in
            // Astronomical Units (AU)
            std::array<double, 6> sunpos;

            std::vector<double> etStartVec = {etStart};
            std::vector<std::string> kernels_to_use = {"/moon/tspk/moon_pa_de421_1900-2050.bpc", "/lro/tspk/de421.bsp", "/base/pck/pck[0-9]{5}.tpc", "/moon/pck/moon_080317.tf", "/moon/pck/moon_assoc_me.tf"};
            auto [sunLt, kernels] = SpiceQL::getTargetStates(etStartVec, "sun", "moon", "MOON_ME", "LT+S", "lroc", {"reconstructed"}, {"reconstructed"}, useWeb, true, false, -1, 1, kernels_to_use);
            std::copy(sunLt[0].begin(), sunLt[0].begin() + 6, sunpos.begin());

            calParams.solarDistance = vnorm_c(sunpos.data()) / KM_PER_AU;
          }
          catch (IException &e) {
            QString msg = "Cannot find necessary SPICE kernels for converting to IOF";
            throw IException(e, IException::User, msg, _FILEINFO_);
          }
        }
      }
      else {
        responsivity = radPvl["Radiance"];
        for (int i = 0; i < filterNumStrings.size(); i++) {
          radianceResponsivity.push_back(toDouble(responsivity[toInt(filterNumStrings[i]) - 1]));
        }
      }
    }

    if (calParams.specpix) {
      if (specpixFile.toLower() == "default" || specpixFile.length() == 0) {
        specpixFile = LroWacCal::GetCalibrationDirectory("wac_masks") + "WAC_" + instModeId;
        double temp = (double)inst["MiddleTemperatureFpa"];
        if (instModeId == "BW") {
          specpixFile += "_" + filter + "_Mode" + mode;
        }
        specpixFile += "_*C_SpecialPixels.????.cub";
        LroWacCal::GetMask(specpixFile, temp, specpixCube);
      }
      else {
        LroWacCal::CopyCubeIntoBuffer(specpixFile, specpixCube);
      }
    }

    PvlKeyword temperaturePvl("TemperatureFile");
    if (calParams.temperature) {
      if (tempFile.toLower() == "default" || tempFile.length() == 0)
        tempFile = LroWacCal::GetCalibrationDirectory("") + "WAC_TemperatureConstants.????.pvl";

      FileName tempFileName(tempFile);
      if (tempFileName.isVersioned()) {
        tempFileName = tempFileName.highestVersion();
      }
      if (!tempFileName.fileExists()) {
        QString msg = tempFile + " does not exist.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      PvlKeyword &filterNumStrings = icube->label()->findGroup("BandBin", Pvl::Traverse).findKeyword("FilterNumber");
      Pvl tempPvl(tempFileName.expanded());
      temperaturePvl.addValue(tempFileName.expanded());
      for (int b = 0; b < filterNumStrings.size(); b++) {
        temperatureConstants[bands[b] - 1][0] = toDouble(tempPvl[filterNumStrings[b]][0]);
        temperatureConstants[bands[b] - 1][1] = toDouble(tempPvl[filterNumStrings[b]][1]);
      }
    }

    if (instModeId == "BW") {
      if (mode == "1" || mode == "0") {
        p.SetBrickSize(NO_POLAR_MODE_SAMPLES, VIS_LINES, std::min(BW_BANDS, static_cast<int>(bands.size())));
      }
      else {
        p.SetBrickSize(POLAR_MODE_SAMPLES, VIS_LINES, std::min(BW_BANDS, static_cast<int>(bands.size())));
      }
    }
    else if (instModeId == "COLOR") {
      p.SetBrickSize(NO_POLAR_MODE_SAMPLES, VIS_LINES, std::min(COLOR_BANDS, static_cast<int>(bands.size())));
    }
    else if (instModeId == "UV") {
      p.SetBrickSize(UV_SAMPLES, UV_LINES, std::min(UV_BANDS, static_cast<int>(bands.size())));
    }

    calParams.exposure = inst["ExposureDuration"];

    // Calibrate each framelet
    auto Calibrate = [calParams, &bands, startTemperature, endTemperature, darkCube1,
                      darkCube2, flatCube, specpixCube, &temperatureConstants, numFrames,
                      temp1, temp2, &iofResponsivity, &radianceResponsivity](Buffer &inCube, Buffer &outCube) -> void {
      int correctBand = -1;
      // If we are passed in a single band (img.cub+4) we need to pay special attention that we don't start with band1
      if (inCube.BandDimension() == 1 && bands.size() == 1) {
        correctBand = bands.front();
      }

      int frameHeight = inCube.LineDimension();
      int frameSize = inCube.SampleDimension() * inCube.LineDimension();
      int frame = inCube.Line() / frameHeight;

      // Calculate a temperature factor for the current frame (this is done to avoid doing this for each pixel
      // Used in dark and temperature correction
      // frameTemp:
      //
      //    (WAC end temp - WAC start temp)
      //    -------------------------------   *   frame   +   WAC start temp
      //         (WAC num framelets)
      double frameTemp = (endTemperature - startTemperature) / numFrames * frame + startTemperature;

      for (int i = 0; i < outCube.size(); i++) {
        outCube[i] = inCube[i];
      }

      if (calParams.dark) {
        LroWacCal::CorrectDark(inCube, outCube, correctBand, startTemperature, endTemperature,
                    frame, frameHeight, frameSize, numFrames, frameTemp, darkCube1,
                    temp1, darkCube2, temp2);
      }

      if (calParams.flatfield) {
        LroWacCal::CorrectFlatfield(inCube, outCube, correctBand, frame, frameHeight, frameSize,
                         flatCube);
      }

      if (calParams.radiometric) {
        LroWacCal::CorrectRadiometric(outCube, calParams.exposure, calParams.solarDistance, calParams.iof,
                           iofResponsivity, radianceResponsivity);
      }

      if (calParams.specpix) {
        LroWacCal::CorrectSpecialPixels(inCube, outCube, correctBand, frame, frameHeight,
                             frameSize, specpixCube);
      }

      if (calParams.temperature) {
        LroWacCal::CorrectTemperature(outCube, correctBand, frameTemp, temperatureConstants);
      }
    };

    Cube *ocube = nullptr;
    ocube = p.SetOutputCube(ui.GetCubeName("TO"), ui.GetOutputAttribute("TO"));
    p.ProcessCube(Calibrate, false);

    // Add an output group with the appropriate information
    PvlGroup calgrp("Radiometry");
    if (calParams.temperature) {
      calgrp += PvlKeyword("TemperatureFile", temperaturePvl);
    }
    if (calParams.dark) {
      PvlKeyword darks("DarkFiles");
      darks.addValue(darkFiles[0]);
      if (darkFiles.size() > 1)
        darks.addValue(darkFiles[1]);
      calgrp += darks;
    }
    if (calParams.flatfield) {
      calgrp += PvlKeyword("FlatFile", flatFile);
    }
    if (calParams.radiometric) {
      PvlKeyword vals("ResponsivityValues");
      if (calParams.iof) {
        calgrp += PvlKeyword("RadiometricType", "IOF");
        for (auto i = 0UL; i < iofResponsivity.size(); i++) {
          vals.addValue(toString(iofResponsivity[i]));
        }
      }
      else {
        calgrp += PvlKeyword("RadiometricType", "AbsoluteRadiance", "W/m2/sr/um");
        for (auto i = 0UL; i < radianceResponsivity.size(); i++) {
          vals.addValue(toString(radianceResponsivity[i]));
        }
      }
      calgrp += vals;
      calgrp += PvlKeyword("SolarDistance", toString(calParams.solarDistance));
    }
    if (calParams.specpix) {
      calgrp += PvlKeyword("SpecialPixelsFile", specpixFile);
    }

    ocube->putGroup(calgrp);

    delete darkCube1;
    darkCube1 = nullptr;
    delete darkCube2;
    darkCube2 = nullptr;
    delete flatCube;
    flatCube = nullptr;
    delete specpixCube;
    specpixCube = nullptr;
  }

  namespace LroWacCal {
    void CopyCubeIntoBuffer(QString &fileString, Buffer *&data) {
      Cube cube;
      FileName filename(fileString);
      if (filename.isVersioned()) {
        filename = filename.highestVersion();
      }

      if (!filename.fileExists()) {
        QString msg = fileString + " does not exist.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      cube.open(filename.expanded());
      Brick brick(cube.sampleCount(), cube.lineCount(), cube.bandCount(), cube.pixelType());
      brick.SetBasePosition(1, 1, 1);
      cube.read(brick);

      data = nullptr;
      data = new Buffer(brick);

      fileString = filename.expanded();
    }

    void GetDark(const QString &fileString, double temp, double time, Buffer *&data1, Buffer *&data2, double &temp1,
                 double &temp2, QString &file1, QString &file2) {
      FileName filename(fileString);
      QString basename = FileName(filename.baseName()).baseName(); // We do it twice to remove the ".????.cub"

      // create a regular expression to capture the temp and time from filenames
      QString regexPattern(basename);
      regexPattern.replace("*", "([0-9\\.-]*)");
      QRegExp regex(regexPattern);

      // create a filter for the QDir to only load files matching our name
      QString filter(basename);
      filter.append(".*");

      // get a list of dark files that match our basename
      QDir dir(filename.path(), filter);

      std::vector<DarkFileInfo> darkFiles;
      darkFiles.reserve(dir.count());

      // Loop through all files in the dir that match our basename and extract time and temp
      for (auto indx = 0U; indx < dir.count(); indx++) {
        // match against our regular expression
        int pos = regex.indexIn(dir[indx]);
        if (pos == -1) {
          continue; // filename did not match basename regex (time or temp contain non-digit)
        }

        // Get a list of regex matches. Item 0 should be the full QString, item 1
        // is temp and item 2 is time.
        QStringList texts = regex.capturedTexts();
        if (texts.size() < 3) {
          continue; // could not find time and/or temp
        }

        // extract time/temp from regex texts
        bool tempOK, timeOK;
        double fileTemp = texts[1].toDouble(&tempOK);
        int fileTime = texts[2].toInt(&timeOK);
        if (!tempOK || !timeOK) {
          continue; // time or temp was not a valid numeric value
        }

        DarkFileInfo info(fileTemp, fileTime);
        darkFiles.push_back(info);
      }

      // we require at least 2 different dark files to interpolate/extrapolate
      if (darkFiles.size() < 2) {
        QString msg = "Not enough Dark files exist for these image options [" + basename + "]. Need at least 2 files with different temperatures\n";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      // sort the files by distance from wac temp and time
      DarkComp darkComp(temp, static_cast<int>(time));
      sort(darkFiles.begin(), darkFiles.end(), darkComp);

      std::size_t temp1Index = 0;
      std::size_t temp2Index = 0;

      temp1 = darkFiles[temp1Index].temp;

      for (temp2Index = temp1Index + 1; temp2Index < darkFiles.size(); temp2Index++) {
        if (darkFiles[temp2Index].temp != temp1) {
          break;
        }
      }

      if (temp2Index >= darkFiles.size()) {
        temp2Index = 1;
      }

      temp2 = darkFiles[temp2Index].temp;

      int time1 = darkFiles[temp1Index].time;
      int time2 = darkFiles[temp2Index].time;

      int tempIndex = fileString.indexOf("*C");
      int timeIndex = fileString.indexOf("*T");

      file1 = fileString;
      file1.replace(timeIndex, 1, toString(time1));
      file1.replace(tempIndex, 1, toString(static_cast<int>(temp1)));

      file2 = fileString;
      file2.replace(timeIndex, 1, toString(time2));
      file2.replace(tempIndex, 1, toString(static_cast<int>(temp2)));

      CopyCubeIntoBuffer(file1, data1);
      CopyCubeIntoBuffer(file2, data2);
    }

    void GetMask(QString &fileString, double temp, Buffer *&data) {
      FileName filename(fileString);
      QString basename = FileName(filename.baseName()).baseName(); // We do it twice to remove the ".????.cub"

      int index = basename.indexOf("*");

      // create a filter for the QDir to only load files matching our name
      QString filter(basename);
      filter.append(".*");

      QDir dir(filename.path(), filter);

      // create a regular expression to capture the temp and time from filenames
      QString regexPattern(basename);
      regexPattern.replace("*", "([0-9\\.-]*)");
      QRegExp regex(regexPattern);

      double bestTemp = std::numeric_limits<double>::max();
      for (auto indx = 0U; indx < dir.count(); indx++) {
        // match against our regular expression
        int pos = regex.indexIn(dir[indx]);
        if (pos == -1) {
          continue; // filename did not match basename regex (temp contain non-digit)
        }

        // Get a list of regex matches. Item 0 should be the full QString, item 1 is temp
        QStringList texts = regex.capturedTexts();
        if (texts.size() < 2) {
          continue; // could not find temp
        }

        // extract time/temp from regex texts
        bool tempOK;
        double fileTemp = texts[1].toDouble(&tempOK);
        if (!tempOK) {
          continue; // temp was not a valid numeric value
        }

        if (std::abs(temp - fileTemp) < std::abs(temp - bestTemp)) {
          bestTemp = fileTemp;
        }
      }

      if (bestTemp == std::numeric_limits<double>::max()) {
        QString msg = "No files exist for these mask options [" + basename + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      index = fileString.indexOf("*");
      fileString.replace(index, 1, toString(static_cast<int>(bestTemp)));

      CopyCubeIntoBuffer(fileString, data);
    }

    QString GetCalibrationDirectory(QString calibrationType) {
      // Get the directory where the CISS calibration directories are.
      PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
      QString missionDir = (QString)dataDir["LRO"];
      if (calibrationType != "") {
        calibrationType += "/";
      }

      return missionDir + "/calibration/" + calibrationType;
    }

    void CorrectDark(const Buffer &in, Buffer &out, int correctBand, double startTemp, double endTemp, int frame,
                     int frameHeight, int frameSize, int numFrames, double frameTemp, Buffer *darkCube1, double temp1,
                     Buffer *darkCube2, double temp2) {
      const double tempFactor = (frameTemp - temp2) / (temp1 - temp2);

      for (int b = 0; b < in.BandDimension(); b++) {
        // We find the index of the corresponding dark frame band as the offset
        int offset = 0;
        if (correctBand != -1) {
          offset = darkCube1->Index(1, frameHeight * std::min(frame, darkCube1->LineDimension() / frameHeight - 1) + 1, correctBand);
        }
        else {
          offset = darkCube1->Index(1, frameHeight * std::min(frame, darkCube1->LineDimension() / frameHeight - 1) + 1, b + 1);
        }

        // We're bypassing Buffer::at for speed, so we need to make sure our
        // index will not overrun the buffer
        if (offset + frameSize > darkCube1->size()) {
          QString message = Message::ArraySubscriptNotInRange(offset + frameSize) + " (Dark cube 1)";
          throw IException(IException::Programmer, message, _FILEINFO_);
        }
        if (offset + frameSize > darkCube2->size()) {
          QString message = Message::ArraySubscriptNotInRange(offset + frameSize) + " (Dark cube 2)";
          throw IException(IException::Programmer, message, _FILEINFO_);
        }

        for (int i = 0; i < frameSize; i++) {
          double dark1Pixel = (*darkCube1)[offset + i];
          double dark2Pixel = (*darkCube2)[offset + i];
          double &outputPixel = out[i + b * frameSize];
          // Interpolate between the two darks with the current temperature
          if (!IsSpecial(dark1Pixel) && !IsSpecial(dark2Pixel) && !IsSpecial(outputPixel)) {
            if (temp1 != temp2) {
              // Dark correction formula:
              //
              //    (dark1Pixel - dark2Pixel)
              //    -------------------------   *   (frameTemp - dark2Temp)   +   dark2Pixel
              //     (dark1Temp - dark2Temp)
              //
              // frameTemp:
              //
              //    (WAC end temp - WAC start temp)
              //    -------------------------------   *   frame   +   WAC start temp
              //         (WAC num framelets)
              //
              // tempFactor (calculated outside the loops for speed):
              //
              //    (frameTemp - dark2Temp)
              //    -----------------------
              //    (dark1Temp - dark2Temp)
              //
              outputPixel -= (dark1Pixel - dark2Pixel) * tempFactor + dark2Pixel;
            }
            else {
              outputPixel -= dark1Pixel;
            }
          }
          else {
            outputPixel = Isis::Null;
          }
        }
      }
    }

    void CorrectFlatfield(const Buffer &in, Buffer &out, int correctBand, int frame, int frameHeight, int frameSize,
                          Buffer *flatCube) {
      for (int b = 0; b < in.BandDimension(); b++) {
        // We find the index of the corresponding flat frame band as the offset
        int offset = 0;
        if (correctBand != -1) {
          offset = flatCube->Index(1, frameHeight * std::min(frame, (flatCube->LineDimension() - 1) / frameHeight) + 1, correctBand);
        }
        else {
          offset = flatCube->Index(1, frameHeight * std::min(frame, (flatCube->LineDimension() - 1) / frameHeight) + 1, b + 1);
        }

        // We're bypassing Buffer::at for speed, so we need to make sure our
        // index will not overrun the buffer
        if (offset + frameSize > flatCube->size()) {
          QString message = Message::ArraySubscriptNotInRange(offset + frameSize) + " (Flat-field cube)";
          throw IException(IException::Programmer, message, _FILEINFO_);
        }

        const int outFrameOffset = b * frameSize;
        for (int i = 0; i < frameSize; i++) {
          const double flatPixel = (*flatCube)[offset + i];
          double &outputPixel = out[i + outFrameOffset];

          if (flatPixel > 0.0 && !IsSpecial(flatPixel) && !IsSpecial(outputPixel)) {
            outputPixel *= flatPixel; // The flat-field data was inverted during load so we don't have to divide here.
          }
          else {
            outputPixel = Isis::Null;
          }
        }
      }
    }
  }
}


