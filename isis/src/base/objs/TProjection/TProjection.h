#ifndef TProjection_h
#define TProjection_h
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
#include <vector>

#include "Projection.h"  
#include "PvlGroup.h" // protected data member object (m_mappingGrp) 

namespace Isis {
  class Displacement;
  class Pvl;
  class WorldMapper;
  /**
   *
   *
   * @brief Base class for Map TProjections
   *
   * This is a second level virtual base class for map projections of triaxial bodies.  It
   * must be used to create specific map projection classes such as Sinusoidal, 
   * Mercator, etc. The foundation of this class is the ability to convert ground 
   * coordinates (latitude and longitude) into projection coordinates (x and y) and
   * vice versa. Options exist to allow conversion to and from programmer specified 
   * world coordinates. The world coordinates can be cube pixels, paper units in
   * millimeters, or any other unit the program may need. Generally, you should
   * never directly instantiate this class.
   *
   * Here is an example of how to use TProjection
   * @code
   *   Pvl lab;
   *   lab.Read("projection.map");
   *   TProjection *p = TProjectionFactory::Create(lab);
   * @endcode
   * If you would like to see TProjection being used in implementation,
   * see mappos.cpp
   * @ingroup MapProjection
   * @author 2003-01-29 Jeff Anderson
   * @internal
   *   @history 2003-01-30 Jeff Anderson - Add the SetWorldMapper() method and
   *                           removed setting the mapper from the constructor
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-05-30 Jeff Anderson - Updated unit test to fix error for
   *                           optimization
   *   @history 2003-08-25 Jeff Anderson - Added LatitudeTypeString(),
   *                           LongitudeDirectionString(), and
   *                           LongitudeDomainString() methods
   *   @history 2003-09-26 Jeff Anderson - Added ToWorldX(), ToWorldY(),
   *                           ToProjectionX(), ToProjectionY(), and
   *                           Resolution() methods
   *   @history 2003-09-26 Jeff Anderson - Added virtual Name, operator==, and
   *                           operator!= methods
   *   @history 2003-09-26 Jeff Anderson - Remove virtual from operator!=
   *   @history 2003-10-14 Jeff Anderson - Added Scale() and TrueScaleLatitude()
   *                           methods
   *   @history 2003-11-04 Jeff Anderson - Replace the pure virtual methods for
   *                           SetGround() and SetCoordinate() with virtual
   *                           method which simply copy lat/lon to x/y and vice
   *                           versa. This is essentially no projection.
   *   @history 2003-11-04 Jeff Anderson - Added LocalRadius() methods
   *   @history 2004-02-23 Jeff Anderson - Added Eccentricity(), tCompute(),
   *                           mCompute(), e4Compute(), and phi2Compute()
   *                           methods
   *   @history 2004-02-24 Jeff Anderson - Fixed bug in eccentricity computation
   *   @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2005-02-25 Elizabeth Ribelin - Added 3 static conversion
   *                           methods: ToHours(), ToDMS(), and  ToHMS()
   *   @history 2005-03-08 Elizabeth Ribelin - Fixed unitTest
   *   @history 2005-06-24 Jeff Anderson - Tweaked format for DMS and HMS
   *                           methods
   *   @history 2005-07-22 Jeff Anderson - Added static ToPlanetographic()
   *                           method
   *   @history 2006-04-20 Tracie Sucharski - Fixed bug looking at Target
   *                           instead of TargetName for sky.
   *   @history 2006-06-20 Stuart Sides - Added ability to lookup the radii if
   *                           given a "TargetName" in the mapping group.
   *   @history 2006-07-10 Elizabeth Miller - Fixed bugs in new static method
   *   @history 2007-06-27 Steven Lambright - Added static ToPlanetocentric(),
   *                           ToPositiveWest() and ToPositiveEast() methods.
   *                           Changed To180Domain() and To360Domain() to be
   *                           static instead of const. Added Mapping(),
   *                           MappingLatitudes() and MappingLongitudes()
   *                           methods for map2map.
   *   @history 2007-08-14 Steven Lambright - Mapping() fixed: should not return
   *                           cube specific parameters, because they did not go
   *                           into making the projection.
   *   @history 2008-05-09 Steven Lambright - Added Name(), Version(),
   *                           IsEquatorialCylindrical() methods
   *   @history 2008-06-12 Christopher Austin - Elaborated error messages.
   *   @history 2008-06-19 Steven Lambright - Fixed memory leak
   *   @history 2009-01-29 Stacy Alley - added a overloaded STATIC
   *                           method for convenience.  TargetRadii, which takes
   *                           a Pvl, the cube label, and a PvlGroup, a mapping
   *                           group.
   *   @history 2011-02-10 Jai Rideout - added SetUpperLeftCorner() because
   *                           ProjectionFactory needed a way to set
   *                           UpperLeftCornerX and UpperLeftCornerY keywords
   *                           after creating the projection. Mapping() now adds
   *                           optional keywords UpperLeftCornerX,
   *                           UpperLeftCornerY, PixelResolution, and Scale to
   *                           original mapping group so that cam2map can
   *                           properly display a clean mapping group.
   *   @history 2011-07-05 Jeannie Backer - Added qCompute() method. Updated
   *                           documentation.
   *   @history 2011-08-11 Steven Lambright - phi2Compute was running out of
   *                           iterations for vesta (an asteroid). Fixes #279
   *   @history 2012-03-01 Jeff Anderson - Fixed bug in SetUpperLeftCorner by
   *                           adding Pvl::Replace when updating the mapping
   *                           labels 
   *   @history 2012-03-30 Steven Lambright and Stuart Sides - To360Domain() and
   *                           To180Domain() are now constant time operations.
   *                           Fixes #656.
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declarations to header file. Ordered includes in
   *                           implementation file. Moved destructor and
   *                           accessor methods to the implementation file.
   *                           Moved the following methods from the
   *                           ObliqueCylindrical class for
   *                           generalized xyRangeOblique() method - doSearch(),
   *                           findExtreme(), setSearchGround(). Changed member
   *                           variable prefix to m_. Improved unitTest.
   *                           Resolves #928.
   *   @history 2012-07-25 Jeannie Backer - Modified the new methods related to
   *                           xyRangeOblique() to verify the validity of the
   *                           values when setSearchGround() is called.
   *                           References #954.
   *   @history 2012-08-01 Kimberly Oyama - Added a qFuzzyCompare() to ToPlanetographic() to
   *                           account for rounding error at the latitude boundaries (-90 and
   *                           90 degrees). Updated the unit test to exercise this change.
   *                           References #604.
   *   @history 2012-08-09 Steven Lambright - Added NaifStatus calls to protect
   *                           the TargetRadii() method from naif throwing an
   *                           error/crashing with inputs that have non-target naif codes.
   *   @history 2012-11-18 Debbie A. Cook - Branched Projection into another level of parent classes
   *                           that inherit from Projection.
   *   @history 2016-04-22 Jeannie Backer - Modified TargetRadii(cubeLab, mapGroup) to
   *                           look for radii values in the NaifKeywords object of the labels
   *                           if we fail to find these values elsewhere. Modified
   *                           constructor to call TargetRadii(cubeLab, mapGroup), which will
   *                           in turn call TargetRadii(targetName), if needed. Added the
   *                           private method targetRadii() which is called by the public
   *                           TargetRadii() methods to find radii values using the given
   *                           NAIF target code. Fixes #3892
   *   @history 2016-05-10 Jeannie Backer - Moved TargetRadii() methods to Target class.
   *   @history 2016-05-25 Jeannie Backer - Updated documentation. References #3877
   *   @history 2016-08-28 Kelvin Rodriguez - Removed redundant var=var lines
   *                           causing warnings in clang. Part of porting to OS X 10.11.
   *   @history 2016-12-28 Jeannie Backer - Added inLatitudeRange, and inLongitudeRange methods.
   *                           References #3877
   *   @history 2017-06-26 Jesse Mapel - Added a new method to set the universal ground point
   *                           without adjusting for the longitude domain. Fixes #2185.
   *   @todo Continue to modify Projection class to comply with coding
   *         standards. Some of these include, but may not be limited to remove
   *         "Get" from methods GetX and GetY, change methods to lower camel
   *         case, add methods for RelativeScaleFactorLatitude and
   *         RelativeScaleFactorLongitude (see LambertAzimuthalEqualArea.cpp).
   *   @history 2018-09-28 Kaitlyn Lee - Removed unnecessary lines of code that were
   *                           causing build warnings on MacOS 10.13. References #5520.
   */
  class TProjection : public Projection {
    public:
      // constructor
      TProjection(Pvl &label);
      virtual ~TProjection();
      virtual bool operator== (const Projection &proj);

      double EquatorialRadius() const;
      double PolarRadius() const;
      double Eccentricity() const;
      double LocalRadius(double lat) const;
      double LocalRadius() const; // requires SetGround or SetCoordinate

      // These return or change properties of the projection, independent of calculations
      /**
       * This method returns the name of the map projection.  It is a pure 
       * virtual method (requires all subclasses to override). 
       *
       * @return string The name of the map projection.
       */
      virtual QString Name() const = 0;


      /**
       * This method returns the Version of the map projection.  It is a pure 
       * virtual method (requires all subclasses to override).
       *
       * @return string The Version number of the map projection.
       */
      virtual QString Version() const = 0;
      virtual double TrueScaleLatitude() const;
      virtual bool IsEquatorialCylindrical();


      /**
       * This enum defines the types of Latitude supported in this class
       */
      enum LatitudeType { 
        Planetocentric, /**< Latitudes are measured as the angle from the equatorial plane 
                             to the plane through the center of the planet and a given  point
                             on the surface of the planet.*/
        Planetographic  /**< Latitudes are measured as the angle from the equatorial plane 
                             to the normal to the surface of the planet at a given point.*/
      };


      // Check latitude type or get latitude type as a string
      bool IsPlanetocentric() const;
      bool IsPlanetographic() const;
      QString LatitudeTypeString() const;
      // change latitude type
      double ToPlanetocentric(const double lat) const;
      double ToPlanetographic(const double lat) const;

      /**
       * This enum defines the types of Longitude directions supported in this class.  
       */
      enum LongitudeDirection { PositiveEast, /**< Longitude values increase in 
                                                   the easterly direction.*/
                                PositiveWest  /**< Longitude values increase in 
                                                   the westerly direction.*/
                              };

      // Check longitude direction or get longitude direction as a string
      bool IsPositiveEast() const;
      bool IsPositiveWest() const;
      QString LongitudeDirectionString() const;

      // Check longitude domain or get longitude domain as a string
      bool Has180Domain() const;
      bool Has360Domain() const;
      QString LongitudeDomainString() const;

      // Get min/max lat/lon
      virtual double MinimumLatitude() const;
      virtual double MaximumLatitude() const;
      virtual double MinimumLongitude() const;
      virtual double MaximumLongitude() const;

      //  Calculations
      // Set ground position or x/y coordinate
      virtual bool SetGround(const double lat, const double lon);
      virtual bool SetCoordinate(const double x, const double y);

      // Methods that depend on successful completion
      // of SetGround/SetCoordinate Get lat,lon, x,y
      virtual double Latitude() const;
      virtual double Longitude() const;

      // Set the universal ground coordinate (calls SetGround)
      virtual bool SetUniversalGround(const double lat, const double lon);
      bool SetUnboundUniversalGround(const double coord1, const double coord2);

      // Return the universal ground coordinate after successful SetCoordinate
      virtual double UniversalLatitude();
      virtual double UniversalLongitude();

      // get scale for mapping world coordinates
      double Scale() const;

      // Return the x/y range which covers the lat/lon range in the labels
      virtual bool XYRange(double &minX, double &maxX,
                           double &minY, double &maxY);

      // get mapping information
      virtual PvlGroup Mapping();
      virtual PvlGroup MappingLatitudes();
      virtual PvlGroup MappingLongitudes();

      // change latitude type
      static double ToPlanetocentric(double lat,
                                     double eRadius, double pRadius);
      static double ToPlanetographic(double lat,
                                     double eRadius, double pRadius);
      // change longitude direction
      static double ToPositiveEast(const double lon, const int domain);
      static double ToPositiveWest(const double lon, const int domain);

      // change longitude domain 
      static double To180Domain(const double lon);
      static double To360Domain(const double lon);

    protected:
      void XYRangeCheck(const double latitude, const double longitude);
      bool inLongitudeRange(double longitude);
      bool inLongitudeRange(double minLon, double maxLon, double longitude);
      bool inLatitudeRange(double latitude);
      bool xyRangeOblique(double &minX, double &maxX, 
                          double &minY, double &maxY);

      // Convience methods for typical projection computations from Snyder
      double qCompute(const double sinPhi) const; // page 16
      double phi2Compute(const double t) const; // page 44
      double mCompute(const double sinphi, const double cosphi) const; // page 101
      double tCompute(const double phi, const double sinphi) const; //page 108
      double e4Compute() const; // page 161

    private:
      void doSearch(double minBorder, double maxBorder, 
                    double &extremeVal, const double constBorder,
                    bool searchX, bool searchLongitude, bool findMin);
      void findExtreme(double &minBorder,  double &maxBorder, 
                       double &minBorderX, double &minBorderY, 
                       double &maxBorderX, double &maxBorderY, 
                       const double constBorder, bool searchX, 
                       bool searchLongitude, bool findMin);
      void setSearchGround(const double variableBorder, 
                           const double constBorder, bool variableIsLat);

    protected:
      double m_latitude;   /**< This contains the currently set latitude value.
                                The value is only usable if m_good is true.*/
      double m_longitude;  /**< This contains the currently set longitude value.
                                The value is only usable if m_good is true.*/

      LatitudeType m_latitudeType; /**< An enumerated type indicating the LatitudeType read from the
                                        labels. It can be either Planetographic or Planetocentric.*/

      LongitudeDirection m_longitudeDirection; /**< An enumerated type indicating the 
                                                    LongitudeDirection read from the labels. 
                                                    It can be either PositiveEast or PositiveWest.
                                                    Indicating which direction the positive axis for 
                                                    longitude is.*/

      // TODO** Can this be generalized for both longitude and azimuth???
      int m_longitudeDomain; /**< This integer is either 180 or 360 and is read from the labels. 
                                  It represents the longitude domain when returning values through 
                                  Longitude method. The domain is either -180 to 180 or 0 to 360.*/

      double m_equatorialRadius;  /**< Polar radius of the target. This is a unitless value so that
                                       if the radii are in inches then the m_x and m_y will be in
                                       inches. This value is set on construction. It is either read
                                       directly from the mapping group of the given PVL label or it
                                       is found in NAIF kernels by using the Target value
                                       in the given label. When pulled from NAIF kernels, the
                                       equatorial radius is the first value of NAIF's radii array.*/
      double m_polarRadius;       /**< Polar radius of the target. This is a unitless value so that
                                       if the radii are in inches then the m_x and m_y will be in
                                       inches. Of course the units must be the same as the
                                       equatorial radius. This value is set on construction. 
                                       It is either read directly from the mapping group of the
                                       given PVL label or it is found in NAIF kernels by using the 
                                       Target value in the given label.When pulled from NAIF 
                                       kernels, the equatorial radius is the third value of NAIF's 
                                       radii array.*/

      double m_eccentricity;      //!< The eccentricity of the target body.

      double m_minimumLatitude;   /**< Contains the minimum latitude for the entire ground range. 
                                       Only usable if m_groundRangeGood is true.*/
      double m_maximumLatitude;   /**< Contains the maximum latitude for the entire ground range.
                                       Only usable if m_groundRangeGood is true.*/
      double m_minimumLongitude;  /**< Contains the minimum longitude for the entire ground range.
                                       Only usable if m_groundRangeGood is true.*/
      double m_maximumLongitude;  /**< Contains the maximum longitude for the entire ground range.
                                       Only usable if m_groundRangeGood is true.*/
    private:
      // These are necessary for calculating oblique X/Y range with 
      // discontinuity
      std::vector<double> m_specialLatCases; /**< Constant Latitudes that 
                                                  intersect a discontinuity.*/
      std::vector<double> m_specialLonCases; /**< Constant Longitudes that 
                                                  intersect a discontinuity.*/
  };
};
#endif

