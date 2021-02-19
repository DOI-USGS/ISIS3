#ifndef LineScanCameraGroundMap_h
#define LineScanCameraGroundMap_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

#include "CameraGroundMap.h"

namespace Isis {
  /** Convert between undistorted focal plane and ground coordinates
   *
   * This class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and ground coordinates lat/lon
   * for line scan cameras.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2005-02-08 Jeff Anderson
   *
   * @internal
   *
   *   @history 2005-02-08 Jeff Anderson - Original version
   *   @history 2005-08-24 Jeff Anderson - Fixed bug when algorithm was checking
   *          for the backside of the planet and convergence failed (checkHidden)
   *   @history 2005-11-16 Jeff Anderson - Fixed bug when a image covers more
   *          than 180 degrees in an orbit which would cause two focal plane roots
   *          to be inside the start/end time range.
   *   @history 2007-12-21 Debbie A. Cook - Added overloaded method SetGround that
   *          includes a radius argument
   *   @history 2009-03-02 Steven Lambright - Added an additional method of
   *            finding the point in SetGround(...) if the original algorithm
   *            fails. The spacecraft position at the beginning and end of the
   *            image are now being used to estimate the correct line if the
   *            bounding check fails the first time through.
   *   @history 2010-06-17 Steven Lambright - More tolerant of failures in the
   *            distortion models for finding the bounds of the search in
   *            FindSpacecraftDistance
   *   @history 2010-12-07 Steven Lambright - SetGround(double,double) now goes
   *            straight to the radius instead of using SetUniversalGround to
   *            get the radius.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *            coding standards. References #972.
   *
   */
  class LineScanCameraGroundMap : public CameraGroundMap {
    public:

      //! Constructor
      LineScanCameraGroundMap(Camera *cam);

      //! Destructor
      virtual ~LineScanCameraGroundMap();

      virtual bool SetGround(const Latitude &lat, const Longitude &lon);
      virtual bool SetGround(const SurfacePoint &surfacePoint);
      virtual bool SetGround(const SurfacePoint &surfacePoint, const int &approxLine);

    protected:
      enum FindFocalPlaneStatus {
        Success,
        BoundingProblem,
        Failure
      };

      FindFocalPlaneStatus FindFocalPlane(const int &approxLine,
                                          const SurfacePoint &surfacePoint);
      double FindSpacecraftDistance(int line, const SurfacePoint &surfacePoint);

  };
};
#endif
