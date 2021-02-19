#ifndef ApolloPanIO_h
#define ApolloPanIO_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                

namespace Isis {
 /* Numbering scheme for the fiducial marks
  * (gages on the right)
  * :--------------------------------------------------------------------------------------------:0
  * :0  2  4 ....                                                                  ... 84  86  88:0
  * :                                                                                            :0
  * :1  3  5 ....                                                                  ... 85  87  89:0
  * ----------------------------------------------------------------------------------------------0
 */


 /**
  * @brief Calculates a series of affine transformations from the measured coordinates of the
  *   up to 90 fiducial marks on a complete (stiched) apollo panoramic cube
  *
  * Description: The 90 Fiducial marks effectively divide a complete (stitched) apollo panormaic
  *   cube into 44 rectangular regions.  42 of these represent the same period of time, T, the
  *   center two are 0.5T.  This class uses up to 44 2 dimensional affine transformations linked
  *   together with first order continuity conditions at the boundaries to enforce consistent
  *   line scan durations and correct warping in the film.
  *
  * @ingroup Apollo
  *
  * @author 2011-09-19 Orrin Thomas
  *
  * @internal
  *   @history 2011-09-19 Orrin Thomas - Original version
  *   @history 2012-07-10 Orrin Thomas - Updated to current coding standards
  *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
  */
  class ApolloPanIO
  {
  private:

    /**
     * C style structure for storing the calculation optimized parameters of a forward and
     * reverse 2D affine transformation
     */
    typedef struct Affine2D {
      int flag;         //!< solve flag
      double A2I[6];    //!< transformation coefficients to go to image coordinates
      double A2M[6];    //!< transformation coefficients to go to machine coordinates

      /**
       * Coefficients to rotate machine coordinates so that the right edge of the region is vertical
       */
      double rotM[2];

      /**
       * Coefficients to rotate Image coordinates so that the right edge of the region is vertical
       */
      double rotI[2];

      double mI;        //!< max rotated image y coordinate in the region
      double mM;        //!< max rotate machine y coordinate in the region
      int indeces[2];   //!< indeces of the first and last fiducial marks included in this region
    }Affine2D;

    /**
     * C Style structure for storing measured fiducial coordinates
     */
    typedef struct FidObs {
      int flag;             //!< flag to tell if the observation has been made
      double mach[2];       //!< machine coordinates of the fiducial mark
      double image[2];      //!< theoretically perfect image coordinates
      double residuals[2];  //!< residuals of the fiducial mark measurement in machine coordinates
    }FidObs;

    /**
     * an array of discreet affine transformations pertaining to regions of the image
     * (there can never be more than 44)
     */
    Affine2D affines[44];

    FidObs obs[90];  //!< array to hold all possible observations of fiducial marks
    int n;           //!< the number of affines used to model the image
    double maxR;     //!< maximum residual vector length
    double meanR;    //!< mean of residual vector lenghts
    double stdevR;   //!< standard deviation of residual vector lengths

    void calc_residual_stats();

  public:

    ApolloPanIO();

    ~ApolloPanIO();

    void initialize();

    int fiducialObservation(int fiducialNumber, double machine_x, double machine_y);

    int clearFiducialObservation(int fiducialNumber);

    int computeInteriorOrienation();

    int machine2Image(double *machineX, double *machineY);

    int machine2Image(double machineX, double machineY, double *imageX, double *imageY);

    int image2Machine(double *imageX, double *imageY);

    int image2Machine(double imageX, double imageY, double *machineX, double *machineY);

    double stdevResiduals();
    double meanResiduals();
    double maxResiduals();
  };
} //end namespace Isis
#endif
