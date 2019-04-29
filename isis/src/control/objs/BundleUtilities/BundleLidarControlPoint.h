#ifndef BundleLidarControlPoint_h
#define BundleLidarControlPoint_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/22 01:35:17 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
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

