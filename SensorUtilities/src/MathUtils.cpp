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


  GroundPt2D radiansToDegrees(GroundPt2D radianLatLon) {
    double degreeLon = radianLatLon.lon;
    if (degreeLon < 0) {
      degreeLon += 2 * M_PI;
    }

    degreeLon *= RAD2DEG;
    double degreeLat = radianLatLon.lat * RAD2DEG;
    return {degreeLat, degreeLon};
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

  double groundAzimuth(GroundPt2D groundPt, GroundPt2D subPt) {
    double a;
    double b;

    if (groundPt.lat >= 0.0) {
      a = (90.0 - subPt.lat) * M_PI / 180.0;
      b = (90.0 - groundPt.lat) * M_PI / 180.0;
    }
    else {
      a = (90.0 + subPt.lat) * M_PI / 180.0;
      b = (90.0 + groundPt.lat) * M_PI / 180.0;
    }

    GroundPt2D cs;
    GroundPt2D cg;

    cs.lon = subPt.lon;
    cg.lon = groundPt.lon;

    if (cs.lon > cg.lon) {
      if ((cs.lon - cg.lon) > 180.0) {
        while ((cs.lon - cg.lon) > 180.0) cs.lon = cs.lon - 360.0;
      }
    }
    if (cg.lon > cs.lon) {
      if ((cg.lon-cs.lon) > 180.0) {
        while ((cg.lon-cs.lon) > 180.0) cg.lon = cg.lon - 360.0;
      }
    }

    int quad;
    if (subPt.lat > groundPt.lat) {
      if (cs.lon < cg.lon) {
        quad = 2;
      }
      else {
        quad = 1;
      }
    }
    else if (subPt.lat < groundPt.lat) {
      if (cs.lon < cg.lon) {
        quad = 3;
      }
      else {
        quad = 4;
      }
    }
    else {
      if (cs.lon > cg.lon) {
        quad = 1;
      }
      else if (cs.lon < cg.lon) {
        quad = 2;
      }
      else {
        return 0.0;
      }
    }

    double C = (cg.lon - cs.lon) * M_PI / 180.0;
    if (C < 0) C = -C;

    double c = acos( cos(a) * cos(b) + sin(a) * sin(b) * cos(C));

    double azimuth = 0.0;

    if (sin(b) == 0.0 || sin(c) == 0.0) {
      return azimuth;
    }

    double intermediate = (cos(a) - cos(b) * cos(c)) / (sin(b) * sin(c));
    if (intermediate < -1.0) {
      intermediate = -1.0;
    }
    else if (intermediate > 1.0) {
      intermediate = 1.0;
    }

    double A = acos(intermediate) * 180.0 / M_PI;

    if (groundPt.lat >= 0.0) {
      if (quad == 1 || quad == 4) {
        azimuth = A;
      }
      else if (quad == 2 || quad == 3) {
        azimuth = 360.0 - A;
      }
    }
    else {
      if (quad == 1 || quad == 4) {
        azimuth = 180.0 - A;
      }
      else if (quad == 2 || quad == 3) {
        azimuth = 180.0 + A;
      }
    }
    return azimuth;
  }

  Vec crossProduct(Vec aVec, Vec bVec) {
    double x = aVec.y * bVec.z - aVec.z * bVec.y;
    double y = aVec.z * bVec.x - aVec.x * bVec.z;
    double z = aVec.x * bVec.y - aVec.y * bVec.x;
    return {x, y, z};
  }

  Vec unitVector(Vec vec) {
    double mag = magnitude(vec);
    return {vec.x / mag, vec.y / mag, vec.z / mag};
  }

  Vec scaleVector(Vec vec, double scalar) {
    return {vec.x * scalar, vec.y * scalar, vec.z * scalar};
  }

  Vec perpendicularVec(Vec aVec, Vec bVec) {
    if (magnitude(aVec) == 0){
      return bVec;
    }

    Vec aNorm = unitVector(aVec);
    Vec bNorm = unitVector(bVec);

    double angle = bNorm.x * aNorm.x + bNorm.y * aNorm.y + bNorm.z * aNorm.z;
    double aMag = magnitude(aVec);
    double magP = angle * aMag;

    Vec p = {bNorm.x * magP, bNorm.y * magP, bNorm.z * magP};
    Vec q = aVec - p;

    return q; 
  }

  Vec matrixVecProduct(Matrix mat, Vec vec) {
    Vec result;

    result.x = mat.a.x * vec.x + mat.a.y * vec.y + mat.a.z * vec.z;
    result.y = mat.b.x * vec.x + mat.b.y * vec.y + mat.b.z * vec.z;
    result.z = mat.c.x * vec.x + mat.c.y * vec.y + mat.c.z * vec.z;

    return result;
  }
}