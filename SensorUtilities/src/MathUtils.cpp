#include "MathUtils.h"

#include <cmath>

namespace SensorUtilities {

  bool operator==(const GroundPt2D& lhs, const GroundPt2D& rhs) {
    return lhs.lat == rhs.lat && lhs.lon == rhs.lon;
  }


  bool operator==(const GroundPt3D& lhs, const GroundPt3D& rhs) {
    return lhs.lat == rhs.lat && lhs.lon == rhs.lon && lhs.radius == rhs.radius;
  }


  bool operator==(const ImagePt& lhs, const ImagePt& rhs) {
    return lhs.line == rhs.line && lhs.sample == rhs.sample;
  }


  bool operator==(const Vec& lhs, const Vec& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }


  Vec operator+(const Vec& lhs, const Vec& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z};
  }


  Vec operator-(const Vec& lhs, const Vec& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
  }


  Vec::Vec(double a, double b, double c) {
    x = a;
    y = b;
    z = c;
  }


  Vec::Vec(const double data[3]) {
    x = data[0];
    y = data[1];
    z = data[2];
  }


  Vec::operator std::vector<double>() const {
    return {x, y, z};
  }


  double sepAngle(Vec aPt, Vec bPt, Vec cPt) {
    return sepAngle(aPt - bPt, cPt - bPt);
  }


  double sepAngle(Vec aVec, Vec bVec) {
    double dotProd = aVec.x * bVec.x + aVec.y * bVec.y + aVec.z * bVec.z;
    dotProd /= magnitude(aVec) * magnitude(bVec);

    if(dotProd >= 1.0) return 0.0;
    if(dotProd <= -1.0) return M_PI;

    return acos(dotProd);
  }


  double magnitude(Vec vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
  }


  double distance(Vec start, Vec stop) {
    return magnitude(stop - start);
  }


  Vec sphericalToRect(GroundPt3D spherical) {
    return {
      spherical.radius * cos(spherical.lat) * cos(spherical.lon),
      spherical.radius * cos(spherical.lat) * sin(spherical.lon),
      spherical.radius * sin(spherical.lat)};
  }


  GroundPt3D rectToSpherical(Vec rectangular) {
    double rad = magnitude(rectangular);
    if (rad < 1e-15) {
      return {0.0, 0.0, 0.0};
    }
    return {
      asin(rectangular.z / rad),
      atan2(rectangular.y, rectangular.x),
      rad
    };
  }

}

