#ifndef BundleControlPoint_h
#define BundleControlPoint_h
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

#include <QVector>

#include <QSharedPointer>

#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "ControlPoint.h"
#include "SparseBlockMatrix.h"
#include "SurfacePoint.h"

namespace Isis {

  class ControlMeasure;

  /**
   * This class holds information about a control point that BundleAdjust needs to run corretly.
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
   */
  class BundleControlPoint : public QVector<BundleMeasureQsp> {

    public:
      BundleControlPoint(ControlPoint *point); // default constructor
      BundleControlPoint(const BundleControlPoint &src);
      ~BundleControlPoint();

      // copy
      BundleControlPoint &operator=(const BundleControlPoint &src);// ??? not implemented
      void copy(const BundleControlPoint &src);

      // mutators
      BundleMeasureQsp addMeasure(ControlMeasure *controlMeasure);
      void computeResiduals();
      void setAdjustedSurfacePoint(SurfacePoint surfacePoint);
      void setNumberOfRejectedMeasures(int numRejected);
      void setRejected(bool reject);
      void setWeights(const BundleSettingsQsp settings, double metersToRadians);
      void zeroNumberOfRejectedMeasures();

      // accessors
      ControlPoint *rawControlPoint() const;
      bool isRejected() const;
      int numberOfMeasures() const;
      int numberOfRejectedMeasures() const;
      double residualRms() const;
      SurfacePoint adjustedSurfacePoint() const;
      QString id() const;
      ControlPoint::PointType type() const;
      boost::numeric::ublas::bounded_vector< double, 3 > &corrections();
      boost::numeric::ublas::bounded_vector< double, 3 > &aprioriSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 > &adjustedSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 > &weights();
      boost::numeric::ublas::bounded_vector<double, 3> &nicVector();
      SparseBlockRowMatrix &cholmodQMatrix();

      // string format methods
      QString formatBundleOutputSummaryString(bool errorPropagation) const;
      QString formatBundleOutputDetailString(bool errorPropagation, double RTM, bool solveRadius=false) const;
      QString formatValue(double value, int fieldWidth, int precision) const;
      QString formatAprioriSigmaString(int type, int fieldWidth, int precision, bool solveRadius=false) const;
      QString formatLatitudeAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatLongitudeAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatRadiusAprioriSigmaString(int fieldWidth, int precision, bool solveRadius=false) const;
      QString formatAdjustedSigmaString(int type, int fieldWidth, int precision,
                                        bool errorPropagation) const;
      QString formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                bool errorPropagation) const;
      QString formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                 bool errorPropagation) const;
      QString formatRadiusAdjustedSigmaString(int fieldWidth, int precision, 
                                              bool errorPropagation) const;

    private:
      //!< pointer to the control point object this represents
      ControlPoint *m_controlPoint;

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
  };

  // typedefs
  //! Definition for BundleControlPointQSP, a shared pointer to a BundleControlPoint.
  typedef QSharedPointer<BundleControlPoint> BundleControlPointQsp;
}

#endif // BundleControlPoint_h

