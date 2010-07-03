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
#ifndef CameraDistortionMap_h
#define CameraDistortionMap_h

#include <vector>
#include "Camera.h"

namespace Isis {
  /** Distort/undistort focal plane coordinates
   *
   * Creates a map for adding/removing optical distortions
   * from the focal plane of a camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2005-02-01 Jeff Anderson
   *
   * @history 2008-02-05 Jeff Anderson - Modified to allow for variable focal
   *                                     length
   * @history 2008-02-21 Steven Lambright - Fixed a problem that resulted in infinities
   *                                      and NaNs
   *
   * @internal
   */
    class CameraDistortionMap {
    public:
      CameraDistortionMap(Camera *parent, double zDirection = 1.0);

      void SetDistortion(const int naifIkCode);

      //! Destructor
      virtual ~CameraDistortionMap() {};

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);

      //! Return distorted focal plane x
      inline double FocalPlaneX() const { return p_focalPlaneX; };

      //! Return distorted focal plane y
      inline double FocalPlaneY() const { return p_focalPlaneY; };

      //! Return undistorted focal plane x
      inline double UndistortedFocalPlaneX() const { return p_undistortedFocalPlaneX; };

      //! Return undistorted focal plane y
      inline double UndistortedFocalPlaneY() const { return p_undistortedFocalPlaneY; };

      //! Return undistorted focal plane z
      inline double UndistortedFocalPlaneZ() const
        { return p_zDirection * p_camera->FocalLength(); };

    protected:
      Camera *p_camera;

      double p_focalPlaneX;
      double p_focalPlaneY;
      double p_undistortedFocalPlaneX;
      double p_undistortedFocalPlaneY;
      double p_zDirection;

      std::vector<double> p_odk;
  };
};
#endif
