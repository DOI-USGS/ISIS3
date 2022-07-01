#ifndef SensorUtilities_H
#define SensorUtilities_H


#include <vector>

#include "MathUtils.h"


namespace SensorUtilities {


  /**
   * Structure for storing the state of an observer at a specific image coordinate
   * and time.
   */
  struct ObserverState {
    // The look vector for the image coordinate in object space
    Vec lookVec;
    // The look vector for the image coordinate in the universal reference frame
    Vec j2000LookVec;
    // The position of the observer in object space
    Vec sensorPos;
    // The time that the observer state exists at in ephemeris seconds
    double time;
    // The image coordinate that was captured at the time
    ImagePt imagePoint;
  };


  /**
   * Structure for storing an intersection with a surface.
   */
  struct Intersection {
    Vec groundPt;
    Vec normal;
  };


  /**
   * Interface for sensors.
   * Implementaions of this interface are responsible for operating in both object
   * space and the universal reference frame.
   */
  class Sensor {
    public:
      /**
       * Get the observer state at a given image coordinate.
       */
      virtual ObserverState getState(const ImagePt &imagePoint) = 0;

      /**
       * Get the observer state that observers a given ground point.
       */
      virtual ObserverState getState(const GroundPt3D &groundPt) = 0;
  };


  /**
   * Interface for surface models.
   * Implementations of this interface operate in object space.
   */
  class Shape {
    public:
      /**
       * Intersect a vector with the surface model.
       *
       * @param sensorPos The starting point of the vector to intersect
       * @param lookVec The direction component of the vector. This may or may not be normalized.
       * @param computeLocalNormal If the more accurate local normal should be computed instead of the
       *                           potentially faster ellipsoid normal.
       */
      virtual Intersection intersect(const Vec &sensorPos, const Vec &lookVec, bool computeLocalNormal=true) = 0;
  };


  /**
   * Interface for the location of the illumination source.
   * Primarily this will be the location of the sun.
   * Implementations of this interface operate in object space.
   */
  class Illuminator {
    public:
      /**
       * Get the position for the illumination source at a given time.
       */
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
}

#endif

