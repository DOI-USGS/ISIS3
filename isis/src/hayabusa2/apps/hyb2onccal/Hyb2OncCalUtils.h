#ifndef Hyb2OncCalUtils_h
#define Hyb2OncCalUtils_h

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
#include "CSVReader.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "LineManager.h"
#include "NaifStatus.h"
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
 */
using namespace cv;
using namespace std;


static int g_bitDepth(12);

//For subimage and binning mapping

static bool g_cropped(true);

static QString g_filter = "";
static QString g_target = "";


//Bias calculation variables
static double g_b0(0);
static double g_b1(0);
static double g_b2(0);
static double g_bae0(0);
static double g_bae1(0);
static double g_bias(0);

//Device (AE/CCD/ECT temps for ONC-T, ONC-W1, ONC-W2

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

static double g_L[3] = {0.0, 0.0, 0.0};

// TODO: we do not have the readout time (transfer period) for Hayabusa2 ONC.
//Smear calculation variables
static bool g_onBoardSmearCorrection(false);
static double g_Tvct(0);       // Vertical charge-transfer period (in seconds).
static double g_texp(1);       // Exposure time.
static double g_timeRatio(1.0);

// Calibration parameters
static int binning(1);         //!< The number of samples/lines which are binned
static double g_compfactor(1.0);  // Default if OutputMode = LOSS-LESS; 16.0 for LOSSY

static QString g_calStep("IOF");  //!< Which calibrations step should we stop at?
static double g_calibrationScale = 1.0; //!< DN, Radiance or I/F conversion factor

// I/F variables
static double g_solarDist(1.0);
static double g_iofScale(1.0);
static double g_solarFlux(1.0);  //!< The solar flux (used to calculate g_iof).
static double g_sensitivity(1.0);
static double g_effectiveBandwidth(1.0);
static double g_J(1.0);

namespace Isis {

  // Temporary cube file pointer deleter
  struct TemporaryCubeDeleter {
    static inline void cleanup(Cube *cube) {
      if (cube) {
        FileName filename(cube->fileName());
        delete cube;
        remove(filename.expanded().toLatin1().data());
      }
    }
  };

  enum InstrumentType{ONCW1, ONCW2, ONCT};
  InstrumentType g_instrument;

  static AlphaCube *alpha(0);
  static Pvl g_configFile;


  /**
   * @brief linearFun:  The linear correction function (used by the newton_rapheson method)
   * @author 2019-02-12  Tyler Wilson
   * @param Iobs:  The observed intensity
   * @param x:  The ideal intensity.
   * @param g:  The vector of empirically derived coefficients for the third-order polynomial
   * modelling the linear correction (for DN values < 3400 DN)
   * @return The value of the function at the point x.
   */
  double linearFun(double Iobs,double x, double g[3]) {
    return Iobs - (g[0] * x) - (g[1] * pow(x, 2.0)) - (g[2] * pow(x, 3.0));
  }

  /**
   * @brief dFun:  The first-order derivative of linearFun
   * @author 2019-02-12  Tyler Wilson
   * @param x:  The ideal intensity.
   * @param g:  The vector of empirically derived coefficients for the third-order polynomial
   * modelling the linear correction (for DN values < 3400 DN)
   * @return
   */
  double dFun(double x, double g[3]) {
    return -g[0] - (2 * g[1] * x) - (3 * g[2] * pow(x, 2.0));

  }

  /**
   * @brief newton_rapheson
   * @author 2019-02-12 Tyler Wilson
   * @param Iobs:  The observed DN intensity
   * @param x0:  The starting value for the Newton-Rapheson method
   * @param g:  A vector of the coefficients for the linearity function.  It is a third-order
   * polynomial.
   * @param result:  The final approximation of the root of the equation.
   * @param epsilon:  The tolerance on the final solution.
   * @return A root of the linearity correction function, centered near the origin.
   */
  bool newton_rapheson(double Iobs,double x0, double g[3],double &result, double epsilon=1e-6 )  {

     double x[2];
     double dx = 1.0;
     int iter = 0;
     int maxIterations = 500;
     x[0] = x0;

     while (dx > epsilon) {
       x[1] = x[0] - linearFun(Iobs, x[0], g) / dFun(x[0], g);
       dx = fabs(x[1] - x[0]);
       x[0] = x[1];
       iter++;
       if (iter > maxIterations) {
         return false;
       }
     }
     result = x[1];
     return true;
  }


  /**
  * @brief Apply radiometric correction to each line of a Hayabusa2 image.
  * @author 2016-03-30 Kris Becker
  * @param in   Raw image and flat field
  * @param out  Radometrically corrected image
  * @internal
  *   @history 2017-07-2017 Ian Humphrey & Kaj Williams - Adapted from amicacal.
  *   @history 2019-02-12 Tyler Wilson - Modified to support new calibration settings/formulas.
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

    if ( (alphaSample <= pixelsToNull) || (alphaSample >= (1024 - pixelsToNull)) ) {
      for (int i = 0; i < imageIn.size(); i++) {
        imageOut[i] = Isis::Null;
      }
      return;
    }

    double smear = 0;
    int skipCount = 0;
    for (int i = 0; i < imageIn.size(); i++) {
      if (!IsSpecial(imageIn[i])) {
        // Left out g_darkCurrent subtraction for now.
        smear += (imageIn[i] * pow(2.0, 12 - g_bitDepth) - g_bias);
      }
      else {
        skipCount++;
      } 
    }
    smear /= (imageIn.size() - skipCount);
    smear *= g_timeRatio;

    // Iterate over the line space
    for (int i = 0; i < imageIn.size(); i++) {
      // Check for special pixel in input image and pass through
      if ( IsSpecial(imageIn[i]) ) {
        imageOut[i] = Isis::Null;
        continue;
      }

      imageOut[i] = imageIn[i] * pow(2.0, 12 - g_bitDepth);

      // Apply compression factor here to raise LOSSY dns to proper response

      // BIAS Removal - Only needed if not on-board corrected
      if (!g_onBoardSmearCorrection) {
        if ( (imageOut[i] - g_bias) <= 0.0) {
          imageOut[i] = Null;
          continue;
        }
        else {

          imageOut[i] = imageOut[i] - g_bias;
        }
      }

      // Smear correction
      if (!g_onBoardSmearCorrection) {
        imageOut[i] = imageOut[i] - smear;
      }

      // Both dark current and linearity correction are commented out for now since the JAXA team
      // does not currently do these steps.

      // DARK Current
      // imageOut[i] = imageOut[i] - g_darkCurrent;    
      
      // Linearity Correction
      // double dn = imageOut[i];    
      // double result = 1.0;
      // double x0 = 1.0;
      // newton_rapheson(dn,x0, g_L,result );   
      // imageOut[i] = result;

      // FLATFIELD correction
      // Check for any special pixels in the flat field (unlikely)
      // If we have only one input cube, that means that we do not have a flat-field (W1/W2).
      if (IsSpecial(flatField[i]) || IsSpecial(imageOut[i])) {
        imageOut[i] = Isis::Null;
        continue;
      }
      else {
        if (flatField[i] != 0) {
          imageOut[i] /= (flatField[i]);
        }
      }

      // DN, Radiance, or I/F Conversion 
      imageOut[i] *= g_calibrationScale;
    }
    return;
  }


  /**
   * @brief Translates a 1-banded Isis::Cube to an OpenMat object
   *
   * @author 2016-04-19 Tyler Wilson
   *
   * @param icube A pointer to the input cube
   *
   * @return @b Mat A pointer to the OpenMat object
   */
  Mat *isis2mat(Cube *icube) {
    int nlines = icube->lineCount();
    int nsamples = icube->sampleCount();
    Mat *matrix = new Mat(nlines, nsamples, CV_64F);

    // Set up line manager and read in the data
    LineManager linereader(*icube);
    for (int line = 0; line < nlines; line++) {
      linereader.SetLine(line + 1);
      icube->read(linereader);
      for (int samp = 0;  samp < nsamples; samp++) {
        matrix->at<double>(line, samp) = (double)linereader[samp];
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
   */
  void mat2isis(Mat *matrix, QString cubeName) {

    int nlines = matrix->rows;
    int nsamples = matrix->cols;
    CubeAttributeOutput set;
    set.setPixelType(Real);

    Cube ocube;
    ocube.setDimensions(nsamples, nlines,1);
    ocube.create(cubeName, set);

    LineManager linewriter(ocube);

    for (int line = 0; line < nlines; line++) {
      linewriter.SetLine(line+1);

      for ( int samp=0; samp < nsamples; samp++ ) {

        linewriter[samp] = matrix->at<double>(linewriter.Line() - 1,samp);
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
   */
  void translate(Cube *flatField,double *transform, QString fname) {
    Mat *originalMat = isis2mat(flatField);
    double scale = transform[0];

    double startsample = transform[1];
    double startline = transform[2];
    double lastsample = transform[3];
    double lastline = transform[4];

    double width = (lastsample - startsample);
    double height = (lastline - startline);

    Size sz(flatField->lineCount() / scale, flatField->sampleCount() / scale);

    Mat *resizedMatrix = new Mat();

    Mat temp = *originalMat;

    Mat originalCropped = temp(Rect(startsample, startline, width, height));

    if (scale == 1) {
      mat2isis(&originalCropped,fname);
    }
    else {
      //Bilinear interpolation
      resize(originalCropped, *resizedMatrix, sz, INTER_LINEAR);
      mat2isis(resizedMatrix, fname);
    }

  }


  /**
  * @brief Determine name of flat field file to apply
  * @author 2016-03-30 Kris Becker
  * @param filter  Name of ONC filter
  * @return FileName Path and name of flat file file
  * @internal
  *   @history 2017-07-27 Ian Humphrey & Kaj Williams - Adapted from amicacal.
  *   @history 2019-02-12 Tyler Wilson - Modified to support new calibration settings/formulas.
  */
  FileName DetermineFlatFieldFile(const QString &filter) {
    QString fileName = "$hayabusa2/calibration/flatfield/";

    if(g_instrument == InstrumentType::ONCT) {
      if(filter.toLower() == 'v') {
        // There is no updated v filter flat file
        fileName += "flat_" + filter.toLower() + "_norm.cub";
      }
      else {
        fileName += "hyb2_onc_flat_t" + filter.toLower() + "f_nr_trim_20190131.cub";
      }
    }
    else if(g_instrument == InstrumentType::ONCW1) {
      fileName += "hyb2_onc_flat_w1f_nr_20190131.cub";
    }
    else {
      fileName += "hyb2_onc_flat_w2f_nr_20190131.cub";
    }
    
    FileName final(fileName);
    return final;
  }



  /**
  * @brief Loads the calibration variables into the program.
  * @param config QString Name of the calibration file to load.
  *
  * Loads g_b0-g_b2,g_bae0-g_bae1,g_d0-g_d1
  */
  QString loadCalibrationVariables(const QString &config)  {
    FileName calibFile(config);
    if ( config.contains("?") ) calibFile = calibFile.highestVersion();

    // Pvl configFile;
    g_configFile.read(calibFile.expanded());

    // Load the groups
    PvlGroup &Bias = g_configFile.findGroup("Bias");
    PvlGroup &DarkCurrent = g_configFile.findGroup("DarkCurrent");
    PvlGroup &Smear = g_configFile.findGroup("SmearRemoval");
    PvlGroup &solar = g_configFile.findGroup("SOLARFLUX");
    PvlGroup &sensitivity = g_configFile.findGroup("SENSITIVITYFACTOR");
    PvlGroup & effectiveBW = g_configFile.findGroup("EFFECTIVEBW");
    PvlGroup &linearity = g_configFile.findGroup("Linearity");

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

     g_darkCurrent = g_texp * exp(g_d0 * (CCDTemp + g_d1));

    // Load Bias variables
    g_b0 = Bias["B"][0].toDouble();
    g_b1 = Bias["B"][1].toDouble();
    g_b2 = Bias["B"][2].toDouble();
    g_bae0 = Bias["B_AE"][0].toDouble();
    g_bae1 = Bias["B_AE"][1].toDouble();

    // Compute BIAS correction factor (it's a constant so do it once!)
    g_bias = g_b0 + (g_b1 * g_CCD_T_temperature) + (g_b2 * g_ECT_T_temperature);

    g_bias *= (g_bae0 - (g_bae1 * g_AEtemperature)); //bias correction factor
    
    // Load the Solar Flux for the specific filter
    g_solarFlux = solar[g_filter.toLower()];
    g_sensitivity = sensitivity[g_filter.toLower()];
    g_effectiveBandwidth = effectiveBW[g_filter.toLower()];

    g_J = g_solarFlux / (g_effectiveBandwidth * .0001);

    // Load the linearity variables
    g_L[0] = linearity["L"][0].toDouble();
    g_L[1] = linearity["L"][1].toDouble();
    g_L[2] = linearity["L"][2].toDouble();

    return calibFile.original();
  }
}

#endif

