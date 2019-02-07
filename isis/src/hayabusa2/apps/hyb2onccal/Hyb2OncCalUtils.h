#ifndef Hyb2OncCalUtils_h
#define Hyb2OncCalUtils_h




#include "CSVReader.h"
#include "IException.h"
#include "LineManager.h"
#include "NaifStatus.h"



#include <string>
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





// OpenCV libraries
#include <opencv2/opencv.hpp>


/**
 * @author 2016-04-04 Tyler Wilson
 *
 *
 */
using namespace cv;
using namespace std;


static int g_bitDepth(12);

//For subimage and binning mapping

static bool g_cropped(true);

static QString g_filter = "";
static QString g_target ="";


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
static double g_L[3] = {0.0,0.0,0.0};

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

namespace Isis {

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
InstrumentType g_instrument;

static AlphaCube *alpha(0);

static Pvl g_configFile;

double linearFun(double Iobs,double x, double g[3]) {
  return Iobs - g[0]*x -g[1]*pow(x,2.0) -g[2]*pow(x,3.0);

}

double dFun(double x, double g[3]) {
  return -g[0] - 2*g[1]*x -3*g[2]*pow(x,2.0);

}


bool newton_rapheson(double Iobs,double x0, double g[3],double &result, double epsilon=1e-6 )  {

   double x[2];
   double dx = 1.0;
   int iter = 0;
   int maxIterations=500;
   x[0] = x0;
   while (dx > epsilon)  {

     x[1]=x[0] - linearFun(Iobs,x[0],g)/dFun(x[0],g);
     dx = fabs(x[1]-x[0]) ;
     x[0]=x[1];
     iter++;
     if (iter > maxIterations) {

       return false;
     }
   }
   result = x[1];
   return true;
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


    double dn = imageOut[i];
    double linearCorrection;
    double result = 1.0;
    double x0 = 1.0;
    newton_rapheson(imageOut[i],x0, g_L,result );
    //linearCorrection = g_L[0]*dn+g_L[1]*pow(dn,2.0)+g_L[2]*pow(dn,3.0);
    imageOut[i] = result;

    //qDebug() << dn << ","<< result;


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



    //imageOut[i]*=linearCorrection;



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



/**
 * @brief Load required NAIF kernels required for timing needs.
 *
 * This method maintains the loading of kernels for HAYABUSA timing and
 * planetary body ephemerides to support time and relative positions of planet
 * bodies.
 */
/* Helper function for sunDistanceAu, don't need this until we have radiance calibration
   parameters for Hayabusa2 ONC-T filters to calculate radiance and I/F
static void loadNaifTiming() {
  static bool naifLoaded = false;
  if (!naifLoaded) {

//  Load the NAIF kernels to determine timing data
    Isis::FileName leapseconds("$base/kernels/lsk/naif????.tls");
    leapseconds = leapseconds.highestVersion();
    Isis::FileName sclk("$hayabusa2/kernels/sclk/hyb2_20141203-20161231_v01.tsc");    
    Isis::FileName pck1("$hayabusa2/kernels/tspk/de430.bsp");
    Isis::FileName pck2("$hayabusa2/kernels/tspk/jup329.bsp");
    Isis::FileName pck3("$hayabusa2/kernels/tspk/sat375.bsp");
    Isis::FileName pck4("$hayabusa2/kernels/spk/hyb2_20141203-20141214_0001m_final_ver1.oem.bsp");
    Isis::FileName pck5("$hayabusa2/kernels/spk/hyb2_20141203-20151231_0001h_final_ver1.oem.bsp");
    Isis::FileName pck6("$hayabusa2/kernels/spk/hyb2_20151123-20151213_0001m_final_ver1.oem.bsp");

//  Load the kernels
    QString leapsecondsName(leapseconds.expanded());
    QString sclkName(sclk.expanded());

    QString pckName1(pck1.expanded());
    QString pckName2(pck2.expanded());
    QString pckName3(pck3.expanded());
    QString pckName4(pck4.expanded());
    QString pckName5(pck5.expanded());
    QString pckName6(pck6.expanded());

    furnsh_c(leapsecondsName.toLatin1().data());
    furnsh_c(sclkName.toLatin1().data());

    furnsh_c(pckName1.toLatin1().data());
    furnsh_c(pckName2.toLatin1().data());
    furnsh_c(pckName3.toLatin1().data());
    furnsh_c(pckName4.toLatin1().data());
    furnsh_c(pckName5.toLatin1().data());
    furnsh_c(pckName6.toLatin1().data());


//  Ensure it is loaded only once
    naifLoaded = true;
  }
  return;
}
*/


/**
 * @brief Computes the distance from the Sun to the observed body.
 *
 * This method requires the appropriate NAIK kernels to be loaded that
 * provides instrument time support, leap seconds and planet body ephemeris.
 *  
 * @return @b double Distance in AU between Sun and observed body.
 */
/* commented out until we have radiance values (RAD/IOF group in calibration trn) for Hayabusa2.
 static bool sunDistanceAU(const QString &scStartTime,
                          const QString &target,
                          double &sunDist) {

  //  Ensure NAIF kernels are loaded
  loadNaifTiming();
  sunDist = 1.0;

  //  Determine if the target is a valid NAIF target
  SpiceInt tcode;
  SpiceBoolean found;
  bodn2c_c(target.toLatin1().data(), &tcode, &found);

  if (!found) return (false);

  //  Convert starttime to et
  double obsStartTime;
  scs2e_c(-37, scStartTime.toLatin1().data(), &obsStartTime);

  //  Get the vector from target to sun and determine its length
  double sunv[3];
  double lt;
  spkpos_c(target.toLatin1().data(), obsStartTime, "J2000", "LT+S", "sun",
                  sunv, &lt);
  NaifStatus::CheckErrors();

  double sunkm = vnorm_c(sunv);


  //  Return in AU units
  sunDist = sunkm / 1.49597870691E8;

  //cout << "sunDist = " << sunDist << endl;
  return (true);
}
*/


/**
 * @brief Translates a 1-banded Isis::Cube to an OpenMat object
 *
 * @author 2016-04-19 Tyler Wilson
 *
 * @param icube A pointer to the input cube
 *
 * @return @b Mat A pointer to the OpenMat object
 */
Mat * isis2mat(Cube *icube) {

  int nlines = icube->lineCount();
  int nsamples = icube->sampleCount();
  Mat *matrix = new Mat(nlines,nsamples,CV_64F);


  // Set up line manager and read in the data
  LineManager linereader(*icube);
  for (int line = 0 ; line < nlines ; line++) {
    linereader.SetLine(line+1);
    icube->read(linereader);
    for (int samp = 0 ;  samp < nsamples ; samp++) {
      matrix->at<double>(line,samp) = (double)linereader[samp];
    }
 }


return matrix;

}


/**
 * @brief Translates an OpenMat object to an ISIS::Cube with one band
 *
 * @author 2016-04-19 Tyler Wilson
 *
 * @param matrix A pointer to the OpenMat object
 *
 * @param cubeName The name of the Isis::Cube that is being created.
 *
 */
void mat2isis(Mat *matrix, QString cubeName) {

  int nlines = matrix->rows;
  int nsamples = matrix->cols;
  CubeAttributeOutput set;
  set.setPixelType(Real);

  Cube ocube;
  ocube.setDimensions(nsamples,nlines,1);
  ocube.create(cubeName,set);

  LineManager linewriter(ocube);

  for (int line =0; line < nlines; line++) {
    linewriter.SetLine(line+1);

    for ( int samp=0; samp<nsamples; samp++ ) {

      linewriter[samp] = matrix->at<double>(linewriter.Line()-1,samp);

    }
    ocube.write(linewriter);

  }

  ocube.close();

}


/**
 * @brief Translates/scales a cube using Bilinear Interpolation
 *
 * @author 2016-04-19 Tyler Wilson
 *
 * @param matrix A pointer to the OpenMat object
 *
 * @param cubeName The name of the ISIS::Cube that is being created.
 *
 */

void  translate(Cube *flatField,double *transform, QString fname) {


  Mat * originalMat = isis2mat(flatField);
  double scale = transform[0];

  double startsample = transform[1];
  double startline = transform[2];
  double lastsample = transform[3];
  double lastline = transform[4];

  double  width  = (lastsample-startsample);
  double height = (lastline-startline);

  Size sz(flatField->lineCount()/scale,flatField->sampleCount()/scale);

  Mat * resizedMatrix = new Mat();

  Mat temp = *originalMat;


  Mat originalCropped = temp(Rect(startsample,startline,width,height));

  if (scale ==1) {
    mat2isis(&originalCropped,fname);
  }
  else {
    //Bilinear interpolation
    resize(originalCropped,*resizedMatrix,sz,INTER_LINEAR);
    mat2isis(resizedMatrix,fname);
  }

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
  g_L[0] = linearity["L"][0].toDouble();
  g_L[1] = linearity["L"][1].toDouble();
  g_L[2] = linearity["L"][2].toDouble();


  // radiance = g_v_standard * g_iofScale
  // iof      = radiance * pi *dist_au^2
  // g_iofScale   = iof[g_filter];

  return ( calibFile.original() );
}












}

#endif

