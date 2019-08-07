/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/02/21 16:04:33 $
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
#ifndef KaguyaTcCameraDistortionMap_h
#define KaguyaTcCameraDistortionMap_h

#include "CameraDistortionMap.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates for Kaguya's TC cameras.
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2019-04-04 Kristin Berry
   *
   * @internal
   *   @history 2019-04-04 Kristin Berry - Original version.
   *
   */
  class KaguyaTcCameraDistortionMap : public CameraDistortionMap {
    public:
      KaguyaTcCameraDistortionMap(Camera *parent, int naifIkCode);

      virtual ~KaguyaTcCameraDistortionMap();

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);

    protected:
      std::vector<double> p_odkx; //!< distortion x coefficients
      std::vector<double> p_odky; //!< distortion y coefficients
  };
};
#endif
