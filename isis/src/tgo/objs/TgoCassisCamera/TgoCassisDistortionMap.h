#ifndef TgoCassisDistortionMap_h
#define TgoCassisDistortionMap_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/11/24 16:40:31 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {
  /** 
   * Distort/undistort focal plane coordinates.
   *
   * Creates a map for adding/removing optical distortions from the focal plane of a camera.
   *
   * Model derived by Stepan Tulyakov and Anoton Ivanov, 
   * EPFL (Ecole Polytechnique Federale de Lausanne). 
   *  
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Tgo
   *
   * @see TgoCassisCamera
   * @see $ISIS3DATA/tgo/assets/distortion
   *
   * @author 2017-04-03 Jeannie Backer
   *
   * @internal
   *   @history 2017-04-03 Jeannie Backer - Original version from model
   *                           provided by Anton Ivanov.
   *   @history 2017-04-06 Jeannie Backer - Fixed bugs and updated unitTest.
   *   @history 2017-09-14 Jeannie Backer - Updated distortion model provided
   *                           by Stepan Tulyakov. References #5155
   *   @history 2017-09-18 Jeannie Backer - Added check to verify that values
   *                           passed into SetFocalPlane and SetUndistortedFocalPlane
   *                           are within valid range. References #5155
   */
  class TgoCassisDistortionMap : public CameraDistortionMap {
    public:
      TgoCassisDistortionMap(Camera *parent, int naifIkCode);

      virtual ~TgoCassisDistortionMap();

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double chiDotA(double x, double y, QList<double> A);

      QList<double> m_A1_corr; /**< Coefficients for rational distortion model used to compute 
                                    ideal x from distorted x. */
      QList<double> m_A2_corr; /**< Coefficients for rational distortion model used to compute 
                                    ideal y from distorted y. */
      QList<double> m_A3_corr; /**< Coefficients for rational distortion model used to find scaling
                                    factor used when computing ideal coordinates from distorted. */
      QList<double> m_A1_dist; /**< Coefficients for rational distortion model used to compute 
                                    distorted x from ideal x. */
      QList<double> m_A2_dist; /**< Coefficients for rational distortion model used to compute 
                                    distorted y from ideal y. */
      QList<double> m_A3_dist; /**< Coefficients for rational distortion model used to find scaling
                                    factor used when computing distorted coordinates from ideal. */
      double m_pixelPitch; /**< The pixel pitch of the camera. Used to check if far off the ccd
                                prior to correcting or applying distortion. */
      double m_width; /**< The width of the ccd in pixels. Used to check if far off the ccd prior
                           to correcting or applying distortion. */
      double m_height; /**< The height of the ccd in pixels. Used to check if far off the ccd prior
                            to correcting or applying distortion. */
  };
};
#endif
