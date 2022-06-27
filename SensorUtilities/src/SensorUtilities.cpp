#include "SensorUtilities.h"

namespace SensorUtilities {
  double phaseAngle(ImagePt imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator) {
    Vec lookVec = sensor->lookVec(imagePoint);
    Vec sensorPos = sensor->sensorPos(imagePoint);
    Vec groundPt = shape->intersect(sensorPos, lookVec);
    Vec illumPos = illuminator->position( sensor->time(imagePoint) );

    Vec illumDiff = illumPos - groundPt;
    Vec sensorDiff = sensorPos - groundPt;
    return sepAngle(illumDiff, sensorDiff);
  }


  double emissionAngle(ImagePt imagePoint, Sensor *sensor, Shape *shape) {
    Vec lookVec = sensor->lookVec(imagePoint);
    Vec sensorPos = sensor->sensorPos(imagePoint);
    Vec groundPt = shape->intersect(sensorPos, lookVec);
    Vec surfaceNormal = shape->normal(groundPt);

    Vec sensorDiff = sensorPos - groundPt;
    return sepAngle(surfaceNormal, sensorDiff);
  }


  double illuminationDistance(ImagePt imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator) {
    Vec lookVec = sensor->lookVec(imagePoint);
    Vec sensorPos = sensor->sensorPos(imagePoint);
    Vec groundPt = shape->intersect(sensorPos, lookVec);
    Vec illumPos = illuminator->position( sensor->time(imagePoint) );

    return distance(illumPos, groundPt);
  }


  GroundPt2D subSpacecraftPoint(ImagePt imagePoint, Sensor *sensor)  {
    Vec sensorPos = sensor->sensorPos(imagePoint);
    GroundPt3D latLonRad = rectToSpherical(sensorPos);
    return {latLonRad.lat, latLonRad.lon};
  }


  Vec subSpacecraftPoint(ImagePt imagePoint, Sensor *sensor, Shape *shape)  {
    Vec sensorPos = sensor->sensorPos(imagePoint);
    Vec lookVec = {-sensorPos.x, -sensorPos.y, -sensorPos.z};
    return shape->intersect(sensorPos, lookVec);
  }


  GroundPt2D subSolarPoint(ImagePt imagePoint, Sensor *sensor, Illuminator *illuminator)  {
    Vec illumPos = illuminator->position(sensor->time(imagePoint));
    GroundPt3D latLonRad = rectToSpherical(illumPos);
    return {latLonRad.lat, latLonRad.lon};
  }


  Vec subSolarPoint(ImagePt imagePoint, Sensor *sensor, Illuminator *illuminator, Shape *shape) {
    Vec illumPos = illuminator->position(sensor->time(imagePoint));
    Vec lookVec = {-illumPos.x, -illumPos.y, -illumPos.z};
    return shape->intersect(illumPos, lookVec);
  }


  double localRadius(ImagePt imagePoint, Sensor *sensor, Shape *shape) {
    Vec lookVec = sensor->lookVec(imagePoint);
    Vec sensorPos = sensor->sensorPos(imagePoint);
    Vec groundPt = shape->intersect(sensorPos, lookVec);
    return magnitude(groundPt);
  }


  double localRadius(GroundPt2D groundPt, Shape *shape, double maxRadius) {
    Vec position = sphericalToRect({groundPt.lat, groundPt.lon, maxRadius});
    Vec lookVec = {-position.x, -position.y, -position.z};
    Vec radPt = shape->intersect(position, lookVec);
    return magnitude(radPt);
  }
}