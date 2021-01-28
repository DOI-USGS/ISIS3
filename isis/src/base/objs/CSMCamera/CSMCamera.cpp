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
#include "NaifStatus.h"
#include "SpecialPixel.h"
#include "StringBlob.h"

#include "csm/Plugin.h"

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

    m_instrumentNameLong = QString::fromStdString(m_model->getSensorIdentifier());
    m_instrumentNameShort = QString::fromStdString(m_model->getSensorIdentifier());
    m_spacecraftNameLong = QString::fromStdString(m_model->getPlatformIdentifier());
    m_spacecraftNameShort = QString::fromStdString(m_model->getPlatformIdentifier());

    m_pixelPitchX = 10; // dummy value no way to get from CSM
    m_pixelPitchY = 10; // dummy value

    // CSMCamera(cube) -> Camera(cube) -> Sensor(cube) -> Spice(cube)
    // Spice::init() creates a Target in such a way that requires spice data / tables, so 
    // this works around that.
    m_target = new Target();

    Pvl *label = cube.label();
    PvlGroup &inst = label->findGroup("Instrument", Pvl::Traverse);
    QString targetName = inst["TargetName"][0];
    m_target->setName(targetName);

    // TODO: Get radii from CSM -- add correct radii
    std::vector<Distance> radii;

    // get ellipsoid, cast to settable elllipsoid? 
    radii.push_back(Distance());
    radii.push_back(Distance());
    radii.push_back(Distance());
    m_target->setRadii(radii);

    // set shape
    m_target->setShapeEllipsoid();
    return;
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

        // Set X, Y, Z in surface point
        double naifValues[3] = {result.x, result.y, result.z};
        target()->shape()->surfaceIntersection()->FromNaifArray(naifValues);

        // check set of coordinate:
        double test[3];
        Coordinate(test);
        std::cout << "TEST:  " << test[0] << ", "  << test[1] << ", " << test[2] << std::endl;
        std::cout << "UniversalLatitude: " << UniversalLatitude() << std::endl; 

            // get a lat, lon and store in variables
        // (1) how to get lat/lon
        // (2) which variables to store in

        // fill in whatever stuff ISIS needs from this
//    }
//    else {
        // handle projected case
//    }

    std::cout << "Hello World!" << std::endl;
    // how to do this -- csm returns the _closest pixel_ to the intersection
    return true;
  }

  double CSMCamera::LineResolution() {
    // Camera version uses detectorMap
    return m_pixelPitchX;
  }

  double CSMCamera::SampleResolution() {
    // Camera version uses detectorMap
    return m_pixelPitchY;
  }

 double CSMCamera::PixelResolution() {
    // Camera version uses detectorMap
    return m_pixelPitchY;
  }

  double CSMCamera::ObliqueLineResolution() {
    return 10.0;
  }

  double CSMCamera::ObliqueSampleResolution() {
    return 10.0;
  }


  double CSMCamera::ObliquePixelResolution() {
    return 10.0;
  }


  // Return the target
  Target *CSMCamera::target() const {
    return m_target;
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
    csm::ImageCoord imagePt(line, sample);
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


  // Need to override because Camera::SetGround(SurfacePoint) calls the GroundMap which we don't have
  //bool CSMCamera::SetGround(){
//    return false;
//  }


  /**
   * Returns the pixel ifov offsets from center of pixel.  For vims this will be a rectangle or
   * square, depending on the sampling mode.  The first vertex is the top left.
   *
   * @internal
   *   @history 2013-08-09 Tracie Sucharski - Add more vertices along each edge.  This might need
   *                          to be a user parameter evenually?  Might be dependent on resolution.
   */
   QList<QPointF> CSMCamera::PixelIfovOffsets() {
     // throw error 

     QList<QPointF> offsets;
  
     //  Create 100 pts on each edge of pixel
     int npts = 100;

     //  Top edge of pixel
     for (double x = -m_pixelPitchX / 2.0; x <= m_pixelPitchX / 2.0; x += m_pixelPitchX / (npts-1)) {
       offsets.append(QPointF(x, -m_pixelPitchY / 2.0));
     }
     //  Right edge of pixel
     for (double y = -m_pixelPitchY / 2.0; y <= m_pixelPitchY / 2.0; y += m_pixelPitchY / (npts-1)) {
       offsets.append(QPointF(m_pixelPitchX / 2.0, y));
     }
     //  Bottom edge of pixel
     for (double x = m_pixelPitchX / 2.0; x >= -m_pixelPitchX / 2.0; x -= m_pixelPitchX / (npts-1)) {
       offsets.append(QPointF(x, m_pixelPitchY / 2.0));
     }
     //  Left edge of pixel
     for (double y = m_pixelPitchY / 2.0; y >= -m_pixelPitchY / 2.0; y -= m_pixelPitchY / (npts-1)) {
       offsets.append(QPointF(-m_pixelPitchX / 2.0, y));
     }

     return offsets;
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
