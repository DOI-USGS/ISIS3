#ifndef BundleLidarPointVector_h
#define BundleLidarPointVector_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include "BundleLidarControlPoint.h"

namespace Isis {


  /**
   * This class is a container class for BundleLidarControlPoints.
   *
   * Contained BundleLidarControlPoints are stored as shared pointers, so are automatically deleted when
   * all shared pointers are deleted.
   *
   * @author 2019-03-13 Ken Edmundson
   *
   * @internal
   *   @history 2019-03-13 Ken Edmundson - Original version.
   */
  class BundleLidarPointVector : public QVector<BundleLidarControlPointQsp> {

    public:
      BundleLidarPointVector();
      BundleLidarPointVector(const BundleLidarPointVector &src);
      ~BundleLidarPointVector();

      BundleLidarPointVector &operator=(const BundleLidarPointVector &src);

      void applyParameterCorrections(SparseBlockMatrix &normalsMatrix,
                                     LinearAlgebra::Vector &imageSolution,
                                     const BundleTargetBodyQsp target);

      void computeMeasureResiduals();
      double vtpvContribution();
      double vtpvMeasureContribution();
      double vtpvRangeContribution();
  };
}

#endif // BundleLidarPointVector_h
