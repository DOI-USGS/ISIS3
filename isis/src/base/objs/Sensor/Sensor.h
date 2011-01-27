#ifndef Sensor_h
#define Sensor_h
/**
 * @file
 * $Revision: 1.14 $
 * $Date: 2010/05/22 00:08:59 $
 *
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

#include "Spice.h"
#include "Cube.h"
#include "ProjectionFactory.h"
#include "Portal.h"
#include "Interpolator.h"

namespace Isis {
  class Distance;
  class Latitude;
  class Longitude;

  /**
   * @brief Class for computing sensor ground coordinates
   *
   * The sensor class allows for the computation of parameters related to orbiting
   * instruments. In particular, a time and look direction can be set and from
   * those the ground coordinate (latitude/longitude) along with phase, incidence,
   * and emission angles can be computed. Likewise, a ground point can be set and
   * look direction can be computed. This class is derived from the Spice class.
   *
   * An important capability of this class is
   * the ability to use a surface model other than an ellipsoid when intersecting
   * the look direction of the sensor with the planetary body. This allows for the
   * generation of othrorectified products. The file containing the surface model
   * is a cube and is obtained from the labels in the follow form:
   *   @code
   *     Group = Kernels
   *       ElevationModel = file.cub
   *     EndGroup
   *   @endcode
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2003-04-11 Jeff Anderson
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides -  Modified schema from astrogeology...
   *                                      isis.astrogeology...
   *  @history 2003-05-30 Jeff Anderson - Updated unitTest and truth to account
   *                                      for precision due to optimization
   *  @history 2003-10-16 Jeff Anderson - Added LoadEulerMounting and
   *                                      LoadFrameMounting methods
   *  @history 2003-11-26 Jeff Anderson - Modified FrameMounting methods to allow
   *                                      for fixed frames or time dependent
   *                                      frames
   *  @history 2004-01-14 Jeff Anderson - Remove an unused constructor
   *  @history 2004-01-20 Jeff Anderson - Added an option to the
   *                                      SetUniversalGround method to eliminate
   *                                      checks for points on the backside of the
   *                                      target
   *  @history 2004-02-18 Jeff Anderson - Fixed a problem with the FrameMounting
   *                                      methods as the frame kernel is
   *                                      unloaded if a cache is created.
   *  @history 2004-02-23 Jeff Anderson - Fixed two bugs in the handling of DEMs.
   *                                      Used universal lat/lon when appropriate
   *                                      and the radius needed to be converted
   *                                      to km when read from the DEM file.
   *  @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
   *                                      documentation
   *  @history 2005-02-24 Jeff Anderson - Added SlantDistance method and made
   *                                      the constructor ignore the
   *                                      ElevationModel keyword if it is null
   *  @history 2005-02-25 Jeff Anderson - Added LocalSolarTime method
   *  @history 2005-06-09 Elizabeth Ribelin - Added LoadEulerMounting method that
   *                                      accepts a matrix as a parameter
   *  @history 2005-08-24 Jeff Anderson - Make sure LocalSolarTime always return
   *                                      positive hours
   *  @history 2005-09-20 Jeff Anderson - Added tests for trying to intersect the
   *                                      sky
   *  @history 2006-03-31 Elizabeth Miller - Added SpacecraftAltitude &
   *                                      SolarDistance methods
   *  @history 2006-09-07 Debbie A. Cook - Changed back-of-planet test to use
   *                                      emission angle instead of length
   *                                      of vector to surface point
   *  @history 2006-10-11 Jeff Anderson - Modified reading radius
   *                                      from dem using bilinear
   *                                      interpolation
   *  @history 2006-10-12 Jeff Anderson - Modified the
   *                                      SetLookDirection method
   *                                      to use the DEM (if
   *                                      available) when
   *                                      computing a ground point
   *  @history 2007-05-18 Jeff Anderson - Modify SpacecraftAltitude method
   *                                      to use DEM
   *  @history 2007-06-11 Debbie A. Cook - Added alternative  method that includes radius
   *  @history 2007-08-24 Debbie A. Cook - Replaced references to p_sB since it was removed from Spice
   *  @history 2007-11-27 Debbie A. Cook - Added overloaded method SetUniversalGround(lat, lon, radius)
   *  @history 2008-05-21 Steven Lambright - CubeManager is now used to speed up DEM Cube I/O
   *  @history 2008-06-18 Debbie A. Cook - Made DemRadius radius public instead of private and added
   *                                       method HasElevationModel
   *  @history 2008-08-06 Stuart Sides   - Modified SetLookDirection to better
   *                                       handle oblique views. In the past it
   *                                       would oscillate and run out of
   *                                       iterations.
   *  @history 2009-02-06 Debbie A. Cook - Changed the tolerance from 1e-6 to 1e-12 for dist**2 (mm)
   *  @history 2009-02-06 Debbie A. Cook - Changed the tolerance back to 1e-6 (mm)
   *  @history 2009-02-15 Debbie A. Cook - Added virtual Resolution method
   *  @history 2009-06-30 Steven Lambright - Added IgnoreElevationModel and fixed
   *                                       DemRadius
   *  @history 2009-07-09 Debbie A. Cook - Corrected documentation on Resolution method
   *  @history 2009-09-23  Tracie Sucharski - Convert negative longitudes
   *                                      returned by reclat in SetLookDirection.
   *  @history 2010-09-15 Janet Barrett - Modified the SetLookDirection method to use a new
   *                                      algorithm for finding the intersection of a ray with
   *                                      the DEM. This was required to take care of problems that
   *                                      were encountered at the poles of global DEM files. The
   *                                      algorithm that is being used was taken from "Intersection
   *                                      between spacecraft viewing vectors and digital elevation
   *                                      models" by N.A. Teanby.
   *  @history 2010-11-09 Eric Hyer - moved some private members to protected
   *                                      section for Camera
   *  @history 2010-12-07 Steven Lambright - Added LocalRadius(lat,lon) and
   *                                     removed a sqrt from SetLookDirection
   *  @history 2011-01-26 Steven Lambright - The LocalRadius methods now return
   *                                     Distance objects and added a
   *                                     LocalRadius method that uses the
   *                                     Latitude and Longitude classes.
   */
  class Sensor : public Isis::Spice {
    public:
      Sensor(Isis::Pvl &lab);

      virtual ~Sensor();

      void SetEphemerisTime(const double time);
      bool SetLookDirection(const double v[3]);
      bool SetRightAscensionDeclination(const double ra, const double dec);
      bool SetUniversalGround(const double latitude, const double longitude,
                              bool backCheck = true);
      bool SetUniversalGround(const double latitude, const double longitude,
                              const double radius, bool backCheck = true);

      /**
       * Returns if the last call to either SetLookDirection or
       * SetUniversalGround had a valid intersection with the target. If so then
       * other methods such as Coordinate, UniversalLatitude, UniversalLongitude,
       * etc can be used with confidence.
       */
      inline bool HasSurfaceIntersection() const {
        return p_hasIntersection;
      };

      void Coordinate(double p[3]) const;

      /**
       * Returns the planetocentric latitude.
       */
      double UniversalLatitude() const {
        return p_latitude;
      };

      /**
       * Returns a positive east, 0-360 domain longitude.
       */
      double UniversalLongitude() const {
        return p_longitude;
      };

      /**
       * Returns the local radius at the intersection point. This is either the
       * radius on the ellipsoid, the radius from the surface model passed into
       * the constructor, or the radius set with SetUniversalGround.
       */
      Distance LocalRadius() const;
      Distance LocalRadius(Latitude lat, Longitude lon);
      Distance LocalRadius(double lat, double lon);

      double PhaseAngle() const;
      double EmissionAngle() const;
      double IncidenceAngle() const;

      void LookDirection(double v[3]) const;

      double RightAscension();
      double Declination();

      double SlantDistance() const;
      double LocalSolarTime();
      double SolarDistance() const;
      double SpacecraftAltitude();
      double DemRadius(double lat, double lon);   //!<Return local radius from dem
      bool HasElevationModel() {
        return p_hasElevationModel;
      };
      /**
       * Virtual method that returns the pixel resolution of the sensor in meters/pix
       */
      virtual double Resolution() {
        return 1.0;
      };
      void IgnoreElevationModel(bool ignore);

    protected:
      bool p_hasIntersection; /**<This indicates if the surface point or look
                                  direction is valid. It is made protected so
                                  inheriting classes can change it if
                                  necessary.*/
      Isis::Cube *p_demCube;         //!< The cube containing the model
      SpiceDouble p_latitude;  //!< Latitude at p_pB
      SpiceDouble p_longitude; //!< Longitude at p_pB
      SpiceDouble p_pB[3];     //!< Surface intersection point in body fixed

    private:
      void CommonInitialize(const std::string &demCube);

      SpiceDouble p_lookB[3];  //!< Look direction in body fixed
      SpiceDouble p_radius;    //!< Local radius at p_pB

      bool p_newLookB;      //!< flag to indicate we need to recompute ra/dec
      SpiceDouble p_ra;     //!< Right ascension (sky longitude)
      SpiceDouble p_dec;    //!< Decliation (sky latitude)
      void computeRaDec();  //!< Computes the ra/dec from the look direction

      bool p_hasElevationModel;     //!< Does sensor use an elevation model
      Isis::Projection *p_demProj;  //!< The projection of the model
      Isis::Portal *p_portal;       //!< Buffer used to read from the model
      Isis::Interpolator *p_interp; //!< Use bilinear interpolation from dem
      bool SetGroundLocal(bool backCheck);   //!<Computes look vector

      double p_minRadius;  //!< Minimum radius value in DEM file
      double p_maxRadius;  //!< Maximum radius value in DEM file
      double p_demScale;   //!< Scale of DEM file in pixels per degree
  };
};

#endif
