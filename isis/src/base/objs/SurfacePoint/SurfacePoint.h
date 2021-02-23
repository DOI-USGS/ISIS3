#ifndef SurfacePoint_h
#define SurfacePoint_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include <cmath>

// Qt library
#include <QString>

#include "boost/numeric/ublas/symmetric.hpp"
#include "boost/numeric/ublas/io.hpp"

// ISIS library
#include "Displacement.h"
#include "Distance.h"
#include "Angle.h"

namespace Isis {
  class Latitude;
  class Longitude;

  /**
   * @brief This class defines a body-fixed surface point 
   *  
   * This class is a container for body-fixed surface points.  It provides 
   * methods to set and present the coordinates of surface points in various 
   * usable units to support projection of image points to the ground and 
   * bundle adjustment. 
   *
   * @ingroup Geometry
   *
   * @author 2010-07-30 Tracie Sucharski, Ken L. Edmunson, and Debbie A. Cook
   *
   * @internal
   *   @history  2010-08-25 Debbie A. Cook  Added more error checking and
   *                            testing
   *   @history  2010-09-10 Debbie A. Cook  Made ocentric methods specific to
   *                            units of sigmas (degrees or meters)
   *   @history  2010-10-04 Debbie A. Cook  Remove using boost to avoid compile
   *                            errors throughout Isis3 classes and added boost
   *                            namespace name to all uses of matrix
   *   @history  2010-10-20 Debbie A. Cook and Steven Lambright  Simplified the
   *                            class by using new Displacement, Distance,
   *                            Latitude, Longitude, and Angle objects.
   *   @history 2010-12-28 Steven Lambright and Sharmila Prasad
   *                           Fixed a problem with retreiving longitudes
   *                           outside of 0-360.
   *   @history 2011-02-11 Steven Lambright Added SphericalDistanceToPoint and
   *                           optimized for speed inside the cameras and
   *                           typical use cases where we only have an x,y,z but
   *                           no other data. Fixed a problem where points were
   *                           not properly considered valid at some boundary
   *                           conditions.
   *   @history 2011-03-05 Ken Edmundson Added GetLatWeight, GetLonWeight,
   *                           GetLocalRadiusWeight, and SetSphericalCoordinates
   *                           methods for use in BundleAdjust.
   *   @history 2011-04-18 Steven Lambright GetLatSigmaDistance and
   *                           GetLonSigmaDistance now return appropriate
   *                           results when the point is invalid but has radii.
   *   @history 2011-08-08 Steven Lambright and Jai Rideout -
   *                           SetSphericalSigmas() called with invalid
   *                           parameters will clear the sigmas. Improved some
   *                           safety checks.
   *   @history 2011-10-06 Steven Lambright - Get*SigmaDistance will no longer
   *                           throw an exception if there is no stored sigma
   *                           and there is no stored target radii.
   *   @history 2018-06-28 Debbie A. Cook - Removed target body radii and all
   *                           related methods.  Any reference to the major axis (equatorial
   *                           radius) was replaced with the local radius. Technically I think
   *                           we should be using the target body minor axis (polar) for the
   *                           conversion of latitude degrees to/from distance and the local 
   *                           radius of the surface point for conversion of longitude degrees 
   *                           to/from distance.  For large bodies the percent error will be small, 
   *                           so we will use the local radius for convenience.  For small bodies, 
   *                           any bundle adjustment will likely use rectangular coordinates 
   *                           where degree conversions will not be necessary.  Added new
   *                           member p_localRadius to avoid recalculating when coordinates
   *                           have not changed.  Also corrected the longitude conversion equation
   *                           in SetSphericalSigmasDistance and GetLonSigmaDistance.
   *                           References #5457.
   *   @history 2018-08-15 Debbie A. Cook - Initialized the local radius whenever any 
   *                           SurfacePoint coordinate was changed, removed memory errors,
   *                           and cleaned up documentation.  Changed localRadius member
   *                           from a pointer to value to reduce extraneous if blocks.  
   *                           References #5457
   *   @history 2018-09-06 Debbie A. Cook - Originally added to BundleXYZ branch on 
   *                           2017-06-25 - Added CoordinateType, CoordUnits, and CoordIndex
   *                           to support new convenience methods GetCoord, GetSigma, and GetWeight.
   *                           Also added methods GetXWeight, GetYWeight, GetZWeight, LatToDouble, 
   *                           LonToDouble, DistanceToDouble, DisplacementToDouble,  
   *                           getCoordSigmaDistance, stringToCoordinateType, and 
   *                           coordinateTypeToString.  Fixed comment in GetLocalRadiusWeight method 
   *                           to indicate kilometers instead of meters.  References #4649 and #501.
   *   @history 2018-09-06 Debbie A. Cook - Originally added to BundleXYZ branch on
   *                           2017-07-25 - Corrected covar(2,2) units in SetSphericalSigmas,
   *                           and all diagonal units in SetRectangularSigmas.  Corrected spelling 
   *                           of equatorial in comments.  Corrected conversion of longitude sigma
   *                           from meters to radians in SetSphericalSigmasDistance and from radians
   *                           to meters in GetLonSigmaDistance.  Fixed SetRectangularMatrix to take
   *                           input in km instead of m. 
   *   @history 2018-09-06 Debbie A. Cook - Originally added to BundleXYZ branch on
   *                           2017-11-20  - Added an additional argument to SetRectangularMatrix
   *                           and SetSphericalMatrix to specify the units of the matrix.  This will allow the 
   *                           bundle adjust to set in km and existing software to continue setting in the default 
   *                           units (meters).  The matrix will be stored in km in this object to avoid extra 
   *                           conversions during processing.
   *   @history 2018-09-06 Debbie A. Cook - Originally added to BundleXYZ branch on 
   *                           2018-03-07 - Added an additional argument to GetRectangularMatrix
   *                           and GetSphericalMatrix to specify the units of the matrix.  This will allow existing
   *                           callers to get in m (the default) and bundle adjust software to get in km and 
   *                           minimize conversions. The matrix is held in this object in km to avoid extra 
   *                           conversions during the bundle adjustment.  The control net stores the distance
   *                           values of the matrix in m**2.
   *   @history 2018-09-20 Debbie A. Cook - Added new methods 
   *                           LatitudeToMeters, MetersToLatitude,
   *                           LongitudeToMeters, and MetersToLongitude
   *                           for converting sigmas and corrections at the current SurfacePoint 
   *                           coordinates.  References #4649 and #501.
   *   @history 2018-10-12 Debbie A. Cook - Initialized local radius in 
   *                           SetRectangularCoordinates 
   *   @history  2019-05-29 Debbie A. Cook  Changed test constant from DBL_EPSILON to
   *                           1.0e-50 to avoid false positives and negatives in MetersToLongitude, 
   *                           GetLatSigmaDistance, and GetLonSigmaDistance.
   *                                                    
   */

  class SurfacePoint {
    public:

      // definitions
      /**
       * Defines the coordinate typ, units, and coordinate index  for some of the output methods
       */
      enum CoordinateType {
        Latitudinal,                  /**< Planetocentric latitudinal (lat/lon/rad) coordinates */
        Rectangular                   /**< Body-fixed rectangular x/y/z coordinates */
      }; 
      enum CoordUnits {
        Degrees,
        Kilometers,
        Meters,
        Radians
      };
      enum CoordIndex {
        One=0,
        Two=1,
        Three=2
    };
      
      // Constructors
      SurfacePoint();
      SurfacePoint(const SurfacePoint &other);
      SurfacePoint(const Latitude &lat, const Longitude &lon,
                   const Distance &radius);
      SurfacePoint(const Latitude &lat, const Longitude &lon,
          const Distance &radius, const Angle &latSigma, const Angle &lonSigma,
          const Distance &radiusSigma);
      SurfacePoint(const Latitude &lat, const Longitude &lon,
                   const Distance &radius,
                   const boost::numeric::ublas::symmetric_matrix
                     <double,boost::numeric::ublas::upper>& covar);
      SurfacePoint(const Displacement &x, const Displacement &y,
                   const Displacement &z);
      SurfacePoint(const Displacement &x, const Displacement &y,
          const Displacement &z, const Distance &xSigma, const Distance &ySigma,
          const Distance &zSigma);
      SurfacePoint(const Displacement &x, const Displacement &y,
                   const Displacement &z,
                   const boost::numeric::ublas::symmetric_matrix
                     <double,boost::numeric::ublas::upper>& covar);
      ~SurfacePoint();

// Rectangular loading utilities
      void SetRectangular(const Displacement &x, const Displacement &y,
          const Displacement &z, const Distance &xSigma=Distance(),
          const Distance &ySigma=Distance(), const Distance &zSigma=Distance());

      void SetRectangular(const Displacement x, const Displacement y, const Displacement z,
        const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar);

      void SetRectangularCoordinates(const Displacement &x, const Displacement &y,
                                   const Displacement &z);

      //! Set surface point and sigmas in rectangular coordinates and convert to planetocentric
      void SetRectangularSigmas(const Distance &xSigma, const Distance &ySigma,
                                const Distance &zSigma);

      void SetRectangularMatrix(
                                const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar,
                                SurfacePoint::CoordUnits units = SurfacePoint::Meters);

// Spherical loading utilities

      //! Set surface point and covariance matrix in planetocentric coordinates and convert to rectangular
      //! (Latitude, Longitude in degrees, Radius in meters; matrix in radians and radians**2)
      void SetSpherical (const Latitude &lat, const Longitude &lon,
          const Distance &radius, const Angle &latSigma=Angle(),
          const Angle &lonSigma=Angle(),
          const Distance &radiusSigma=Distance());

      void SetSpherical (const Latitude &lat, const Longitude &lon,
          const Distance &radius,
          const boost::numeric::ublas::symmetric_matrix
            <double,boost::numeric::ublas::upper>& covar);

      void SetSphericalCoordinates(const Latitude &lat, const Longitude &lon,
                                   const Distance &radius);

      void SetSphericalMatrix(
                              const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar,
                              SurfacePoint::CoordUnits units = SurfacePoint::Meters);

      void SetSphericalSigmas(const Angle &latSigma, const Angle &lonSigma,
                              const Distance &radiusSigma);

      void SetSphericalSigmasDistance(const Distance &latSigma,
                                      const Distance &lonSigma,
                                      const Distance &radiusSigma);

      void ResetLocalRadius(const Distance &radius);
      bool Valid() const;
      
// Generic utilities for convenience
      
      //! Set the covariance matrix
      void SetMatrix(CoordinateType type,
         const boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper>& covar);        
   
      //! Compute partial derivative of conversion from body-fixed coordinates to the specified
      //    coordinate type with respect to the indicated coordinate (specified by index).     
      std::vector<double> Partial(CoordinateType type, CoordIndex index);
      
// Output methods
      double GetCoord(CoordinateType type, CoordIndex index, CoordUnits units);
      // Consider making this GetSigmaDistance and use the Distance methods to specify units for
      // maximum flexibility and safety. ***TBD***
      double GetSigma(CoordinateType type, CoordIndex index, CoordUnits units);
      Distance GetSigmaDistance(CoordinateType type, CoordIndex index);
      double GetWeight(CoordinateType type, CoordIndex index);
      Displacement GetX() const;
      Displacement GetY() const;
      Displacement GetZ() const;
      Distance GetXSigma() const;
      Distance GetYSigma() const;
      Distance GetZSigma() const;
      double GetXWeight() const;
      double GetYWeight() const;
      double GetZWeight() const;
      boost::numeric::ublas::symmetric_matrix<double,boost::numeric::ublas::upper> 
        GetRectangularMatrix(SurfacePoint::CoordUnits units = SurfacePoint::Meters) const;
      Latitude GetLatitude() const; 
      Longitude GetLongitude() const;
      Distance GetLocalRadius() const;
      Angle GetLatSigma() const;
      Distance GetLatSigmaDistance() const;
      double GetLatWeight() const;
      Angle GetLonSigma() const;
      Distance GetLonSigmaDistance() const;
      double GetLonWeight() const;
      Distance GetLocalRadiusSigma() const;
      double GetLocalRadiusWeight() const;
      boost::numeric::ublas::symmetric_matrix
          <double,boost::numeric::ublas::upper> GetSphericalMatrix
            (SurfacePoint::CoordUnits units = SurfacePoint::Meters) const;

// Conversion methods (for convenience)
      double DisplacementToDouble(Displacement disp, CoordUnits units);
      double DistanceToDouble(Distance dist, CoordUnits units);
      double MetersToLatitude(double latLength);
      double MetersToLongitude(double lonLength);
      double LatitudeToMeters(double latitude) const;
      double LongitudeToMeters(double longitude) const;
      double LatToDouble(Latitude lat, CoordUnits units);
      double LonToDouble(Longitude lon, CoordUnits units);
      static CoordinateType stringToCoordinateType(QString type);
      static QString coordinateTypeToString(CoordinateType type);
        
// Computational methods
      Distance GetDistanceToPoint(const SurfacePoint &other) const;
      Distance GetDistanceToPoint(const SurfacePoint &other,
          const Distance &sphereRadius) const;
      std::vector<double> LatitudinalDerivative(CoordIndex index);
      std::vector<double> RectangularDerivative(CoordIndex index);

// Misc methods
      void ToNaifArray(double naifOutput[3]) const;
      void FromNaifArray(const double naifValues[3]);

// Operators
      bool operator==(const SurfacePoint &other) const;
      SurfacePoint &operator=(const SurfacePoint &other);

    private:
      void ComputeLocalRadius();
      void InitCovariance();
      void InitPoint();
      void SetRectangularPoint(const Displacement &x, const Displacement &y, const Displacement &z);
      void SetSphericalPoint(const Latitude &lat, const Longitude &lon, const Distance &radius);
      void FreeAllocatedMemory();

      Distance p_localRadius;
      Displacement *p_x;
      Displacement *p_y;
      Displacement *p_z;
      //! 3x3 upper triangular covariance matrix rectangular coordinates
      boost::numeric::ublas::symmetric_matrix
          <double,boost::numeric::ublas::upper> *p_rectCovar;
      //! 3x3 upper triangular covariance matrix ocentric coordinates
      boost::numeric::ublas::symmetric_matrix
          <double,boost::numeric::ublas::upper> *p_sphereCovar;
  };
};

#endif

