#ifndef AmicaCalUtils_h
#define AmicaCalUtils_h


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



// OpenCV stuff


#include <opencv2/opencv.hpp>


/**
 * @author 2016-04-04 Tyler Wilson
 *
 *
 */

using namespace std;
using namespace cv;




namespace Isis {


/**
 * @brief Load required NAIF kernels required for timing needs
 *
 * This method maintains the loading of kernels for HAYABUSA timing and
 * planetary body ephemerides to support time and relative positions of planet
 * bodies.
 */
static void loadNaifTiming() {
  static bool naifLoaded = false;
  if (!naifLoaded) {
//  Load the NAIF kernels to determine timing data
    Isis::FileName leapseconds("$base/kernels/lsk/naif????.tls");
    leapseconds = leapseconds.highestVersion();

    Isis::FileName sclk("$hayabusa/kernels/sclk/hayabusa.tsc");    

    Isis::FileName pck1("$hayabusa/kernels/tspk/de403s.bsp");
    Isis::FileName pck2("$hayabusa/kernels/tspk/sb_25143_140.bsp");
    Isis::FileName pck3("$hayabusa/kernels/spk/hay_jaxa_050916_051119_v1n.bsp");
    Isis::FileName pck4("$hayabusa/kernels/spk/hay_osbj_050911_051118_v1n.bsp");

//  Load the kernels
    QString leapsecondsName(leapseconds.expanded());
    QString sclkName(sclk.expanded());


    QString pckName1(pck1.expanded());
    QString pckName2(pck2.expanded());
    QString pckName3(pck3.expanded());
    QString pckName4(pck4.expanded());



    furnsh_c(leapsecondsName.toAscii().data());
    furnsh_c(sclkName.toAscii().data());

    furnsh_c(pckName1.toAscii().data());
    furnsh_c(pckName2.toAscii().data());
    furnsh_c(pckName3.toAscii().data());
    furnsh_c(pckName4.toAscii().data());


//  Ensure it is loaded only once
    naifLoaded = true;
  }
  return;
}


/**
 * @brief Computes the distance from the Sun to the observed body
 *
 * This method requires the appropriate NAIK kernels to be loaded that
 * provides instrument time support, leap seconds and planet body ephemeris.
 *
 * @return double Distance in AU between Sun and observed body
 */
static bool sunDistanceAU(const QString &scStartTime,
                          const QString &target,
                          double &sunDist) {

  //  Ensure NAIF kernels are loaded
  loadNaifTiming();
  sunDist = 1.0;

  //  Determine if the target is a valid NAIF target
  SpiceInt tcode;
  SpiceBoolean found;
  bodn2c_c(target.toAscii().data(), &tcode, &found);
  //cout << "tcode = " << tcode << endl;
  //cout << "found = " << found << endl;
  if (!found) return (false);

  //  Convert starttime to et
  double obsStartTime;
  scs2e_c(-130, scStartTime.toAscii().data(), &obsStartTime);

  //cout << "obsStartTime =  "  << obsStartTime << endl;

  //  Get the vector from target to sun and determine its length
  double sunv[3];
  double lt;
  spkpos_c(target.toAscii().data(), obsStartTime, "J2000", "LT+S", "sun",
                  sunv, &lt);
  NaifStatus::CheckErrors();

  double sunkm = vnorm_c(sunv);

  //cout << "lt = " << lt << endl;
  //  Return in AU units
  sunDist = sunkm / 1.49597870691E8;

  //cout << "sunDist = " << sunDist << endl;
  return (true);
}


/**
 * @brief Translates a 1-banded Isis::Cube to an OpenMat object
 *
 * @author 2016-04-19 Tyler Wilson
 *
 * @param icube A pointer to the input cube
 *
 * @return Mat A pointer to the OpenMat object
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
 * @brief Translates an OpenMat object to an Isis::Cube with one band
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
 * @param cubeName The name of the Isis::Cube that is being created.
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

/**
 * @brief This function characterizes the point spread function
 * for near light sources.  See equation [1] in:
 * "Scattered light correction of Hayabusa/AMICA data and quantitative spectral
 * comparisons of Itokawa", Ishiguro (2014)
 *
 * @author 2016-04-19 Tyler Wilson
 *
 * @param alpha An empirically derived constant which is stored in
 *        $hayabusa/calibration/amica/amicaCalibration????.trn"
 *        Each filter has a different alpha value.
 *
 * @param r The distance (in pixels) from the optical center of the
 *        point source
 *
 * @return
 *
 */



static double f_focused(double alpha,int binning,double x,double y) {

        double X = x*binning;
        double Y = y*binning;
        double r = sqrt(X*X+Y*Y);
        return exp(-alpha*r);

}



/**
 * @brief This function characterizes an attenuation function modeled as a sum
 * of Gaussian functions.  See equation [2] in:
 * "Scattered light correction of Hayabusa/AMICA data and quantitative spectral
 * comparisons of Itokawa", Ishiguro (2014)
 *
 * @author 2016-04-19 Tyler Wilson
 *
 * @param A An empirically derived vector of constants of length N which is stored in
 *        $hayabusa/calibration/amica/amicaCalibration????.trn"
 *        Each filter has a different A vector.
 *
 * @param sigma An empirically derived vector of standard deviations of length N
 *        also stored in $hayabusa/calibration/amica/amicaCalibration????.trn"
 *
 * @return
 *
 */

/*
 * static double f_diffuse(double *A,double *sigma,int N,double r){

  double sum = 0;

  for (int i = 0; i < N; i ++)   {
    sum += (A[i]/(sigma[i]*sqrt(2.0*pi_c() ) ) )*exp(-(r*r)/(2*sigma[i]*sigma[i]) );
  }


  return sum;


}
*/


static double f_unfocused(double * A,double * sigma, int N,int binning,double x,double y)  {

  //double X = abs(binning*x-512);
  //double Y = abs(binning*y-512);

  double X = binning*x;
  double Y = binning*y;

  double r = sqrt(X*X+Y*Y);

  double sum = 0;

  for (int i = 0; i < N; i ++)   {     
    sum += (A[i]/(sigma[i]*sqrt(2.0*pi_c() ) ) )*exp(-(r*r)/(2*sigma[i]*sigma[i]) );


  }


  return sum;

}


/*

static double psf(double *A, double *sigma, int N,double alpha,double Ixy,int binning,double x, double y) {



    //double psfVal = f_unfocused(A,sigma,N,x,y);
    //double psfVal = Ixy*f_unfocused(A,sigma,N,binning,x,y);

    //double psfVal = Ixy*(f_unfocused(A,sigma,N,binning,x,y)  + f_focused(alpha,x,y) );

    double psfVal = Ixy*f_unfocused(A,sigma,N,binning,x,y);



    return psfVal;



}


*/





double * setPSFFilter(int size, double *A,double *sigma, double alpha,int N,int binning) {


  double * psfVals = new double[size*size];


  int i = 0;


  for(double y = -(size / 2) ; y <= (size / 2) ; y++) {
    for(double x = -(size / 2) ; x <= (size / 2) ; x++) {

       psfVals[i]=f_unfocused(A,sigma,N,binning,x,y)+f_focused(alpha,binning,x,y);
       //psfVals[i]=f_unfocused(A,sigma,N,binning,x,y);
         //psfVals[i]=f_focused(alpha,binning,x,y);

      i++;
    }
  }

  return psfVals;
}




}






#endif

