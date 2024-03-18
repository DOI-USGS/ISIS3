#ifndef MathUtils_H
#define MathUtils_H

#include <vector>

namespace SensorUtilities {

  const double RAD2DEG = 57.29577951308232087679815481;

  /**
   * Structure for a 2-dimensional spherical ground point.
   * Latitude and longitude are in radians.
   * Longitude is planeto-centric, positive east, and in [-pi, pi].
   */
  struct GroundPt2D {
    double lat;
    double lon;
  };


  /**
   * Structure for a 3-dimensional spherical ground point.
   * Latitude and longitude are in radians.
   * Longitude is planeto-centric, positive east, and in [-pi, pi].
   * Radius is in meters.
   */
  struct GroundPt3D {
    double lat;
    double lon;
    double radius;
  };


  /**
   * Structure for right ascension and declination angles.
   * Right Ascenion angle in degrees.
   * Declination angle in degrees.
   */
  struct RaDec {
    double ra;
    double dec;
  };


  /**
   * Structure for a point in an image.
   * The line and sample origin is the upper-left corner at (0, 0).
   * The first band in the image is 0.
   */
  struct ImagePt {
    double line;
    double sample;
    int band;
  };


  /**
   * Structure for a 3-dimensional rectangular point or vector.
   * Distances are in meters.
   */
  struct Vec {
    double x;
    double y;
    double z;

    /**
     * Default Vec constructor needed for python wrapper.
     */
    Vec() {};

    /**
     * Construct a vector from 3 values
     */
    Vec(double a, double b, double c);

    /**
     * Construct a vector from an array with at least 3 values.
     * The data is copied into the structure.
     */
    Vec(const double data[3]);

    /**
     * Create a std::vector from the structure.
     * The data in the std:vector is a copy.
     */
    operator std::vector<double>() const;
  };

  /**
   * Structure for a 3 by 3 Matrix.
   */
  struct Matrix {
    Vec a;
    Vec b;
    Vec c;
  };


  bool operator==(const GroundPt2D& lhs, const GroundPt2D& rhs);


  bool operator==(const GroundPt3D& lhs, const GroundPt3D& rhs);


  bool operator==(const ImagePt& lhs, const ImagePt& rhs);


  bool operator==(const Vec& lhs, const Vec& rhs);


  Vec operator+(const Vec& lhs, const Vec& rhs);


  Vec operator-(const Vec& lhs, const Vec& rhs);


  /**
   * Compute the separation angle between the vectors defined by three points.
   * The separation angle is the angle inscribed by the points
   * A, B, and C as follows:
   *
   *    A
   *   /
   *  /
   * B - - - C
   *
   * @param aPt The first point
   * @param bPt The middle point
   * @param cPt The second point
   *
   * @return The separation angle in radians between 0 and pi
   */
  double sepAngle(Vec aPt, Vec bPt, Vec cPt);


  /**
   * Compute the separation angle between two vectors
   *
   * @param aVec The first vector
   * @param bVec The second vector
   *
   * @return The separation angle in radians between 0 and pi
   */
  double sepAngle(Vec aVec, Vec bVec);


  /**
   * Compute the magnitude of a vector
   */
  double magnitude(Vec vec);


  /**
   * Compute the distance between two points in 3d space
   */
  double distance(Vec start, Vec stop);


  /**
   * Converts coordinate from radians to degrees
   * 
   * @param radianLatLon coordinate in radians
   * 
   * @return Coordinate in degrees
   */
  GroundPt2D radiansToDegrees(GroundPt2D radianLatLon);


  /**
   * Convert spherical coordinates to rectangular coordinates
   *
   * @param spherical The spherical coordinate as geocentric latitude, longitude, radius
   *
   * @return The rectangular coordinate as x, y, z
   */
   Vec sphericalToRect(GroundPt3D spherical);


  /**
   * Convert rectangular coordinates to spherical coordinates.
   * Returns 0, 0, 0 if the input is the zero vector.
   *
   * @param rectangular The rectangular coordinate as x, y, z
   *
   * @return The spherical coordinate as geocentric latitude, longitude, radius
   */
   GroundPt3D rectToSpherical(Vec rectangular);

    
    
  /**
   * Computes and returns the ground azimuth between the ground point and
   * another point of interest, such as the subspacecraft point or the
   * subsolar point. 
   *
   * @param groundPt Ground point latitude and longitude
   * @param subPt latitude and longitude of a point of interest (e.g. subspacecraft or subsolar)
   *
   * @return double The azimuth in degrees
   */
  double groundAzimuth(GroundPt2D groundPt, GroundPt2D subPt);


  /**
   * Find the component of a vector that is perpendicular to a second vector.
   */
  Vec perpendicularVec(Vec aVec, Vec bVec);


  /**
   * Compute the cross product of two Vecs.
   */
  Vec crossProduct(Vec aVec, Vec bVec);


  /**
   * Multiply a scalar and a Vec.
   */
  Vec scaleVector(Vec vec, double scalar);


  /**
   * Find the unit vector along a Vec.
   */
  Vec unitVector(Vec vec);


  /**
   * Multiply a Matrix with a Vec.
   */
  Vec matrixVecProduct(Matrix mat, Vec vec);
}

#endif