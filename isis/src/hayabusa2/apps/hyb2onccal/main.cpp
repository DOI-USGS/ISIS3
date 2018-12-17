// $Id: hyb2onccal.cpp 6045 2015-02-07 02:06:59Z moses@GS.DOI.NET $
#include "Isis.h"

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

#include "AlphaCube.h"
#include "Buffer.h"
#include "FileName.h"
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


using namespace Isis;
using namespace std;

// Calibration support routines
FileName DetermineFlatFieldFile(const QString &filter);
void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out);

QString loadCalibrationVariables(const QString &config);

// Temporary cube file pointer deleter
struct TemporaryCubeDeleter {
  static inline void cleanup(Cube *cube) {
    if ( cube ) {

      FileName filename( cube->fileName() );
      delete cube;
      remove( filename.expanded().toLatin1().data() );
    }
  }
};

enum InstrumentType{ONCW1,ONCW2,ONCT};

static int g_bitDepth(12);

InstrumentType g_instrument;
//For subimage and binning mapping
static AlphaCube *alpha(0);
static bool g_cropped(true);

static QString g_filter = "";
static QString g_target ="";
static Pvl g_configFile;

//Bias calculation variables
static double g_b0(0);
static double g_b1(0);
static double g_b2(0);
static double g_bae0(0);
static double g_bae1(0);
static double g_bias(0);

//Device (AE/CCD/ECT temps for ONC-T,ONC-W1,ONC-W2

static double g_AEtemperature(0.0);

static double g_CCD_T_temperature(0.0);
static double g_ECT_T_temperature(0.0);


static double g_CCD_W1_temperature(0.0);
static double g_ECT_W1_temperature(0.0);


static double g_CCD_W2_temperature(0.0);
static double g_ECT_W2_temperature(0.0);


static QString g_startTime;

//Dark Current variables
static double g_d0(0);
static double g_d1(0);
static double g_darkCurrent(0);

//Linearity correction variables
static double g_L0(0);
static double g_L1(0);
static double g_L2(0);

// TODO: we do not have the readout time (transfer period) for Hayabusa2 ONC.
//Smear calculation variables
static bool g_onBoardSmearCorrection(false);
static double g_Tvct(0);       // Vertical charge-transfer period (in seconds).
static double g_texp(1);       // Exposure time.
static double g_timeRatio(1.0);

// Calibration parameters
static int binning(1);         //!< The number of samples/lines which are binned
static double g_compfactor(1.0);  // Default if OutputMode = LOSS-LESS; 16.0 for LOSSY

static QString g_iofCorrection("IOF");  //!< Is I/F correction to be applied?

//  I/F variables
static double g_solarDist(1.0);  /**< Distance from the Sun to the target body
(used to calculate g_iof) */
static double g_iof(1.0);        //!< I/F conversion value
static double g_iofScale(1.0);
static double g_solarFlux(1.0);  //!< The solar flux (used to calculate g_iof).
// TODO: we do not have this conversion factor for Hayabusa 2 ONC.
static double g_v_standard(1.0);
// static double g_v_standard(3.42E-3);//!< Base conversion for all filters (Tbl. 9)

void IsisMain() {

  UserInterface& ui = Application::GetUserInterface();
  // g_iofCorrection = ui.GetString("UNITS");

  const QString hyb2cal_program = "hyb2onccal";
  const QString hyb2cal_version = "1.1";
  const QString hyb2cal_revision = "$Revision$";
  QString hyb2cal_runtime = Application::DateTime();

  ProcessBySample p;

  Cube *icube = p.SetInputCube("FROM");

  // Basic assurances...
  if (icube->bandCount() != 1) {
    throw IException(IException::User,
      "ONC images may only contain one band", _FILEINFO_);
    }

  PvlGroup &inst = icube->group("Instrument");
  PvlGroup &bandbin = icube->group("BandBin");

  try{
    g_filter = bandbin["FilterName"][0];
  }

  catch(IException &e) {
    QString msg = "Unable to read FilterName keyword in the BandBin group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);

  }

  QString instrument("");

  try {
    instrument = inst["InstrumentId"][0];
  }
  catch(IException &e) {
    QString msg = "Unable to read InstrumentId keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);

  }

  if ( instrument=="ONC-W1" ) {
    g_instrument = InstrumentType::ONCW1;
  }
  else if ( instrument=="ONC-W2" ) {
    g_instrument = InstrumentType::ONCW2;
  }
  else if ( instrument == "ONC-T" ) {
      g_instrument = InstrumentType::ONCT;
  }
  else {
        QString msg = "Unidentified instrument key in the "
                      "InstrumentId key of the Instrument Pvl group.";
          throw IException(IException::Io,msg, _FILEINFO_);
  }



  //Set up binning and image subarea mapping
  binning = inst["Binning"];
  int startLine = inst["SelectedImageAreaY1"];
  int startSample = inst["SelectedImageAreaX1"];
  int lastLine = inst["SelectedImageAreaY2"];
  int lastSample = inst["SelectedImageAreaX2"];

  AlphaCube myAlpha(1024,1024,icube->sampleCount(), icube->lineCount(),
  startSample,startLine,lastSample,lastLine);

  try {
    g_bitDepth = inst["BitDepth"];
  }
  catch (IException &e) {
    QString msg = "Unable to read [BitDepth] keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    //qDebug() << msg;
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
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }


  try {
    g_AEtemperature = inst["ONCAETemperature"][0].toDouble();
    qDebug() << "g_AEtemperature:  " << g_AEtemperature;
  }
  catch(IException &e) {
    QString msg = "Unable to read [ONCAETemperature] keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);

  }

  try {  
    g_CCD_T_temperature = inst["ONCTCCDTemperature"][0].toDouble();
    qDebug() << "g_CCD_T_Temperature:  " << g_CCD_T_temperature;
  }
  catch(IException &e) {
    QString msg = "Unable to read [ONCTCCDTemperature] keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }

  try {
    g_ECT_T_temperature = inst["ONCTElectricCircuitTemperature"][0].toDouble();
    qDebug() << "g_ECT_T_temperature: " << g_ECT_T_temperature;
  }
  catch(IException &e) {
    QString msg = "Unable to read [ONCTElectricCircuitTemperature] keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }


  try {

    g_startTime=inst["SpacecraftClockStartCount"][0];
  }

  catch (IException &e) {
    QString msg = "Unable to read [SpacecraftClockStartCount] keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }


  QString smearCorrection;

  try {
    smearCorrection = inst["SmearCorrection"][0];
  }

  catch (IException &e) {
    QString msg = "Unable to read [SmearCorrection] keyword in the Instrument group "
    "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);
  }

  if (smearCorrection=="ONBOARD") {
    g_onBoardSmearCorrection=true;
  }

  QString compmode = inst["Compression"];
  // TODO: verify that the compression factor/scale is actually 16 for compressed Hayabusa2 images.
  g_compfactor = ( "lossy" == compmode.toLower() ) ? 16.0 : 1.0;

  QString target = inst["TargetName"];
  g_target = target;


  // NOTE we do not have a valid flat-field for the W1 or W2 images.
  FileName flatfile = "NONE";
  PvlGroup alphaCube;

  if (g_instrument == InstrumentType::ONCT) {
    QScopedPointer<Cube, TemporaryCubeDeleter> flatcube;
    flatfile = DetermineFlatFieldFile(g_filter);
    QString reducedFlat(flatfile.expanded());

    try {
      alphaCube = icube->group("AlphaCube");
    }

   catch(IException &e) {
     g_cropped =false;
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
        //QScopedPointer<Cube, TemporaryCubeDeleter> reduced(new Cube(reducedFlat, "r"));
        //flatcube.swap( reduced );
      }

      // Set up processing for flat field as a second input file
      CubeAttributeInput att;
      p.SetInputCube(reducedFlat, att);
    }
    else {

      // Image is cropped so we have to deal with it
      FileName transFlat =
      FileName::createTempFile("$TEMPORARY/" + flatfile.baseName() + "_translated.cub");

      Cube *flatOriginal = new Cube(flatfile.expanded() );

      //int transform[5] = {binning,startSample,startLine,lastSample,lastLine};

      double alphaStartSample = alphaCube["AlphaStartingSample"][0].toDouble();
      double alphaStartLine = alphaCube["AlphaStartingLine"][0].toDouble();
      double alphaEndSample = alphaCube["AlphaEndingSample"][0].toDouble();
      double alphaEndLine = alphaCube["AlphaEndingLine"][0].toDouble();

      //int transform[5] = {binning,startSample,startLine,lastSample,lastLine};
      double transform[5] = {(double)binning,alphaStartSample,alphaStartLine,alphaEndSample,alphaEndLine};

      // Translates and scales the flatfield image.  Scaling
      // might be necessary in the event that the raw image was also binned.

      translate(flatOriginal,transform,transFlat.expanded());

      QScopedPointer<Cube, TemporaryCubeDeleter> translated(new Cube(transFlat.expanded(), "r"));
      flatcube.swap(translated);

      CubeAttributeInput att;   

      p.SetInputCube(transFlat.expanded(),att);

  }  //Finished setting flatfield file for ONC-T

 }



  Cube *ocube  = p.SetOutputCube("TO");
  //QString fname = ocube->fileName();

  QString calfile = loadCalibrationVariables(ui.GetAsString("CONFIG"));

  g_timeRatio = g_Tvct/(g_texp + g_Tvct);

  g_iof = 1.0;  // Units of DN

  QString g_units = "DN";
  // if ( "radiance" == g_iofCorrection.toLower() ) {
  //   // Units of RADIANCE
  //   g_iof = g_iof * g_iofScale;
  //   g_units = "W / (m**2 micrometer sr)";
  // }
  //
  // if ( !sunDistanceAU(startTime, target, g_solarDist) ) {
  //    throw IException(IException::Programmer, "Cannot calculate distance to sun!",
  //                      _FILEINFO_);
  // }
  //
  // if ( "iof" == g_iofCorrection.toLower() ) {
  //   // Units of I/F
  //   // TODO: Note, we do not have a correct g_iofScale (== 1 right now). This was provided for
  //   // Hayabusa AMICA's v-band and all other bands were normalized according to this value. We do
  //   // not have this value for Hayabusa2 ONC. Need to correct this value.
  //   g_iof = pi_c() * (g_solarDist * g_solarDist) *
  //           g_iofScale / g_solarFlux / g_texp;
  //   g_units = "I over F";
  // }

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
  calibrationLog.addKeyword(PvlKeyword("CompressionFactor", toString(g_compfactor, 2)));

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

#if 0
  PvlKeyword linearityCoefs("Linearity");
  linearityCoefs.addValue(toString(g_L0,8));
  linearityCoefs.addValue(toString(g_L1,8));
  linearityCoefs.addValue(toString(g_L2,8));
  calibrationLog.addKeyword(linearityCoefs);

#endif


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

  calibrationLog.addKeyword(PvlKeyword("CalibrationUnits", g_iofCorrection));
  calibrationLog.addKeyword(PvlKeyword("RadianceStandard", toString(g_v_standard, 16)));
  calibrationLog.addKeyword(PvlKeyword("RadianceScaleFactor", toString(g_iofScale, 16)));
  calibrationLog.addKeyword(PvlKeyword("SolarDistance", toString(g_solarDist, 16), "AU"));
  calibrationLog.addKeyword(PvlKeyword("SolarFlux", toString(g_solarFlux, 16)));
  calibrationLog.addKeyword(PvlKeyword("IOFFactor", toString(g_iof, 16)));
  calibrationLog.addKeyword(PvlKeyword("Units", g_units));

  // Write Calibration group to output file
  ocube->putGroup(calibrationLog);
  Application::Log(calibrationLog);
  //configFile.clear();
  p.EndProcess();

}


/**
* @brief Determine name of flat field file to apply
* @author 2016-03-30 Kris Becker
* @param filter  Name of ONC filter
* @return FileName Path and name of flat file file
* @internal
*   @history 2017-07-27 Ian Humphrey & Kaj Williams - Adapted from amicacal.
*/
FileName DetermineFlatFieldFile(const QString &filter) {

  QString fileName = "$hayabusa2/calibration/flatfield/";
  // FileName consists of binned/notbinned, camera, and filter
  fileName += "flat_" + filter.toLower() + ".cub";
  FileName final(fileName);
  return final;
}



/**
* @brief Loads the calibration variables into the program.
* @param config QString Name of the calibration file to load.
*
* Loads g_b0-g_b2,g_bae0-g_bae1,g_d0-g_d1
*
*
*/
QString loadCalibrationVariables(const QString &config)  {



  //  UserInterface& ui = Application::GetUserInterface();

  FileName calibFile(config);
  if ( config.contains("?") ) calibFile = calibFile.highestVersion();

  // Pvl configFile;
  g_configFile.read(calibFile.expanded());

  // Load the groups
  PvlGroup &Bias = g_configFile.findGroup("Bias");
  PvlGroup &DarkCurrent = g_configFile.findGroup("DarkCurrent");
  PvlGroup &Smear = g_configFile.findGroup("SmearRemoval");
  PvlGroup &solar = g_configFile.findGroup("SOLARFLUX");
  PvlGroup &linearity = g_configFile.findGroup("Linearity");
  // PvlGroup &iof = g_configFile.findGroup("RAD");

  // Load Smear Removal Variables
  g_Tvct = Smear["Tvct"];



  // Load DarkCurrent variables and calculate the dark current
  g_d0 = DarkCurrent["D"][0].toDouble();
  g_d1 = DarkCurrent["D"][1].toDouble();
  double CCDTemp(0.0);

  switch (g_instrument) {
    case InstrumentType::ONCT:
      CCDTemp = g_CCD_T_temperature;
    break;
    case InstrumentType::ONCW1:
      CCDTemp = g_CCD_W1_temperature;
    break;
    case InstrumentType::ONCW2:
      CCDTemp = g_CCD_W2_temperature;
    break;
    default:
      CCDTemp = g_CCD_T_temperature;
  }

   g_darkCurrent = g_texp*exp(0.10*CCDTemp+0.52);

  // Load Bias variables
  g_b0 = Bias["B"][0].toDouble();
  g_b1 = Bias["B"][1].toDouble();
  g_b2 = Bias["B"][2].toDouble();
  g_bae0 = Bias["B_AE"][0].toDouble();
  g_bae1 = Bias["B_AE"][1].toDouble();



  // Compute BIAS correction factor (it's a constant so do it once!)
  g_bias = g_b0+g_b1*g_CCD_T_temperature+g_b2*g_ECT_T_temperature;
  g_bias *= (g_bae0 + g_bae1*g_AEtemperature); //correction factor
  qDebug() << "Bias: " << g_bias;

  // Load the Solar Flux for the specific filter
  g_solarFlux = solar[g_filter.toLower()];

  //Load the linearity variables
  g_L0 = linearity["L"][0].toDouble();
  g_L1 = linearity["L"][1].toDouble();
  g_L2 = linearity["L"][2].toDouble();


  // radiance = g_v_standard * g_iofScale
  // iof      = radiance * pi *dist_au^2
  // g_iofScale   = iof[g_filter];

  return ( calibFile.original() );
}


/**
* @brief Apply radiometric correction to each line of an AMICA image.
* @author 2016-03-30 Kris Becker
* @param in   Raw image and flat field
* @param out  Radometrically corrected image
* @internal
*   @history 2017-07-2017 Ian Humphrey & Kaj Williams - Adapted from amicacal.
*/
void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out) {

  Buffer& imageIn   = *in[0];
  Buffer& flatField = *in[1];
  Buffer& imageOut  = *out[0];

  int pixelsToNull = 0;

  // Note that is isn't currently tested, as we do not have a test with a hayabusa2 image that
  // has been on-board cropped.
  int currentSample = imageIn.Sample();
  int alphaSample = alpha->AlphaSample(currentSample);

  if ( (alphaSample <= pixelsToNull)  || (alphaSample >= (1024 - pixelsToNull ) ) ) {

    for (int i = 0; i < imageIn.size(); i++ ) {
      imageOut[i] = Isis::Null;
    }
    return;
  }

  // Iterate over the line space
  for (int i = 0; i < imageIn.size(); i++) { 
    imageOut[i] = imageIn[i]*pow(2.0,12-g_bitDepth);



    // Check for special pixel in input image and pass through
    if ( IsSpecial(imageOut[i]) ) {
      imageOut[i] = imageIn[i];
      continue;
    }

    // Apply compression factor here to raise LOSSY dns to proper response
    imageOut[i] *= g_compfactor;


    // 1) BIAS Removal - Only needed if not on-board corrected


    if ( !g_onBoardSmearCorrection ) {


      if ( (imageOut[i] - g_bias) <= 0.0) {
        imageOut[i] = Null;
        continue;
      }
      else {
        imageOut[i] = imageOut[i] - g_bias;
      }
    }


#if 0
    double linearCorrection;
    linearCorrection = g_L0+g_L1*pow(imageOut[i],2.0)+g_L2*pow(imageOut[i],3.0);
    imageOut[i]*=linearCorrection;

#endif


    // DARK Current
    imageOut[i] = imageOut[i] - g_darkCurrent;




    // READOUT Smear Removal - Not needed if on-board corrected.  Binning is
    //    accounted for in computation of c1 before loop.
    // if (nsubImages <= 1) {
    //  imageOut[i] = c1*(imageOut[i] - smear);
    // }
    if (!g_onBoardSmearCorrection) {

      double smear = 0;
      for (int j=0;j < imageIn.size();j++) {
        smear += (imageOut[j]/imageIn.size() );
      }
      smear*=g_timeRatio;
      imageOut[i] = imageOut[i] - smear;

      }

    //Linearity Correction
    //In the SIS this adjustment is made just after the bias, but
    //in the Calibration paper it happens just before the flat field correction.

#if 0
    double linearCorrection;
    linearCorrection = g_L0+g_L1*pow(imageOut[i],2.0)+g_L2*pow(imageOut[i],3.0);
    qDebug() << "linearCorrection=" << linearCorrection;
    imageOut[i]*=linearCorrection;
 #endif


    // FLATFIELD correction
    //  Check for any special pixels in the flat field (unlikely)
    // If we have only one input cube, that means that we do not have a flat-field (W1/W2).
    if (in.size() == 2) {
      // Note that our current flat-fields to not have special pixel values.
      if ( IsSpecial(flatField[i])  || IsSpecial(imageOut[i]) )
      {
        imageOut[i] = Isis::Null;
        continue;
      }
      else {
        if (flatField[i] != 0) {                 
          imageOut[i] /= flatField[i];  
        }
      }
    }

    // TODO: once the radiance values are known for each band, we can correctly compute I/F.
    // For now, g_iof is 1, so output will be in DNs.
    // 7) I/F or Radiance Conversion (or g_iof might = 1, in which case the output will be in DNs)
    imageOut[i] *= g_iof;

  }


  return;
}
