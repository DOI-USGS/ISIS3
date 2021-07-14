#ifndef CameraDistortionMap_h
#define CameraDistortionMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   * @internal
   *   @history 2008-02-05 Jeff Anderson - Modified to allow for variable focal length
   *   @history 2008-02-21 Steven Lambright - Fixed a problem that resulted in infinities
   *                           and NaNs
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2013-02-11 E. Howington-Kraus - Added accessor methods
   *                           OpticalDistortionCoefficients() and ZDirection().
   *                           These are tested by application socetlinescankeywords
   *                           since no unitTest exists. Fixed indentation
   *                           of history entries.  References #1490.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *   @history 2017-09-04 Kristin Berry - Made SetDistortion virtual so that
   *                           individual camera model distortion maps can
   *                           set the values.
   */
  class CameraDistortionMap {
    public:
      CameraDistortionMap(Camera *parent, double zDirection = 1.0);

      virtual void SetDistortion(int naifIkCode);

      virtual ~CameraDistortionMap();

      virtual bool SetFocalPlane(double dx, double dy);

      virtual bool SetUndistortedFocalPlane(double ux, double uy);

      std::vector<double> OpticalDistortionCoefficients() const;

      double ZDirection() const;
      double FocalPlaneX() const;
      double FocalPlaneY() const;
      double UndistortedFocalPlaneX() const;
      double UndistortedFocalPlaneY() const;
      double UndistortedFocalPlaneZ() const;

    protected:
      Camera *p_camera;                 //!< The camera to distort/undistort

      double p_focalPlaneX;             //!< Distorted focal plane x
      double p_focalPlaneY;             //!< Distorted focal plane y
      double p_undistortedFocalPlaneX;  //!< Undistorted focal plane x
      double p_undistortedFocalPlaneY;  //!< Undistorted focal plane y
      double p_zDirection;              //!< Undistorted focal plane z

      std::vector<double> p_odk;        //!< Vector of distortion coefficients
  };
};
#endif
