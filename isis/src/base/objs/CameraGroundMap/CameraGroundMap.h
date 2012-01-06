/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/03/27 06:36:41 $
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

#ifndef CameraGroundMap_h
#define CameraGroundMap_h

#include "Camera.h"
#include "CameraDistortionMap.h"
#include "SpicePosition.h"
#include "SurfacePoint.h"

namespace Isis {
  /** Convert between undistorted focal plane and ground coordinates
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
   *          proper boolean return value
   *  @history 2007-06-11 Debbie A. Cook - Added overloaded method SetGround that
   *                     includes a radius argument and the method
   *                     LookCtoFocalPlaneXY() to handle the common functionality
   *                     between the SetGround methods
   *  @history 2008-07-14 Steven Lambright Added NaifStatus calls
   *  @history 2009-10-14 Debbie A. Cook Added new virtual method GetXY(lat,lon,radius, lookJ)
   *  @history 2009-11-27 Debbie A. Cook Modified virtual method GetXY(lat,lon,radius,lookJ,cudx,cudy)
   *  @history 2010-03-19 Debbie A. Cook Modified virtual method to return cudx and cudy; added
   *                       methods GetdXYdPosition, GetdXYdOrientation, GetdXYdPoint, PointPartial,
   *                       and DQuotient; and added members PartialType (from BundleAdjust) and p_lookJ.
   *  @history 2010-08-05 Debbie A. Cook Added another version of GetXY to support changes from binary
   *                       control net upgrade
   *  @history 2010-11-22 Debbie A. Cook Moved PointPartial call out of GetdXYdPoint
   *                       to allow BundleAdjust to avoid multiple calls for
   *                       every measure.  The application must call PointPartial
   *                       to get the body-fixed look vector derivative prior to
   *                       calling this method.
   *  @history 2011-02-09 Steven Lambright SetGround now uses the Latitude,
   *                       Longitude and SurfacePoint classes.
   *  @history 2011-03-18 Debbie A. Cook Added reference to surface point in GetXY
   *
   */
  class CameraGroundMap {
    public:
      CameraGroundMap(Camera *parent);

      //! Destructor
      virtual ~CameraGroundMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);

      enum PartialType {
        WRT_Latitude,
        WRT_Longitude,
        WRT_Radius
      };

      virtual bool SetGround(const Latitude &lat, const Longitude &lon);
      virtual bool SetGround(const SurfacePoint &surfacePoint);
      virtual bool GetXY(const SurfacePoint &spoint, double *cudx, double *cudy);
      virtual bool GetXY(const double lat, const double lon,
                         const double radius, double *cudx, double *cudy);
      virtual bool GetdXYdPosition(const SpicePosition::PartialType varType,
                                   int coefIndex,
                                   double *cudx, double *cudy);
      virtual bool GetdXYdOrientation(const SpiceRotation::PartialType varType,
                                      int coefIndex,
                                      double *cudx, double *cudy);
      virtual bool GetdXYdPoint(std::vector<double> d_lookB,
                                double *cudx, double *cudy);
      std::vector<double> PointPartial(SurfacePoint spoint, PartialType wrt);
      double DQuotient(std::vector<double> &look, std::vector<double> &dlook,
                       int index);

      //! Return undistorted focal plane x
      inline double FocalPlaneX() const {
        return p_focalPlaneX;
      };

      //! Return undistorted focal plane y
      inline double FocalPlaneY() const {
        return p_focalPlaneY;
      };

    protected:
      Camera *p_camera;
      double p_focalPlaneX;
      double p_focalPlaneY;

    private:
      void LookCtoFocalPlaneXY();  //!< Calculate focalplane x/y from lookvector in camera
      std::vector<double> p_lookJ;    //!< Look vector in J2000 calculated from ground coordinates in GetXY and used for partials
  };
};
#endif
