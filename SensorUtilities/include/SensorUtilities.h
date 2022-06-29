#ifndef SensorUtilities_H
#define SensorUtilities_H


#include <vector>

#include "MathUtils.h"


namespace SensorUtilities {


  struct ObserverState {
    Vec lookVec;
    Vec j2000LookVec;
    Vec sensorPos;
    double time;
    ImagePt imagePoint;
  };


  struct Intersection {
    Vec groundPt;
    Vec normal;
  };


    // Interface only
  class Sensor {
    public:
      virtual ObserverState getState(const ImagePt &imagePoint) = 0;
      virtual ObserverState getState(const GroundPt3D &groundPt) = 0;
  };


  // Interface
  class Shape {
    public:
      virtual Intersection intersect(const Vec &sensorPos, const Vec &lookVec, bool computeLocalNormal=true) = 0;
  };


  // Interface only
  class Illuminator {
    public:
      virtual Vec position(double time) = 0;
  };


  /**
   * Compute the phase angle at an image point.
   * The phase angle is the separation angle between the vector from the ground point to the
   * illuminator and the vector from the ground point to the sensor.
   *
   * @return The phase angle in radians
   */
  double phaseAngle(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator);


  /**
   * Compute the phase angle from an observer and ground point.
   * The phase angle is the separation angle between the vector from the ground point to the
   * illuminator and the vector from the ground point to the sensor.
   *
   * @return The phase angle in radians
   */
  double phaseAngle(const Vec &sensorPos, const Vec &groundPoint, const Vec &illumPos);


  /**
   * Compute the emission angle at an image point.
   * The emission angle is the separation angle between the surface normal and the vector from
   * the ground point to the sensor.
   *
   * @return The emission angle in radians
   */
  double emissionAngle(const ImagePt &imagePoint, Sensor *sensor, Shape *shape);


  /**
   * Compute the emission angle at an image point using the ellipsoid surface normal.
   * Computing an ellipsoid surface normal is much faster than computing a local
   * surface normal but less precise. The exact differences will depend upon what type
   * of shape you use.
   *
   * @return The emission angle in radians
   */
  double ellipsoidEmissionAngle(const ImagePt &imagePoint, Sensor *sensor, Shape *shape);


  /**
   * Compute the distance from a ground point to an illuminator.
   *
   * @return The distance to the illuminator in meters
   */
  double illuminationDistance(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator);


  /**
   * Compute the latitude and longitude on the body below the sensor
   * when an image coordinate was observed.
   */
  GroundPt2D subSpacecraftPoint(const ImagePt &imagePoint, Sensor *sensor);


  /**
   * Compute the point on the body below the sensor when an image coordinate was observed.
   */
  Vec subSpacecraftPoint(const ImagePt &imagePoint, Sensor *sensor, Shape *shape);


  /**
   * Compute the latitude and longitude on the body below the illuminator
   * when an image coordinate was observed.
   */
  GroundPt2D subSolarPoint(const ImagePt &imagePoint, Sensor *sensor, Illuminator *illuminator);


  /**
   * Compute the point on the body below the illuminator when an image coordinate was observed.
   */
  Vec subSolarPoint(const ImagePt &imagePoint, Sensor *sensor, Illuminator *illuminator, Shape *shape);


  /**
   * Compute the local radius of the ground point observed at an image coordinate.
   *
   * @return The local radius in meters
   */
  double localRadius(const ImagePt &imagePoint, Sensor *sensor, Shape *shape);


  /**
   * Compute the local radius of a shape at a latitude and longitude
   *
   * @param groundPt The latitude and longitude
   * @param shape The shape to compute the local radius of
   * @param maxRadius The maximum radius of the shape. This only needs to be greater than the true
   *                  maximum radius, but results may be more precise if it's closer to the
   *                  true maximum radius.
   *
   * @return The local radius in meters
   */
  double localRadius(const GroundPt2D &groundPt, Shape *shape, double maxRadius=1e9);


  // RaDec computeRightAscensionDeclination(ImagePt imagePoint, Sensor *sensor) {
  //   Vec lookVec = sensor->j2000LookVec(imagePoint);
  //   GroundPt3D latLonRad = rectToSpherical(lookVec);
  //   return {latLonRad.lat, latLonRad.lon};
  // }


  // double sunAzimuth(ImagePt imagePoint, Sensor *sensor, Illuminator *illuminator, Shape *shape) {
  //   Vec lookVec = sensor->lookVec(imagePoint);
  //   Vec sensorPos = sensor->sensorPos(imagePoint);
  //   Vec groundPt = shape->intersect(sensorPos, lookVec);

  //   // imagePoint + 1 sample (assuming sample is the second element in imagePoint)
  //   Vec rightLookVec = sensor->lookVec({imagePoint.line, imagePoint.sample + 1.0});
  //   Vec rightSensorPos = sensor->sensorPos({imagePoint.line, imagePoint.sample + 1.0});
  //   Vec rightGroundPt = shape->intersect(rightSensorPos, rightLookVec);

  //   GroundPt2D subSolar = subSolarPoint(imagePoint, sensor, illuminator);

  //   Vec baseVec = {rightGroundPt.x - groundPt.x, rightGroundPt.y - groundPt.y, rightGroundPt.z - groundPt.z};
  //   //Vec toSolarVec = {subSolar.x - groundPt.x, subSolar.y - groundPt.y, subSolar.z - groundPt.z};
  //   Vec surfaceNormal = shape->normal(groundPt);
  //   // project onto plane tangent to surface
  //   // vector math functions here
  //   // baseVecProj
  //   // toSolarVecProj
  //   // return sepAngle(baseVecProj, toSolarVecProj);
  //   return 0.0;
  //   // There is some stuff here about + being clockwise and weird domain
  // }


  // -ocentric to -ographic latitude conversions???
}

#endif

