/**
 * New blurb TODO
 */

#include "CSMCamera.h"
//#include "VimsGroundMap.h"
//#include "VimsSkyMap.h"
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
    std::cout << "Set the names" << std::endl;
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

    std::cout << "instrument name, spacecraft name: " << m_instrumentNameLong << ", " << m_spacecraftNameLong << std::endl;
    m_pixelPitchX = 10; // dummy value
    m_pixelPitchY = 10; // dummy value

    m_target = new Target();

    Pvl *label = cube.label();
    PvlGroup &inst = label->findGroup("Instrument", Pvl::Traverse);
    QString targetName = inst["TargetName"][0];
    m_target->setName(targetName);

    // get radii from CSM -- add correct radii
    std::vector<Distance> radii;
    radii.push_back(Distance());
    radii.push_back(Distance());
    radii.push_back(Distance());
    m_target->setRadii(radii);

    // set shape
    m_target->setShapeEllipsoid();
//    std::cout << m_model->getModelState() << std::endl;
    return;
  }


  bool CSMCamera::SetImage(const double sample, const double line) {
// move to protected? 
//    p_childSample = sample;
//    p_childLine = line;
//    p_pointComputed = true;

//    if (p_projection == NULL || p_ignoreProjection) {    
//        double parentSample = p_alphaCube->AlphaSample(sample);
//        double parentLine = p_alphaCube->AlphaLine(line);
//        bool success = false;
        double height = 10.0;

        csm::ImageCoord imagePt(line, sample);

        // do image to ground with csm
        csm::EcefCoord result = m_model->imageToGround(imagePt, height);
        // put result in whatever stateful place ISIS needs it to be? 

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


        std::cout << "Hello result: " << result.x << ", " << result.y << ", " << result.z << std::endl;
        // fill in whatever stuff ISIS needs from this
//    }
//    else {
        // handle projected case
//    }

    std::cout << "Hello World!" << std::endl;
    // how to do this -- csm returns the _closest pixel_ to the intersection
    return true;
  }


  // Return the target
  Target *CSMCamera::target() const {
    return m_target;
  }


  /**
   * Returns the pixel ifov offsets from center of pixel.  For vims this will be a rectangle or
   * square, depending on the sampling mode.  The first vertex is the top left.
   *
   * @internal
   *   @history 2013-08-09 Tracie Sucharski - Add more vertices along each edge.  This might need
   *                          to be a user parameter evenually?  Might be dependent on resolution.
   */
   QList<QPointF> CSMCamera::PixelIfovOffsets() {

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
