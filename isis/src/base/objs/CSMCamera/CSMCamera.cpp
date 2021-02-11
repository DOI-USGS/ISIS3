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

//  /**
//   * Constructor for the USGS CSM Camera Model inside ISIS.
//   */
//  CSMCamera::CSMCamera(Cube &cube, QString pluginName, QString modelName, QString stateString) : Camera(cube) {
//    init(cube, pluginName, modelName, stateString);
//  }
//

  /**
   * Constructor for the USGS CSM Camera Model inside ISIS.
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
   * Init method which performs most of the setup for the USGS CSM
   * Camera Model inside ISIS. 
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

    // TODO: Find out why this is 12 hrs different than ths StartTime in the ISIS label.
    // std::cout << m_model->getReferenceDateAndTime() << std::endl;
    // We have to strip off any trailing Zs and then add separators in order for iTime to work
    // TODO make this work with more time string formats and move to iTime
    QString timeString = QString::fromStdString(m_model->getReferenceDateAndTime());
    timeString.remove("z", Qt::CaseInsensitive);
    timeString.insert(13, ":");
    timeString.insert(11, ":");
    timeString.insert(6, "-");
    timeString.insert(4, "-");

    m_refTime = iTime(timeString);

    setTarget(*cube.label());
  }


  // TODO make this an iterative technique that updates the locus each iteration
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
    // TODO do we need to apply the alpha cube?
    p_childSample = sample;
    p_childLine = line;
    if (!m_et) {
      m_et = new iTime();
    }
    *m_et = m_refTime + m_model->getImageTime(imagePt);
    return true;
  }


  bool CSMCamera::SetUniversalGround(const double latitude, const double longitude) {
    return SetGround(Latitude(latitude, Angle::Degrees), Longitude(longitude, Angle::Degrees));
  }


  bool CSMCamera::SetUniversalGround(const double latitude, const double longitude, double radius) {
    return SetGround(SurfacePoint(Latitude(latitude, Angle::Degrees), Longitude(longitude, Angle::Degrees), Distance(radius, Distance::Meters)));
  }


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
    // TODO should this be in meters or kilometers?
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
      // TODO is this the correct time to apply the alpha cube?
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


  double CSMCamera::LineResolution() {
    vector<double> imagePartials = ImagePartials();
    return sqrt(imagePartials[0]*imagePartials[0] +
                imagePartials[2]*imagePartials[2] +
                imagePartials[4]*imagePartials[4]);
  }


  double CSMCamera::SampleResolution() {
    vector<double> imagePartials = ImagePartials();
    return sqrt(imagePartials[1]*imagePartials[1] +
                imagePartials[3]*imagePartials[3] +
                imagePartials[5]*imagePartials[5]);
  }


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


  double CSMCamera::ObliqueLineResolution() {
    // CSM resolution is always the oblique resolution so just return it
    return LineResolution();
  }


  double CSMCamera::ObliqueSampleResolution() {
    // CSM resolution is always the oblique resolution so just return it
    return SampleResolution();
  }


  double CSMCamera::ObliqueDetectorResolution() {
    // CSM resolution is always the oblique resolution so just return it
    return DetectorResolution();
  }


  double CSMCamera::parentLine() const {
    return p_alphaCube->AlphaLine(Line());
  }


  double CSMCamera::parentSample() const {
    return p_alphaCube->AlphaSample(Sample());
  }


  void CSMCamera::instrumentBodyFixedPosition(double p[3]) const {
    std::vector<double> position = sensorPositionBodyFixed();
    p[0] = position[0];
    p[1] = position[1];
    p[2] = position[2];
  }


  // TODO change SPICE to use this or instrumentBodyFixedPosition instead of doing
  // The rotation inside of other functions.
  //
  // Broke sensorPosition call out to separate function in preparation for in the future separating the corresponding
  // call out of Spice::subSpacecraft to decrease repeated code and just override this function.
  std::vector<double> CSMCamera::sensorPositionBodyFixed() const {
    return sensorPositionBodyFixed(parentLine(), parentSample());
  }


  // stateless version
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


  // Override the subSpacecraftPoint function in Spice because it requires m_bodyRotation and m_instrumentPosition to
  // exist, and does a rotation from J2000 to body-fixed. Since CSM already operates in body-fixed coordinates
  // such a rotation is not necessary.
  void CSMCamera::subSpacecraftPoint(double &lat, double &lon) {
    subSpacecraftPoint(lat, lon, parentLine(), parentSample());
  }


  // stateless version
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
    // TODO find a better way to do this. It would better if we could set this up
    //      inside of the Target constructor so that it always has a Spice object.
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
   */
  void CSMCamera::isisToCsmPixel(double line, double sample, csm::ImageCoord &csmPixel) const {
    csmPixel.line = line - 0.5;
    csmPixel.samp = sample - 0.5;
  }


  /**
   * Convert a CSM pixel coordinate to an ISIS pixel coordinate.
   * The ISIS image origin is (0.5, 0.5), the CSM image origin is (0, 0). This
   * function accounts for that and unpacks the csm::ImageCoord.
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
   */
  SurfacePoint CSMCamera::csmToIsisGround(const csm::EcefCoord &groundPt) const {
    return SurfacePoint(Displacement(groundPt.x, Displacement::Meters),
                        Displacement(groundPt.y, Displacement::Meters),
                        Displacement(groundPt.z, Displacement::Meters));
  }


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


  double CSMCamera::EmissionAngle() const {
    return target()->shape()->emissionAngle(sensorPositionBodyFixed());
  }


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
   * Calculates and returns the distance from the spacecraft to the target center
   *
   * @return double Distance to the center of the target from the spacecraft
   */
  double CSMCamera::targetCenterDistance() const {
    std::vector<double> sensorPosition = sensorPositionBodyFixed();
    return sqrt(
        sensorPosition[0] * sensorPosition[0] +
        sensorPosition[1] * sensorPosition[1] +
        sensorPosition[2] * sensorPosition[2]);
  }


  void CSMCamera::setTime(const iTime &time) {
    QString msg = "Setting the image time is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Returns the sub-solar latitude/longitude in universal coordinates (0-360
   * positive east, ocentric)
   *
   * This is not supported for CSM sensors because we cannot get the position
   * of the sun, only the illumination direction
   *
   * @param lat Sub-solar latitude
   * @param lon Sub-solar longitude
   *
   * @see setTime()
   * @throw Isis::IException::Programmer - "You must call SetTime
   *             first."
   */
  void CSMCamera::subSolarPoint(double &lat, double &lon) {
    QString msg = "Sub solar point is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Returns the pixel ifov offsets from center of pixel.  For vims this will be a rectangle or
   * square, depending on the sampling mode.  The first vertex is the top left.
   *
   * The CSM API does not support this type of internal information about the sensor.
   */
  QList<QPointF> CSMCamera::PixelIfovOffsets() {
    QString msg = "Pixel Field of View is not supported for CSM camera models";
    throw IException(IException::User, msg, _FILEINFO_);
  }


  void CSMCamera::sunPosition(double p[3]) const {
    QString msg = "Sun position is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  SpicePosition *CSMCamera::sunPosition() const {
    QString msg = "Sun position is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  SpicePosition *CSMCamera::instrumentPosition() const {
    QString msg = "Instrument position is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  SpiceRotation *CSMCamera::bodyRotation() const {
    QString msg = "Target body orientation is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  SpiceRotation *CSMCamera::instrumentRotation() const {
    QString msg = "Instrument orientation is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Computes the solar longitude for the given ephemeris time.  If the target
   * is sky, the longitude is set to -999.0.
   *
   * This is not supported for CSM models because we cannot get the sun position
   *
   * @param et Ephemeris time
   */
  void CSMCamera::computeSolarLongitude(iTime et) {
    QString msg = "Solar longitude is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  double CSMCamera::SolarDistance() const {
    QString msg = "Solar distance is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  double CSMCamera::RightAscension() {
    QString msg = "Right Ascension is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  double CSMCamera::Declination() {
    QString msg = "Declination is not supported for CSM camera models";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
}

// Plugin
/**
 * This is the function that is called in order to instantiate a CSMCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* CSMCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Cassini namespace.
 */
extern "C" Isis::Camera *CSMCameraPlugin(Isis::Cube &cube) {
  return new Isis::CSMCamera(cube);
}
