#include "Isis.h"

#include <vector>
#include <stdio.h>

#include <QFile>

#include "Camera.h"
#include "EndianSwapper.h"
#include "IException.h"
#include "IString.h"
#include "LeastSquares.h"
#include "LineManager.h"
#include "PolynomialUnivariate.h"
#include "ProcessByLine.h"
#include "ProgramLauncher.h"
#include "Statistics.h"
#include "Table.h"
#include "UserInterface.h"

using namespace Isis;
using namespace std;

//! map from <sample, band> to dark correction value
map< pair<int, int>, double > sampleBasedDarkCorrections;

//! map from <line, band> to dark correction value
map< pair<int, int>, double > lineBasedDarkCorrections;

//! specific energy corrections for each band of the cube
vector<double> specificEnergyCorrections;

//! list of temp files that need deleted
vector<QString> tempFiles;

//! solar remove coefficient
double solarRemoveCoefficient;

//! Output in I/F units
bool iof;

void calculateDarkCurrent(Cube *);
void calculateVisDarkCurrent(Cube *);
void calculateIrDarkCurrent(Cube *);

void chooseFlatFile(Cube *, ProcessByLine *);

void calculateSpecificEnergy(Cube *);
void calculateSolarRemove(Cube *, ProcessByLine *);

void calibrate(vector<Buffer *> &in, vector<Buffer *> &out);

QString createCroppedFile(Cube *icube, QString cubeFileName, bool flatFile = false);
void GetOffsets(const Pvl &lab, int &finalSampOffset, int &finalLineOffset);

// This is the results group
PvlGroup calibInfo;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  tempFiles.clear();
  specificEnergyCorrections.clear();
  sampleBasedDarkCorrections.clear();
  lineBasedDarkCorrections.clear();
  solarRemoveCoefficient = 1.0;
  iof = (ui.GetString("UNITS") == "IOF");

  calibInfo = PvlGroup("Results");

  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");

  bool isVims = true;

  try {
    isVims = (icube->label()->FindGroup("Instrument",
              Pvl::Traverse)["InstrumentId"][0] == "VIMS");
  }
  catch(IException &e) {
    isVims = false;
  }

  if(!isVims) {
    QString msg = "The input cube [" + QString(ui.GetAsString("FROM")) + "] is not a Cassini VIMS cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(icube->label()->FindObject("IsisCube").HasGroup("AlphaCube")) {
    QString msg = "The input cube [" + QString(ui.GetAsString("FROM")) + "] has had its dimensions modified and can not be calibrated";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // done first since it's likely to cause an error if one exists
  calculateSolarRemove(icube, &p);

  if(ui.GetBoolean("DARK"))
    calculateDarkCurrent(icube);

  chooseFlatFile(icube, &p);
  calculateSpecificEnergy(icube);

  calibInfo += PvlKeyword("OutputUnits", ((iof) ? "I/F" : "Specific Energy"));

  Application::Log(calibInfo);

  p.SetOutputCube("TO");
  p.StartProcess(calibrate);
  p.EndProcess();

  for(unsigned int i = 0; i < tempFiles.size(); i++) {
    QFile::remove(tempFiles[i]);
  }

  tempFiles.clear();
}

/**
 * This applies the calculated calibration coefficients to the file.
 *
 * @param inBuffers
 * @param outBuffers
 */
void calibrate(vector<Buffer *> &inBuffers, vector<Buffer *> &outBuffers) {
  Buffer *inBuffer = inBuffers[0];
  Buffer *flatFieldBuffer = inBuffers[1];
  Buffer *solarRemoveBuffer = NULL; // this is optional

  if(inBuffers.size() > 2) {
    inBuffer = inBuffers[0];
    solarRemoveBuffer = inBuffers[1];
    flatFieldBuffer = inBuffers[2];
  }

  Buffer *outBuffer = outBuffers[0];

  for(int i = 0; i < inBuffer->size(); i++) {
    (*outBuffer)[i] = (*inBuffer)[i];

    if(IsSpecial((*outBuffer)[i])) continue;

    map< pair<int, int>, double>::iterator darkCorrection =
      sampleBasedDarkCorrections.find(pair<int, int>(i + 1, inBuffer->Band()));

    if(darkCorrection != sampleBasedDarkCorrections.end()) {
      (*outBuffer)[i] -= darkCorrection->second;
    }

    darkCorrection = lineBasedDarkCorrections.find(pair<int, int>(inBuffer->Line(), inBuffer->Band()));

    if(darkCorrection != lineBasedDarkCorrections.end()) {
      if(!IsSpecial(darkCorrection->second))
        (*outBuffer)[i] -= darkCorrection->second;
      else
        (*outBuffer)[i] = Null;
    }

    if(!IsSpecial((*flatFieldBuffer)[i]) && !IsSpecial((*outBuffer)[i])) {
      (*outBuffer)[i] /= (*flatFieldBuffer)[i];
    }

    if(inBuffer->Band() <= (int)specificEnergyCorrections.size() && !IsSpecial((*outBuffer)[i])) {
      (*outBuffer)[i] *= specificEnergyCorrections[inBuffer->Band()-1];
    }

    if(iof && solarRemoveBuffer && !IsSpecial((*outBuffer)[i])) {
      (*outBuffer)[i] = (*outBuffer)[i] / ((*solarRemoveBuffer)[i] / solarRemoveCoefficient) * Isis::PI;
    }
  }
}

/**
 * This calculates the values necessary to convert from
 * specific energy to I/F. A cube is used as part of the
 * equation (which probably* just contains a vector of values)
 * so p->SetInputCube(...) will be called with the appropriate
 * filename.
 *
 * @param icube
 * @param p
 */
void calculateSolarRemove(Cube *icube, ProcessByLine *p) {
  UserInterface &ui = Application::GetUserInterface();
  if(ui.GetString("UNITS") != "IOF") return;

  Camera *cam = NULL;

  try {
    cam = icube->camera();
  }
  catch(IException &e) {
    QString msg = "Unable to create a camera model from [" +
                  icube->fileName() + "]. Please run "
                  "spiceinit on this file";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }


  solarRemoveCoefficient = -1;

  // try center first
  if(cam->SetImage(icube->sampleCount() / 2, icube->lineCount() / 2)) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  // try 4 corners
  if(solarRemoveCoefficient < 0 && cam->SetImage(1.0, 1.0)) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  if(solarRemoveCoefficient < 0 && cam->SetImage(icube->sampleCount(), 1.0)) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  if(solarRemoveCoefficient < 0 && cam->SetImage(icube->sampleCount(),
      icube->lineCount())) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  if(solarRemoveCoefficient < 0 && cam->SetImage(1.0, icube->lineCount())) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  // try center of 4 edges
  if(solarRemoveCoefficient < 0 && cam->SetImage(icube->sampleCount() / 2, 1.0)) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  if(solarRemoveCoefficient < 0 && cam->SetImage(icube->sampleCount(),
      icube->lineCount() / 2)) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  if(solarRemoveCoefficient < 0 && cam->SetImage(icube->sampleCount() / 2,
      icube->lineCount())) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  if(solarRemoveCoefficient < 0 && cam->SetImage(1.0, icube->lineCount() / 2)) {
    solarRemoveCoefficient = cam->SolarDistance() * cam->SolarDistance();
  }

  // Default to original vimscal's solar distance if we can't find a target
  if(solarRemoveCoefficient < 0) {
    solarRemoveCoefficient = 81.595089;
    /*
    QString msg = "Unable to project image at four corners, center of edges or ";
    msg += "at center. The solar distance can not be calculated, try using";
    msg += " [UNITS=SPECENERGY] on [";
    msg += icube->FileName() + "]";
    throw iException::Message(iException::Camera, msg, _FILEINFO_);
    */
  }

  bool vis = (icube->label()->
              FindGroup("Instrument", Pvl::Traverse)["Channel"][0] != "IR");

  QString attributes;

  // vis is bands 1-96, ir is bands 97-352 in this calibration file
  if(vis) {
    attributes = "+1-96";
  }
  else {
    attributes = "+97-352";
  }

  CubeAttributeInput iatt(attributes);

  FileName solarFileName("$cassini/calibration/vims/solar_v????.cub");
  solarFileName = solarFileName.highestVersion();

  p->SetInputCube(createCroppedFile(icube, solarFileName.expanded()), iatt);
}


/**
 * This calculates the coefficients for specific energy corrections
 */
void calculateSpecificEnergy(Cube *icube) {
  PvlGroup &inst = icube->label()->FindGroup("Instrument", Pvl::Traverse);
  bool vis = (inst["Channel"][0] != "IR");

  double coefficient = 1.0;

  if(inst["GainMode"][0] == "HIGH") {
    coefficient /= 2;
  }

  if(vis && inst["SamplingMode"][0] == "HI-RES") {
    coefficient *= 3;
  }

  if(vis) {
    coefficient /= toDouble(inst["ExposureDuration"][1]) / 1000.0;
  }
  else {
    coefficient /= (toDouble(inst["ExposureDuration"][0])) / 1000.0 - 0.004;
  }

  QString specEnergyFile = "$cassini/calibration/vims/";

  if(vis) {
    specEnergyFile += "vis_perf_v????.cub";
  }
  else {
    specEnergyFile += "ir_perf_v????.cub";
  }

  QString waveCalFile = "$cassini/calibration/vims/wavecal_v????.cub";

  FileName specEnergyFileName(specEnergyFile);
  specEnergyFileName = specEnergyFileName.highestVersion();

  FileName waveCalFileName(waveCalFile);
  waveCalFileName = waveCalFileName.highestVersion();

  Cube specEnergyCube;
  specEnergyCube.open(specEnergyFileName.expanded());

  Cube waveCalCube;
  waveCalCube.open(waveCalFileName.expanded());

  LineManager specEnergyMgr(specEnergyCube);
  LineManager waveCalMgr(waveCalCube);

  for(int i = 0; i < icube->bandCount(); i++) {
    Statistics specEnergyStats;
    Statistics waveCalStats;

    if(vis) {
      specEnergyMgr.SetLine(1, i + 1);
      waveCalMgr.SetLine(1, i + 1);
    }
    else {
      specEnergyMgr.SetLine(1, i + 1);
      // ir starts at band 97
      waveCalMgr.SetLine(1, i + 96 + 1);
    }

    specEnergyCube.read(specEnergyMgr);
    waveCalCube.read(waveCalMgr);

    specEnergyStats.AddData(specEnergyMgr.DoubleBuffer(), specEnergyMgr.size());
    waveCalStats.AddData(waveCalMgr.DoubleBuffer(), waveCalMgr.size());

    double bandCoefficient = coefficient * specEnergyStats.Average() * waveCalStats.Average();

    specificEnergyCorrections.push_back(bandCoefficient);
  }
}

/**
 * This decides if we have a VIS or IR dark current correction and calls the
 * appropriate method.
 *
 * @param currCube
 */
void calculateDarkCurrent(Cube *icube) {
  PvlGroup &inst = icube->label()->FindGroup("Instrument", Pvl::Traverse);
  bool vis = (inst["Channel"][0] != "IR");

  calibInfo += PvlKeyword("Vis", ((vis) ? "true" : "false"));

  if(vis) {
    calculateVisDarkCurrent(icube);
  }
  else {
    calculateIrDarkCurrent(icube);
  }
}

/**
 * This populates darkCorrections with the result of the equation:
 *   dark = a + x * b
 * for each line,band. a, b are from the "vis_*_dark_model.tab" files
 * and x is the ExposureDuration.
 *
 * @param icube
 */
void calculateVisDarkCurrent(Cube *icube) {
  PvlGroup &inst = icube->label()->FindGroup("Instrument", Pvl::Traverse);

  // This is the dark current corrections for VIS
  bool hires = ((inst["SamplingMode"][0] == "HIGH") || (inst["SamplingMode"][0] == "HI-RES"));
  QString calFile = "$cassini/calibration/vims/vis_";

  if(hires) {
    calFile += "hires";
  }
  else {
    calFile += "lowres";
  }

  calFile += "_dark_model_v????.tab";

  FileName calFileName(calFile);
  calFileName = calFileName.highestVersion();

  calibInfo += PvlKeyword("DarkCurrentFile", calFileName.originalPath() + "/" + calFileName.name());

  calFile = calFileName.expanded();

  EndianSwapper swapper("LSB");

  FILE *calFilePtr = fopen(calFile.toAscii().data(), "r");

  double visExposure = toDouble(inst["ExposureDuration"][1]);

  int sampleOffset, lineOffset;
  GetOffsets(*icube->label(), sampleOffset, lineOffset);

  /**
   * Reading in one parameter at a time:
   *   parameter 1 = constant coefficient
   *   parameter 2 = exposure coefficient
   *   param1 + param2*exposure = dark correction
   *
   * Do byte swapping where necessary.
   */
  for(int parameter = 1; parameter <= 2; parameter ++) {
    for(int band = 1; band <= 96; band++) {
      for(int sample = 1; sample <= 64; sample++) {
        float calData;

        if(fread(&calData, sizeof(calData), 1, calFilePtr) != 1) {
          // error!
          QString msg = "Error reading file [" + calFile + "]";
          throw IException(IException::Io, msg, _FILEINFO_);
        }

        int associatedSample = sample - sampleOffset + 1;

        calData = swapper.Float(&calData);
        pair<int, int> index = pair<int, int>(associatedSample, band);

        map< pair<int, int>, double>::iterator pos = sampleBasedDarkCorrections.find(index);
        if(pos == sampleBasedDarkCorrections.end()) {
          sampleBasedDarkCorrections.insert(pair< pair<int, int>, double>(index, calData));
        }
        else {
          (*pos).second = (*pos).second + visExposure * calData;
        }
      }
    }
  }

  fclose(calFilePtr);

  // If spectral summing is on, go in sets of 8 and set darks to the average
  /*
  PvlGroup &archive = icube->Label()->FindGroup("Archive", Pvl::Traverse);
  if(archive["SpectralSummingFlag"][0] == "ON") {
    for(int band = 1; band <= 96; band += 8) {
      for(int sample = 1; sample <= 64; sample++) {
        Statistics stats;

        // we're calculating an average of 8 of these values through the bands
        for(int avgIndex = 0; avgIndex < 8; avgIndex ++) {
          // this wont go out of bounds as long as we have 8 as the increment
          pair<int,int> index = pair<int,int>(sample, band+avgIndex);
          stats.AddData(sampleBasedDarkCorrections.find(index)->second);
        }

        // now set the values to the average
        for(int setIndex = 0; setIndex < 8; setIndex ++) {
          pair<int,int> index = pair<int,int>(sample, band+setIndex);
          if(!setIndex) printf("Changing %.4f to %.4f\n", sampleBasedDarkCorrections.find(index)->second, stats.Average());
          sampleBasedDarkCorrections.find(index)->second = stats.Average();
        }
      }
    }
  }*/
}

/**
 * This calculates the dark current corrections for IR. If
 * IRDARKAVG is false, then it translates the sideplane data
 * into the lineBasedDarkCorrections map directly and does nothing further
 * with the data. Otherwise, this will apply a least squares linear
 * fit (the original script did chi-squared, but this is okay) for
 * each band and uses the points on the line instead of the sideplane
 * data directly.
 *
 * @param icube
 */
void calculateIrDarkCurrent(Cube *icube) {
  UserInterface &ui = Application::GetUserInterface();

  bool found = false;

  // verify if IR we have sideplane data
  for(int obj = 0; !found && obj < icube->label()->Objects(); obj++) {
    PvlObject &object = icube->label()->Object(obj);

    if(object.Name() != "Table") continue;

    if(object.HasKeyword("Name") && object["Name"][0] == "SideplaneIr") found = true;
  }

  if(!found) {
    calibInfo += PvlKeyword("SideplaneCorrection", "None");
    return;
  }

  Table sideplane("SideplaneIr", ui.GetFileName("FROM"));

  // If spectal summing is on OR compressor_id isnt N/A then
  //   just return.
  PvlGroup &archive = icube->label()->FindGroup("Archive", Pvl::Traverse);

  // If dark subtracted (compressorid is valid) and cant do linear
  //   correction (spectral editing flag on) then do not do dark
  if(archive["CompressorId"][0] != "N/A" &&
      archive["SpectralEditingFlag"][0] == "ON") {
    calibInfo += PvlKeyword("SideplaneCorrection", "None");
    return;
  }

  // If subtracted (compressor id is valid) and dont do linear then return
  if(archive["CompressorId"][0] != "N/A" && ui.GetBoolean("IRORIGDARK") == true) {
    calibInfo += PvlKeyword("SideplaneCorrection", "None");
    return;
  }

  if(archive["SpectralSummingFlag"][0] == "ON") return;

  // Insert the sideplane data into our lineBasedDarkCorrections map (line,band to correction)
  for(int line = 1; line <= icube->lineCount(); line++) {
    for(int band = 1; band <= icube->bandCount(); band++) {
      pair<int, int> index = pair<int, int>(line, band);
      int value = (int)sideplane[(line-1)*icube->bandCount() + (band-1)][2];

      if(value != 57344)
        lineBasedDarkCorrections.insert(pair< pair<int, int>, double>(index, value));
      else
        lineBasedDarkCorrections.insert(pair< pair<int, int>, double>(index, Null));
    }
  }

  if(ui.GetBoolean("IRORIGDARK") == true) {
    calibInfo += PvlKeyword("SideplaneCorrection", "Sideplane");
    return;
  }

  // do linear fits
  for(int band = 1; band <= icube->bandCount(); band++) {
    PolynomialUnivariate basis(1);
    LeastSquares lsq(basis);

    for(int line = 1; line <= icube->lineCount(); line++) {
      pair<int, int> index = pair<int, int>(line, band);
      map< pair<int, int>, double>::iterator val = lineBasedDarkCorrections.find(index);

      if(val != lineBasedDarkCorrections.end()) {
        vector<double> input;
        input.push_back(line);
        double expected = val->second;

        if(!IsSpecial(expected))
          lsq.AddKnown(input, expected);
      }
    }

    if(lsq.Rows() == 0) return;
    lsq.Solve();

    double coefficients[2] = {
      basis.Coefficient(0),
      basis.Coefficient(1)
    };

    for(int line = 1; line <= icube->lineCount(); line++) {
      pair<int, int> index = pair<int, int>(line, band);

      map< pair<int, int>, double>::iterator val = lineBasedDarkCorrections.find(index);
      if(val != lineBasedDarkCorrections.end()) {
        double currentDark = val->second;

        if(!IsSpecial(currentDark)) {
          double newDark = coefficients[0] + line * coefficients[1];

          // initial dark applied by compressor
          if(archive["CompressorId"][0] != "N/A") {
            // input is in (dn-dark) units
            // (dn-dark) - (fit-dark) = dn-fit
            newDark -= currentDark;
          }

          val->second = newDark;
        }
      }
    }
  }

  if(archive["CompressorId"][0] != "N/A") {
    calibInfo += PvlKeyword("SideplaneCorrection", "Fit Delta");
  }
  else {
    calibInfo += PvlKeyword("SideplaneCorrection", "Fit");
  }
}

/**
 * This calls p->SetInputCube with the appropriate flat file needed for
 * icube.
 *
 * @param icube
 * @param p
 */
void chooseFlatFile(Cube *icube, ProcessByLine *p) {
  PvlGroup &inst = icube->label()->FindGroup("Instrument", Pvl::Traverse);
  bool vis = (inst["Channel"][0] != "IR");
  bool hires = ((inst["SamplingMode"][0] == "HIGH") || (inst["SamplingMode"][0] == "HI-RES"));

  QString calFile = "$cassini/calibration/vims/flatfield/";

  if(vis) {
    calFile += "vis_";
  }
  else {
    calFile += "ir_";
  }

  if(hires) {
    calFile += "hires_flatfield_v????.cub";
  }
  else {
    calFile += "flatfield_v????.cub";
  }

  FileName calibrationFileName(calFile);
  calibrationFileName = calibrationFileName.highestVersion();

  calibInfo += PvlKeyword("FlatFile", calibrationFileName.originalPath() + "/" + calibrationFileName.name());

  CubeAttributeInput iatt;
  p->SetInputCube(createCroppedFile(icube, calibrationFileName.expanded(), true), iatt);
}

/**
 * This makes our calibration files match the input cube described
 * by the swath keywords.
 *
 * @param icube
 * @param cubeFileName
 *
 * @return QString
 */
QString createCroppedFile(Cube *icube, QString cubeFileName, bool flatFile) {
  int sampOffset = 1;
  int lineOffset = 1;

  if(flatFile) {
    GetOffsets(*icube->label(), sampOffset, lineOffset);
  }


  QString appArgs = "from=" + cubeFileName + " ";
  appArgs += "sample=" + toString(sampOffset) + " ";
  appArgs += "line=" + toString(lineOffset) + " ";
  appArgs += "nsamples=" + toString(icube->sampleCount()) + " ";
  appArgs += "nlines=" + toString(icube->lineCount()) + " ";

  FileName tempFile("$TEMPORARY/tmp_" + FileName(cubeFileName).baseName() +
                    "_" + FileName(icube->fileName()).name());

  appArgs += "to=" + tempFile.expanded();

  ProgramLauncher::RunIsisProgram("crop", appArgs);
  tempFiles.push_back(tempFile.expanded());
  return tempFile.expanded();
}

void GetOffsets(const Pvl &lab, int &finalSampOffset, int &finalLineOffset) {
  const PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);

  //  Get sample/line offsets
  int sampOffset = inst ["XOffset"];
  int lineOffset = inst ["ZOffset"];

  //  Get swath width/length which will be image size unless occultation image
  int swathWidth = inst ["SwathWidth"];
  int swathLength = inst ["SwathLength"];

  bool vis = (inst["Channel"][0] != "IR");
  finalSampOffset = sampOffset;
  finalLineOffset = lineOffset;

  QString samplingMode = QString(inst["SamplingMode"]).toUpper();
  if(vis) {
    if(samplingMode == "NORMAL") {
      finalSampOffset = sampOffset - 1;
      finalLineOffset = lineOffset - 1;
    }
    else if(samplingMode == "HI-RES") {
      finalSampOffset = (3 * ((sampOffset - 1) + swathWidth / 2)) - swathWidth / 2;
      finalLineOffset = (3 * (lineOffset + swathLength / 2)) - swathLength / 2;
    }
    else {
      QString msg = "Unsupported sampling mode [" + samplingMode + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }
  else {
    if(samplingMode == "NORMAL") {
      finalSampOffset = sampOffset - 1;
      finalLineOffset = lineOffset - 1;
    }
    else if(samplingMode == "HI-RES") {
      finalSampOffset = 2 * ((sampOffset - 1) + ((swathWidth - 1) / 4));
      finalLineOffset = lineOffset - 1;
    }
    else if(samplingMode == "NYQUIST") {
      QString msg = "Cannot process NYQUIST (undersampled) mode ";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    else {
      QString msg = "Unsupported sampling mode [" + samplingMode + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }

  finalSampOffset ++;
  finalLineOffset ++;
}
