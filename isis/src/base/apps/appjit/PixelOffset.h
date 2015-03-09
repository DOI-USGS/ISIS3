#ifndef PixelOffset_h
#define PixelOffset_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/05/18 00:05:29 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <vector>

#include <QString>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "PolynomialUnivariate.h"

namespace Isis {
  /**
   * @brief Obtain rotation angles for a line scan camera pixel offsets
   *
   * This class will obtain the rotation from existing camera pointing to
   * "corrected" camera pointing calculated from a table of pixel offsets
   * and time (sample line et)
   *
   * This class was created to calculate the rotation matrix needed to
   * remove jitter from a hirise image based on the table of offsets.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2008-08-20 Debbie A. Cook
   *
   * @internal
   *  @history 2008-08-20  Debbie A. Cook Original Version
   *  @history 2008-12-19  Debbie A. Cook Removed couts to prepare for commiting
   *  @history 2009-10-01  Debbie A. Cook Cleaned up extrapolation code
   *  @history 2010-05-17  Debbie A. Cook Recoded extrapolation to add 2
   *           extra points on each end of the input data.  The first point
   *           added on each end should be the start and end time of the
   *           observation.  The added points are linearly interpolated from
   *           the beginning and final 2 points of input data.
   */
  class PixelOffset {
    public:
      //! Constructors
      PixelOffset(const QString &tableList, double fl, double pixPitch, double baseTime,
                  double timeScale, int degree);

      //! Destructor
      virtual ~PixelOffset() { }

      //! Load the angles by interpolating the pixel offsets and converting to angles
      void LoadAngles(std::vector<double> cacheTime);

      //! Compute cubic interpolation for current time et
      void Interp();

      //! Compute coefficients for a polynomial fit to each angle based on scaled time
      void SetPolynomial();

      void SetEphemerisTime(const double et);

      std::vector<double> SetEphemerisTimeHPF(const double et);

      double WrapAngle(double compareAngle, double angle);
//      std::vector<double> &Matrix() { return p_TC; };



    private:
      std::vector<double> p_samples;
      std::vector<double> p_lines;
      std::vector<double> p_times;
      std::vector<double> p_I;
      double p_focalLen;                //!< Focal length of instrument in mm
      double p_pixPitch;                //!< Pixel pitch of instrument in mm/pixel
      double p_et;                      //!< Current ephemeris time

      std::vector<double> p_cacheAngle1;         //!
      std::vector<double> p_cacheAngle2;
      std::vector<double> p_cacheTime;
      //   rotation at et
      double p_baseTime;                //!< Base time used in fit equations
      double p_timeScale;               //!< Time scale used in fit equations
      std::vector<double> p_ang1Coefficients;
      std::vector<double> p_ang2Coefficients;
      //!< Coefficients of polynomials fit to
      //    each of three rotation angles
      int p_degree;                     //!< Degree of fit polynomial for angles
      double p_angle1;                  //!< Angle1 for current et in radians
      double p_angle2;                  //!< Angle2 for current et in radians
      double p_angleScale;              //!< Scaler to reduce roundoff
      double p_sampOff;
      double p_sampScale;
      double p_lineOff;
      double p_lineScale;
  };
};

#endif

