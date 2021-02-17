#ifndef LambertAzimuthalEqualArea_h
#define LambertAzimuthalEqualArea_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "TProjection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief Lambert Azimuthal Equal Area Map Projection
   *
   * This class provides methods for the forward and inverse formulas of a 
   * Lambert Azimuthal equal-area map projection from a sphere or an ellipsoid 
   * to a disk. 
   *  
   * The Lambert Azimuthal equal-area projection maps to circular regions. For 
   * this projection, area is accurately represented, that is, the area of a 
   * region on the surface is may be found by calculating the area of the 
   * corresponding region of the map. Scale is true at the point (center 
   * latitude, center longitude). This projection can handle polar or oblique 
   * projections, such as Equatorial aspect projections. For polar aspect 
   * projections, longitudes are straight lines and latitudes are circles.  For 
   * Equatorial aspect, the center longitude and equator are straight lines. 
   * For Equatorial aspect on a sphere, the longitudes that are 90 degrees to 
   * either side of the center longitude form a circle. For oblique projections, 
   * all other latitudes and longitudes are complex curves. The only point of 
   * the projection without distortion is at the center latitude and center 
   * longitude.  The antipodal point is represented as a circle surronding the 
   * map. 
   *  
   * The code was converted to C++ from the C version of the USGS General 
   * Cartographic Transformation Package (GCTP). 
   *  
   * This class inherits Projection and implements the virtual methods 
   * <UL> 
   *   <LI> SetGround - forward projection takes an lat/lon value from the given
   *   planet and calculates the corresponding x/y value on the projected plane
   *   <LI> SetCoordinate - inverse projection takes an x/y coordinate from the
   *   plane and calculates the lat/lon value on the planet
   *   <LI> XYRange - obtains projection coordinate coverage for a
   *   latitude/longitude window
   * </UL>
   *  
   * @b Caveats: There are a few situations in which the Lambert Azimuthal 
   *    equal-area formulas fail or lose accuracy.  Therefore, the following
   *    situations will fail or cause an error to be thrown.
   * <UL> 
   *   <LI> For minimum or maximum longitude, no more than 360 degrees from
   *     center longitude is allowed.
   *   <LI> Center longitude must be between -360 and 360.
   *   <LI> Projection of the antipodal point is only allowed for polar
   *     projections since this point is represented as a circle.  This does not
   *     cause trouble finding the lat/lon for a particular x/y value.  However,
   *     there are several x/y coordinates that correspond to this single
   *     lat/lon point.  For polar projections, the antiopodal point is the
   *     opposite pole. Thus, we can find unique x/y values depending on the
   *     given longitude.
   *   <LI> For Polar Aspect projections, if SetGround(phi, lambda) is
   *     called with phi equal to the opposite pole, then the relative scale
   *     factor is 0 for longitude and inf (or a very large number) for
   *     latitude.
   *   <LI> For Polar Aspect projections, if SetGround(phi, lambda) is
   *     called with phi equal to the center pole, then the relative scale
   *     factor is 0 for latitude and inf (or a very large number) for
   *     longitude.
   *   <LI> For Ellipsoidal Polar Aspect projections, if SetGround(phi, lambda)
   *     is called with phi and lambda equal to Center Latitude and Center
   *     Longitude, then if the Radius is large, x and y may not be exactly at
   *     (0,0)
   * </UL>
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *  
   * @see Snyder, John P. <em>Map Projections - A Working Manual.</em> 
   *      Washington: United States Government Printing Office, 1987. Print.
   *  
   * @ingroup MapProjection
   *
   * @author 2012-07-25 Jeannie Backer
   *
   * @internal
   *   @history 2012-07-25 Jeannie Backer - Original version. Fixes #954.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection. References #775.
   */
  class LambertAzimuthalEqualArea : public TProjection {
    public:
      LambertAzimuthalEqualArea(Pvl &label, bool allowDefaults = false);
      ~LambertAzimuthalEqualArea();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;
      double TrueScaleLatitude() const;
      // since this projection is not cylindrical the following method does not
      // need to be be overwritten, returning false by default 
      // virtual bool IsEquatorialCylindrical();

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

      // Unused methods as of June 2012 - See comments in LambertAzimuthalEqualArea.cpp
      double relativeScaleFactorLongitude() const;
      double relativeScaleFactorLatitude() const;

    private:
      void init(double centerLatitude, double centerLongitude);
      void initEllipsoid(); 
      bool setGroundEllipsoid(double phi, double lambda); 
      bool setCoordinateEllipsoid(const double x, const double y);
      bool xyRangeLambertAzimuthalPolar(double &minX, double &maxX, 
                                        double &minY, double &maxY); 
      void checkLongitude(double longitude);
      void validateRelativeScaleFactor() const;

      // projection flags
      bool m_spherical;        /**< Indicates whether the body to be projected 
                                    is spherical. (True if m_e = 0)**/
      bool m_northPolarAspect; /**< Indicates whether this is a north polar 
                                    aspect projection. (True if m_phi1 = 90)**/
      bool m_southPolarAspect; /**< Indicates whether this is a south polar 
                                    aspect projection. (True if m_phi1 = -90)**/
      bool m_equatorialAspect; /**< Indicates whether this is a equatorial 
                                    aspect projection. (True if m_phi1 = 0)**/

      // Snyder variables
      double m_a;         /**< Equitorial Radius of the ellipsoid. See Snyder 
                               page viii.**/
      double m_e;         /**< Eccentricity of the ellipsoid. See Snyder page 
                               viii.**/
      double m_lambda0;   /**< The center longitude for the map projection 
                               measured positive east, in radians. If the 
                               center latitude is the north pole, this is the 
                               meridian extending down. If the center latitude 
                               is the south pole, this is the meridian 
                                extending up. See Snyder page ix.**/ 
      double m_phi1;      /**< The center latitude for the map projection, in 
                               radians. See Snyder page ix.**/
      double m_sinPhi1;   //!< The sine of the center latitude.
      double m_cosPhi1;   //!< The cosine of the center latitude.

      // The following Snyder variables are only needed if we implement the 
      // ellipsoidal projection
      double m_qp;        /**< Snyder's q variable from equation (3-12) on page 
                               187, computed for the north pole, phi = 90.**/
      double m_q1;        /**< Snyder's q variable from equation (3-12) on page 
                               187, computed for the center latitude, 
                               phi = m_phi1.**/
      double m_m1;        /**< Snyder's m variable from equation (14-15) on page
                               187, computed from the center latitude, 
                               phi = m_phi1. **/
      double m_beta1;     /**< The authalaic latitude.  (Snyder's beta variable 
                               from equation (3-11) on pages 16 and 187, 
                               computed by using q = q1.) **/
      double m_sinBeta1;  /**< The sine of m_beta1.**/
      double m_cosBeta1;  /**< The cosine of m_beta1.**/
      double m_Rq;        /**< The radius of the sphere having the same surface
                               area as the ellipsoid. (Snyder's Rq variable from
                               equation (3-13) on pages 16 and 187.) **/
      double m_D;         /**< Value used to correct scale in all directions 
                               at the center of the projection. (Snyder's D 
                               variable from equation (24-20) on page 187.) **/

      double m_h; //!< Relative scale factor along a meridian of longitude.
      double m_k; //!< Relative scale factor along a parallel of latitude.
  };
};

#endif
