#include "BundleControlPointVector.h"

#include <QDebug>
#include <QFutureWatcher>
#include <QtConcurrentRun>

#include "BundleTargetBody.h"
#include "IException.h"

namespace Isis {

  /**
   * Constructs an empty BundleControlPointVector.
   */
  BundleControlPointVector::BundleControlPointVector() {
  }


  /**
   * Copy constructor.
   * 
   * @param src A reference to the BundleControlPointVector to copy from.
   */
  BundleControlPointVector::BundleControlPointVector(const BundleControlPointVector &src)
      :QVector<BundleControlPointQsp>(src) {
  }


  /**
   * Destructor.
   *
   * Contained BundleControlPoints will remain until all shared pointers to them are deleted.
   */
  BundleControlPointVector::~BundleControlPointVector() {
    clear();
  }


  /**
   * Assignment operator.
   * 
   * Assigns the state of the source BundleControlPointVector to this BundleControlPointVector.
   * 
   * @param src The BundleControlPointVector to assign from.
   * 
   * @return BundleControlPointVector& A reference to this BundleControlPointVector.
   */
  BundleControlPointVector &BundleControlPointVector::operator=(const BundleControlPointVector &src) {
    if (&src != this) {
      QVector<BundleControlPointQsp>::operator=(src);
    }
    return *this;
  }


  /**
   * Apply point parameter corrections.
   *
   * @param normalsMatrix Normal equations matrix.
   * @param imageSolution Current iteration solution vector for image parameters.
   *
   */
  void BundleControlPointVector::applyParameterCorrections(SparseBlockMatrix &normalsMatrix,
                                                           LinearAlgebra::Vector &imageSolution,
                                                           const BundleTargetBodyQsp target) {
    for (int i = 0; i < size(); i++) {
      at(i)->applyParameterCorrections(normalsMatrix, imageSolution, target);
    }
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained point residuals.
   *
   * @return double Weighted sum of squares of constrained point residuals.
   */
  void BundleControlPointVector::computeMeasureResiduals() {

    for (int i = 0; i < size(); i++) {
      at(i)->computeResiduals();
    }
  }


  /**
   * Compute vtpv of image measures (weighted sum of squares of measure residuals).
   *
   * @return double weighted sum of squares of measure residuals (vtpv).
   */
  double BundleControlPointVector::vtpvMeasureContribution() {
    double vtpv = 0;

    for (int i = 0; i < size(); i++) {
      vtpv += at(i)->vtpvMeasures();
    }

    return vtpv;
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained point residuals.
   *
   * @return double Weighted sum of squares of constrained point residuals.
   */
  double BundleControlPointVector::vtpvContribution() {
    double vtpvControl = 0;

    for (int i = 0; i < size(); i++) {
      vtpvControl += at(i)->vtpv();
    }

    return vtpvControl;
  }
}


