#include "BundleLidarRangeConstraint.h"

// Qt Library
#include <QDebug>

// Isis Library
#include "Camera.h"
#include "CameraGroundMap.h"
#include "SpicePosition.h"

// Boost Library
#include <boost/numeric/ublas/vector_proxy.hpp>

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Default constructor
   *
   */
  BundleLidarRangeConstraint::BundleLidarRangeConstraint(LidarControlPointQsp lidarControlPoint,
                                                         BundleMeasureQsp measure) {
    m_lidarControlPoint = lidarControlPoint;
    m_simultaneousMeasure = measure;
    m_bundleObservation = measure->parentBundleObservation();

    m_scaledTime = 0.0;
    m_rangeObserved = m_lidarControlPoint->range();
    m_rangeObservedSigma = m_lidarControlPoint->sigmaRange() * 0.001; // converting from m to km

    if (m_rangeObservedSigma <= 0) {
      QString msg ="In BundleLidarRangeConstraint::BundleLidarRangeConstraint():"
                   "observed range sigma for lidar point must be positive (Point Id: "
                   + measure->parentControlPoint()->id() + ")\n.";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    m_rangeObservedWeightSqrt = 1.0/m_rangeObservedSigma;
    m_adjustedSigma = 0.0;

    m_coeff_range_image.resize(1, m_bundleObservation->numberPositionParameters());
    m_coeff_range_point3D.resize(1,3);
    m_coeff_range_RHS.resize(1);

    m_pointBodyFixed.resize(3);
    m_camPositionJ2K.resize(3);
    m_camPositionBodyFixed.resize(3);
    m_matrixTargetToJ2K.resize(9);

    // update to initialize member parameters based on current point coordinate values and SPICE
    update();
  }


  /**
   * Destructor
   */
  BundleLidarRangeConstraint::~BundleLidarRangeConstraint() {
  }


  /**
   * TODO: complete
   * Copy constructor
   *
   * Constructs a BundleLidarRangeConstraint from another.
   *
   * @param src Source BundleLidarRangeConstraint to copy.
   */
  BundleLidarRangeConstraint::BundleLidarRangeConstraint(const BundleLidarRangeConstraint &src) {
  }


  /**
   * TODO: complete
   * Assignment operator
   *
   * Assigns state of this BundleLidarRangeConstraint from another.
   *
   * @param src Source BundleLidarRangeConstraint to assign state from.
   *
   * @return    BundleLidarRangeConstraint& Returns reference to this
   *            BundleLidarRangeConstraint.
   */
  BundleLidarRangeConstraint &BundleLidarRangeConstraint::
      operator=(const BundleLidarRangeConstraint &src) {

    // Prevent self assignment
    if (this != &src) {
    }

    return *this;
  }


  /**
   * Set parameters based on current point coordinates and SPICE
   *
   */
  void BundleLidarRangeConstraint::update() {

    // establish camera model for this measure (the unpleasant statefulness thing)
    m_simultaneousMeasure->setImage();

    // set time of current location of simultaneous measure
    m_scaledTime = m_simultaneousMeasure->camera()->instrumentPosition()->scaledTime();

    // current body fixed XYZ coordinates of lidar control point
    SurfacePoint adjustedSurfacePoint = m_lidarControlPoint->GetAdjustedSurfacePoint();
    m_pointBodyFixed[0]  = adjustedSurfacePoint.GetX().kilometers();
    m_pointBodyFixed[1]  = adjustedSurfacePoint.GetY().kilometers();
    m_pointBodyFixed[2]  = adjustedSurfacePoint.GetZ().kilometers();

    // get spacecraft coordinates in J2000 reference system
    m_camPositionJ2K
        = m_simultaneousMeasure->camera()->instrumentPosition()->Coordinate();

    // body rotation "ReferenceVector" rotates spacecraft coordinates from J2000 to body-fixed
    m_camPositionBodyFixed
        = m_simultaneousMeasure->camera()->bodyRotation()->ReferenceVector(m_camPositionJ2K);

    // get matrix that rotates spacecraft from J2000 to body-fixed
    m_matrixTargetToJ2K = m_simultaneousMeasure->camera()->bodyRotation()->Matrix();

    // calculate "computed" distance between spacecraft and lidar point (based on current SPICE)
    double dX = m_camPositionBodyFixed[0] - m_pointBodyFixed[0];
    double dY = m_camPositionBodyFixed[1] - m_pointBodyFixed[1];
    double dZ = m_camPositionBodyFixed[2] - m_pointBodyFixed[2];
    m_rangeComputed = sqrt(dX*dX+dY*dY+dZ*dZ);

    // compute current contribution to vtpv from spacecraft-to-lidar point constraint equation
    // vtpv is the weight sum of squares of the residuals
    double t = m_rangeObserved - m_rangeComputed;
    m_vtpv = t * t * m_rangeObservedWeightSqrt * m_rangeObservedWeightSqrt;
  }


  /**
   * Computes partial derivatives of range condition equation and adds contribution into the bundle
   * adjustment normal equation matrices.
   */
  bool BundleLidarRangeConstraint::applyConstraint(SparseBlockMatrix &normalsMatrix,
                                                   LinearAlgebra::MatrixUpperTriangular& N22,
                                                   SparseBlockColumnMatrix& N12,
                                                   LinearAlgebra::VectorCompressed& n1,
                                                   LinearAlgebra::Vector& n2,
                                                   BundleMeasureQsp measure) {
    if (m_simultaneousMeasure != measure) {
      return false;
    }

    int positionBlockIndex = measure->positionNormalsBlockIndex();

    m_coeff_range_image.clear();
    m_coeff_range_point3D.clear();
    m_coeff_range_RHS.clear();

    // compute partial derivatives for camstation-to-range point condition

    // matrix that rotates spacecraft from J2000 to body-fixed
    double m11 = m_matrixTargetToJ2K[0];
    double m12 = m_matrixTargetToJ2K[1];
    double m13 = m_matrixTargetToJ2K[2];
    double m21 = m_matrixTargetToJ2K[3];
    double m22 = m_matrixTargetToJ2K[4];
    double m23 = m_matrixTargetToJ2K[5];
    double m31 = m_matrixTargetToJ2K[6];
    double m32 = m_matrixTargetToJ2K[7];
    double m33 = m_matrixTargetToJ2K[8];

    // a1, a2, a3 are auxiliary values used in computation of partial derivatives below
    double a1 = m11*m_camPositionJ2K[0] + m12*m_camPositionJ2K[1] + m13*m_camPositionJ2K[2]
              - m_pointBodyFixed[0];
    double a2 = m21*m_camPositionJ2K[0] + m22*m_camPositionJ2K[1] + m23*m_camPositionJ2K[2]
              - m_pointBodyFixed[1];
    double a3 = m31*m_camPositionJ2K[0] + m32*m_camPositionJ2K[1] + m33*m_camPositionJ2K[2]
              - m_pointBodyFixed[2];

    double b1 = -(m11*a1 + m21*a2 + m31*a3)/m_rangeComputed;
    double b2 = -(m12*a1 + m22*a2 + m32*a3)/m_rangeComputed;
    double b3 = -(m13*a1 + m23*a2 + m33*a3)/m_rangeComputed;

    // partials with respect to camera body fixed X polynomial coefficients
    double c = 1.0;
    int numPositionParameters = m_bundleObservation->numberPositionParameters()/3;
    int index = 0;
    for (int i = 0; i < numPositionParameters; i++) {
      m_coeff_range_image(0,index) = b1*c;
      c *= m_scaledTime;
      index++;
    }

    // partials with respect to camera body fixed Y polynomial coefficients
    c = 1.0;
    for (int i = 0; i < numPositionParameters; i++) {
      m_coeff_range_image(0,index) = b2*c;
      c *= m_scaledTime;
      index++;
    }

    // partials with respect to camera body fixed Z polynomial coefficients
    c = 1.0;
    for (int i = 0; i < numPositionParameters; i++) {
      m_coeff_range_image(0,index) = b3*c;
      c *= m_scaledTime;
      index++;
    }

    // partials w/r to point
    SurfacePoint adjustedSurfacePoint = m_lidarControlPoint->GetAdjustedSurfacePoint();
    double lat    = adjustedSurfacePoint.GetLatitude().radians();
    double lon    = adjustedSurfacePoint.GetLongitude().radians();
    double radius = adjustedSurfacePoint.GetLocalRadius().kilometers();

    double sinlat = sin(lat);
    double coslat = cos(lat);
    double sinlon = sin(lon);
    double coslon = cos(lon);

    // partials with respect to point latitude, longitude, and radius
    m_coeff_range_point3D(0,0)
        = radius*(-sinlat*coslon*a1 - sinlat*sinlon*a2 + coslat*a3)/m_rangeComputed;
    m_coeff_range_point3D(0,1)
        = radius*(-coslat*sinlon*a1 + coslat*coslon*a2)/m_rangeComputed;
    m_coeff_range_point3D(0,2)
        = (coslat*coslon*a1 + coslat*sinlon*a2 + sinlat*a3)/m_rangeComputed;

    // right hand side (observed distance - computed distance)
    m_coeff_range_RHS(0) = m_rangeObserved - m_rangeComputed;

    // multiply coefficients by observation weight
    m_coeff_range_image   *= m_rangeObservedWeightSqrt;
    m_coeff_range_point3D *= m_rangeObservedWeightSqrt;
    m_coeff_range_RHS     *= m_rangeObservedWeightSqrt;

    // form matrices to be added to normal equation auxiliaries
    // TODO: be careful about need to resize if if different images have different numbers of
    // parameters

    // add range condition contribution to N11 portion of normal equations matrix
    (*(*normalsMatrix[positionBlockIndex])[positionBlockIndex])
        += prod(trans(m_coeff_range_image), m_coeff_range_image);

    // add range condition contribution to N12 portion of normal equations matrix
    *N12[positionBlockIndex] += prod(trans(m_coeff_range_image), m_coeff_range_point3D);

    // contribution to n1 vector
    int startColumn = normalsMatrix.at(positionBlockIndex)->startColumn();
    vector_range<LinearAlgebra::VectorCompressed >
        n1_range (n1, range (startColumn, m_coeff_range_image.size2()));

    n1_range += prod(trans(m_coeff_range_image), m_coeff_range_RHS);

    // form N22
    N22 += prod(trans(m_coeff_range_point3D), m_coeff_range_point3D);

    // contribution to n2 vector
    n2 += prod(trans(m_coeff_range_point3D), m_coeff_range_RHS);

    return true;
  }


  /**
   * Return current value of weighted sum-of-squares of residual.
   *
   * @return double Current value of weighted sum-of-squares of residual.
   *
   */
  double BundleLidarRangeConstraint::vtpv() {
    return m_vtpv;
  }


  /**
   * TODO: to be completed.
   * Error propagation for adjusted range sigma.
   *
   * @todo need more documentation on technical aspects of approach.
   */
  void BundleLidarRangeConstraint::errorPropagation() {

  }


  /**
   * Creates & returns formatted QString for lidar range constraint to output to bundleout_lidar.csv
   * file.
   *
   * @return QString Formatted QString summarizing lidar range constraint to output to
   *                 bundleout_lidar.csv.
   */
  QString BundleLidarRangeConstraint::formatBundleOutputString(bool errorProp) {

    QString outstr;

    FileName imageFileName = m_simultaneousMeasure->parentBundleObservation()->imageNames().at(0);
    QString imageName = imageFileName.baseName();


    //                     measured   apriori   adjusted               adjusted
    //                      range      sigma     range      residual     sigma
    // point id  image       (km)       (km)      (km)        (km)       (km)

    if (errorProp) {
      outstr = QString("%1,%2,%3,%4,%5,%6,%7\n").arg(m_lidarControlPoint->GetId(), 16)
                                          .arg(imageName, 16)
                                          .arg(m_rangeObserved, -16, 'f', 8)
                                          .arg(m_rangeObservedSigma, -16, 'f', 2)
                                          .arg(m_rangeComputed, -16, 'f', 8)
                                          .arg(m_rangeObserved - m_rangeComputed, -16, 'f', 6)
                                          .arg(m_adjustedSigma, -16, 'f', 8);
    }
    else {
      outstr = QString("%1,%2,%3,%4,%5,%6\n").arg(m_lidarControlPoint->GetId())
                                          .arg(imageName)
                                          .arg(m_rangeObserved)
                                          .arg(m_rangeObservedSigma)
                                          .arg(m_rangeComputed)
                                          .arg(m_rangeObserved - m_rangeComputed);
    }

    return outstr;
  }
}






