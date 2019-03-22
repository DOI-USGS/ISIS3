#include "BundleLidarControlPoint.h"

// qt lib
#include <QDebug>

// std lib
#include <vector>

// boost lib
#include <boost/lexical_cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

// Isis lib
#include "BundleLidarControlPoint.h"
#include "BundleLidarRangeConstraint.h"
#include "Camera.h"
#include "ControlMeasure.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "SpecialPixel.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Constructs a BundleLidarControlPoint object from a LidarControlPoint. Only the
   * non-ignored measures are added to the BundleLidarControlPoint.
   *  
   * @param bundleSettings BundleSettings.
   *
   * @param controlPoint Pointer to a ControlPoint that will be used to 
   *                     construct this BundleLidarControlPoint.
   *
   * TODO: is the typecast expensive?
   */
  BundleLidarControlPoint::BundleLidarControlPoint(const BundleSettingsQsp bundleSettings,
                                                   LidarControlPointQsp lidarControlPoint)
    : BundleControlPoint(bundleSettings, (ControlPoint*)  lidarControlPoint.data()) {

    m_lidarControlPoint = lidarControlPoint;
  }


  /**
   * Initialize range constraints.
   *
   */
  void BundleLidarControlPoint::initializeRangeConstraints() {

    QStringList simultaneousSerialNumbers = m_lidarControlPoint->snSimultaneous();
    for (int i= 0; i < size(); i++) {
      BundleMeasureQsp measure = at(i);
      if (!simultaneousSerialNumbers.contains(measure->cubeSerialNumber())) {
          continue;
      }

      m_rangeConstraints.append(BundleLidarRangeConstraintQsp
                                (new BundleLidarRangeConstraint(m_lidarControlPoint, measure)));
    }
  }


  /**
   * Copy constructor. Constructs a BundleLidarControlPoint object from an existing
   * BundleLidarControlPoint.
   *  
   * @param src The BundleLidarControlPoint to be copied.
   */
//  BundleLidarControlPoint::BundleLidarControlPoint(const BundleLidarControlPoint &src) {
//    : BundleControlPoint(((ControlPoint) src) {
//    copy(src);
//  }


  /**
   * Destructor for BundleLidarControlPoint.
   */
  BundleLidarControlPoint::~BundleLidarControlPoint() {
  }


  /**
   * Copies given BundleLidarControlPoint to this BundleLidarControlPoint.
   *  
   * @param src The BundleLidarControlPoint to be copied.
   */
  void BundleLidarControlPoint::copy(const BundleLidarControlPoint &src) {

    BundleControlPoint::copy((BundleControlPoint) src);
/*
    // sanity check
    clear();

    m_controlPoint = src.m_controlPoint;

    int numMeasures = src.size();

    for (int i = 0; i < numMeasures; i++)
      append(BundleMeasureQsp( new BundleMeasure(*(src.at(i))) ));

    m_corrections = src.m_corrections;
    m_aprioriSigmas = src.m_aprioriSigmas;
    m_adjustedSigmas = src.m_adjustedSigmas;
    m_weights = src.m_weights;
    m_nicVector = src.m_nicVector;
*/
  }

  /**
   * Computes the residuals for this BundleControlPoint.
   *
   */
  void BundleLidarControlPoint::computeResiduals() {
    // test calling normal version of compute residuals
    m_lidarControlPoint->ComputeResiduals();

    // compute and store focal plane residuals in millimeters
    for (int i = 0; i < size(); i++) {
      at(i)->setFocalPlaneResidualsMillimeters();
    }
  }


  /**
   * Applies range constraint between image and lidar point acquired simultaneously.
   */
  int BundleLidarControlPoint::applyLidarRangeConstraint(SparseBlockMatrix &normalsMatrix,
                                                         LinearAlgebra::MatrixUpperTriangular& N22,
                                                         SparseBlockColumnMatrix& N12,
                                                         LinearAlgebra::VectorCompressed& n1,
                                                         LinearAlgebra::Vector& n2,
                                                         BundleMeasureQsp measure) {
    int constraintsApplied = 0;

    // loop over range constraints
    for (int i = 0; i < m_rangeConstraints.size(); i++) {
      if (m_rangeConstraints.at(i)->applyConstraint(normalsMatrix, N22, N12, n1, n2, measure)) {
        constraintsApplied++;
      }
    }

    return constraintsApplied;
  }


  /**
   *
   */
  double BundleLidarControlPoint::vtpvRangeContribution() {
    double vtpv = 0.0;
    for (int i = 0; i < m_rangeConstraints.size(); i++) {
      vtpv += m_rangeConstraints.at(i)->vtpv();
    }

    return vtpv;
  }
}
