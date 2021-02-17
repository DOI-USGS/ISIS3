/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CSMCamera.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include <QDebug>
#include <QList>
#include <QPointF>
#include <QString>

#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Constants.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "Latitude.h"
#include "Longitude.h"
#include "LinearAlgebra.h"
#include "NaifStatus.h"
#include "SpecialPixel.h"
#include "StringBlob.h"

#include "csm/Warning.h"
#include "csm/Error.h"
#include "csm/Plugin.h"
#include "csm/Ellipsoid.h"
#include "csm/SettableEllipsoid.h"

using namespace std;

namespace Isis {

  /**
   * Constructor for an ISIS Camera model that uses a Community Sensor Model (CSM)
   * for the principal transformations.
   *
   * @param cube The Cube containing image data and CSM Model information for the
   *             ISIS Camera Model.
   */
  CSMCamera::CSMCamera(Cube &cube) : Camera(cube) {
    StringBlob state("","CSMState");
    cube.read(state);
    PvlObject &blobLabel = state.Label();
    QString pluginName = blobLabel.findKeyword("PluginName")[0];
    QString modelName = blobLabel.findKeyword("ModelName")[0];
    init(cube, pluginName, modelName, QString::fromStdString(state.string()));
  }


  /**
   * Init method which performs most of the setup for the CSM Camera Model inside ISIS.
   *
   * @param cube The cube with the image data
   * @param pluginName The name of the CSM::Plugin that will create the CSM::Model
   * @param modelName The name of the CSM::Model that will be created
   * @param stateString The state string the the CSM::Model will be created from
   */
  void CSMCamera::init(Cube &cube, QString pluginName, QString modelName, QString stateString){
    const csm::Plugin *plugin = csm::Plugin::findPlugin(pluginName.toStdString());
    if (!plugin) {
      QStringList availablePlugins;
      for (const csm::Plugin *plugin: csm::Plugin::getList()) {
        availablePlugins.append(QString::fromStdString(plugin->getPluginName()));
      }
      QString msg = "Failed to find plugin [" + pluginName + "] for image [" + cube.fileName() +
                    "]. Check that the corresponding CSM plugin library is in the directory "
                    "specified by your IsisPreferences. Loaded plugins [" +
                    availablePlugins.join(", ") + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    if (!plugin->canModelBeConstructedFromState(modelName.toStdString(), stateString.toStdString())) {
      QString msg = "CSM state string attached to image [" + cube.fileName() + "]. cannot "
                    "be converted to a [" + modelName + "] using [" + pluginName + "].";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    m_model = dynamic_cast<csm::RasterGM*>(plugin->constructModelFromState(stateString.toStdString()));
    // If the dynamic cast failed, raise an exception
    if (!m_model) {
      QString msg = "Failed to convert CSM Model to RasterGM.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_instrumentNameLong = QString::fromStdString(m_model->getSensorIdentifier());
    m_instrumentNameShort = QString::fromStdString(m_model->getSensorIdentifier());
    m_spacecraftNameLong = QString::fromStdString(m_model->getPlatformIdentifier());
    m_spacecraftNameShort = QString::fromStdString(m_model->getPlatformIdentifier());

    QString timeString = QString::fromStdString(m_model->getReferenceDateAndTime());
    // Strip the UTC time zone indicator for iTime
    timeString.remove("Z");
    m_refTime.setUtc(timeString);

    setTarget(*cube.label());
  }


  /**
   * Set the image sample and line for the Camera Model and then compute the
   * corresponding image time, look vector, and ground point.
   *
   * @param sample The image sample coordinate
   * @param line The image line coordinate
   *
   * @returns @b bool If the image coordinate was set successfully
   */
  bool CSMCamera::SetImage(const double sample, const double line) {
    csm::ImageCoord imagePt;
    isisToCsmPixel(p_alphaCube->AlphaLine(line), p_alphaCube->AlphaSample(sample), imagePt);
    double achievedPrecision = 0;
    csm::WarningList warnings;

    csm::EcefLocus imageLocus;
    try {
      imageLocus = m_model->imageToRemoteImagingLocus(imagePt,
                                                      0.001,
                                                      &achievedPrecision,
                                                      &warnings);
    }
    catch (csm::Error &e) {
      return false;
    }

    // Check for issues on the CSM end
    if (achievedPrecision > 0.001) {
      return false;
    }
    if (!warnings.empty()) {
      for (csm::Warning warning : warnings) {
        if (warning.getWarning() == csm::Warning::IMAGE_COORD_OUT_OF_BOUNDS){
          return false;
        }
      }
    }

    // ISIS sensors work in Kilometers, CSM works in meters
    std::vector<double> obsPosition = {imageLocus.point.x / 1000.0,
                                       imageLocus.point.y / 1000.0,
                                       imageLocus.point.z / 1000.0};
    std::vector<double> locusVec = {imageLocus.direction.x,
                                    imageLocus.direction.y,
                                    imageLocus.direction.z};
    if(!target()->shape()->intersectSurface(obsPosition, locusVec)) {
      return false;
    }

    // If we are here then everything went well so save the pixel and return true
    m_lookB[0] = locusVec[0];
    m_lookB[1] = locusVec[1];
    m_lookB[2] = locusVec[2];
    m_newLookB = true;
    p_pointComputed = true;
    p_childSample = sample;
    p_childLine = line;
    if (!m_et) {
      m_et = new iTime();
    }
    *m_et = m_refTime + m_model->getImageTime(imagePt);
    return true;
  }


  /**
   * Set the latitude and longitude for the Camera Model and then compute the
   * corresponding image time, look vector, and image coordinate. The ground
   * point radius will be computed from the shape model.
   *
   * @param latitude The ground point latitude in degrees
   * @param longitude The ground point longitude in positive East, 360 domain degrees
   *
   * @returns @b bool If the ground point was set successfully
   */
  bool CSMCamera::SetUniversalGround(const double latitude, const double longitude) {
    return SetGround(
        Latitude(latitude, Angle::Degrees),
        Longitude(longitude, Angle::Degrees));
  }


/**
   * Set the latitude, longitude, and radius for the Camera Model and then compute the
   * corresponding image time, look vector, and image coordinate.
   *
   * @param latitude The ground point latitude in degrees
   * @param longitude The ground point longitude in positive East, 360 domain degrees
   * @param radius The ground point radius in meters
   *
   * @returns @b bool If the ground point was set successfully
   */
  bool CSMCamera::SetUniversalGround(const double latitude, const double longitude, double radius) {
    return SetGround(SurfacePoint(
        Latitude(latitude, Angle::Degrees),
        Longitude(longitude, Angle::Degrees),
        Distance(radius, Distance::Meters)));
  }


  /**
   * Set the latitude and longitude for the Camera Model and then compute the
   * corresponding image time, look vector, and image coordinate. The ground
   * point radius will be computed from the shape model.
   *
   * @param latitude The ground point latitude
   * @param longitude The ground point longitude
   *
   * @returns @b bool If the ground point was set successfully
   */
  bool CSMCamera::SetGround(Latitude latitude, Longitude longitude) {
    ShapeModel *shape = target()->shape();
    Distance localRadius;

    if (shape->name() != "Plane") { // this is the normal behavior
      localRadius = LocalRadius(latitude, longitude);
    }
    else {
      localRadius = Distance(latitude.degrees(),Distance::Kilometers);
      latitude = Latitude(0.,Angle::Degrees);
    }

      if (!localRadius.isValid()) {
      target()->shape()->clearSurfacePoint();
      return false;
    }

    return SetGround(SurfacePoint(latitude, longitude, localRadius));
  }


  /**
   * Set the ground point for the Camera Model and then compute the
   * corresponding image time, look vector, and image coordinate.
   *
   * @param surfacePt The ground point
   *
   * @returns @b bool If the ground point was set successfully
   */
  bool CSMCamera::SetGround(const SurfacePoint & surfacePt) {
    ShapeModel *shape = target()->shape();
    if (!surfacePt.Valid()) {
      shape->clearSurfacePoint();
      return false;
    }

    bool validBackProject = true;

    // Back project through the CSM model
    csm::ImageCoord imagePt;
    double achievedPrecision = 0;
    csm::WarningList warnings;
    csm::EcefCoord groundPt = isisToCsmGround(surfacePt);
    try {
      imagePt = m_model->groundToImage(groundPt, 0.01, &achievedPrecision, &warnings);
    }
    catch (csm::Error &e) {
      validBackProject = false;
    }
    if (achievedPrecision > 0.01) {
      validBackProject = false;
    }
    if (!warnings.empty()) {
      for (csm::Warning warning : warnings) {
        if (warning.getWarning() == csm::Warning::IMAGE_COORD_OUT_OF_BOUNDS){
          validBackProject = false;
        }
      }
    }

    // Check for occlusion
    double line, sample;
    csmToIsisPixel(imagePt, line, sample);
    csm::EcefLocus imageLocus = m_model->imageToRemoteImagingLocus(imagePt);
    std::vector<double> sensorPosition = {imageLocus.point.x, imageLocus.point.y, imageLocus.point.z};
    shape->clearSurfacePoint();
    shape->intersectSurface(surfacePt,
                            sensorPosition,
                            true);
    if (!shape->hasIntersection()) {
      validBackProject = false;
    }

    // If the back projection was successful, then save it
    if (validBackProject) {
      m_lookB[0] = imageLocus.direction.x;
      m_lookB[1] = imageLocus.direction.y;
      m_lookB[2] = imageLocus.direction.z;
      m_newLookB = true;
      p_childSample = p_alphaCube->BetaSample(sample);
      p_childLine = p_alphaCube->BetaLine(line);
      p_pointComputed = true;
      shape->setHasIntersection(true);
      if (!m_et) {
        m_et = new iTime();
      }
      *m_et = m_refTime + m_model->getImageTime(imagePt);
      return true;
    }

    // Otherwise reset
    shape->clearSurfacePoint();
    return false;
  }


  /**
   * Compute the line resolution in meters per pixel for the current set point.
   *
   * CSM sensor models do not expose all of the necessary parameters to do the
   * same calculation as ISIS sensor models, so this uses a more time consuming but
   * more accurate method and thus is equivalent to the oblique line resolution.
   *
   * For time dependent sensor models, this may also be the line-to-line resolution
   * and not the resolution within a line or framelet. This is determined by the
   * CSM model's ground computeGroundPartials method.
   *
   * @returns @b double The line resolution in meters per pixel
   */
  double CSMCamera::LineResolution() {
    vector<double> imagePartials = ImagePartials();
    return sqrt(imagePartials[0]*imagePartials[0] +
                imagePartials[2]*imagePartials[2] +
                imagePartials[4]*imagePartials[4]);
  }


  /**
   * Compute the sample resolution in meters per pixel for the current set point.
   *
   * CSM sensor models do not expose all of the necessary parameters to do the
   * same calculation as ISIS sensor models, so this uses a more time consuming but
   * more accurate method and thus is equivalent to the oblique sample resolution.
   *
   * @returns @b double The sample resolution in meters per pixel
   */
  double CSMCamera::SampleResolution() {
    vector<double> imagePartials = ImagePartials();
    return sqrt(imagePartials[1]*imagePartials[1] +
                imagePartials[3]*imagePartials[3] +
                imagePartials[5]*imagePartials[5]);
  }


  /**
   * Compute the detector resolution in meters per pixel for the current set point.
   *
   * CSM sensor models do not expose all of the necessary parameters to do the
   * same calculation as ISIS sensor models, so this uses a more time consuming but
   * more accurate method and thus is equivalent to the oblique detector resolution.
   *
   * @returns @b double The detector resolution in meters per pixel
   */
  double CSMCamera::DetectorResolution() {
    // Redo the line and sample resolution calculations because it avoids
    // a call to ImagePartials which could be a costly call
    vector<double> imagePartials = ImagePartials();
    double lineRes =  sqrt(imagePartials[0]*imagePartials[0] +
                           imagePartials[2]*imagePartials[2] +
                           imagePartials[4]*imagePartials[4]);
    double sampRes =  sqrt(imagePartials[1]*imagePartials[1] +
                           imagePartials[3]*imagePartials[3] +
                           imagePartials[5]*imagePartials[5]);
    return (sampRes + lineRes) / 2.0;
  }


  /**
   * Compute the oblique line resolution in meters per pixel for the current set point.
   *
   * CSM sensor models do not expose all of the necessary parameters to do the
   * same calculation as ISIS sensor models, so obliqueness does not need to be
   * accounted for. Thus, this is equivalent to the line resolution.
   *
   * @returns @b double The oblique line resolution in meters per pixel
   */
  double CSMCamera::ObliqueLineResolution() {
    // CSM resolution is always the oblique resolution so just return it
    return LineResolution();
  }


  /**
   * Compute the oblique sample resolution in meters per pixel for the current set point.
   *
   * CSM sensor models do not expose all of the necessary parameters to do the
   * same calculation as ISIS sensor models, so obliqueness does not need to be
   * accounted for. Thus, this is equivalent to the sample resolution.
   *
   * @returns @b double The oblique sample resolution in meters per pixel
   */
  double CSMCamera::ObliqueSampleResolution() {
    // CSM resolution is always the oblique resolution so just return it
    return SampleResolution();
  }


  /**
   * Compute the oblique detector resolution in meters per pixel for the current set point.
   *
   * CSM sensor models do not expose all of the necessary parameters to do the
   * same calculation as ISIS sensor models, so obliqueness does not need to be
   * accounted for. Thus, this is equivalent to the detector resolution.
   *
   * @returns @b double The oblique detector resolution in meters per pixel
   */
  double CSMCamera::ObliqueDetectorResolution() {
    // CSM resolution is always the oblique resolution so just return it
    return DetectorResolution();
  }


  /**
   * Returns the currently set parent line for the camera model.
   * This is the line from the original image before any cropping, scaling, or
   * other transformations.
   *
   * @returns @b double The currently set line
   */
  double CSMCamera::parentLine() const {
    return p_alphaCube->AlphaLine(Line());
  }


  /**
   * Returns the currently set parent sample for the camera model.
   * This is the sample from the original image before any cropping, scaling, or
   * other transformations.
   *
   * @returns @b double The currently set sample
   */
  double CSMCamera::parentSample() const {
    return p_alphaCube->AlphaSample(Sample());
  }


  /**
   * Get the position of the sensor in the body fixed coordinate system at the
   * currently set time.
   *
   * @param[out] p A double array that will be filled with the (X, Y, Z)
   *               position in kilometers.
   */
  void CSMCamera::instrumentBodyFixedPosition(double p[3]) const {
    std::vector<double> position = sensorPositionBodyFixed();
    p[0] = position[0];
    p[1] = position[1];
    p[2] = position[2];
  }


  /**
   * Get the position of the sensor in the body fixed coordinate system at the
   * currently set time.
   *
   * @returns @b std::vector<double> The (X, Y, Z) position in kilometers.
   */
  std::vector<double> CSMCamera::sensorPositionBodyFixed() const {
    return sensorPositionBodyFixed(parentLine(), parentSample());
  }


  /**
   * Get the position of the sensor in the body fixed coordinate system at an
   * image coordinate
   *
   * @param line The line of the image coordinate
   * @param sample the sample of the image coordinate
   *
   * @returns @b std::vector<double> The (X, Y, Z) position in kilometers.
   */
  std::vector<double> CSMCamera::sensorPositionBodyFixed(double line, double sample) const {
    csm::ImageCoord imagePt;
    isisToCsmPixel(line, sample, imagePt);
    csm::EcefCoord sensorPosition =  m_model->getSensorPosition(imagePt);
    // CSM uses meters, but ISIS wants this in Km
    std::vector<double> result {
        sensorPosition.x / 1000.0,
        sensorPosition.y / 1000.0,
        sensorPosition.z / 1000.0};
    return result;
  }


  /**
   * Get the latitude and longitude of the sub-spacecraft point at the currently
   * set time.
   *
   * @param[out] lat Will be filled with the latitude in degrees
   * @param[out] lon Will be filled with the longitude in positive East,
   *                 360 domain degrees
   */
  void CSMCamera::subSpacecraftPoint(double &lat, double &lon) {
    subSpacecraftPoint(lat, lon, parentLine(), parentSample());
  }


  /**
   * Get the latitude and longitude of the sub-spacecraft point at the an image
   * coordinate.
   *
   * @param[out] lat Will be filled with the latitude in degrees
   * @param[out] lon Will be filled with the longitude in positive East,
   *                 360 domain degrees
   * @param line The line of the image coordinate
   * @param sample the sample of the image coordinate
   */
  void CSMCamera::subSpacecraftPoint(double &lat, double &lon, double line, double sample) {
    // Get s/c position from CSM because it is vector from center of body to that
    vector<double> sensorPosition = sensorPositionBodyFixed(line, sample);
    SurfacePoint surfacePoint(
        Displacement(sensorPosition[0], Displacement::Kilometers),
        Displacement(sensorPosition[1], Displacement::Kilometers),
        Displacement(sensorPosition[2], Displacement::Kilometers));
    lat = surfacePoint.GetLatitude().degrees();
    lon = surfacePoint.GetLongitude().degrees();
  }


  /**
  * Compute the partial derivatives of the ground point with respect to
  * the line and sample at the current ground point.
  *
  * The resultant partials are
  * x WRT line
  * x WRT sample
  * y WRT line
  * y WRT sample
  * z WRT line
  * z WRT sample
  *
  * @return @b std::vector<double> The partial derivatives of the image to ground
  *                                transformation
  */
  vector<double> CSMCamera::ImagePartials() {
    return ImagePartials(GetSurfacePoint());
  }


  /**
  * Compute the partial derivatives of the ground point with respect to
  * the line and sample at a ground point.
  *
  * The resultant partials are
  * x WRT line
  * x WRT sample
  * y WRT line
  * y WRT sample
  * z WRT line
  * z WRT sample
  *
  * These are not normally available from the CSM model, so we use
  * csm::RasterGM::computeGroundPartials to get the Jacobian of the ground to
  * image transformation. Then we use the pseudoinverse of that to get the
  * Jacobian of the image to ground transformation.
  *
  * @param groundPoint The ground point to compute the partials at
  *
  * @return @b std::vector<double> The partial derivatives of the image to ground
  *                                transformation
  */
  vector<double> CSMCamera::ImagePartials(SurfacePoint groundPoint) {
    csm::EcefCoord groundCoord = isisToCsmGround(groundPoint);
    vector<double> groundPartials = m_model->computeGroundPartials(groundCoord);

    // Jacobian format is
    // line WRT X  line WRT Y  line WRT Z
    // samp WRT X  samp WRT Y  samp WRT Z
    LinearAlgebra::Matrix groundMatrix(2, 3);
    groundMatrix(0,0) = groundPartials[0];
    groundMatrix(0,1) = groundPartials[1];
    groundMatrix(0,2) = groundPartials[2];
    groundMatrix(1,0) = groundPartials[3];
    groundMatrix(1,1) = groundPartials[4];
    groundMatrix(1,2) = groundPartials[5];

    LinearAlgebra::Matrix imageMatrix = LinearAlgebra::pseudoinverse(groundMatrix);

    vector<double> imagePartials = {imageMatrix(0,0),
                                    imageMatrix(0,1),
                                    imageMatrix(1,0),
                                    imageMatrix(1,1),
                                    imageMatrix(2,0),
                                    imageMatrix(2,1)};
    return imagePartials;
  }


  /**
   * Set the Target object for the camera model.
   *
   * @param label The label containing information to create the Target from
   */
  void CSMCamera::setTarget(Pvl label) {
    Target *target = new Target(label);

    // get radii from CSM
    csm::Ellipsoid targetEllipsoid = csm::SettableEllipsoid::getEllipsoid(m_model);
    std::vector<Distance> radii  = {Distance(targetEllipsoid.getSemiMajorRadius(), Distance::Meters),
                                    Distance(targetEllipsoid.getSemiMajorRadius(), Distance::Meters),
                                    Distance(targetEllipsoid.getSemiMinorRadius(), Distance::Meters)};
    target->setRadii(radii);

    // Target needs to be able to access the camera to do things like
    // compute resolution
    target->setSpice(this);

    if (m_target) {
      delete m_target;
      m_target = nullptr;
    }

    m_target = target;
  }


  /**
   * Convert an ISIS pixel coordinate to a CSM pixel coordinate.
   * The ISIS image origin is (0.5, 0.5), the CSM image origin is (0, 0). This
   * function accounts for that and wraps the coordinate in a csm::ImageCoord.
   *
   * @param line The ISIS line of the image coordinate
   * @param sample The ISIS sample of the image coordinate
   * @param[out] csmPixel The CSM image coordinate
   */
  void CSMCamera::isisToCsmPixel(double line, double sample, csm::ImageCoord &csmPixel) const {
    csmPixel.line = line - 0.5;
    csmPixel.samp = sample - 0.5;
  }


  /**
   * Convert a CSM pixel coordinate to an ISIS pixel coordinate.
   * The ISIS image origin is (0.5, 0.5), the CSM image origin is (0, 0). This
   * function accounts for that and unpacks the csm::ImageCoord.
   *
   * @param csmPixel The CSM image coordinate
   * @param[out] line The ISIS line of the image coordinate
   * @param[out] sample The ISIS sample of the image coordinate
   */
  void CSMCamera::csmToIsisPixel(csm::ImageCoord csmPixel, double &line, double &sample) const {
    line = csmPixel.line + 0.5;
    sample = csmPixel.samp + 0.5;
  }


  /**
   * Convert an ISIS ground point into a CSM ground point.
   * ISIS ground points can be created from and converted to many different
   * units and coordinate systems. CSM ground points are always rectangular,
   * body-fixed coordinates in meters.
   *
   * @param groundPt The ISIS ground coordinate
   *
   * @returns @b csm::EcefCoord the CSM ground coordinate in meters
   */
  csm::EcefCoord CSMCamera::isisToCsmGround(const SurfacePoint &groundPt) const {
    return csm::EcefCoord(groundPt.GetX().meters(),
                          groundPt.GetY().meters(),
                          groundPt.GetZ().meters());
  }


  /**
   * Convert a CSM ground point into an ISIS ground point.
   * ISIS ground points can be created from and converted to many different
   * units and coordinate systems. CSM ground points are always rectangular,
   * body-fixed coordinates in meters.
   *
   * @param groundPt The CSM ground coordinate in meters
   *
   * @returns @b SurfacePointthe ISIS ground coordinate
   */
  SurfacePoint CSMCamera::csmToIsisGround(const csm::EcefCoord &groundPt) const {
    return SurfacePoint(Displacement(groundPt.x, Displacement::Meters),
                        Displacement(groundPt.y, Displacement::Meters),
                        Displacement(groundPt.z, Displacement::Meters));
  }


  /**
   * Compute the phase angle at the currently set ground point.
   *
   * @returns @b double The phase angle in degrees
   */
  double CSMCamera::PhaseAngle() const {
    csm::EcefCoord groundPt = isisToCsmGround(GetSurfacePoint());
    csm::EcefVector sunEcefVec = m_model->getIlluminationDirection(groundPt);
    // ISIS wants the position of the sun, not just the vector from the ground
    // point to the sun. So, we approximate this by adding in the ground point.
    // ISIS wants this in Km so convert
    std::vector<double> sunVec = {
        (groundPt.x - sunEcefVec.x) / 1000.0,
        (groundPt.y - sunEcefVec.y) / 1000.0,
        (groundPt.z - sunEcefVec.z) / 1000.0};
    return target()->shape()->phaseAngle(sensorPositionBodyFixed(), sunVec);
  }


  /**
   * Compute the emission angle at the currently set ground point.
   *
   * @returns @b double The emission angle in degrees
   */
  double CSMCamera::EmissionAngle() const {
    return target()->shape()->emissionAngle(sensorPositionBodyFixed());
  }


  /**
   * Compute the incidence angle at the currently set ground point.
   *
   * @returns @b double The incidence angle in degrees
   */
  double CSMCamera::IncidenceAngle() const {
    csm::EcefCoord groundPt = isisToCsmGround(GetSurfacePoint());
    csm::EcefVector sunEcefVec = m_model->getIlluminationDirection(groundPt);
    // ISIS wants the position of the sun, not just the vector from the ground
    // point to the sun. So, we approximate this by adding in the ground point.
    // ISIS wants this in Km so convert
    std::vector<double> sunVec = {
        (groundPt.x - sunEcefVec.x) / 1000.0,
        (groundPt.y - sunEcefVec.y) / 1000.0,
        (groundPt.z - sunEcefVec.z) / 1000.0};
    return target()->shape()->incidenceAngle(sunVec);
  }


  /**
   * Compute the slant distance form the sensor to the ground point at the
   * currently set time.
   *
   * @returns @b double The distance from the sensor to the ground point in kilometers
   */
  double CSMCamera::SlantDistance() const {
    std::vector<double> sensorPosition = sensorPositionBodyFixed();
    SurfacePoint groundPoint = GetSurfacePoint();

    std::vector<double> sensorToGround = {
        groundPoint.GetX().kilometers() - (sensorPosition[0]),
        groundPoint.GetY().kilometers() - (sensorPosition[1]),
        groundPoint.GetZ().kilometers() - (sensorPosition[2])};

    return sqrt(
        sensorToGround[0] * sensorToGround[0] +
        sensorToGround[1] * sensorToGround[1] +
        sensorToGround[2] * sensorToGround[2]);
  }


  /**
   * Calculates and returns the distance from the spacecraft to the target center at the
   * currently set time.
   *
   * @returns @b double Distance to the center of the target from the spacecraft in kilometers.
   */
  double CSMCamera::targetCenterDistance() const {
    std::vector<double> sensorPosition = sensorPositionBodyFixed();
    return sqrt(
        sensorPosition[0] * sensorPosition[0] +
        sensorPosition[1] * sensorPosition[1] +
        sensorPosition[2] * sensorPosition[2]);
  }


  /**
   * Set the time and update the sensor position and orientation.
   *
   * This is not supported for CSM cameras because the time is a function of the
   * image coordinate and the two cannot be changed independently.
   *
   * @param time The time to set
   */
  void CSMCamera::setTime(const iTime &time) {
    QString msg = "Setting the image time is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Returns the sub-solar latitude/longitude in universal coordinates (0-360
   * positive east, ocentric).
   *
   * This is not supported for CSM sensors because we cannot get the position
   * of the sun, only the illumination direction.
   *
   * @param lat Sub-solar latitude
   * @param lon Sub-solar longitude
   */
  void CSMCamera::subSolarPoint(double &lat, double &lon) {
    QString msg = "Sub solar point is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Returns the pixel ifov offsets from center of pixel. The first vertex is the top left.
   *
   * The CSM API does not support this type of internal information about the sensor.
   *
   * @returns @b QList<QPointF> The field of view offsets
   */
  QList<QPointF> CSMCamera::PixelIfovOffsets() {
    QString msg = "Pixel Field of View is not supported for CSM camera models";
    throw IException(IException::User, msg, _FILEINFO_);
  }


  /**
   * Get the body fixed position of the sun in kilometers.
   *
   * This is not supported for CSM sensors because we cannot get the position
   * of the sun, only the illumination direction.
   *
   * @param[out] p The position of the sun
   */
  void CSMCamera::sunPosition(double p[3]) const {
    QString msg = "Sun position is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Get the SpicePosition object that contains the state information for the sun in J2000.
   *
   * This is not supported for CSM sensors because we cannot get the position
   * of the sun, only the illumination direction.
   *
   * @returns @b SpicePosition* A pointer to the SpicePosition object for the Sun
   */
  SpicePosition *CSMCamera::sunPosition() const {
    QString msg = "Sun position is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Get the SpicePosition object the contains the state information for the sensor in J2000.
   *
   * This is not supported for CSM sensors because we can only query the sensor position
   * and velocity at specific image coordinates or times. We cannot access the internal
   * representation inside of the CSM model, if it even exists.
   *
   * @returns @b SpicePosition* A pointer to the SpicePosition object for the sensor
   */
  SpicePosition *CSMCamera::instrumentPosition() const {
    QString msg = "Instrument position is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Get the SpiceRotation object the contains the orientation of the target body
   * relative to J2000.
   *
   * This is not supported for CSM sensors because the CSM API only supports the
   * body fixed coordinate system and does not provide rotations to any others.
   *
   * @returns @b SpiceRotation* A pointer to the SpiceRotation object for the body orientation
   */
  SpiceRotation *CSMCamera::bodyRotation() const {
    QString msg = "Target body orientation is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Get the SpiceRotation object the contains the orientation of the sensor
   * relative to J2000.
   *
   * This is not supported for CSM sensors because the CSM API only supports the
   * body fixed coordinate system and does not provide rotations to any others.
   *
   * @returns @b SpiceRotation* A pointer to the SpiceRotation object for the sensor orientation
   */
  SpiceRotation *CSMCamera::instrumentRotation() const {
    QString msg = "Instrument orientation is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Computes the solar longitude for the given ephemeris time.  If the target
   * is sky, the longitude is set to -999.0.
   *
   * This is not supported for CSM models because we cannot get the sun position.
   *
   * @param et Ephemeris time
   */
  void CSMCamera::computeSolarLongitude(iTime et) {
    QString msg = "Solar longitude is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Computes the distance to the sun from the currently set ground point
   *
   * This is not supported for CSM models because we cannot get the sun position.
   *
   * @returns @b double The distance to the sun
   */
  double CSMCamera::SolarDistance() const {
    QString msg = "Solar distance is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Computes the Right Ascension of the currently set image coordinate.
   *
   * This is not supported for CSM sensors because the CSM API only supports the
   * body fixed coordinate system and does not provide rotations to any others.
   *
   * @returns @b double The Right Ascension
   */
  double CSMCamera::RightAscension() {
    QString msg = "Right Ascension is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Computes the Declination of the currently set image coordinate.
   *
   * This is not supported for CSM sensors because the CSM API only supports the
   * body fixed coordinate system and does not provide rotations to any others.
   *
   * @returns @b double The Declination
   */
  double CSMCamera::Declination() {
    QString msg = "Declination is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
}
