#ifndef BundleLidarControlPoint_h
#define BundleLidarControlPoint_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QSharedPointer>

#include "BundleControlPoint.h"
#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "LidarControlPoint.h"

namespace Isis {

//  class LidarControlPoint;
  class BundleLidarRangeConstraint;

  typedef QSharedPointer<BundleLidarRangeConstraint> BundleLidarRangeConstraintQsp;

  /**
   * This class holds information about a lidar control point that BundleAdjust requires.
   *
   * This class was created to extract functionality from BundleAdjust and wrap a LidarControlPoint
   * with the extra necessary information to correctly perform a bundle adjustment.
   *
   * Note that only non-ignored lidar control points should be used to construct a
   * BundleLidarControlPoint. Similarly, a BundleLidarControlPoint should only contain non-ignored
   * control measures.
   *
   * @author 2018-02-08 Ken Edmundson
   *
   * @internal
   *   @history 2018-02-08 Ken Edmundson - Original version.
   */
  class BundleLidarControlPoint : public BundleControlPoint {

    public:
      BundleLidarControlPoint(BundleSettingsQsp bundleSettings,
                              LidarControlPointQsp lidarControlPoint);
      ~BundleLidarControlPoint();

      // copy
      BundleLidarControlPoint &operator=(const BundleLidarControlPoint &src);// ??? not implemented
      void copy(const BundleLidarControlPoint &src);

      void initializeRangeConstraints();
      int applyLidarRangeConstraints(SparseBlockMatrix &normalsMatrix,
                                    LinearAlgebra::MatrixUpperTriangular& N22,
                                    SparseBlockColumnMatrix& N12,
                                    LinearAlgebra::VectorCompressed& n1,
                                    LinearAlgebra::Vector& n2);
      void computeResiduals();

      virtual void applyParameterCorrections(LinearAlgebra::Vector imageSolution,
                                             SparseBlockMatrix &sparseNormals,
                                             const BundleTargetBodyQsp target);

      BundleLidarRangeConstraintQsp rangeConstraint(int n);
      double vtpvRangeContribution();
      int numberRangeConstraints();
      double range();
      double sigmaRange();

    private:
      LidarControlPointQsp m_lidarControlPoint;
      QVector<BundleLidarRangeConstraintQsp> m_rangeConstraints;
  };

  // typedefs
  //! QSharedPointer to a BundleLidarControlPoint.
  typedef QSharedPointer<BundleLidarControlPoint> BundleLidarControlPointQsp;
}

#endif // BundleLidarControlPoint_h

