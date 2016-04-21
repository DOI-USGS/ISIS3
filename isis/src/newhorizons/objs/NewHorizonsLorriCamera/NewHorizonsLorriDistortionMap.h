#ifndef NewHorizonsLorriDistortionMap_h
#define NewHorizonsLorriDistortionMap_h

/**
 * @file
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

#include "CameraDistortionMap.h"
#include "Camera.h"

namespace Isis {
  /** 
   * New Horizons LORRI Distortion Map 
   *  
   * @author 2014-06-14 Stuart Sides
   *
   * @internal 
   *    @history 2014-06-08 Staurt Sides - Original version. Equations and coefficients
   *    taken from Jet Propulsion Laboratory Interoffice Memorandum 2011/06/08 "New Horizons
   *    LORRI Geometric Calibration of August 2006" From: W. M. Owen Jr. and D. O'Coonnell
   *  
   *    @history 2016-02-24 Staurt Sides - New Horizons LORRI distortion model changed to
   *    subtract the distortion when going from distorted to undistorted instead of adding, and
   *    adding the distortion when going from undistorted to destorted.
   */
  class NewHorizonsLorriDistortionMap : public CameraDistortionMap {
    public:
      NewHorizonsLorriDistortionMap(Camera *parent, double e2, double e5, double e6, 
                                    double zDirection = 1.0);
      ~NewHorizonsLorriDistortionMap() {};

      bool SetFocalPlane(const double ux, const double uy);
      bool SetUndistortedFocalPlane(const double dx, const double dy);

    private:
      double p_e2;
      double p_e5;
      double p_e6;
  };
};
#endif
