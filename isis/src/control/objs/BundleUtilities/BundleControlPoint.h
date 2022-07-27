#ifndef BundleControlPoint_h
#define BundleControlPoint_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QVector>

#include <QSharedPointer>

#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "ControlPoint.h"
#include "SparseBlockMatrix.h"
#include "SurfacePoint.h"

namespace Isis {

  class ControlMeasure;
  class LinearAlgebra;
  class SparseBlockMatrix;

  /**
   * This class holds information about a control point that BundleAdjust needs to run correctly.
   *
   * This class was created to extract functionality from BundleAdjust and wrap a ControlPoint
   * with the extra necessary information to correctly perform a bundle adjustment.
   *
   * Note that only non-ignored control points should be used to construct a BundleControlPoint.
   * Similarly, a BundleControlPoint should only contain non-ignored control measures.
   *
   * @author 2014-05-22 Ken Edmundson
   *
   * @internal
   *   @history 2014-05-22 Ken Edmundson - Original version.
   *   @history 2015-02-20 Jeannie Backer - Added unitTest.  Reformatted output
   *                           strings. Brought closer to ISIS coding standards.
   *   @history 2016-06-27 Jesse Mapel - Updated documentation and ISIS coding standards in
   *                           preparation for merging IPCE into ISIS.  Fixes #4075.
   *   @history 2016-08-15 Ian Humphrey - Added computeResiduals(), setNumberOfRejectedMeasures(),
   *                           setRejected(), zeroNumberOfRejectedMeasures(),
   *                           numberOfRejectedMeasures(), residualRms(), and type(). Modified
   *                           numberMeasures() to return this->size(), since only non-ignored
   *                           control measures will be added to this BundleControlPoint.
   *                           Updated unit test for these methods (except computeResiduals).
   *                           References #4173.
   *   @history 2016-08-15 Jesse Mapel - Changed contained member data type to a shared pointer.
   *                           Added wrapper methods for several ControlPoint methods.
   *                           Fixes #4159.
   *   @history 2016-10-27 Tyler Wilson - Modified formatRadiusAprioriSigmaString, formatAprioriSigmaString,
   *                          and formatBundleOutputDetailString to accept a third argument (bool solveRadius)
   *                          with a default value = false.  References #4317.
   *   @history 2017-07-26 Debbie A. Cook - Added BundleSettings and metersToRadians as arguments
   *                           to constructor and moved setWeights call from BundleAdjust::init into
   *                           constructor.   Added m_bundleControlPointCoordinateType.  This option
   *                           determines how control point coordinates are entered into
   *                           BundleControlPoint, interpreted throughout the adjustment, and
   *                           output. The coordinate type needs to be in this class, because
   *                           BundleControlPoints are created without a parent control net and added to
   *                           a control net later.  Made format methods generic in regards to coordinate type.
   *                           Added utility method setSigmaWeightFromGlobals.
   *                           Merged methods formatLatitudeAdjustedSigmaString,
   *                           formatLongitudeAdjustedSigmaString, and formatRadiusAdjustedSigmaString
   *                           into a single generic coordinate method with an additional argument
   *                           for the coordinate index.  Did a similar merge for the family of
   *                           methods like formatLatitudeAprioriSigmaString.  Moved some of the
   *                           functionality from BundleAdjust to this class as a new method
   *                           applyParameterCorrections.  Also had to move BundleAdjust method
   *                           productAlphaAV to this class to support applyParameterCorrections.
   *                           References #4649 and #501.
   *  @history 2017-08-24 Debbie A. Cook - Revised output units to be compatible with output from
   *                           previous versions, corrected units throughout in comments and code.
   *                           Reference #4649 and #501.
   *  @history 2018-01-05 Debbie A. Cook - Added new members m_coordTypeReports and
   *                           m_coordTypeBundle to copy method.  Reference #4649 and #501.
   *  @history 2018-05-31 Debbie A. Cook - Moved code from BundleAdjust::applyParameterCorrections
   *                           pertaining to updating the adjusted surface point and method productAlphaAv into new
   *                           methods applyParameterCorrections, updateAdjustedSurfacePointLatitudinally, and
   *                           updateAdjustedSurfacePointRectangularly in BundleControlPoint.  Reference
   *                           #4649 and #501.
   *  @history 2018-09-28 Debbie A. Cook - Removed the metersToRadians argument from
   *                           the constructor and from the setWeights method since we are now
   *                           using the local radius of the point to convert lat/lon sigmas
   *                           from meters to radians.  References #4649 and #501.
  */
  class BundleControlPoint : public QVector<BundleMeasureQsp> {

    public:
      // default constructor
      BundleControlPoint(BundleSettingsQsp bundleSettings,
                         ControlPoint *point);
      // copy constructor
      BundleControlPoint(const BundleControlPoint &src);

      //destructor
      virtual ~BundleControlPoint();

      // equals operator
      BundleControlPoint &operator=(const BundleControlPoint &src);// ??? not implemented

      // copy method
      void copy(const BundleControlPoint &src);

      // mutators
      BundleMeasureQsp addMeasure(ControlMeasure *controlMeasure);
      void computeResiduals();
      void setAdjustedSurfacePoint(SurfacePoint surfacePoint);
      void setNumberOfRejectedMeasures(int numRejected);
      void setRejected(bool reject);
      void setWeights(const BundleSettingsQsp settings);
      void setSigmaWeightFromGlobals(double gSigma, int index);
      void setSigmaWeightFromGlobals(double gSigma, int index, double cFactor);
      void zeroNumberOfRejectedMeasures();
      void productAlphaAV(double alpha,
                          SparseBlockMatrix &sparseNormals,
                          // boost::numeric::ublas::bounded_vector< double, 3 >  &v2,
                          // SparseBlockRowMatrix                                &Q,
                          LinearAlgebra::Vector                               &v1);
      virtual void applyParameterCorrections(LinearAlgebra::Vector imageSolution,
                                             SparseBlockMatrix &sparseNormals,
                                             const BundleTargetBodyQsp target);
      double vtpv();
      double vtpvMeasures();

      // accessors
      ControlPoint *rawControlPoint() const;
      bool isRejected() const;
      int numberOfMeasures() const;
      int numberOfRejectedMeasures() const;
      double residualRms() const;
      SurfacePoint adjustedSurfacePoint() const;
      QString id() const;
      ControlPoint::PointType type() const;
      SurfacePoint::CoordinateType coordTypeReports() const;
      SurfacePoint::CoordinateType coordTypeBundle() const;
      boost::numeric::ublas::bounded_vector< double, 3 > &corrections();
      boost::numeric::ublas::bounded_vector< double, 3 > &aprioriSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 > &adjustedSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 > &weights();
      boost::numeric::ublas::bounded_vector<double, 3> &nicVector();
      SparseBlockRowMatrix &cholmodQMatrix();

      // string format methods
      QString formatBundleOutputSummaryString(bool errorPropagation) const;
      QString formatBundleOutputDetailString(bool errorPropagation, bool solveRadius=false) const;
      QString formatBundleLatitudinalOutputDetailString(bool errorPropagation,
                                                        bool solveRadius=false) const;
      QString formatBundleRectangularOutputDetailString(bool errorPropagation) const;
      QString formatValue(double value, int fieldWidth, int precision) const;
      QString formatAprioriSigmaString(SurfacePoint::CoordIndex index, int fieldWidth,
                                       int precision, bool solveRadius=false) const;
      QString formatCoordAprioriSigmaString(SurfacePoint::CoordIndex index, int fieldWidth,
                                            int precision, bool solveRadius=false) const;
      QString formatAdjustedSigmaString(SurfacePoint::CoordIndex, int fieldWidth, int precision,
                                        bool errorPropagation) const;
      QString formatCoordAdjustedSigmaString(SurfacePoint::CoordIndex, int fieldWidth, int precision,
                                                bool errorPropagation) const;

    protected:
      //!< pointer to the control point object this represents
      ControlPoint *m_controlPoint;

    private:
      // methods
      void updateAdjustedSurfacePointLatitudinally(const BundleTargetBodyQsp target);
      void updateAdjustedSurfacePointRectangularly();

      //! corrections to point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_corrections;
      //! apriori sigmas for point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_aprioriSigmas;
      //! adjusted sigmas for point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_adjustedSigmas;
      //! weights for point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_weights;
      //! array of NICs (see Brown, 1976)
      boost::numeric::ublas::bounded_vector<double, 3> m_nicVector;
      //! The CholMod matrix associated with this point
      SparseBlockRowMatrix m_cholmodQMatrix;
      //! BundleControlPoint coordinate type
      SurfacePoint::CoordinateType m_coordTypeReports;
      SurfacePoint::CoordinateType m_coordTypeBundle;
  };

  // typedefs
  //! Definition for BundleControlPointQSP, a shared pointer to a BundleControlPoint.
  typedef QSharedPointer<BundleControlPoint> BundleControlPointQsp;
}

#endif // BundleControlPoint_h
