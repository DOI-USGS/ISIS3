#ifndef CameraGroundMap_h
#define CameraGroundMap_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"
#include "CameraDistortionMap.h"
#include "SpicePosition.h"
#include "SurfacePoint.h"

namespace Isis {
  /** 
   * Convert between undistorted focal plane and ground coordinates
   *
   * This base class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and ground coordinates lat/lon.
   * This class handles the case of framing cameras.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2005-02-08 Jeff Anderson
   *
   * @internal
   *  @history 2005-11-16 Jeff Anderson  Fixed bug in SetGround not setting the
   *                          proper boolean return value
   *  @history 2007-06-11 Debbie A. Cook - Added overloaded method SetGround that
   *                          includes a radius argument and the method
   *                          LookCtoFocalPlaneXY() to handle the common functionality
   *                          between the SetGround methods
   *  @history 2008-07-14 Steven Lambright Added NaifStatus calls
   *  @history 2009-10-14 Debbie A. Cook Added new virtual method GetXY(lat,lon,radius, lookJ)
   *  @history 2009-11-27 Debbie A. Cook Modified virtual method 
   *                          GetXY(lat,lon,radius,lookJ,cudx,cudy)
   *  @history 2010-03-19 Debbie A. Cook Modified virtual method to return cudx and cudy; added
   *                          methods GetdXYdPosition, GetdXYdOrientation, GetdXYdPoint, 
   *                          PointPartial, and DQuotient; and added members PartialType 
   *                          (from BundleAdjust) and p_lookJ.
   *  @history 2010-08-05 Debbie A. Cook Added another version of GetXY to support changes from 
   *                          binary control net upgrade
   *  @history 2010-11-22 Debbie A. Cook Moved PointPartial call out of GetdXYdPoint
   *                          to allow BundleAdjust to avoid multiple calls for
   *                          every measure.  The application must call PointPartial
   *                          to get the body-fixed look vector derivative prior to
   *                          calling this method.
   *  @history 2011-02-09 Steven Lambright SetGround now uses the Latitude,
   *                          Longitude and SurfacePoint classes.
   *  @history 2011-03-18 Debbie A. Cook Added reference to surface point in GetXY
   *  @history 2012-07-06 Debbie A. Cook Updated Spice members to be more 
   *                          compliant with Isis coding standards. References #972.
   *  @history 2012-10-10 Debbie A. Cook Modified to use new Target class.
   *                          References Mantis ticket #775 and #1114.
   *  @history 2013-02-22 Debbie A. Cook Fixed LookCtoFocalPlaneXY method 
   *                          to properly handle instruments with a look direction along 
   *                          the negative z axis.  Fixes Mantis ticket #1524
   *  @history 2014-04-17 Jeannie Backer - Replaced local variable names with more
   *                          descriptive names. References #1659.
   *  @history 2015-07-24 Debbie A. Cook - Added new methods GetdXYdTOrientation(), 
   *                          EllipsoidPartial() and MeanRadiusPartial() along with new member
   *                          p_pB. References Mantis ticket TBD.
   *  @history 2016-06-27 Ian Humphrey - Updated documentation and coding standards. Fixes #3971.
   *  @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *  @history 2019-04-15 Debbie A. Cook - Added optional bool argument to main GetXY method to 
   *                          allow the bundle adjustment to skip the back of planet test during iterations. 
   *                          Also changed the name of the angle variable to cosangle to be more 
   *                          descriptive. References #2591.
   *  @history 2018-07-26 Kris Becker - Move all local variables  and methods to
   *                         protected scope so derived objects can be developed
   *                         properly
   */
  class CameraGroundMap {
    public:
      CameraGroundMap(Camera *parent);

      //! Destructor
      virtual ~CameraGroundMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);

      /**
       * Radius axes types to use when computing partials. When computing partials, this enum
       * represents the "with respect to" variable in the computation. 
       */
      enum PartialType {
        WRT_Latitude,
        WRT_Longitude,
        WRT_Radius,
        WRT_MajorAxis,
        WRT_MinorAxis,
        WRT_PolarAxis
      };

      virtual bool SetGround(const Latitude &lat, const Longitude &lon);
      virtual bool SetGround(const SurfacePoint &surfacePoint);
      virtual bool GetXY(const SurfacePoint &spoint, double *cudx, 
                       double *cudy, bool test=true);
      virtual bool GetXY(const double lat, const double lon,
                         const double radius, double *cudx, double *cudy);
      virtual bool GetdXYdPosition(const SpicePosition::PartialType varType,
                                   int coefIndex,
                                   double *cudx, double *cudy);
      virtual bool GetdXYdOrientation(const SpiceRotation::PartialType varType,
                                      int coefIndex,
                                      double *cudx, double *cudy);
      virtual bool GetdXYdTOrientation(const SpiceRotation::PartialType varType,
                                       int coefIndex,
                                       double *cudx, double *cudy);
      virtual bool GetdXYdPoint(std::vector<double> d_pB,
                                double *dx, double *dy);
      std::vector<double> PointPartial(SurfacePoint spoint, PartialType wrt);
      std::vector<double> EllipsoidPartial(SurfacePoint spoint, PartialType raxis);
      std::vector<double> MeanRadiusPartial(SurfacePoint spoint, Distance meanRadius);
      double DQuotient(std::vector<double> &look, std::vector<double> &dlook,
                       int index);

      /**
       * @return The undistorted focal plane x
       */
      inline double FocalPlaneX() const {
        return p_focalPlaneX;
      };

      /**
       * @return The undistorted focal plane y
       */
      inline double FocalPlaneY() const {
        return p_focalPlaneY;
      };

    protected:
      Camera *p_camera;     //!< Camera
      double p_focalPlaneX; //!< Camera's x focal plane coordinate
      double p_focalPlaneY; //!< Camera's y focal plane coordinate

      void LookCtoFocalPlaneXY();  //!< Calculate focalplane x/y from lookvector in camera
      /** Surface point calculated from ground coordinates in GetXY and used for partials*/
      std::vector<double> m_pB;
      /** Look vector in J2000 calculated from ground coordinates in GetXY and used for partials*/
      std::vector<double> m_lookJ;
  };
};
#endif
