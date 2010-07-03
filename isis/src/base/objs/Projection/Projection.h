#ifndef Projection_h
#define Projection_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/01/29 21:21:53 $
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

#include <string>
#include "Pvl.h"
#include "WorldMapper.h"

namespace Isis {
  /**
   *
   *
   * @brief Base class for Map Projections
   *
   * This is a virtual base class for map projections. It must be used to create
   * specific map projection classes such as Sinusoidal, Mercator, etc. The
   * foundation of this class is the ability to convert ground coordinates
   * (latitude and longitude) into projection coordinates (x and y) and vice
   * versa. Options exist to allow conversion to and from programmer specified
   * world coordinates. The world coordinates can be cube pixels, paper units in
   * millimeters, or any other unit the program may need. Generally, you should
   * never directly instantiate this class.
   *
   * Here is an example of how to use Projection
   * @code
   *   Pvl lab;
   *   lab.Read("projection.map");
   *   Projection *p = ProjectionFactory::Create(lab);
   * @endcode
   * If you would like to see Projection being used in implementation,
   * see mappos.cpp
   * @ingroup MapProjection
   * @author 2003-01-29 Jeff Anderson
   * @internal
   *  @history 2003-01-30 Jeff Anderson - Add the SetWorldMapper method and
   *                                      removed setting the mapper from the
   *                                      constructor
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2003-05-30 Jeff Anderson - Updated unit test to fix error for
   *                                      optimization
   *  @history 2003-08-25 Jeff Anderson - Added LatitudeTypeString,
   *                                      LongitudeDirectionString, and
   *                                      LongitudeDomainString methods
   *  @history 2003-09-26 Jeff Anderson - Added ToWorldX, ToWorldY,
   *                                      ToProjectionX, ToProjectionY,
   *                                      and Resolution methods
   *  @history 2003-09-26 Jeff Anderson - Added virtual Name, operator==, and
   *                                      operator!= methods
   *  @history 2003-09-26 Jeff Anderson - Remove virtual from operator!=
   *  @history 2003-10-14 Jeff Anderson - Added Scale and TrueScaleLatitude
   *                                      methods
   *  @history 2003-11-04 Jeff Anderson - Replace the pure virtual methods for
   *                                      SetGround and SetCoordinate with
   *                                      virtual method which simply copy
   *                                      lat/lon to x/y and vice versa. This is
   *                                      essentially no projection.
   *  @history 2003-11-04 Jeff Anderson - Added LocalRadius methods
   *  @history 2004-02-23 Jeff Anderson - Added Eccentricity, tCompute, mCompute,
   *                                      e4Compute, and phi2Compute methods
   *  @history 2004-02-24 Jeff Anderson - Fixed bug in eccentricity computation
   *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2005-02-25 Elizabeth Ribelin - Added 3 static conversion methods:
   *                                          ToHours(), ToDMS(), and  ToHMS()
   *  @history 2005-03-08 Elizabeth Ribelin - Fixed unitTest
   *  @history 2005-06-24 Jeff Anderson - Tweaked format for DMS and HMS methods
   *  @history 2005-07-22 Jeff Anderson - Added static ToPlanetographic method
   *  @history 2006-04-20 Tracie Sucharski - Fixed bug looking at Target instead
   *                                       of TargetName for sky.
   *  @history 2006-06-20 Stuart Sides - Added ability to lookup the radii if given
   *                                      a "TargetName" in the mapping group.
   *  @history 2006-07-10 Elizabeth Miller - Fixed bugs in new static method
   *  @history 2007-06-27 Steven Lambright - Added static ToPlanetocentric,
   *                                         ToPositiveWest and ToPositiveEast methods.
   *                                         Changed To180Domain and To360Domain to be static
   *                                         instead of const. Added Mapping(), MappingLatitudes() and
   *                                         MappingLongitudes() methods for map2map.
   *  @history 2007-08-14 Steven Lambright - Mapping() fixed: should not return cube specific parameters, because
   *                                         they did not go into making the projection.
   *  @history 2008-05-09 Steven Lambright - Added Name, Version, IsEquatorialCylindrical methods
   *  @history 2008-06-12 Christopher Austin - Elaborated error messages.
   *  @history 2008-06-19 Steven Lambright - Fixed memory leak
   *  @history 2009-01-29 Stacy Alley - added a overloaded STATIC
   *           method for convenience.  TargetRadii, which takes
   *           a Pvl, the cube label, and a PvlGroup, a mapping
   *           group.
   *
   */
  class Projection {
    public:
      // constructor
      Projection (Isis::Pvl &label);

      //! Destroys the Projection object
      virtual ~Projection () { if(p_mapper) delete p_mapper; };

     /**
      * If desired the programmer can use this method to set a world mapper to
      * be used in the SetWorld, WorldX, and WorldY methods. Mappers typically
      * transform a projection coordinate (x/y) into the desired working
      * coordinate system, for example, cube pixels or inches on a piece of
      * paper. They transform in both directions (world to projection and
      * projection to world). This allows for conversions from line/sample to
      * latitude/longitude and vice versa. This projection will take ownership 
      * of the WorldMapper pointer. 
      *
      * @param mapper Pointer to the mapper
      */
      void SetWorldMapper(Isis::WorldMapper *mapper) { p_mapper = mapper; };

      // Set world position
      bool SetWorld (const double x, const double y);

      // Set ground position
      virtual bool SetGround (const double lat, const double lon);

      // Set x/y coordinate
      virtual bool SetCoordinate (const double x, const double y);

     /**
      * This indicates if the last invocation of SetGround, SetCoordinate, or
      * SetUniversalGround was with successful or not. If there was success then
      * the Latitude, Longitude, XCoord, YCoord, UniversalLatitude,
      * UniversalLongitude, WorldX, and WorldY methods can be utilized.
      *
      * @return bool
      */
      inline bool IsGood() const { return p_good; };

      // Return computed world X/Y after successful SetGround
      double WorldX() const;
      double WorldY() const;

     /**
      * This returns a longitude with correct longitude direction and domain as
      * specified in the label object. The method can only be used if SetGround,
      * SetCoordinate, SetUniversalGround, or SetWorld return with success.
      * Success can also be checked using the IsGood method.
      *
      * @return double
      */
      inline double Longitude() const { return p_longitude; };

     /**
      * This returns a latitude with correct latitude type as specified in the
      * label object. The method can only be used if SetGround, SetCoordinate,
      * SetUniversalGround, or SetWorld return with success. Success can also
      * be checked using the IsGood method.
      *
      * @return double
      */
      inline double Latitude() const { return p_latitude; };

     /**
      * This returns the projection X provided SetGround, SetCoordinate,
      * SetUniversalGround, or SetWorld returned with success. Success can also
      * be checked using the IsGood method. The units of X will be in the same .
      * units as the radii obtained from the label.
      *
      * @return double
      */
      inline double XCoord() const { return p_x; };


     /**
      * This returns the projection Y provided SetGround, SetCoordinate,
      * SetUniversalGround, or SetWorld returned with success. Success can also
      * be checked using the IsGood method. The units of Y will be in the same
      * units as the radii obtained from the label.
      *
      * @return double
      */
      inline double YCoord() const { return p_y; };

     /**
      * This returns the equatorial radius of the target. The radius was
      * obtained from the label during object construction.
      *
      * @return double
      */
      inline double EquatorialRadius() const { return p_equatorialRadius; };

     /**
      * This returns the polar radius of the target. The radius was obtained
      * from the label during object construction.
      *
      * @return double
      */
      inline double PolarRadius() const { return p_polarRadius; };


     /**
      * This returns the eccentricity of the target
      *
      * @return double
      */
      inline double Eccentricity() const { return p_eccentricity; };

      // Obtain latitude type
      enum LatitudeType { Planetographic, Planetocentric };

     /**
      * This indicates if the latitude type is planetographic (as opposed to
      * planetocentric). The latitude type was obtained from the label during
      * object construction.
      *
      * @return bool
      */
      inline bool IsPlanetographic() const { return p_latitudeType == Planetographic; };


     /**
      * This indicates if the latitude type is planetocentric (as opposed to
      * planetographic). The latitude type was obtained from the label during
      * object construction.
      *
      * @return bool
      */
      inline bool IsPlanetocentric() const { return p_latitudeType == Planetocentric; };

      // Obtain latitude type as a string
      std::string LatitudeTypeString () const;

      // Obtain longitude directions as a string
      std::string LongitudeDirectionString () const;

      // Obtain longitude domain as a string
      std::string LongitudeDomainString () const;

      // Obtain longitude direction
      enum LongitudeDirection { PositiveEast, PositiveWest };

     /**
      * This indicates if the longitude direction type is positive east (as
      * opposed to postive west). The longitude type was obtained from the label
      * during object construction.
      *
      * @return bool
      */
      inline bool IsPositiveWest() const { return p_longitudeDirection == PositiveWest; };

     /**
      * This indicates if the longitude direction type is positive west (as
      * opposed to postive east). The longitude type was obtained from the label
      * during object construction.
      *
      * @return bool
      */
      inline bool IsPositiveEast() const { return p_longitudeDirection == PositiveEast; };

     /**
      * This indicates if the longitude domain is 0 to 360 (as opposed to -180
      * to 180). The longitude domain was obtained from the label during object
      * construction.
      *
      * @return bool
      */
      inline bool Has360Domain () const { return p_longitudeDomain == 360; };


     /**
      * This indicates if the longitude domain is -180 to 180 (as opposed to 0
      * to 360). The longitude domain was obtained from the label during object
      * construction.
      *
      * @return bool
      */
      inline bool Has180Domain () const { return p_longitudeDomain == 180; };

     /**
      * This indicates that the labels contained minimum and maximum latitudes
      * and longitudes (e.g., a ground range coverage). If the projection has
      * ground range coverage then the MinimumLatitude, MaximumLatitude,
      * MinimumLongitude, and MaximumLongitude methods can be used. The ground
      * range coverage essentially defines the area of user interest.
      *
      * @return bool
      */
      inline bool HasGroundRange() const { return p_groundRangeGood; };

     /**
      * This returns the minimum latitude of the area of interest. The value was
      * obtained from the labels during object construction. This method can
      * only be used if HasGroundRange returns a true.
      *
      * @return double
      */
      inline double MinimumLatitude() const { return p_minimumLatitude; };

     /**
      * This returns the maximum latitude of the area of interest. The value was
      * obtained from the labels during object construction. This method can
      * only be used if HasGroundRange returns a true.
      *
      * @return double
      */
      inline double MaximumLatitude() const { return p_maximumLatitude; };

     /**
      * This returns the minimum longitude of the area of interest. The value
      * was obtained from the labels during object construction. This method can
      * only be used if HasGroundRange returns a true.
      *
      * @return double
      */
      inline double MinimumLongitude() const { return p_minimumLongitude; };

     /**
      * This returns the maximum longitude of the area of interest. The value
      * was obtained from the labels during object construction. This method can
      * only be used if HasGroundRange returns a true.
      *
      * @return double
      */
      inline double MaximumLongitude() const { return p_maximumLongitude; };

      // Return the x/y range which covers the lat/lon range in the labels
      virtual bool XYRange (double &minX,double &maxX,double &minY,double &maxY);


     /**
      * Return the rotation of the map
      *
      * @return double
      */
      inline double Rotation() const { return p_rotation; };

      // Return the universal ground coordinate after successful SetCoordinate
      double UniversalLatitude();
      double UniversalLongitude();

      // Set the universal ground coordinate
      bool SetUniversalGround (const double lat, const double lon);

      // Convert latitude value from Planetographic to Planetocentric and vice versa
      double ToPlanetocentric (const double lat) const;
      double ToPlanetographic (const double lat) const;

      // Convert longitude value from (0:360) to (-180:180) or vice versa
      static double To360Domain (const double lon);
      static double To180Domain (const double lon);

      // Convert longitude value from positive east to positive west or vice verse
      static double ToPositiveEast (const double lon, const int domain);
      static double ToPositiveWest (const double lon, const int domain);

      double ToWorldX (const double projectionX) const;
      double ToWorldY (const double projectionY) const;
      double ToProjectionX (const double worldX) const;
      double ToProjectionY (const double worldY) const;
      double Resolution () const;
      double Scale () const;
      virtual double TrueScaleLatitude () const;
      virtual PvlGroup Mapping();
      virtual PvlGroup MappingLatitudes();
      virtual PvlGroup MappingLongitudes();

      virtual bool operator== (const Projection &proj);
      bool operator!= (const Projection &proj);

      /** 
       * This method returns true if the projection is 
       *   equatorial cylindrical. In other words, if an image
       *   projected at 0 is the same as an image projected at 360.
       * 
       * 
       * @return bool true if the projection is equatorial cylindrical
       */
      virtual bool IsEquatorialCylindrical() { return false; }

     /**
      * This method returns the name of the map projection
      *
      * @return string
      */
      virtual std::string Name () const = 0;
      virtual std::string Version () const = 0;

      double LocalRadius () const;
      double LocalRadius (double lat) const;

      //! Returns true if projection is sky and false if it is land
      bool IsSky () const { return p_sky; };

      // Static conversion methods
      static double ToHours(double angle);
      static std::string ToDMS(double angle);
      static std::string ToHMS(double angle);
      static double ToPlanetographic(double lat,
                                     double eRadius, double pRadius);
      static double ToPlanetocentric(double lat,
                                     double eRadius, double pRadius);
      static PvlGroup TargetRadii (std::string target);
      static PvlGroup TargetRadii (Pvl &cubeLab, PvlGroup &mapGroup);

    protected:
      Isis::WorldMapper *p_mapper;  /**<This points to a mapper passed into the
                                        SetWorldMapper method. This mapper
                                        allows the programmer to specify a
                                        different world coordinate system. Thus
                                        the programmer could pass in line/sample
                                        positions in order to obtain a
                                        latitude/longitude or set a lat/lon and
                                        get a line/sample.*/

      double p_latitude;   /**<This contain a latitude value. The value is only
                               usable if p_good is true.*/
      double p_longitude;  /**<This contain a longitude value. The value is only
                               usable if p_good is true.*/
      bool p_good;         /**<Indicates if the contents of p_x, p_y, p_latitude,
                               and p_longitude are valid.*/

      LatitudeType p_latitudeType;             /**<An enumerated type indicating
                                                   the LatitudeType read from
                                                   the labels. It can be either
                                                   Planetographic or
                                                   Planetocentric.*/
      LongitudeDirection p_longitudeDirection; /**<An enumerated type indicating
                                                   the LatitudeDirection read
                                                   from the labels. It can be
                                                   either PositiveEast or
                                                   PositiveWest.Indicating which
                                                   direction the positive axis
                                                   for longitude is.*/
      int p_longitudeDomain;                   /**<This integer is either 180 or
                                                   360 and is read from the
                                                   labels. It represents the
                                                   longitude domain when
                                                   returning values through
                                                   Longitude method. The domain
                                                   is either -180 to 180 or
                                                   0 to 360.*/

      double p_equatorialRadius;  /**<Polar radius of the target. This is a
                                      unitless value so that if the radius are
                                      in inches then the p_x and p_y will be in
                                      inches. The value is read from the labels.*/
      double p_polarRadius;       /**<Polar radius of the target. This is a
                                      unitless value so that if the radius are
                                      in inches then the p_x and p_y will be in
                                      inches. Of course the units must be the
                                      same as the equatorial radius. The value
                                      is read from the labels.*/

      double p_eccentricity;      //!<Planet Eccentricity
      bool p_sky;                 //!<Indicates whether projection is sky or land

      bool p_groundRangeGood;     /**<Indicates if the ground range (min/max
                                      lat/lons) were read from the labels.*/
      double p_minimumLatitude;   /**<Contains the minimum latitude for the
                                      entire ground range. Only usable if
                                      p_groundRangeGood is true.*/
      double p_maximumLatitude;   /**<Contains the maximum latitude for the
                                      entire ground range. Only usable if
                                      p_groundRangeGood is true.*/
      double p_minimumLongitude;  /**<Contains the minimum longitude for the
                                      entire ground range. Only usable if
                                      p_groundRangeGood is true.*/
      double p_maximumLongitude;  /**<Contains the maximum longitude for the
                                      entire ground range. Only usable if
                                      p_groundRangeGood is true.*/

      // Convience data/methods for XYRange virtual function
      double p_minimumX;  /**<The data elements p_minimumX, p_minimumY,
                              p_maximumX, and p_maximumY are convience data
                              elements when you write the XYRange virtual
                              function. They are used in conjuction with the
                              XYRangeCheck convience method. After utilizing
                              XYRangeCheck to test boundary conditions in the
                              XYRange method these values will contain the
                              projection x/y coverage for the ground range
                              specified by min/max lat/lon.*/
      double p_maximumX;  //!<See minimumX description.
      double p_minimumY;  //!<See minimumX description.
      double p_maximumY;  //!<See minimumX description.
      PvlGroup p_mappingGrp; //!<Mapping group that created this projection
      void XYRangeCheck(const double latitude, const double longitude);

      // Convience methods for typical projection computations
      double mCompute(const double sinphi, const double cosphi) const;
      double e4Compute() const;
      double tCompute(const double phi, const double sinphi) const;
      double phi2Compute(const double t) const;

      void SetXY (double x, double y);
      void SetComputedXY (double x, double y);
      double GetX () const;
      double GetY () const;

    private:
      double p_rotation;   //! Rotation of map (usually zero)

      double p_x;          /**<This contains the rotated X coordinate for a
                               specific projection at theposition indicated by
                               p_latitude/p_longitude. The value is only
                               usable if p_good is true.*/
      double p_y;          /**<This contains the rotated Y coordinate for a
                               specific projection at the position indicated by
                               p_latitude/p_longitude. The value is only
                               usable if p_good is true.*/
  };
};

#endif

