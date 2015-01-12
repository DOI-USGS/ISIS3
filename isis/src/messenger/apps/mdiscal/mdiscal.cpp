// $Id$
#include "Isis.h"

#include <vector>
#include <QString>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cmath>

#include "DarkModelPixel.h"
#include "MdisCalUtils.h"
#include "MultivariateStatistics.h"
#include "ProcessByLine.h"
#include "ProgramLauncher.h"
#include "Spice.h"
#include "Statistics.h"
#include "TextFile.h"

using namespace Isis;
using namespace std;

bool convertDarkToNull;
enum MdisDarkCurrentMode {
  DarkCurrentNone,
  DarkCurrentStandard,
  DarkCurrentLinear,
  DarkCurrentModel
};


MdisDarkCurrentMode darkCurrentMode;
vector<double> calibrationValues;
bool isNarrowAngleCamera;
bool isBinnedData;
double exposureDuration;
double ccdTemperature;
int filterNumber;
static int nDarkColumns(0);
static int nValidDark(0);
static int nSampsToNull(0);
Statistics darkStrip;
vector<double> prevLineData;
vector<double> smearData;
static double smearComponent(3.4);
static double eventCorrection(1.0);   // Contamination event correction
Pvl configFile;

//  Limit functionality for aiding dark analysis
static bool g_flatfield = true;
static bool g_radiometric = true;

//  Absolute coefficents
static double abs_coef(1.0);

//  I/F variables
static double solarDist(1.0);
static double F_f(1.0);
static double iof(1.0);   //  I/F value for observation
static DarkModelPixel* model(0);

//  Local functions
FileName DetermineFlatFieldFile();
void GatherDarkStatistics(Buffer& in);
void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out);

void IsisMain() {

  const QString mdiscal_program = "mdiscal";
  const QString mdiscal_version = "1.4";
  const QString mdiscal_revision = "$Revision$";
  QString mdiscal_runtime = Application::DateTime();

  // Specify the version of the CDR generated
  const int cdr_version = 4;

  // We will be processing by column in case of a linear dark current fit. This will make the
  //   calibration a one pass system in this case, rather than two.
  ProcessByLine p;
  FileName calibFile("$messenger/calibration/mdisCalibration????.trn");
  calibFile = calibFile.highestVersion();
  configFile.read(calibFile.expanded());

  // Initialize variables
  calibrationValues.clear();
  prevLineData.clear();
  convertDarkToNull = true;
  isNarrowAngleCamera = true;
  isBinnedData = true;
  darkCurrentMode = (MdisDarkCurrentMode) - 1;
  exposureDuration = 0.0;
  ccdTemperature = 0.0; // This needs figured out!
  filterNumber = 1;  // Sufficent for the NAC!
  model = 0;
  eventCorrection = 1.0;

  Cube *icube = p.SetInputCube("FROM");
  PvlGroup& inst = icube->group("Instrument");
  isNarrowAngleCamera = ((QString)inst["InstrumentId"] == "MDIS-NAC");
  exposureDuration = inst["ExposureDuration"] ;
  exposureDuration /= 1000.0;

  // Determine dark columns
  int fpuBin = inst["FpuBinningMode"];
  int pxlBin = inst["PixelBinningMode"];

  nDarkColumns = 4 / (fpuBin + 1);    //  DPU binning gives 2 dark cols
  if (pxlBin > 2) nDarkColumns = 0;   //  MP binning > 2x2 yields no darks
  else if (pxlBin > 0) nDarkColumns /= (pxlBin + 1); // Might be 1 if wo/DPU + MP 2x2

  //  Determine number of valid darks.  For no binning will have 3.  All combos
  //  that have 2x2 total binning will give 1 valid dark.  All other options
  //  have no valid dark columns.
  nValidDark = MIN(nDarkColumns, 3);
  if (nValidDark < 3) {
    if ((fpuBin + pxlBin) > 1) nValidDark = 0;
    else nValidDark = MIN(nValidDark, 1);
  }

// Determine number of samples/columns to NULL.  For no binning it will yield 4
// columns to NULL.  For DPU but no MP binning, 3;  For no DPU but MP binning,
//  2x2 yields 3, 4x4 and 8x8 yields 1.
  nSampsToNull = (pxlBin < 2) ? 0 : ((pxlBin > 2) ? 1 : 3);  // Only MP here
  nSampsToNull = MIN(MAX(nDarkColumns + 1, nSampsToNull), 4);  // No more than 4!
  darkStrip.Reset();

  ccdTemperature = icube->group("Archive")["CCDTemperature"];

  // Binned data only applies to FPUBIN mode.  Pixel binning must be dealt
  // with specially in other calibration support components
  isBinnedData = (fpuBin == 1);

  // Get the trusted filter number
  if (!isNarrowAngleCamera) {
    filterNumber = ((int)(icube->group("BandBin")["Number"])) - 1;
  } else {
    filterNumber = 1;  // For the NAC
  }

  UserInterface& ui = Application::GetUserInterface();
  convertDarkToNull = !ui.GetBoolean("KEEPDARK");
  if (!convertDarkToNull) nSampsToNull = 0;


  QString darkCurr = ui.GetString("DARKCURRENT");
  g_flatfield = ui.GetBoolean("FLATFIELD");
  g_radiometric = ui.GetBoolean("RADIOMETRIC");

  if (icube->bandCount() != 1) {
    throw IException(IException::User,
                     "MDIS images may only contain one band", _FILEINFO_);
  }

  if (icube->sampleCount() < 3) {
    throw IException(IException::User,
                     "Unable to obtain dark current data. Expected a sample dimension of at least 3", _FILEINFO_);
  }

  if ((int)icube->group("Instrument")["Unlutted"] == false) {
    throw IException(IException::User,
                     "Calibration may not be performed on unlutted data.", _FILEINFO_);
  }


  //  Check for cases where certain models cannot be computed.
  //  These would be for cases where more than two factors of compression
  //  occur.  For this case, only the model can be used and only if the
  //  exposure time < 2 secs.
  if ((darkCurr != "NONE") && (nValidDark <= 0)) {
    //  Both cases require dark pixels, model does not
    if ((darkCurr == "STANDARD") || (darkCurr == "LINEAR")) {
      darkCurr = "MODEL";
      QString mess = "There are no valid dark current pixels which are required"
         " for " + darkCurr + " calibration... must use MODEL";
      IException ie(IException::User, mess, _FILEINFO_);
      ie.print();
    }

    //  Model cannot be used for exposure times > 1.0 <sec>
    if ((darkCurr == "MODEL") && (exposureDuration > 1.0)) {
      darkCurr = "NONE";
      QString mess = "There are no valid dark current pixels and the dark model"
         " correction can not be used when the exposure duration"
         " exceeds 1000...image cannot be calibrated";
      IException ie(IException::User, mess, _FILEINFO_);
      ie.print();
    }
  }

  auto_ptr<DarkModelPixel> darkModel;
  if (darkCurr == "NONE") {
    darkCurrentMode = DarkCurrentNone;
  } else if (darkCurr == "STANDARD") {
    darkCurrentMode = DarkCurrentStandard;
    calibrationValues.resize(icube->lineCount());
  } else if (darkCurr == "LINEAR") {
    darkCurrentMode = DarkCurrentLinear;
    calibrationValues.resize(icube->lineCount());
  } else if (darkCurr == "MODEL") {
    if (exposureDuration > 1.0) {
      QString mess = "Dark model correction can not be used when the "
         "exposure duration exceeds 1000...using LINEAR instead.";
      IException ie(IException::User, mess, _FILEINFO_);
      ie.print();

      // set processing to standard
      darkCurrentMode = DarkCurrentLinear;
      calibrationValues.resize(icube->lineCount());
      darkCurr = "STANDARD";
    } else {
      darkCurrentMode = DarkCurrentModel;
    }
  } else {
    // should never happen
    throw IException(IException::Programmer,
                     "Invalid dark current mode [" +
                     darkCurr + "]", _FILEINFO_);
  }

  QString darkCurrentFile("");
  if (darkCurrentMode != DarkCurrentNone) {
    if (darkCurrentMode != DarkCurrentModel) {
      p.Progress()->SetText("Gathering Dark Current Statistics");
      p.StartProcess(GatherDarkStatistics);
    } else {
      // read in dark current table variables and report the filename used
      darkModel = auto_ptr<DarkModelPixel>(new DarkModelPixel(pxlBin, ccdTemperature, exposureDuration));
      darkCurrentFile = darkModel->loadCoefficients(isNarrowAngleCamera, isBinnedData);
      model = darkModel.get();
    }
  }

  // We need to figure out our flat-field file
  if (darkCurrentMode == DarkCurrentLinear) {
    // We need to perform a linear regression with our data,
    //   convert statistics to a line.
    double *xdata = new double[calibrationValues.size()];
    double *ydata = new double[calibrationValues.size()];

    for (unsigned int x = 0; x < calibrationValues.size(); x++) {
      xdata[x] = x;
      ydata[x] = calibrationValues[x];
    }

    // Perform a regression
    MultivariateStatistics stats;
    stats.AddData(xdata, ydata, calibrationValues.size());

    // y = A + Bx
    double a, b;
    stats.LinearRegression(a, b);
    delete[] xdata;
    delete[] ydata;

    // Store a,b in calibration data instead of our line
    calibrationValues.resize(2);
    calibrationValues[0] = a;
    calibrationValues[1] = b;
  }

  //  Compute the (new) absolute calibration
  QString respfile("");
  vector<double> rsp = loadResponsivity(isNarrowAngleCamera, isBinnedData,
                                        filterNumber + 1, respfile);
  // abs_coef = 1.0 / (rsp[0] * ((rsp[2] * ccdTemperature) + rsp[1]));
  double T = 1.0;
  double Rt = rsp[0];
  double Resp = 0;
  for (unsigned int i = 1; i < rsp.size(); i++) {
    Resp += Rt * (rsp[i] * T);
    T *= ccdTemperature;
  }
  abs_coef = 1.0 / Resp;

//  Retrieve filter dependant SMEAR component
  QString smearfile("");
  smearComponent = loadSmearComponent(isNarrowAngleCamera, filterNumber + 1,
                                      smearfile);

  //  Get s/c clock count
  QString startTime = inst["SpacecraftClockCount"];

  //  Retrieve contamination event correction parameter
  QString eventfile(""), eventDate("N/A");
  if (isNarrowAngleCamera) {  // Not applicable to the NAC
    eventfile = "N/A";
    eventCorrection = 1.0;
  } 
  else {  // Get correction for WAC filters
    eventCorrection = loadContaminationEvent(startTime, filterNumber + 1,
                                             eventfile, eventDate);
  }

  //  Compute I/F if requested by user
  iof = 1.0;
  bool do_iof = ui.GetBoolean("IOF");
  if (!g_radiometric) do_iof = false;
  bool iof_is_good = false;
  QString solirrfile("");
  if (do_iof) {
    PvlGroup& inst = icube->group("Instrument");
    QString target = inst["TargetName"];
    QString startTime = inst["SpacecraftClockCount"];
    if (sunDistanceAU(startTime, target, solarDist)) {
      vector<double> sol = loadSolarIrr(isNarrowAngleCamera, isBinnedData,
                                        filterNumber + 1, solirrfile);
      F_f = sol[2];
      iof = pi_c() * (solarDist * solarDist) / F_f;
      iof_is_good = true;
    } else {
      iof = 1.0;
      iof_is_good = false;
    }
  }

  //  Determine if we need to subsample the flat field should pixel binning
  //  occurred
  QString reducedFlat("");
  FileName flatfield = DetermineFlatFieldFile();
  if (pxlBin > 0) {
    QString scale(toString(pxlBin));
    FileName newflat = FileName::createTempFile("$TEMPORARY/" + flatfield.baseName() + "_reduced.cub");
    reducedFlat = newflat.expanded();
    QString parameters = "FROM=" + flatfield.expanded() +
       " TO="   + newflat.expanded() +
       " MODE=SCALE" +
       " LSCALE=" + scale +
       " SSCALE=" + scale;
    try {
      //  iApp->Exec("reduce", parameters);
      ProgramLauncher::RunIsisProgram("reduce", parameters);
      reducedFlat = newflat.expanded();
    }
    catch (IException&) {
      remove(reducedFlat.toAscii().data());
      throw;
    }
    CubeAttributeInput att;
    p.SetInputCube(reducedFlat, att);
  } else {
    CubeAttributeInput att;
    p.SetInputCube(flatfield.expanded(), att);
  }

  //  Set output file for processing
  Cube *ocube = p.SetOutputCube("TO");

  try {
    p.Progress()->SetText("Calibrating MDIS Cube");
    p.StartProcess(Calibrate);
  }
  catch (...) {
    if (!reducedFlat.isEmpty()) remove(reducedFlat.toAscii().data());
    throw;
  }

  //  Remove the temporary reduced input file if generated
  if (!reducedFlat.isEmpty()) remove(reducedFlat.toAscii().data());

  // Log calibration activity
  PvlGroup calibrationLog("RadiometricCalibration");
  calibrationLog.addKeyword(PvlKeyword("SoftwareName", mdiscal_program));
  calibrationLog.addKeyword(PvlKeyword("SoftwareVersion", mdiscal_version));
  calibrationLog.addKeyword(PvlKeyword("ProcessDate", mdiscal_runtime));
  calibrationLog.addKeyword(PvlKeyword("DarkCurrentModel", darkCurr));

  if (darkCurrentMode == DarkCurrentLinear) {
    QString equation = "Y = " + toString(calibrationValues[0]) + QString(" + ") + toString(calibrationValues[1]) + QString("x");
    calibrationLog.addKeyword(PvlKeyword("DarkCurrentEquation", (QString)equation));
  } else if (darkCurrentMode == DarkCurrentModel) {
    calibrationLog.addKeyword(PvlKeyword("DarkCurrentFile", darkCurrentFile));
  }

  calibrationLog.addKeyword(PvlKeyword("BinnedImage", toString((int)isBinnedData)));
  calibrationLog.addKeyword(PvlKeyword("FilterNumber", toString(filterNumber + 1)));
  if (g_flatfield) {
    calibrationLog.addKeyword(PvlKeyword("FlatFieldFile", flatfield.originalPath() + "/" + flatfield.name()));
    calibrationLog.addKeyword(PvlKeyword("CalibrationFile", calibFile.originalPath() + "/" + calibFile.name()));
    calibrationLog.addKeyword(PvlKeyword("ResponsivityFile", respfile));
    calibrationLog.addKeyword(PvlKeyword("SmearCompFile", smearfile));
    PvlKeyword rspKey("Response", toString(rsp[0]));
    for (unsigned int i = 1; i < rsp.size(); i++) {
      rspKey.addValue(toString(rsp[i]));
    }
    calibrationLog.addKeyword(rspKey);
    calibrationLog.addKeyword(PvlKeyword("SmearComponent", toString(smearComponent)));
  } else {
    calibrationLog.addKeyword(PvlKeyword("FlatFieldFile", "N/A"));
    calibrationLog.addKeyword(PvlKeyword("CalibrationFile", "N/A"));
    calibrationLog.addKeyword(PvlKeyword("ResponsivityFile", "N/A"));
    calibrationLog.addKeyword(PvlKeyword("SmearCompFile", "N/A"));
    PvlKeyword rspKey("Response");
    calibrationLog.addKeyword(PvlKeyword("Response", "N/A"));
    calibrationLog.addKeyword(PvlKeyword("SmearComponent", "N/A"));
  }

  QString calibType;
  if (do_iof  && iof_is_good) {
    calibrationLog.addKeyword(PvlKeyword("Units", "I over F"));
    calibrationLog.addKeyword(PvlKeyword("SolarDistance", toString(solarDist), "AU"));
    calibrationLog.addKeyword(PvlKeyword("SolarIrrFile", solirrfile));
    calibrationLog.addKeyword(PvlKeyword("FilterIrradianceFactor", toString(F_f)));
    calibrationLog.addKeyword(PvlKeyword("IOFFactor", toString(iof)));
    calibType = "IF";
  } else if (g_radiometric) {
    calibrationLog.addKeyword(PvlKeyword("Units", "W / (m**2 micrometer sr)"));
    calibType = "RA";
  } else {
    calibrationLog.addKeyword(PvlKeyword("Units", "DN"));
    calibType = "DN";
  }

  calibrationLog.addKeyword(PvlKeyword("ContaminationEventFile", eventfile));
  calibrationLog.addKeyword(PvlKeyword("ContaminationEventDate", eventDate));
  calibrationLog.addKeyword(PvlKeyword("ContaminationEventFactor",
                                       toString(eventCorrection)));


  calibrationLog.addKeyword(PvlKeyword("DarkStripColumns", toString(nDarkColumns)),
                            Pvl::Replace);
  calibrationLog.addKeyword(PvlKeyword("ValidDarkColumns", toString(nValidDark)),
                            Pvl::Replace);
  if (darkStrip.TotalPixels() > 0) {
    double avgDark = (darkStrip.ValidPixels() > 0) ? darkStrip.Average() : 0.0;
    calibrationLog.addKeyword(PvlKeyword("DarkStripMean", toString(avgDark)), 
                                         Pvl::Replace); 
  }

  // Report nulled sample count
  calibrationLog.addKeyword(PvlKeyword("LeftSamplesNulled", toString(nSampsToNull)));

  //  Handle updates of ProductId and SourceProduct Id keywords
  PvlGroup& archive = ocube->group("Archive");
  PvlKeyword key = archive["ProductId"];
  QString orgProdId = key[0];
  QString newProdId = orgProdId + "_" + calibType + "_" + toString(cdr_version);
  newProdId[0] = 'C';
  key.setValue(Quote(newProdId));
  archive.addKeyword(key, Pvl::Replace);

  // Now SourceProductId
  if (archive.hasKeyword("SourceProductId")) {
    key = archive["SourceProductId"];
    for (int i = 0; i < key.size(); i++) {
      key[i] = Quote(key[i]);
    }
  } else {
    key = PvlKeyword("SourceProductId", Quote(orgProdId));
  }

  if (!darkCurrentFile.isEmpty()) {
    key.addValue(Quote(FileName(darkCurrentFile).baseName()));
  }
  key.addValue(Quote(flatfield.baseName()));
  key.addValue(Quote(FileName(respfile).baseName()));
  // key.addValue(Quote(FileName(smearfile).baseName()));
  if (iof_is_good) {
    key.addValue(Quote(FileName(solirrfile).baseName()));
  }
  archive.addKeyword(key, Pvl::Replace);

  // Write Calibration group to file and log
  ocube->putGroup(calibrationLog);
  Application::Log(calibrationLog);
  configFile.clear();
}

FileName DetermineFlatFieldFile() {
  QString filename = "$messenger/calibration/FLAT/";

  // FileName consists of binned/notbinned, camera, and filter
  filename += "MDIS";
  filename += ((isNarrowAngleCamera) ? "NAC" : "WAC");
  filename += ((isBinnedData) ? "_BINNED_" : "_NOTBIN_");
  filename += "FLAT";
  if (isNarrowAngleCamera) {
    // NAC spec is simpler
    filename += "_?.cub";
  } else {
    // add a zero if the filter is 1-digit
    filename += "_FIL";
    if (filterNumber < 9) filename += "0";
    filename += toString(filterNumber + 1);
    filename += "_?.cub";
  }

  FileName final(filename);
  final = final.highestVersion();
  return final;
}


void GatherDarkStatistics(Buffer& in) {

  // Some situations cannot use these processes for calibration
  calibrationValues[in.Line() - 1] = Isis::Null;

  if (nValidDark > 0) {
    if (darkCurrentMode == DarkCurrentStandard) {
      // Figure out the median (Isis::Statistics wont do this for us)
      // because we have repeated numbers, put them into a sorted array size
      // of no more than 3 and take the middle
      vector<double> calibValues;
      int nDark = nValidDark;
      for (int i = 0; i < nDark; i++) {
        calibValues.push_back(in[i]);
      }
      sort(calibValues.begin(), calibValues.end());

      // grab the middle element in the array for the median
      calibrationValues[in.Line() - 1] = calibValues[nDark / 2];
    } else if (darkCurrentMode == DarkCurrentLinear) {
      // Presently the linear regression only uses the first sample in the
      // dark current data
      calibrationValues[in.Line() - 1] = in[0];
    }
  }
}

void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out) {
  Buffer& imageIn   = *in[0];
  Buffer& flatField = *in[1];
  Buffer& imageOut  = *out[0];
  double t2 = smearComponent / imageIn.SampleDimension();
  //exposureDuration is in seconds, but we need to work in ms.
  double exposureTime = exposureDuration * 1000.0;

  if (imageIn.Line() == 1) {
    prevLineData.resize(imageIn.SampleDimension());
    smearData.resize(imageIn.SampleDimension());
    double added = 16.0*t2/exposureTime;

    for (unsigned int i = 0; i < prevLineData.size(); i++) {
      prevLineData[i] = added;
      smearData[i] = 0.0;
    }
  }

  for (int i = 0; i < imageIn.size(); i++) {

    // Check for special pixel in input image and pass through
    if (Isis::IsSpecial(imageIn[i])) {
      imageOut[i] = imageIn[i];
      continue;
    }

    if (g_flatfield) {
      //  If the flat field pixel is special, can't calibrate so set to NULL
      //  and pass through (unlikely).
      if (Isis::IsSpecial(flatField[i])) {
        imageOut[i] = Isis::Null;
        continue;
      }
    }
//462
//if(i == 25 && imageIn.Line() == 25) std::cout <<  "In: " << imageIn[i] << std::endl;

    // Step 1: Perform dark current corrections
    if (darkCurrentMode == DarkCurrentNone) {
      imageOut[i] = imageIn[i];
    } else if (darkCurrentMode == DarkCurrentStandard) {
      imageOut[i] = imageIn[i] - calibrationValues[imageIn.Line() - 1];
    } else if (darkCurrentMode == DarkCurrentLinear) {
      // Linear: out = in - bestfitline = in - (A + Bx)
      imageOut[i] = imageIn[i] - (calibrationValues[0] + calibrationValues[1] *
                                  (imageIn.Line() - 1));
    } else if (darkCurrentMode == DarkCurrentModel) {
      imageOut[i] = imageIn[i] - model->getDarkPixel(i, imageIn.Line() - 1);
    }

    if (g_flatfield) {
      // Step 2: Perform linearity correction
      if (isNarrowAngleCamera == true) {
        if (imageOut[i] <= 0.0) {
          imageOut[i] /= 0.912031;
        } else {
          imageOut[i] /= 0.011844 * log10(imageOut[i]) + 0.912031;
        }
      } else {
        // Wide angle camera
        if (imageOut[i] <= 0.0) {
          imageOut[i] /= 0.936321;
        } else {
          imageOut[i] /= 0.008760 * log10(imageOut[i]) + 0.936321;
        }
      }

      // Step 3: Readout Smear Correction (ms -> seconds)
      if (exposureTime > 0.0) {
        smearData[i] += t2 / exposureTime * prevLineData[i];
        imageOut[i] -= smearData[i];
      }

      prevLineData[i] = imageOut[i];

      // Step 4: Uniformity (flat field)
      imageOut[i] /= flatField[i]; // divide by flat field

      // Step 5: Absolute coefficient
      // Using exposureDuration (in seconds). This gives ~ the same results
      // as the previous version of mdiscal did. Using exposureTime gives
      // a factor of 1000 smaller value, as one would expect. 
      if (exposureDuration > 0.0) {
        imageOut[i] = imageOut[i] / exposureDuration * abs_coef;
      }
    }  // End of flat field

    if (g_radiometric) {
      //  Step 6:  Convert to I/F units
      imageOut[i] *= iof;
    }

    //  Apply contamination event factor to final result w/o.r.f output units
    imageOut[i] *= eventCorrection;
  }

  //  Compute dark current statistics
  for (int j = 0; j < nValidDark; j++) {
    darkStrip.AddData(imageOut[j]);
  }

  //  Null specified columns (2011-04-20 - KJB)
  for (int n = 0 ; n < nSampsToNull ; n++) {
    imageOut[n] = Isis::Null;
  }

}

