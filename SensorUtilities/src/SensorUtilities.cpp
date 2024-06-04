#include "SensorUtilities.h"
#include <cmath>
#include <iostream>


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


  double slantDistance(ImagePt imagePoint, Sensor *sensor, Shape *shape) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);

    return distance(sensorState.sensorPos, intersect.groundPt);
  }


  double targetCenterDistance(ImagePt &imagePoint, Sensor *sensor, Body *body) {
    ObserverState sensorState = sensor->getState(imagePoint); 
    Vec sB = body->fixedVector(sensorState.sensorPos);
    return distance(sB, {0,0,0});
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


  RaDec rightAscensionDeclination(ImagePt &imagePoint, Sensor *sensor) {
    ObserverState sensorState = sensor->getState(imagePoint);
    GroundPt3D sphericalPt = rectToSpherical(sensorState.j2000LookVec);
    
    GroundPt2D sphericalPtRadians = {sphericalPt.lat, sphericalPt.lon};
    GroundPt2D sphericalPtDegrees = radiansToDegrees(sphericalPtRadians);

    return {sphericalPtDegrees.lon, sphericalPtDegrees.lat};
  }


  double localSolarTime(ImagePt &imagePoint, Sensor *sensor, Shape *shape, Illuminator *illuminator) { 
    ObserverState sensorState = sensor->getState(imagePoint);

    GroundPt2D subSolarPt = subSolarPoint(imagePoint, sensor, illuminator);
    GroundPt2D subSolarPtDegrees = radiansToDegrees(subSolarPt);
  
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);
    GroundPt3D sphericalPt = rectToSpherical(intersect.groundPt);
    GroundPt2D sphericalPtRadians = {sphericalPt.lat, sphericalPt.lon};
    GroundPt2D sphericalPtDegrees = radiansToDegrees(sphericalPtRadians);

    double lst = sphericalPtDegrees.lon - subSolarPtDegrees.lon + 180.0;
    lst = lst / 15.0; // 15 degrees per hour
    if (lst < 0.0) lst += 24.0;
    if (lst > 24.0) lst -= 24.0;
    return lst;
  }

  double lineResolution(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, double focalLength, double pixelPitch, double lineScaleFactor) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);

    double dist = distance(sensorState.sensorPos, intersect.groundPt) * 1000.0;

    return (dist / (focalLength / pixelPitch)) * lineScaleFactor;
  }


  double sampleResolution(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, double focalLength, double pixelPitch, double sampleScaleFactor) {
    ObserverState sensorState = sensor->getState(imagePoint);
    Intersection intersect = shape->intersect(sensorState.sensorPos, sensorState.lookVec);

    double dist = distance(sensorState.sensorPos, intersect.groundPt) * 1000.0;

    return (dist / (focalLength / pixelPitch)) * sampleScaleFactor;
  }


  double pixelResolution(const ImagePt &imagePoint, Sensor *sensor, Shape *shape, double focalLength, double pixelPitch, double lineScaleFactor, double sampleScaleFactor) {
    double lineRes = lineResolution(imagePoint, sensor, shape, focalLength, pixelPitch, lineScaleFactor);
    double sampRes = sampleResolution(imagePoint, sensor, shape, focalLength, pixelPitch, sampleScaleFactor);
    if (lineRes < 0.0) return 0.0;
    if (sampRes < 0.0) return 0.0;
    return (lineRes + sampRes) / 2.0;
  }


  double solarLongitude(ImagePt &imagePoint, Sensor *sensor, Illuminator *illuminator, Body *body) {
    ObserverState sensorState = sensor->getState(imagePoint);

    Vec illumPos = illuminator->position(sensorState.time);
    Vec illumVel = illuminator->velocity(sensorState.time);

    std::vector<double> bodyRot = body->rotation(sensorState.time);

    Vec sunAv = unitVector(crossProduct(illumPos, illumVel));
    double npole[3];
    for (int i = 0; i < 3; i++) {
      npole[i] = bodyRot[6+i];
    }

    Vec z = sunAv;
    Vec x = unitVector(crossProduct(npole, z));
    Vec y = unitVector(crossProduct(z, x));

    Matrix trans;
    trans.a.x = x.x;
    trans.b.x = y.x;
    trans.c.x = z.x;

    trans.a.y = x.y;
    trans.b.y = y.y;
    trans.c.y = z.y;

    trans.a.z = x.z;
    trans.b.z = y.z;
    trans.c.z = z.z;

    Vec pos = matrixVecProduct(trans, illumPos);
    GroundPt3D spherical = rectToSpherical(pos);

    double longitude360 = spherical.lon * RAD2DEG;

    if (longitude360 != 360.0) {
      longitude360 -= 360 * floor(longitude360 / 360);
    }
    return longitude360;
  }
}
