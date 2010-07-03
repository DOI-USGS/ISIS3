/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/06/17 18:59:12 $
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

#ifndef LineScanCameraGroundMap_h
#define LineScanCameraGroundMap_h

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
   *
   */
  class LineScanCameraGroundMap : public CameraGroundMap {
    public:
      /** Constructor
       *
       * @param cam pointer to camera model
       */
      LineScanCameraGroundMap(Camera *cam) : CameraGroundMap(cam) {};

      //! Destructor
      virtual ~LineScanCameraGroundMap() {};

      virtual bool SetGround(const double lat, const double lon);
      virtual bool SetGround(const double lat, const double lon, const double radius);

    protected:
      enum FindFocalPlaneStatus {
        Success,
        BoundingProblem,
        Failure
      };

      FindFocalPlaneStatus FindFocalPlane(const int &approxLine,
                                          const double &lat, const double &lon, const double &radius);
      double FindSpacecraftDistance(int line, const double lat, const double lon);
  };
};
#endif
