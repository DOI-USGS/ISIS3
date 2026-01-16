#ifndef UniversalGroundMap_h
#define UniversalGroundMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
namespace Isis {
  class Camera;
  class Cube;
  class Projection;
  class Pvl;
  class SurfacePoint;
  class Latitude;
  class Longitude;

  /**
   * @brief Universal Ground Map
   *
   * ???
   *
   * @ingroup Geometry
   *
   * @author  2005-08-09 Jeff Anderson
   *
   * @internal
   *  @todo Jeff Anderson - Finish Class Documentation
   *  @history 2005-11-09 Tracie Sucharski Added HasProjection method.
   *  @history 2006-03-20 Elizabeth Miller Added camera, projection, and
   *                          Resolution methods and documentation
   *  @history 2006-03-31 Elizabeth Miller Added unitTest
   *  @history 2006-05-17 Elizabeth Miller Depricated CameraManager to
   *                          CameraFactory
   *  @history 2007-02-06 Tracie Sucharski Added SetBand method.
   *  @history 2007-02-06 Steven Lambright Fixed documentation
   *  @history 2009-04-24 Steven Koechle Added a check to SetUniversalGround that
   *       makes sure the result is on the cube before returning true.
   *  @history 2010-04-09 Sharmila Prasad Added an API to check for camera in an image
   *  @history 2010-04-28 Mackenzie Boyd Fixed dereferencing issue in constructor
   *                          that takes a cube.
   *  @history 2011-01-25 Steven Lambright Added CameraPriority and SetGround.
   *                          The CameraPriority was for grid to use projections
   *                          by default so they would work across the image
   *                          regardless of the camera. The SetGround is so that
   *                          conversions between lat/lon types are simply handled
   *                          and allows a clean interface to use the new Latitude
   *                          and Longitude classes.
   *  @history 2011-01-25 Eric Hyer - Added SetGround method for Surface Points
   *  @history 2012-03-22 Steven Lambright - Added GroundRange() method because
   *                          I needed a ground range in the qview nomenclature
   *                          tool and this seemed like the best place to put
   *                          it. The Cube paramater is optional.
   *  @history 2012-06-29 Steven Lambright and Kimberly Oyama - Made the estimation
   *                          section of GroundRange() sample more points by walking
   *                          the perimeter, the two corner to corner diagonals, and
   *                          a plus that intersects in the center of the image. This
   *                          ensures that the entire longitude range is returned.
   *                          Fixes #855.
   *  @history 2012-12-20 Debbie A. Cook - Changed to use TProjection 
   *                          instead of Projection.  References #775.
   *  @history 2013-04-24 Tracie Sucharski - Added ring plane functionality to methods
   *                          UniversalLatitude() and UniversalLongitude().  Reference #775.
   *  @history 2012-04-24 Jeannie Backer - Removed prototype for constructor that has
   *                          been removed from this class. References #775.
   *  @history 2017-06-26 Jesse Mapel - Added a new method to set the universal ground point
   *                          without adjusting for the longitude domain. Fixes #2185.
   */
  class UniversalGroundMap {
    public:
      /**
       * This enum is used to define whether to use a camera or projection
       * primarily, and which to fall back on.
       */
      enum CameraPriority {
        /**
         * This is the default because cameras are projection-aware. Use the
         * camera for additional power if available, and fall back to projection
         */
        CameraFirst,
        /**
         * Use the projection for functionality well outside the original image
         * if available, and fall back to camera.
         */
        ProjectionFirst
      };

      UniversalGroundMap(Cube &cube, CameraPriority priority = CameraFirst);
      ~UniversalGroundMap();

      void SetBand(const int band);
      bool SetUniversalGround(double lat, double lon);
      bool SetUnboundGround(Latitude lat, Longitude lon);
      bool SetGround(Latitude lat, Longitude lon);
      bool SetGround(const SurfacePoint &);
      double Sample() const;
      double Line() const;

      bool SetImage(double sample, double line);
      double UniversalLatitude() const;
      double UniversalLongitude() const;
      double Resolution() const;

      bool GroundRange(Cube *cube,
                       Latitude &minLat, Latitude &maxLat,
                       Longitude &minLon, Longitude &maxLon,
                       bool allowEstimation = true);

      /**
       * Returns whether the ground map has a projection or not
       *
       * @return bool Returns true if the ground map has a projection, and false
       *              if it does not
       */
      bool HasProjection() {
        return p_projection != 0;
      };


      /**
       * Returns whether the ground map has a camera or not
       *
       * @return bool Returns true if the ground map has a camera, and false
       *              if it does not
       */
      bool HasCamera() {
        return p_camera != 0;
      };

      //! Return the projection associated with the ground map (NULL implies none)
      Isis::Projection *Projection() const {
        return p_projection;
      };

      //! Return the camera associated with the ground map (NULL implies none)
      Isis::Camera *Camera() const {
        return p_camera;
      };


    private:
      Isis::Camera *p_camera;  //!<The camera (if the image has a camera)
      Isis::Projection *p_projection;  //!<The projection (if the image is projected)
  };
};

#endif
