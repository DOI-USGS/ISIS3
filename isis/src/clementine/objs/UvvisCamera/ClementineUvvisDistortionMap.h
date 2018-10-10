#ifndef ClementineUvvisDistortionMap_h
#define ClementineUvvisDistortionMap_h

/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/22 19:44:53 $
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


#include "Camera.h"
#include "CameraDistortionMap.h"

namespace Isis {

  /**
   * @brief Distortion map for the Clementine UVVIS camera
   *
   * This class is was copied from the Chandrayaan1 M3 camera model as its distortion map.
   * Equations provided by Randy Kirk and code provided by Ken Edmundson.
   *
   * @ingroup Camera
   *
   * @author 2017-12-27 Jeff Anderson & Victor Silva
   *
   * @internal
   *   @history 2017-12-27 Jeff Anderson & Victor Silva - Copied code from Chandrayaan1 M3 camera.
   *   @history 2018-09-01 Jeannie Backer - Added documentation and merged into public repo.
   */

  class ClementineUvvisDistortionMap : public CameraDistortionMap {
    public:
      ClementineUvvisDistortionMap(Camera *parent, double xp, double yp,
                                  double k1, double k2, double k3,
                                  double p1, double p2);
      ~ClementineUvvisDistortionMap();

      bool SetFocalPlane(const double dx, const double dy);
      bool SetUndistortedFocalPlane(const double ux, const double uy);

    private: // parameters below are from camera calibration report
      double p_xp; //!< Principal point x coordinate.
      double p_yp; //!< Principal point y coordinate.
      double p_k1; //!< Constant term coefficient of radial distortion.
      double p_k2; //!< Linear term coefficient of radial distortion.
      double p_k3; //!< Quadratic term coefficient of radial distortion.
      double p_p1; //!< First coefficient of decentering distortion.
      double p_p2; //!< Second coefficient of decentering distortion.
  };
};
#endif
