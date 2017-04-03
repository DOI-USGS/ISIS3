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
   *  Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Tgo
   *
   * @see TgoCassisCamera
   *
   * @author 2008-08-22 Steven Lambright
   *
   * @internal
   *   @history 2017-04-03 Jeannie Walldren - Original version.
   */
  class TgoCassisDistortionMap : public CameraDistortionMap {
    public:
      TgoCassisDistortionMap(Camera *parent, int naifIkCode);

      //! Destructor
      virtual ~TgoCassisDistortionMap() {};

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

    private:
      double normalize(double value, double a, double b);
      double denormalize(double value, double a, double b);
      double divisor(double i, double j);

      QList<double> m_A1; //!< First row of parameters of rational distortion model.
      QList<double> m_A2; //!< Second row of parameters of rational distortion model.
      QList<double> m_A3; //!< Third row of parameters of rational distortion model.
  };
};
#endif
