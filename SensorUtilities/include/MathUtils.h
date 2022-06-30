#ifndef MathUtils_H
#define MathUtils_H

#include <vector>

namespace SensorUtilities {

  struct GroundPt2D {
    double lat;
    double lon;
  };


  struct GroundPt3D {
    double lat;
    double lon;
    double radius;
  };


  struct RaDec {
    double ra;
    double dec;
  };


  struct ImagePt {
    double line;
    double sample;
    double band;
  };


  struct Vec {
    double x;
    double y;
    double z;

    Vec(double a, double b, double c);
    Vec(const double data[3]);

    operator std::vector<double>() const;
  };


  bool operator==(const GroundPt2D& lhs, const GroundPt2D& rhs);


  bool operator==(const GroundPt3D& lhs, const GroundPt3D& rhs);


  bool operator==(const RaDec& lhs, const RaDec& rhs);


  bool operator==(const ImagePt& lhs, const ImagePt& rhs);


  bool operator==(const Vec& lhs, const Vec& rhs);


  Vec operator+(const Vec& lhs, const Vec& rhs);


  Vec operator-(const Vec& lhs, const Vec& rhs);


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

}

#endif

