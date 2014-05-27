#ifndef Chandrayaan1M3DistortionMap_h
#define Chandrayaan1M3DistortionMap_h

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
   * @brief Distortion map for the Chandrayaan1 M3 camera
   *
   * This class is used by the Chandrayaan1 M3 camera model as its distortion map. Equations 
   * provided by Randy Kirk and code provided by Ken Edmundson. 
   *
   * @ingroup Camera
   *
   * @author ????-??-?? Ken Edmundson
   *
   * @internal
   *   @history 2013-11-24 Stuart Sides - Modified from ApolloMetricDistortionMap
   *   
   */

  class Chandrayaan1M3DistortionMap : public CameraDistortionMap {
    public:
      Chandrayaan1M3DistortionMap(Camera *parent, double xp, double yp, 
                                  double k1, double k2, double k3, 
                                  double p1, double p2);
      ~Chandrayaan1M3DistortionMap() {};

      bool SetFocalPlane(const double dx, const double dy);
      bool SetUndistortedFocalPlane(const double ux, const double uy);

    private: // parameters below are from camera calibration report
      double p_xp, p_yp;       //!< principal point coordinates
      double p_k1, p_k2, p_k3; //!< coefficients of radial distortion
      double p_p1, p_p2, p_p3; //!< coefficients of decentering distortion
  };
};
#endif
