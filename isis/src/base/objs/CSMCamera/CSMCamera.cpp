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
#include "LinearAlgebra.h"
#include "NaifStatus.h"
#include "SpecialPixel.h"
#include "StringBlob.h"

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

    m_target = new Target();

    Pvl *label = cube.label();
    PvlGroup &inst = label->findGroup("Instrument", Pvl::Traverse);
    QString targetName = inst["TargetName"][0];
    m_target->setName(targetName);

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
    m_target->setRadii(radii);

    // set shape
    m_target->setShapeEllipsoid();
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

  // Dummy model resolution values for now.
  // We should compute these
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
    csm::EcefCoord groundCoord(groundPoint.GetX().meters(),
                               groundPoint.GetY().meters(),
                               groundPoint.GetZ().meters());
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
