// $Id: mdiscal.cpp,v 1.22 2009/09/04 17:59:05 mboyd Exp $
#include "Isis.h"

#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cmath>

#include "ProcessByLine.h"
#include "Statistics.h"
#include "MultivariateStatistics.h"
#include "TextFile.h"
#include "Spice.h"
#include "MdisCalUtils.h"
#include "DarkModelPixel.h"

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
Statistics darkStrip;
vector<double> prevLineData;
vector<double> smearData;
double smearComponent(3.4);
Pvl configFile;

//  Absolute coefficents
static double abs_coef(1.0);

//  I/F variables
static double solarDist(1.0);
static double F_f(1.0);
static double iof(1.0);   //  I/F value for observation
static DarkModelPixel *model(0);

//  Local functions
Filename DetermineFlatFieldFile();
void GatherDarkStatistics(Buffer &in);
void Calibrate(vector<Buffer *>&in, vector<Buffer *>&out);

void IsisMain() {

  const string mdiscal_program = "mdiscal";
  const string mdiscal_version = "1.2";
  const string mdiscal_revision = "$Revision: 1.22 $";
  string mdiscal_runtime = Application::DateTime();

  // Specify the version of the CDR generated
  const int cdr_version = 3;

  // We will be processing by column in case of a linear dark current fit. This will make the
  //   calibration a one pass system in this case, rather than two.
  ProcessByLine p;
  Filename calibFile("$messenger/calibration/mdisCalibration????.trn");
  calibFile.HighestVersion();
  configFile.Read(calibFile.Expanded());
  
  // Initialize variables
  calibrationValues.clear();
  prevLineData.clear();
  convertDarkToNull = true;
  isNarrowAngleCamera = true;
  isBinnedData = true;
  darkCurrentMode = (MdisDarkCurrentMode)-1;
  exposureDuration = 0.0;
  ccdTemperature = 0.0; // This needs figured out!
  filterNumber = 1;  // Sufficent for the NAC!
  model = 0;

  Cube *icube = p.SetInputCube("FROM");
  PvlGroup &inst = icube->GetGroup("Instrument");
  isNarrowAngleCamera = ((string)inst["InstrumentId"] == "MDIS-NAC");
  exposureDuration = inst["ExposureDuration"];
  exposureDuration /= 1000.0;  //  Convert ms to sec

    // Determine dark columns
  int fpuBin = inst["FpuBinningMode"];
  int pxlBin = inst["PixelBinningMode"];
  nDarkColumns = 4 / (fpuBin+1) / (pxlBin+1);
  nValidDark = MIN(nDarkColumns, 3);
  if(nValidDark < 3) {
    if ((fpuBin+pxlBin) > 1) nValidDark = 0;
    else nValidDark = MIN(nValidDark, 1);
  }
  darkStrip.Reset();
  

  ccdTemperature = icube->GetGroup("Archive")["CCDTemperature"];

  // Binned data only applies to FPUBIN mode.  Pixel binning must be dealt
  // with specially in other calibration support components
  isBinnedData = (fpuBin == 1);

  // Get the trusted filter number
  if( !isNarrowAngleCamera ) {
      filterNumber = ((int) (icube->GetGroup("BandBin")["Number"])) - 1;
  }
  else {
      filterNumber = 1;  // For the NAC
  }
  
  UserInterface &ui = Application::GetUserInterface();
  convertDarkToNull = !ui.GetBoolean("KEEPDARK");
  
  iString darkCurr = ui.GetString("DARKCURRENT");
  
  if(icube->Bands() != 1) {
    throw iException::Message(iException::User,
							  "MDIS images may only contain one band", _FILEINFO_);
  }
  
  if(icube->Samples() < 3) {
    throw iException::Message(iException::User,
							  "Unable to obtain dark current data. Expected a sample dimension of at least 3", _FILEINFO_);
  }

  if((int)icube->GetGroup("Instrument")["Unlutted"] == false) {
	throw iException::Message(iException::User,
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
      string mess = "There are no valid dark current pixels which are required"
                    " for " + darkCurr + " calibration... must use MODEL";
      iException &ie= iException::Message(iException::User, mess, _FILEINFO_);
      ie.Report();
      ie.Clear();
    }

    //  Model cannot be used for exposure times > 1.0 <sec>
    if ((darkCurr == "MODEL") && (exposureDuration > 1.0)) {
      darkCurr = "NONE";
      string mess = "There are no valid dark current pixels and the dark model" 
                    " correction can not be used when the exposure duration"
                    " exceeds 1000...image cannot be calibrated";
      iException &ie= iException::Message(iException::User, mess, _FILEINFO_);
      ie.Report();
      ie.Clear();
    }
  }

  auto_ptr<DarkModelPixel> darkModel;
  if(darkCurr == "NONE") {
    darkCurrentMode = DarkCurrentNone;
  }
  else if(darkCurr == "STANDARD") {
    darkCurrentMode = DarkCurrentStandard;
    calibrationValues.resize(icube->Lines());
  }
  else if(darkCurr == "LINEAR") {
    darkCurrentMode = DarkCurrentLinear;
    calibrationValues.resize(icube->Lines());
  }
  else if(darkCurr == "MODEL") {
    if(exposureDuration > 1.0) {
        string mess = "Dark model correction can not be used when the "
                    "exposure duration exceeds 1000...using LINEAR instead."; 
        iException &ie= iException::Message(iException::User, mess, _FILEINFO_);
        ie.Report();
        ie.Clear();

        // set processing to standard
      darkCurrentMode = DarkCurrentLinear;
      calibrationValues.resize(icube->Lines());
      darkCurr = "STANDARD";
    }
    else {
      darkCurrentMode = DarkCurrentModel;
    }
  }
  else {
    // should never happen
    throw iException::Message(iException::Programmer, 
                              "Invalid dark current mode [" +
                               darkCurr + "]", _FILEINFO_);
  }

  string darkCurrentFile("");
  if(darkCurrentMode != DarkCurrentNone) {
    if(darkCurrentMode != DarkCurrentModel) {
      p.Progress()->SetText("Gathering Dark Current Statistics");
      p.StartProcess(GatherDarkStatistics);
    }
    else {
      // read in dark current table variables and report the filename used
      darkModel = auto_ptr<DarkModelPixel> (new DarkModelPixel(pxlBin,ccdTemperature, exposureDuration));
      darkCurrentFile = darkModel->loadCoefficients(isNarrowAngleCamera, isBinnedData);
      model = darkModel.get();
    }
  }

  // We need to figure out our flat-field file
  if(darkCurrentMode == DarkCurrentLinear) {
    // We need to perform a linear regression with our data,
    //   convert statistics to a line.
    double *xdata = new double[calibrationValues.size()];
    double *ydata = new double[calibrationValues.size()];
    
    for(unsigned int x = 0; x < calibrationValues.size(); x++) {
      xdata[x] = x;
      ydata[x] = calibrationValues[x];
    }
    
    // Perform a regression
    MultivariateStatistics stats;
    stats.AddData(xdata, ydata, calibrationValues.size());
    
    // y = A + Bx
    double a, b;
    stats.LinearRegression(a,b);
    delete [] xdata;
    delete [] ydata;
    
    // Store a,b in calibration data instead of our line
    calibrationValues.resize(2);
    calibrationValues[0] = a;
    calibrationValues[1] = b;
  }

  //  Compute the (new) absolute calibration
  string respfile("");
  vector<double> rsp = loadResponsivity(isNarrowAngleCamera, isBinnedData,
                                       filterNumber+1, respfile);
  // abs_coef = 1.0 / (rsp[0] * ((rsp[2] * ccdTemperature) + rsp[1]));
  double T = 1.0;
  double Rt = rsp[0];
  double Resp = 0;
  for (unsigned int i = 1 ; i < rsp.size() ; i++) {
    Resp += Rt * (rsp[i] * T);
    T *= ccdTemperature;
  }
  abs_coef = 1.0 / Resp;

 //  Retrieve filter dependant SMEAR component
  string smearfile("");
  smearComponent = loadSmearComponent(isNarrowAngleCamera, filterNumber+1, 
                                      smearfile);

  //  Compute I/F if requested by user
  iof = 1.0;
  bool do_iof = ui.GetBoolean("IOF");
  bool iof_is_good = false;
  string solirrfile("");
  if(ui.GetBoolean("IOF")) {
    PvlGroup &inst = icube->GetGroup("Instrument");
    string target = inst["TargetName"];
    string startTime = inst["SpacecraftClockCount"];
    if (sunDistanceAU(startTime, target, solarDist)) {
      vector<double> sol = loadSolarIrr(isNarrowAngleCamera, isBinnedData,
                                             filterNumber+1, solirrfile);
      F_f = sol[2];
      iof = pi_c() * (solarDist * solarDist) / F_f;
      iof_is_good = true;
    }
    else {
        iof = 1.0;
        iof_is_good = false;
    }
  }

  //  Determine if we need to subsample the flat field should pixel binning
  //  occurred
  string reducedFlat("");
  Filename flatfield = DetermineFlatFieldFile();
  if (pxlBin > 0) {
    iString scale(pow(2.0, pxlBin));
    Filename newflat;
    newflat.Temporary(flatfield.Basename()+"_reduced", "cub");
    string parameters = "FROM=" + flatfield.Expanded() +
                             " TO="   + newflat.Expanded() +
                             " REDUCTION_TYPE=SCALE" +
                             " LSCALE=" + scale +
                             " SSCALE=" + scale;
    cout << "Running: reduce " << parameters << endl;
    iApp->Exec("reduce",parameters);
    reducedFlat = newflat.Expanded();
    CubeAttributeInput att;
    p.SetInputCube(reducedFlat,att);
  }
  else {
    CubeAttributeInput att;
    p.SetInputCube(flatfield.Expanded(),att);
  }

  //  Set output file for processing
  Cube *ocube = p.SetOutputCube("TO");

  try {
    p.Progress()->SetText("Calibrating MDIS Cube");
    p.StartProcess(Calibrate);
  } catch (...) {
    if (!reducedFlat.empty()) remove(reducedFlat.c_str());
    throw;
  }

  //  Remove the temporary reduced input file if generated
  if (!reducedFlat.empty()) remove(reducedFlat.c_str());

  // Log calibration activity
  PvlGroup calibrationLog("RadiometricCalibration");
  calibrationLog.AddKeyword(PvlKeyword("SoftwareName", mdiscal_program));
  calibrationLog.AddKeyword(PvlKeyword("SoftwareVersion", mdiscal_version));
  calibrationLog.AddKeyword(PvlKeyword("ProcessDate", mdiscal_runtime));
  calibrationLog.AddKeyword(PvlKeyword("From", (string)ui.GetFilename("FROM")));
  calibrationLog.AddKeyword(PvlKeyword("DarkCurrentModel", darkCurr));
  
  if(darkCurrentMode == DarkCurrentLinear) {
    iString equation = "Y = " + iString(calibrationValues[0]) + iString(" + ") + iString(calibrationValues[1]) + iString("x");
    calibrationLog.AddKeyword(PvlKeyword("DarkCurrentEquation", (string)equation));
  }
  else if(darkCurrentMode == DarkCurrentModel) {
    calibrationLog.AddKeyword(PvlKeyword("DarkCurrentFile", darkCurrentFile));
  }

  calibrationLog.AddKeyword(PvlKeyword("BinnedImage", isBinnedData));
  calibrationLog.AddKeyword(PvlKeyword("FilterNumber", filterNumber+1));
  calibrationLog.AddKeyword(PvlKeyword("FlatFieldFile", flatfield.OriginalPath() + "/" + flatfield.Name()));
  calibrationLog.AddKeyword(PvlKeyword("CalibrationFile", calibFile.OriginalPath() + "/" + calibFile.Name()));
  calibrationLog.AddKeyword(PvlKeyword("ResponsivityFile", respfile));
  calibrationLog.AddKeyword(PvlKeyword("SmearCompFile", smearfile));
  PvlKeyword rspKey("Response", rsp[0]);
  for (unsigned int i = 1 ; i < rsp.size() ; i++) {
    rspKey.AddValue(rsp[i]);
  }
  calibrationLog.AddKeyword(rspKey);
  calibrationLog.AddKeyword(PvlKeyword("SmearComponent", smearComponent));

  string calibType;
  if ( do_iof  && iof_is_good ) {
    calibrationLog.AddKeyword(PvlKeyword("Units", "I over F"));
    calibrationLog.AddKeyword(PvlKeyword("SolarDistance", solarDist, "AU"));
    calibrationLog.AddKeyword(PvlKeyword("SolarIrrFile", solirrfile));
    calibrationLog.AddKeyword(PvlKeyword("FilterIrradianceFactor", F_f));
    calibrationLog.AddKeyword(PvlKeyword("IOFFactor", iof));
    calibType = "IF";
  }
  else {
    calibrationLog.AddKeyword(PvlKeyword("Units", "W / (m**2 micrometer sr )"));
    calibType = "RA";
  }

  calibrationLog.AddKeyword(PvlKeyword("DarkStripColumns",nDarkColumns),
                              Pvl::Replace);
  if (darkStrip.TotalPixels() > 0) {
    calibrationLog.AddKeyword(PvlKeyword("DarkStripMean",darkStrip.Average()),
                              Pvl::Replace);
  }

  //  Handle updates of ProductId and SourceProduct Id keywords
  PvlGroup &archive = ocube->GetGroup("Archive");
  PvlKeyword key = archive["ProductId"];
  iString orgProdId = key[0];
  iString newProdId = orgProdId + "_" + calibType + "_" + iString(cdr_version);
  newProdId[0] = 'C';
  key.SetValue(Quote(newProdId));
  archive.AddKeyword(key, Pvl::Replace);

  // Now SourceProductId
  if(archive.HasKeyword("SourceProductId")) {
    key = archive["SourceProductId"];
    for(int i = 0 ; i < key.Size() ; i++) {
      key[i] = Quote(key[i]);
    }
  }
  else {
    key = PvlKeyword("SourceProductId", Quote(orgProdId));
  }

  if(!darkCurrentFile.empty()) {
    key.AddValue(Quote(Filename(darkCurrentFile).Basename()));
  }
  key.AddValue(Quote(flatfield.Basename()));
  key.AddValue(Quote(Filename(respfile).Basename()));
  // key.AddValue(Quote(Filename(smearfile).Basename()));
  if (iof_is_good) {
    key.AddValue(Quote(Filename(solirrfile).Basename()));
  }
  archive.AddKeyword(key, Pvl::Replace);

  // Write Calibration group to file and log
  ocube->PutGroup(calibrationLog);
  Application::Log(calibrationLog);
  configFile.Clear();
}

Filename DetermineFlatFieldFile() {
  string filename = "$messenger/calibration/FLAT/";

  // Filename consists of binned/notbinned, camera, and filter
  filename += "MDIS";
  filename += ((isNarrowAngleCamera)? "NAC" : "WAC");
  filename += ((isBinnedData)? "_BINNED_" : "_NOTBIN_");
  filename += "FLAT";
  if (isNarrowAngleCamera) {
    // NAC spec is simpler
    filename += "_?.cub";
  }
  else {
    // add a zero if the filter is 1-digit
    filename += "_FIL";
    if (filterNumber < 9) filename += "0";
    filename += iString(filterNumber+1);
    filename += "_?.cub";
  }

  Filename final(filename);
  final.HighestVersion();
  return final;
}


void GatherDarkStatistics(Buffer &in) {

  // Some situations cannot use these processes for calibration
  calibrationValues[in.Line()-1] = Isis::Null;

  if(nDarkColumns > 0) {
    if(darkCurrentMode == DarkCurrentStandard) {
    // Figure out the median (Isis::Statistics wont do this for us) 
    // because we have repeated numbers, put them into a sorted array size
    // of no more than 3 and take the middle
      vector<double> calibValues;
      int nDark = nValidDark;
      for(int i = 0 ; i < nDark ; i++) {
        calibValues.push_back(in[i]);
      }
      sort(calibValues.begin(), calibValues.end());
  
      // grab the middle element in the array for the median
      calibrationValues[in.Line()-1] = calibValues[nDark/2];
    }
    else if(darkCurrentMode == DarkCurrentLinear) {
     // Presently the linear regression only uses the first sample in the
     // dark current data
     calibrationValues[in.Line()-1] = in[0];
    }
  }
}

void Calibrate(vector<Buffer *>&in, vector<Buffer *>&out) {
  Buffer &imageIn   = *in[0];
  Buffer &flatField = *in[1];
  Buffer &imageOut  = *out[0];

  if(imageIn.Line() == 1) {
    prevLineData.resize(imageIn.SampleDimension());
    smearData.resize(imageIn.SampleDimension());

    for(unsigned int i = 0; i < prevLineData.size(); i++) {
      prevLineData[i] = 0.0;
      smearData[i] = 0.0;
    }
  }

  for(int i = 0; i < imageIn.size(); i++) {

    // Check for special pixel in input image and pass through
    if(Isis::IsSpecial(imageIn[i])) {
      imageOut[i] = imageIn[i];
      continue;
    }

    //  If the flat field pixel is special, can't calibrate so set to NULL
    //  and pass through (unlikely).
    if(Isis::IsSpecial(flatField[i])) {
      imageOut[i] = Isis::Null;
      continue;
    }
//462
//	if(i == 25 && imageIn.Line() == 25) std::cout <<  "In: " << imageIn[i] << std::endl;

    // Step 1: Perform dark current corrections
    if(darkCurrentMode == DarkCurrentNone) {
      imageOut[i] = imageIn[i];
    }
    else if(darkCurrentMode == DarkCurrentStandard) {
      imageOut[i] = imageIn[i] - calibrationValues[imageIn.Line()-1];
    }
    else if(darkCurrentMode == DarkCurrentLinear) {
      // Linear: out = in - bestfitline = in - (A + Bx)
      imageOut[i] = imageIn[i] - (calibrationValues[0] + calibrationValues[1] * 
                                  (imageIn.Line()-1));
    }
    else if(darkCurrentMode == DarkCurrentModel) {
      imageOut[i] = imageIn[i] - model->getDarkPixel(i, imageIn.Line()-1);
    }

    // Step 2: Perform linearity correction
    if(isNarrowAngleCamera == true) {
      if(imageOut[i] <= 0.0) {
        imageOut[i] /= 0.912031;
      }
      else {
        imageOut[i] /= 0.011844 * log10(imageOut[i]) + 0.912031;
      }
    }
    else {
      // Wide angle camera
      if(imageOut[i] <= 0.0) {
        imageOut[i] /= 0.936321;
      }
      else {
        imageOut[i] /= 0.008760 * log10(imageOut[i]) + 0.936321;
      }
    }

    // Step 3: Readout Smear Correction (ms -> seconds)
    double t2 = smearComponent / imageIn.SampleDimension() / 1000.0;  

    if(exposureDuration > 0.0 && imageIn.Line() > 1) {
      smearData[i] += t2 / exposureDuration * prevLineData[i];
      imageOut[i] -= smearData[i];
    }

    prevLineData[i] = imageOut[i];

    // Step 4: Uniformity (flat field)
    imageOut[i] /= flatField[i]; // divide by flat field

    // Step 5: Absolute coefficient
    if(exposureDuration > 0.0) {
      imageOut[i] = imageOut[i] / exposureDuration * abs_coef;
    }

    //  Step 6:  Convert to I/F units
    imageOut[i] *= iof;
  }

  //  Compute dark current statistics and NULL'em if requested
  for(int j = 0 ; j < nDarkColumns; j++) {
    darkStrip.AddData(imageOut[j]);
    if(convertDarkToNull) imageOut[j] = Isis::Null;
  }

}

