/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "CsmSensor.h"

#include "RasterGM.h"

#include "Orientations.h"

#include "csm.h"

using namespace std;

namespace Isis {
  CsmSensor::CsmSensor(csm::RasterGM* cam, ale::Orientations *j2000Rot) {
    m_cam = cam;
    m_j2000Rot = j2000Rot;
  }


  SensorUtilities::ObserverState CsmSensor::getState(const SensorUtilities::ImagePt &imagePoint) {

    csm::ImageCoord csmImagePt(imagePoint.line, imagePoint.sample);

    double sensorTime = m_cam->getImageTime(csmImagePt);

    csm::EcefLocus locus = m_cam->imageToRemoteImagingLocus(csmImagePt);

    SensorUtilities::Vec lookVec = {locus.direction.x, locus.direction.y, locus.direction.z};

    ale::Vec3d aleLookVec = {locus.direction.x, locus.direction.y, locus.direction.z};
    ale::Vec3d aleJ2000LookVec = m_j2000Rot->rotateVectorAt(sensorTime, aleLookVec);
    SensorUtilities::Vec lookVecJ2000 = {aleJ2000LookVec.x, aleJ2000LookVec.y, aleJ2000LookVec.z};

    SensorUtilities::Vec sensorPos = {locus.point.x, locus.point.y, locus.point.z};

    SensorUtilities::ObserverState sensorState = {
          lookVec,
          lookVecJ2000,
          sensorPos,
          sensorTime,
          imagePoint};

    return sensorState;
  }


  SensorUtilities::ObserverState CsmSensor::getState(const SensorUtilities::GroundPt3D &groundPt) {
    SensorUtilities::Vec groundCoord = SensorUtilities::sphericalToRect(groundPt);
    csm::EcefCoord csmGroundPt = {groundCoord.x, groundCoord.y, groundCoord.z};
    csm::ImageCoord csmImagePt = m_cam->groundToImage(csmGroundPt);
    SensorUtilities::ImagePt imagePoint = {csmImagePt.line, csmImagePt.samp};
    return getState(imagePoint);
  }

}
