/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IsisSensor.h"

#include "Angle.h"
#include "Camera.h"
#include "Distance.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SurfacePoint.h"

using namespace std;

namespace Isis {
  /**
   * Create an IsisSensor wrapping an ISIS Camera object.
   */
  IsisSensor::IsisSensor(Camera* cam) {
    m_cam = cam;
  }


  /**
   * Get the sensor state at an image coordinate.
   * If the image coordinate matches the image coordinate that the ISIS Camera
   * is already set to, then it will not compute a new intersection.
   * Note that SensorUtilities::ImagePt are 0-based and ISIS image coordiantes
   * are 0.5-based. The input is expected to use the SensorUtilities convention
   * to conform to the interface. This function handles the conversion to and
   * from ISIS image coordinates.
   */
  SensorUtilities::ObserverState IsisSensor::getState(const SensorUtilities::ImagePt &imagePoint) {

    // These image coordinates are in ISIS pixels; (0.5, 0.5, 1) is the origin.
    double oldLine =  m_cam->Line();
    double oldSample = m_cam->Sample();
    int oldBand = m_cam->Band();
    double newLine = imagePoint.line + 0.5;
    double newSample = imagePoint.sample + 0.5;
    int newBand = imagePoint.band + 1;

    bool imagePtChanged = oldLine != newLine ||
                          oldSample != newSample ||
                          (!m_cam->IsBandIndependent() && oldBand != newBand);
    if (imagePtChanged) {
      m_cam->SetBand(newBand);
      m_cam->SetImage(newSample, newLine);
    }

    vector<double> lookBF = m_cam->lookDirectionBodyFixed();
    SensorUtilities::Vec lookVec = {lookBF[0], lookBF[1], lookBF[2]};

    vector<double> lookJ2000 = m_cam->lookDirectionJ2000();
    SensorUtilities::Vec lookVecJ2000 = {lookJ2000[0], lookJ2000[1], lookJ2000[2]};

    vector<double> posBF(3);
    m_cam->instrumentBodyFixedPosition(&posBF[0]);
    // Conver to meters from ISIS's Km
    SensorUtilities::Vec sensorPos = {1000 * posBF[0], 1000 * posBF[1], 1000 * posBF[2]};

    double sensorTime = m_cam->time().Et();

    SensorUtilities::ObserverState sensorState = {
          lookVec,
          lookVecJ2000,
          sensorPos,
          sensorTime,
          imagePoint};

    if (imagePtChanged) {
      m_cam->SetBand(oldBand);
      m_cam->SetImage(oldSample, oldLine);
    }
    return sensorState;
  }


  /**
   * Get the sensor state as it observes a ground point.
   * The ground points is mapped back onto the surface model used by the ISIS
   * Camera prior to back projecting it into the image. So, it is possible this
   * will not perfectly invert with getState(ImagePt) depending on how what
   * surface model you then intersect it with.
   */
  SensorUtilities::ObserverState IsisSensor::getState(const SensorUtilities::GroundPt3D &groundPt) {
    SurfacePoint oldGroundPt = m_cam->GetSurfacePoint();
    SurfacePoint newGroundPt = SurfacePoint(
          Latitude(groundPt.lat, Angle::Radians),
          Longitude(groundPt.lon, Angle::Radians),
          Distance(groundPt.radius, Distance::Meters));
    // For consistency with ISIS, reset with the image point
    double oldLine =  m_cam->Line();
    double oldSample = m_cam->Sample();
    double oldBand = m_cam->Band();

    bool groundPtChanged = !(oldGroundPt == newGroundPt);

    if (groundPtChanged) {
      m_cam->SetGround(newGroundPt);
    }

    vector<double> lookBF = m_cam->lookDirectionBodyFixed();
    SensorUtilities::Vec lookVec = {lookBF[0], lookBF[1], lookBF[2]};

    vector<double> lookJ2000 = m_cam->lookDirectionJ2000();
    SensorUtilities::Vec lookVecJ2000 = {lookJ2000[0], lookJ2000[1], lookJ2000[2]};

    vector<double> posBF(3);
    m_cam->instrumentBodyFixedPosition(&posBF[0]);
    // Conver to meters from ISIS's Km
    SensorUtilities::Vec sensorPos = {1000 * posBF[0], 1000 * posBF[1], 1000 * posBF[2]};

    double sensorTime = m_cam->time().Et();

    SensorUtilities::ImagePt imagePoint = {
          m_cam->Line() - 0.5,
          m_cam->Sample() - 0.5,
          m_cam->Band() - 1};

    SensorUtilities::ObserverState sensorState = {
          lookVec,
          lookVecJ2000,
          sensorPos,
          sensorTime,
          imagePoint};

    if (groundPtChanged) {
      m_cam->SetBand(oldBand);
      m_cam->SetImage(oldSample, oldLine);
    }

    return sensorState;
  }

}
