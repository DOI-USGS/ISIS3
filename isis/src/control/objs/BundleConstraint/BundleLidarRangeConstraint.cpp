#include "BundleLidarRangeConstraint.h"

// Qt Library
#include <QDebug>

// Isis Library
#include "Camera.h"
#include "SpicePosition.h"

// boost lib
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Default constructor
   *
   */
  BundleLidarRangeConstraint::BundleLidarRangeConstraint(LidarControlPointQsp lidarControlPoint,
                                                         BundleMeasureQsp measure) {
    m_parentLidarControlPoint = lidarControlPoint;
    m_bundleObservation = measure->parentBundleObservation();

//    m_instrumentPosition = bundleObservation->spicePosition(); // statefulness?
//    m_bodyRotation = measure->camera()->bodyRotation();        // statefulness?
    m_simultaneousMeasure = measure;

    init();
  }


  /**
   * Destructor
   */
  BundleLidarRangeConstraint::~BundleLidarRangeConstraint() {
  }


  /**
   * Copy constructor
   *
   * Constructs a BundleLidarRangeConstraint from another.
   *
   * @param src Source BundleLidarRangeConstraint to copy.
   */
  BundleLidarRangeConstraint::
      BundleLidarRangeConstraint(const BundleLidarRangeConstraint &src) {
  }


  /**
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
   * Initialization
   *
   * TODO: how to handle size of m_coeff_range_image if we are using piecewise polynomials?
   */
  void BundleLidarRangeConstraint::init() {
    m_observedRange = m_parentLidarControlPoint->range();
    m_observedRangeSigma = m_parentLidarControlPoint->sigmaRange() * 0.001; // converting to km
    m_observedRangeWeight = 1.0/m_observedRangeSigma;
    m_adjustedSigma = 0.0;

    m_coeff_range_image.resize(1, m_bundleObservation->numberPositionParameters());
    m_coeff_range_point3D.resize(1,3);
    m_coeff_range_RHS.resize(1);

    m_pointBodyFixed.resize(3);
    m_cameraJ2K.resize(3);
    m_cameraBodyFixed.resize(3);
    m_matrixTargetToJ2K.resize(9);

    update();
  }


  /**
   * Applies range constraint between image and lidar point acquired simultaneously.
   */
  bool BundleLidarRangeConstraint::applyConstraint(SparseBlockMatrix &normalsMatrix,
                                                   LinearAlgebra::MatrixUpperTriangular& N22,
                                                   SparseBlockColumnMatrix& N12,
                                                   LinearAlgebra::VectorCompressed& n1,
                                                   LinearAlgebra::Vector& n2,
                                                   BundleMeasureQsp measure) {
    int i;

    if (m_simultaneousMeasure != measure) {
      return false;
    }

    if (m_parentLidarControlPoint->GetId() == "Lidar1910")
      int fred=1;


//    int imageIndex = measure->positionNormalsBlockIndex();
    int imageIndex = m_bundleObservation->polyPositionSegmentIndex();

    m_coeff_range_image.clear();
    m_coeff_range_point3D.clear();
    m_coeff_range_RHS.clear();

//    std::cout << "image" << std::endl << coeff_range_image << std::endl;
//    std::cout << "point" << std::endl << coeff_range_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_range_RHS << std::endl;

    // compute partial derivatives for camstation-to-range point condition

    // get matrix that rotates spacecraft from J2000 to body-fixed
    double m11 = m_matrixTargetToJ2K[0];
    double m12 = m_matrixTargetToJ2K[1];
    double m13 = m_matrixTargetToJ2K[2];
    double m21 = m_matrixTargetToJ2K[3];
    double m22 = m_matrixTargetToJ2K[4];
    double m23 = m_matrixTargetToJ2K[5];
    double m31 = m_matrixTargetToJ2K[6];
    double m32 = m_matrixTargetToJ2K[7];
    double m33 = m_matrixTargetToJ2K[8];

    // partials w/r to image
    // auxiliaries
    double a1 = m11*m_cameraJ2K[0] + m12*m_cameraJ2K[1] + m13*m_cameraJ2K[2] - m_pointBodyFixed[0];
    double a2 = m21*m_cameraJ2K[0] + m22*m_cameraJ2K[1] + m23*m_cameraJ2K[2] - m_pointBodyFixed[1];
    double a3 = m31*m_cameraJ2K[0] + m32*m_cameraJ2K[1] + m33*m_cameraJ2K[2] - m_pointBodyFixed[2];

    m_coeff_range_image(0,0) = (m11*a1 + m21*a2 + m31*a3)/m_computedRange;
    m_coeff_range_image(0,1) = (m12*a1 + m22*a2 + m32*a3)/m_computedRange;
    m_coeff_range_image(0,2) = (m13*a1 + m23*a2 + m33*a3)/m_computedRange;

//    std::cout << coeff_range_image << std::endl;

    // partials w/r to point
    SurfacePoint adjustedSurfacePoint = m_parentLidarControlPoint->GetAdjustedSurfacePoint();
    double lat    = adjustedSurfacePoint.GetLatitude().radians();
    double lon    = adjustedSurfacePoint.GetLongitude().radians();
    double radius = adjustedSurfacePoint.GetLocalRadius().kilometers();

    double sinlat = sin(lat);
    double coslat = cos(lat);
    double sinlon = sin(lon);
    double coslon = cos(lon);

    m_coeff_range_point3D(0,0)
        = radius*(sinlat*coslon*a1 + sinlat*sinlon*a2 - coslat*a3)/m_computedRange;
    m_coeff_range_point3D(0,1)
        = radius*(coslat*sinlon*a1 - coslat*coslon*a2)/m_computedRange;
    m_coeff_range_point3D(0,2)
        = -(coslat*coslon*a1 + coslat*sinlon*a2 + sinlat*a3)/m_computedRange;

    // right hand side (observed distance - computed distance)
    m_coeff_range_RHS(0) = m_observedRange - m_computedRange;

    // multiply coefficients by observation weight
    m_coeff_range_image   *= m_observedRangeWeight;
    m_coeff_range_point3D *= m_observedRangeWeight;
    m_coeff_range_RHS     *= m_observedRangeWeight;

    // form matrices to be added to normal equation auxiliaries
    // TODO: be careful about need to resize if if different images have different numbers of
    // parameters
    int numberImagePartials = m_coeff_range_image.size2();
    static vector<double> n1_image(numberImagePartials);
    n1_image.clear();

    // form N11 for the condition partials for image
//    static symmetric_matrix<double, upper> N11(numberImagePartials);
//    N11.clear();

//    std::cout << "N11" << std::endl << N11 << std::endl;

//    std::cout << "image" << std::endl << coeff_range_image << std::endl;
//    std::cout << "point" << std::endl << coeff_range_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_range_RHS << std::endl;

//    N11 = prod(trans(coeff_range_image), coeff_range_image);

//    std::cout << "N11" << std::endl << N11 << std::endl;

    int t = numberImagePartials * imageIndex;

    // insert submatrix at column, row
//    m_sparseNormals.insertMatrixBlock(imageIndex, imageIndex, numberImagePartials,
//                                      numberImagePartials);

    (*(*normalsMatrix[imageIndex])[imageIndex])
        += prod(trans(m_coeff_range_image), m_coeff_range_image);

//    std::cout << (*(*m_sparseNormals[imageIndex])[imageIndex]) << std::endl;

    // form N12_Image
//    static matrix<double> N12_Image(numberImagePartials, 3);
//    N12_Image.clear();

//    N12_Image = prod(trans(coeff_range_image), coeff_range_point3D);

//    std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
//    N12.insertMatrixBlock(imageIndex, numberImagePartials, 3);
    *N12[imageIndex] += prod(trans(m_coeff_range_image), m_coeff_range_point3D);

//  printf("N12\n");
//  std::cout << N12 << std::endl;

    // form n1
    n1_image = prod(trans(m_coeff_range_image), m_coeff_range_RHS);

//  std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    for (i = 0; i < numberImagePartials; i++)
      n1(i + t) += n1_image(i);

    // form N22
    N22 += prod(trans(m_coeff_range_point3D), m_coeff_range_point3D);

//  std::cout << "N22" << std::endl << N22 << std::endl;
//  std::cout << "n2" << std::endl << n2 << std::endl;

    // form n2
    n2 += prod(trans(m_coeff_range_point3D), m_coeff_range_RHS);

//  std::cout << "n2" << std::endl << n2 << std::endl;

    return true;
  }


  /**
   * Applies range constraint between image and lidar point acquired simultaneously.
   */
/*
  bool BundleLidarRangeConstraint::applyConstraint(SparseBlockMatrix &normalsMatrix,
                                                   LinearAlgebra::MatrixUpperTriangular& N22,
                                                   SparseBlockColumnMatrix& N12,
                                                   LinearAlgebra::VectorCompressed& n1,
                                                   LinearAlgebra::Vector& n2,
                                                   BundleMeasureQsp measure) {
    int i;

    if (m_simultaneousMeasure != measure) {
      return false;
    }

    if (m_parentLidarControlPoint->GetId() == "Lidar1910")
      int fred=1;


//    int imageIndex = measure->positionNormalsBlockIndex();
    int imageIndex = m_bundleObservation->polyPositionSegmentIndex();

    m_coeff_range_image.clear();
    m_coeff_range_point3D.clear();
    m_coeff_range_RHS.clear();

//    std::cout << "image" << std::endl << coeff_range_image << std::endl;
//    std::cout << "point" << std::endl << coeff_range_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_range_RHS << std::endl;

    // compute partial derivatives for camstation-to-range point condition

    // get ground point in body-fixed coordinates
    SurfacePoint adjustedSurfacePoint = m_parentLidarControlPoint->GetAdjustedSurfacePoint();
    double xPoint  = adjustedSurfacePoint.GetX().kilometers();
    double yPoint  = adjustedSurfacePoint.GetY().kilometers();
    double zPoint  = adjustedSurfacePoint.GetZ().kilometers();

    // get spacecraft position in J2000 coordinates
    std::vector<double> CameraJ2KXYZ(3);
    CameraJ2KXYZ = m_simultaneousMeasure->camera()->instrumentPosition()->Coordinate();
    double time1 = m_simultaneousMeasure->camera()->instrumentPosition()->EphemerisTime();
    double time2 = m_simultaneousMeasure->camera()->instrumentPosition()->EphemerisTime();
//    CameraJ2KXYZ = m_instrumentPosition->Coordinate();
    double xCameraJ2K  = CameraJ2KXYZ[0];
    double yCameraJ2K  = CameraJ2KXYZ[1];
    double zCameraJ2K  = CameraJ2KXYZ[2];

    // get spacecraft position in body-fixed coordinates
    std::vector<double> CameraBodyFixedXYZ(3);

    // "InstrumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
    // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
//    CameraBodyFixedXYZ = m_bodyRotation->ReferenceVector(CameraJ2KXYZ);
    CameraBodyFixedXYZ
        = m_simultaneousMeasure->camera()->bodyRotation()->ReferenceVector(CameraJ2KXYZ);
    double xCamera  = CameraBodyFixedXYZ[0];
    double yCamera  = CameraBodyFixedXYZ[1];
    double zCamera  = CameraBodyFixedXYZ[2];

    // computed distance between spacecraft and point
    double dX = xCamera - m_pointBodyFixed[0];
    double dY = yCamera - m_pointBodyFixed[1];
    double dZ = zCamera - m_pointBodyFixed[2];
    m_computedRange = sqrt(dX*dX+dY*dY+dZ*dZ);

    // get matrix that rotates spacecraft from J2000 to body-fixed
    std::vector<double> matrix_Target_to_J2K;
    matrix_Target_to_J2K = m_simultaneousMeasure->camera()->bodyRotation()->Matrix();

    double m11 = matrix_Target_to_J2K[0];
    double m12 = matrix_Target_to_J2K[1];
    double m13 = matrix_Target_to_J2K[2];
    double m21 = matrix_Target_to_J2K[3];
    double m22 = matrix_Target_to_J2K[4];
    double m23 = matrix_Target_to_J2K[5];
    double m31 = matrix_Target_to_J2K[6];
    double m32 = matrix_Target_to_J2K[7];
    double m33 = matrix_Target_to_J2K[8];

    // partials w/r to image
    // auxiliaries
    double a1 = m11*xCameraJ2K + m12*yCameraJ2K + m13*zCameraJ2K - xPoint;
    double a2 = m21*xCameraJ2K + m22*yCameraJ2K + m23*zCameraJ2K - yPoint;
    double a3 = m31*xCameraJ2K + m32*yCameraJ2K + m33*zCameraJ2K - zPoint;

    m_coeff_range_image(0,0) = (m11*a1 + m21*a2 + m31*a3)/m_computedRange;
    m_coeff_range_image(0,1) = (m12*a1 + m22*a2 + m32*a3)/m_computedRange;
    m_coeff_range_image(0,2) = (m13*a1 + m23*a2 + m33*a3)/m_computedRange;

//    std::cout << coeff_range_image << std::endl;

    // partials w/r to point
    double lat    = adjustedSurfacePoint.GetLatitude().radians();
    double lon    = adjustedSurfacePoint.GetLongitude().radians();
    double radius = adjustedSurfacePoint.GetLocalRadius().kilometers();

    double sinlat = sin(lat);
    double coslat = cos(lat);
    double sinlon = sin(lon);
    double coslon = cos(lon);

    m_coeff_range_point3D(0,0)
        = radius*(sinlat*coslon*a1 + sinlat*sinlon*a2 - coslat*a3)/m_computedRange;
    m_coeff_range_point3D(0,1)
        = radius*(coslat*sinlon*a1 - coslat*coslon*a2)/m_computedRange;
    m_coeff_range_point3D(0,2)
        = -(coslat*coslon*a1 + coslat*sinlon*a2 + sinlat*a3)/m_computedRange;

    // right hand side (observed distance - computed distance)
    m_coeff_range_RHS(0) = m_observedRange - m_computedRange;

    // multiply coefficients by observation weight
    double dObservationWeight = 1.0/m_observedRangeSigma;
    m_coeff_range_image   *= dObservationWeight;
    m_coeff_range_point3D *= dObservationWeight;
    m_coeff_range_RHS     *= dObservationWeight;

    // form matrices to be added to normal equation auxiliaries
    // TODO: be careful about need to resize if if different images have different numbers of
    // parameters
    int numberImagePartials = m_coeff_range_image.size2();
    static vector<double> n1_image(numberImagePartials);
    n1_image.clear();

    // form N11 for the condition partials for image
//    static symmetric_matrix<double, upper> N11(numberImagePartials);
//    N11.clear();

//    std::cout << "N11" << std::endl << N11 << std::endl;

//    std::cout << "image" << std::endl << coeff_range_image << std::endl;
//    std::cout << "point" << std::endl << coeff_range_point3D << std::endl;
//    std::cout << "rhs" << std::endl << coeff_range_RHS << std::endl;

//    N11 = prod(trans(coeff_range_image), coeff_range_image);

//    std::cout << "N11" << std::endl << N11 << std::endl;

    int t = numberImagePartials * imageIndex;

    // insert submatrix at column, row
//    m_sparseNormals.insertMatrixBlock(imageIndex, imageIndex, numberImagePartials,
//                                      numberImagePartials);

    (*(*normalsMatrix[imageIndex])[imageIndex])
        += prod(trans(m_coeff_range_image), m_coeff_range_image);

//    std::cout << (*(*m_sparseNormals[imageIndex])[imageIndex]) << std::endl;

    // form N12_Image
//    static matrix<double> N12_Image(numberImagePartials, 3);
//    N12_Image.clear();

//    N12_Image = prod(trans(coeff_range_image), coeff_range_point3D);

//    std::cout << "N12_Image" << std::endl << N12_Image << std::endl;

    // insert N12_Image into N12
//    N12.insertMatrixBlock(imageIndex, numberImagePartials, 3);
    *N12[imageIndex] += prod(trans(m_coeff_range_image), m_coeff_range_point3D);

//  printf("N12\n");
//  std::cout << N12 << std::endl;

    // form n1
    n1_image = prod(trans(m_coeff_range_image), m_coeff_range_RHS);

//  std::cout << "n1_image" << std::endl << n1_image << std::endl;

    // insert n1_image into n1
    for (i = 0; i < numberImagePartials; i++)
      n1(i + t) += n1_image(i);

    // form N22
    N22 += prod(trans(m_coeff_range_point3D), m_coeff_range_point3D);

//  std::cout << "N22" << std::endl << N22 << std::endl;
//  std::cout << "n2" << std::endl << n2 << std::endl;

    // form n2
    n2 += prod(trans(m_coeff_range_point3D), m_coeff_range_RHS);

//  std::cout << "n2" << std::endl << n2 << std::endl;

    return true;
  }
*/

  /**
   * TODO: complete
   *
   */
  double BundleLidarRangeConstraint::vtpv() {
    return m_vtpv;
  }


  /**
   * TODO: complete
   *
   */
  void BundleLidarRangeConstraint::update() {

    if (m_parentLidarControlPoint->GetId() == "Lidar1910")
      int fred=1;

    // establish camera model for this measure (the unpleasant statefulness thing)
    m_simultaneousMeasure->setImage();

    SurfacePoint adjustedSurfacePoint = m_parentLidarControlPoint->GetAdjustedSurfacePoint();
    m_pointBodyFixed[0]  = adjustedSurfacePoint.GetX().kilometers();
    m_pointBodyFixed[1]  = adjustedSurfacePoint.GetY().kilometers();
    m_pointBodyFixed[2]  = adjustedSurfacePoint.GetZ().kilometers();

    // "InstrumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
    // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
    // get spacecraft position in J2000 coordinates
    std::vector<double> old_cameraJ2K;
    old_cameraJ2K = m_cameraJ2K;

    m_cameraJ2K = m_simultaneousMeasure->camera()->instrumentPosition()->Coordinate();

    double d0 = old_cameraJ2K[0] - m_cameraJ2K[0];
    double d1 = old_cameraJ2K[1] - m_cameraJ2K[1];
    double d2 = old_cameraJ2K[2] - m_cameraJ2K[2];

    // get spacecraft position in body-fixed coordinates
    // "InstrumentPosition()->Coordinate()" returns the instrument coordinate in J2000;
    // then the body rotation "ReferenceVector" rotates that into body-fixed coordinates
    m_cameraBodyFixed
        = m_simultaneousMeasure->camera()->bodyRotation()->ReferenceVector(m_cameraJ2K);

    // matrix that rotates spacecraft from J2000 to body-fixed
    m_matrixTargetToJ2K = m_simultaneousMeasure->camera()->bodyRotation()->Matrix();

    // computed distance between spacecraft and point
    double dX = m_cameraBodyFixed[0] - m_pointBodyFixed[0];
    double dY = m_cameraBodyFixed[1] - m_pointBodyFixed[1];
    double dZ = m_cameraBodyFixed[2] - m_pointBodyFixed[2];
    m_computedRange = sqrt(dX*dX+dY*dY+dZ*dZ);

    double v = m_observedRange - m_computedRange;
    double p = m_observedRangeWeight * m_observedRangeWeight;
    m_vtpv = v * v * p;
    int fred=1;
  }


  /**
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
      outstr = QString("%1,%2,%3,%4,%5,%6,%7\n").arg(m_parentLidarControlPoint->GetId(), 16)
                                          .arg(imageName, 16)
                                          .arg(m_observedRange, -16, 'f', 8)
                                          .arg(m_observedRangeSigma, -16, 'f', 2)
                                          .arg(m_computedRange, -16, 'f', 8)
                                          .arg(m_observedRange - m_computedRange, -16, 'f', 6)
                                          .arg(m_adjustedSigma, -16, 'f', 8);
    }
    else {
      outstr = QString("%1,%2,%3,%4,%5,%6\n").arg(m_parentLidarControlPoint->GetId())
                                          .arg(imageName)
                                          .arg(m_observedRange)
                                          .arg(m_observedRangeSigma)
                                          .arg(m_computedRange)
                                          .arg(m_observedRange - m_computedRange);
    }

    return outstr;
  }
}






