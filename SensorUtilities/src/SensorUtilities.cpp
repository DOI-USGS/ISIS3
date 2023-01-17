#include "SensorUtilities.h"

namespace SensorUtilities {
  double phaseAngle(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);
    Vec illumPos = illuminator->position(sensorState.time);
    return sepAngle(sensorState.sensorPos, intersect.groundPt, illumPos);
  }


  double emissionAngle(const ImagePt &imagePoint, Sensor *sensor, Shape *shape) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);

    Vec sensorDiff = sensorState.sensorPos - intersect.groundPt;
    return sepAngle(intersect.normal, sensorDiff);
  }


  double ellipsoidEmissionAngle(const ImagePt &imagePoint, Sensor *sensor, Shape *shape) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec, false);

    Vec sensorDiff = sensorState.sensorPos - intersect.groundPt;
    return sepAngle(intersect.normal, sensorDiff);
  }


  double illuminationDistance(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);
    Vec illumPos = illuminator->position(sensorState.time);

    return distance(illumPos, intersect.groundPt);
  }


  GroundPt2D subSpacecraftPoint(const ImagePt &imagePoint, Sensor *sensor)  {
    ObserverState sensorState = sensor->getState(imagePoint);
    GroundPt3D latLonRad = rectToSpherical(sensorState.sensorPos);
    return {latLonRad.lat, latLonRad.lon};
  }


  Vec subSpacecraftPoint(const ImagePt &imagePoint, Sensor *sensor, Shape *shape)  {
    ObserverState sensorState = sensor->getState(imagePoint);
    Vec lookVec = {-sensorState.sensorPos.x, -sensorState.sensorPos.y, -sensorState.sensorPos.z};
    return shape->intersect(sensorState.sensorPos, lookVec).groundPt;
  }


  GroundPt2D subSolarPoint(const ImagePt &imagePoint, Sensor *sensor, Illuminator *illuminator)  {
    ObserverState sensorState = sensor->getState(imagePoint);
    Vec illumPos = illuminator->position(sensorState.time);
    GroundPt3D latLonRad = rectToSpherical(illumPos);
    return {latLonRad.lat, latLonRad.lon};
  }


  Vec subSolarPoint(const ImagePt &imagePoint, Sensor *sensor, Illuminator *illuminator, Shape *shape) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Vec illumPos = illuminator->position(sensorState.time);
    Vec lookVec = {-illumPos.x, -illumPos.y, -illumPos.z};
    return shape->intersect(illumPos, lookVec).groundPt;
  }


  double localRadius(const ImagePt &imagePoint, Sensor *sensor, Shape *shape) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);
    return magnitude(intersect.groundPt);
  }


  double localRadius(const GroundPt2D &groundPt, Shape *shape, double maxRadius) {
    Vec position = sphericalToRect({groundPt.lat, groundPt.lon, maxRadius});
    Vec lookVec = {-position.x, -position.y, -position.z};
    Intersection intersect = shape->intersect(position, lookVec);
    return magnitude(intersect.groundPt);
  }
}