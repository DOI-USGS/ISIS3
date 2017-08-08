#ifndef Hyb2OncCalUtils_h
#define Hyb2OncCalUtils_h

#include <cmath>
#include <string>
#include <vector>
#include <QString>

#include "CSVReader.h"
#include "IException.h"
#include "FileName.h"
#include "LineManager.h"
#include "NaifStatus.h"
#include "IString.h"
#include "Pvl.h"
#include "PvlGroup.h"

#include "Spice.h"



// OpenCV libraries
#include <opencv2/opencv.hpp>


/**
 * @author 2016-04-04 Tyler Wilson
 *
 *
 */
using namespace cv;
using namespace std;

namespace Isis {


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
void  translate(Cube *flatField, int *transform, QString fname) {

  Mat * originalMat = isis2mat(flatField);
  int scale = transform[0];

  int startsample = transform[1];
  int startline = transform[2];
  int lastsample = transform[3];
  int lastline = transform[4];

  int width  = (lastsample-startsample);
  int height = (lastline-startline);

  Size sz(flatField->lineCount()/scale,flatField->sampleCount()/scale);

  Mat * resizedMatrix = new Mat();

  Mat temp = *originalMat;


  Mat originalCropped = temp(Rect(startsample,startline,width+1,height+1));


  if (scale ==1) {   
    mat2isis(&originalCropped,fname);
  }
  else {
    //Bilinear interpolation
    resize(originalCropped,*resizedMatrix,sz,INTER_LINEAR);
    mat2isis(resizedMatrix,fname);
  }

}
}

#endif

