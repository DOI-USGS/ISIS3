/**
 * New blurb TODO
 */

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
   * Constructor for the USGS CSM Camera Model inside ISIS.
   *
   */
  CSMCamera::CSMCamera(Cube &cube) : Camera(cube) {
    StringBlob stateString("","CSMState");
    cube.read(stateString);
    PvlObject &blobLabel = stateString.Label();
    QString pluginName = blobLabel.findKeyword("PluginName")[0];
    const csm::Plugin *plugin = csm::Plugin::findPlugin(pluginName.toStdString());
    m_model = dynamic_cast<csm::RasterGM*>(plugin->constructModelFromState(stateString.string()));
    // If the dynamic cast failed, raise an exception
    if (!m_model) {
      QString msg = "Failed to convert CSM Model to RasterGM.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_instrumentNameLong = QString::fromStdString(m_model->getSensorIdentifier());
    m_instrumentNameShort = QString::fromStdString(m_model->getSensorIdentifier());
    m_spacecraftNameLong = QString::fromStdString(m_model->getPlatformIdentifier());
    m_spacecraftNameShort = QString::fromStdString(m_model->getPlatformIdentifier());

    if (m_target) {
      delete m_target;
      m_target = nullptr;
    }

    m_target = setTarget(*cube.label());

    std::cout << "Target status" << std::endl;
    if (target()) {
      std::cout << "Target name: " << target()->name() << std::endl;
      std::vector<Distance> targetRad = target()->radii();
      std::cout << "Target radii: " << targetRad[0].meters() << ", " << targetRad[1].meters() << ", " << targetRad[2].meters() << std::endl;
      std::cout << "Shape status" << std::endl;
      if (target()->shape()) {
        std::cout << "Target name: " << target()->shape()->name() << std::endl;
        std::cout << "Target is DEM?: " << target()->shape()->name() << std::endl;
      }
      else {
        std::cout << "Shape Uninitialized" << std::endl;
      }
    }
    else {
      std::cout << "Target Uninitialized" << std::endl;
    }
  }


  bool CSMCamera::SetImage(const double sample, const double line) {

// move to protected?
    p_childSample = sample;
    p_childLine = line;
//    p_pointComputed = true;

//    if (p_projection == NULL || p_ignoreProjection) {
        double parentSample = p_alphaCube->AlphaSample(sample);
        double parentLine = p_alphaCube->AlphaLine(line);
//        bool success = false;

        double height = 10.0;
        csm::ImageCoord imagePt(parentLine, parentSample);

        // do image to ground with csm
        csm::EcefCoord result = m_model->imageToGround(imagePt, height);

        target()->shape()->setSurfacePoint(csmToIsisGround(result));

        // check set of coordinate:
        double test[3];
        Coordinate(test);
        // std::cout << "TEST:  " << test[0] << ", "  << test[1] << ", " << test[2] << std::endl;
        // std::cout << "UniversalLatitude: " << UniversalLatitude() << std::endl;

            // get a lat, lon and store in variables
        // (1) how to get lat/lon
        // (2) which variables to store in

        // fill in whatever stuff ISIS needs from this
//    }
//    else {
        // handle projected case
//    }

    // std::cout << "Hello World!" << std::endl;
    // how to do this -- csm returns the _closest pixel_ to the intersection
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
    shape->clearSurfacePoint();
    shape->intersectSurface(surfacePt,
                            sensorPositionBodyFixed(line, sample),
                            true);
    if (!shape->hasIntersection()) {
      validBackProject = false;
    }

    // If the back projection was successful, then save it
    if (validBackProject) {
      p_childSample = p_alphaCube->BetaSample(sample);
      p_childLine = p_alphaCube->BetaLine(line);
      p_pointComputed = true;
      shape->setHasIntersection(true);
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


  double CSMCamera::PixelResolution() {
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
    return LineResolution();
  }


  double CSMCamera::ObliqueSampleResolution() {
    return SampleResolution();
  }


  double CSMCamera::ObliquePixelResolution() {
    return PixelResolution();
  }


  double CSMCamera::parentLine() {
    return p_alphaCube->AlphaLine(Line());
  }


  double CSMCamera::parentSample() {
    return p_alphaCube->AlphaSample(Sample());
  }


  // Broke sensorPosition call out to separate function in preparation for in the future separating the corresponding
  // call out of Spice::subSpacecraft to decrease repeated code and just override this function.
  std::vector<double> CSMCamera::sensorPositionBodyFixed(){
    return sensorPositionBodyFixed(parentLine(), parentSample());
  }


  // stateless version
  std::vector<double> CSMCamera::sensorPositionBodyFixed(double line, double sample){
    csm::ImageCoord imagePt;
    isisToCsmPixel(line, sample, imagePt);
    csm::EcefCoord sensorPosition =  m_model->getSensorPosition(imagePt);
    std::vector<double> result {sensorPosition.x, sensorPosition.y, sensorPosition.z};
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
    SurfacePoint surfacePoint(Displacement(sensorPosition[0], Displacement::Meters), Displacement(sensorPosition[1], Displacement::Meters), Displacement(sensorPosition[2], Displacement::Meters)); // Be careful -- what are the units actually?
    lat = surfacePoint.GetLatitude().degrees();
    lon = surfacePoint.GetLongitude().degrees();
  }


  /**
   * Returns the pixel ifov offsets from center of pixel.  For vims this will be a rectangle or
   * square, depending on the sampling mode.  The first vertex is the top left.
   *
   * The CSM API does not support this type of internal information about the sensor.
   */
   QList<QPointF> CSMCamera::PixelIfovOffsets() {
     QString msg = "Pixel Field of View is not computable for a CSM camera model.";
     throw IException(IException::User, msg, _FILEINFO_);
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

  // CSMCamera(cube) -> Camera(cube) -> Sensor(cube) -> Spice(cube)
  // Spice::init() creates a Target in such a way that requires spice data / tables, so
  // this works around that.
  Target *CSMCamera::setTarget(Pvl label) {
    Target *target = new Target();
    PvlGroup &inst = label.findGroup("Instrument", Pvl::Traverse);
    QString targetName = inst["TargetName"][0];
    target->setName(targetName);

    // get radii from CSM
    csm::SettableEllipsoid *ellipsoidModel = dynamic_cast<csm::SettableEllipsoid*>(m_model);
    if (!ellipsoidModel) {
      // TODO is there a fallback we can do here?
      QString msg = "Failed to get ellipsoid from CSM Model.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    csm::Ellipsoid targetEllipsoid = ellipsoidModel->getEllipsoid();
    std::vector<Distance> radii {Distance(targetEllipsoid.getSemiMajorRadius(), Distance::Meters),
                                 Distance(targetEllipsoid.getSemiMajorRadius(), Distance::Meters),
                                 Distance(targetEllipsoid.getSemiMinorRadius(), Distance::Meters)};
    target->setRadii(radii);

    // TODO: Set it to the appropriate shape model (might not be an ellipse)
    target->setShapeEllipsoid();
    return target;
  }


  /**
   * Convert an ISIS pixel coordinate to a CSM pixel coordinate.
   * The ISIS image origin is (0.5, 0.5), the CSM image origin is (0, 0). This
   * function accounts for that and wraps the coordinate in a csm::ImageCoord.
   */
  void CSMCamera::isisToCsmPixel(double line, double sample, csm::ImageCoord &csmPixel) {
    csmPixel.line = line - 0.5;
    csmPixel.samp = sample - 0.5;
  }


  /**
   * Convert a CSM pixel coordinate to an ISIS pixel coordinate.
   * The ISIS image origin is (0.5, 0.5), the CSM image origin is (0, 0). This
   * function accounts for that and unpacks the csm::ImageCoord.
   */
  void CSMCamera::csmToIsisPixel(csm::ImageCoord csmPixel, double &line, double &sample) {
    line = csmPixel.line + 0.5;
    sample = csmPixel.samp + 0.5;
  }


  /**
   * Convert an ISIS ground point into a CSM ground point.
   * ISIS ground points can be created from and converted to many different
   * units and coordinate systems. CSM ground points are always rectangular,
   * body-fixed coordinates in meters.
   */
  csm::EcefCoord CSMCamera::isisToCsmGround(const SurfacePoint &groundPt) {
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
  SurfacePoint CSMCamera::csmToIsisGround(const csm::EcefCoord &groundPt) {
    return SurfacePoint(Displacement(groundPt.x, Displacement::Meters),
                        Displacement(groundPt.y, Displacement::Meters),
                        Displacement(groundPt.z, Displacement::Meters));
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
