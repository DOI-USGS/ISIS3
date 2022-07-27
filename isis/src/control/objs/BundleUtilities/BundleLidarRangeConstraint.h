#ifndef BundleLidarRangeConstraint_h
#define BundleLidarRangeConstraint_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
// Qt Library
#include <QSharedPointer>

// Isis Library
#include "BundleLidarControlPoint.h"
#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "LinearAlgebra.h"
#include "SparseBlockMatrix.h"

namespace Isis {
  /**
   * @brief Implements range constraint between image position and lidar point acquired
   * simultaneously with the image.
   *
   * @ingroup ControlNetworks
   *
   * @author 2018-04-13 Ken Edmundson
   *
   * @internal
   *   @history 2018-04-13 Ken Edmundson - Original version.
   *   @history 2018-06-27 Ken Edmundson - Code clean up.
   *   @history 2018-06-28 Ken Edmundson - Removed partial derivative matrices as member variables
   *                                       and now declare them as static in the applyConstraint
   *                                       method. Now coeff_range_image matrix is resized if number
   *                                       of image parameters changes. This is consistent with
   *                                       BundleAdjust::computePartials. Added IExceptions to
   *                                       verify m_rangeObserved and m_rangeComputed are positive.
   *   @history 2021-12-20 Jesse Mapel - Removed parent BundleConstraint class.
   *
   */
  class BundleLidarRangeConstraint {
    public:
      // constructor
      BundleLidarRangeConstraint(LidarControlPointQsp lidarControlPoint, BundleMeasureQsp measure);

      // copy constructor
      BundleLidarRangeConstraint(const BundleLidarRangeConstraint &src);

      // destructor
      ~BundleLidarRangeConstraint();

      // Assignment operator
      BundleLidarRangeConstraint &operator= (const BundleLidarRangeConstraint &src);

      bool applyConstraint(SparseBlockMatrix &normalsMatrix,
                           LinearAlgebra::MatrixUpperTriangular& N22,
                           SparseBlockColumnMatrix& N12,
                           LinearAlgebra::VectorCompressed& n1,
                           LinearAlgebra::Vector& n2);

      double vtpv();
      void errorPropagation();

      void computeRange();
      double rangeObserved();
      double rangeComputed();
      double rangeObservedSigma();
      double rangeAdjustedSigma();

      QString formatBundleOutputString(bool errorProp=false);

    private:
      LidarControlPointQsp m_lidarControlPoint;       //!< Parent lidar control point
      BundleObservationQsp m_bundleObservation;       //!< BundleObservation associated with measure

      BundleMeasureQsp m_simultaneousMeasure;         /**! 2D image point corresponding to 3D lidar
                                                           point on surface. The image has been
                                                           acquired simultaneously with the lidar
                                                           observation. NOTE this point is a
                                                           fictitious "measurement". A priori
                                                           coordinates are obtained by back
                                                           projection of the lidar point into the
                                                           image using the current exterior
                                                           orientation (EO-SPICE). This "measure" is
                                                           corrected in each iteration of the bundle
                                                           adjustment by it's residuals.*/

      double m_dX, m_dY, m_dZ;                       /**! deltas between spacecraft & lidar point
                                                          in body-fixed coordinates */
      double m_rangeObserved;                        //!< Observed range from lidar input data
      double m_rangeComputed;                        //!< Computed range from distance condition
      double m_rangeObservedSigma;                   //!< Uncertainty of observed range
      double m_rangeObservedWeightSqrt;              //!< Square-root of observed range weight
      double m_adjustedSigma;                        //!< Adjusted uncertainty of range
      double m_vtpv;                                 //!< Weighted sum-of-squares of residual
  };

  //! Typdef for BundleLidarRangeConstraint QSharedPointer.
  typedef QSharedPointer<BundleLidarRangeConstraint> BundleLidarRangeConstraintQsp;
}

#endif
