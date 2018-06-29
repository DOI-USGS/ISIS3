#ifndef BundleLidarRangeConstraint_h
#define BundleLidarRangeConstraint_h
/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/03/27 07:01:33 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
// Qt Library
#include <QSharedPointer>

// Isis Library
#include "BundleConstraint.h"
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
   *
   */
  class BundleLidarRangeConstraint : public BundleConstraint {
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
                           LinearAlgebra::Vector& n2,
                           BundleMeasureQsp measure);

      void update();
      double vtpv();
      void errorPropagation();

      QString formatBundleOutputString(bool errorProp=false);

    private:
      LidarControlPointQsp m_lidarControlPoint;       //!< Parent lidar control point
      BundleObservationQsp m_bundleObservation;       //!< BundleObservation associated with measure

      BundleMeasureQsp m_simultaneousMeasure;         /**! Point in image acquired simultaneously
                                                           with a lidar observation. NOTE this point
                                                           is a fictitious "measurement". A priori
                                                           coordinates are obtained by back
                                                           projection of the lidar 3D point into the
                                                           image using the image's current exterior
                                                           orientation (SPICE). The "measure" is
                                                           corrected in each iteration of the bundle
                                                           adjustment by it's residuals.*/

      std::vector<double> m_pointBodyFixed;          //!< Body fixed coordinates of lidar point
      std::vector<double> m_camPositionJ2K;          //!< J2K coordinates of camera
      std::vector<double> m_camPositionBodyFixed;    //!< Body fixed coordinates of camera
      std::vector<double> m_matrixTargetToJ2K;       //!< Rotates spacecraft from J2K to body-fixed

      double m_scaledTime;                           //!< Current time for simultaneous measure
      double m_rangeObserved;                        //!< Observed range from lidar input data
      double m_rangeComputed;                        //!< Computed range from distance condition
      double m_rangeObservedSigma;                   //!< Uncertainty of observed range
      double m_rangeObservedWeightSqrt;              //!< Square-root of observed range weight
      double m_adjustedSigma;                        //!< Uncertainty of range after adjustment
      double m_vtpv;                                 //!< weighted sum-of-squares of residual
  };

  //! Typdef for BundleLidarRangeConstraint QSharedPointer.
  typedef QSharedPointer<BundleLidarRangeConstraint> BundleLidarRangeConstraintQsp;
}

#endif
