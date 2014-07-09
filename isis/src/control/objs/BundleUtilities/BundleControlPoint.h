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

#include "BundleMeasure.h"
#include "SparseBlockMatrix.h"
#include "SurfacePoint.h"

namespace Isis {

  class BundleSettings;
  class ControlMeasure;
  class ControlPoint;

  /**
   * @author 2014-05-22 Ken Edmundson
   *
   * @internal
   *   @history 2014-05-22 Ken Edmundson
   */
  class BundleControlPoint : public QVector<BundleMeasure*> {

    public:
      BundleControlPoint(ControlPoint* point); // default constructor
      ~BundleControlPoint();

      // copy constructor
      BundleControlPoint(const BundleControlPoint& src);

      BundleControlPoint& operator=(const BundleControlPoint& src);

      void copy(const BundleControlPoint& src);

      BundleMeasure *addMeasure(ControlMeasure *controlMeasure);
      void setWeights(const BundleSettings *settings, double metersToRadians);

      void setAdjustedSurfacePoint(SurfacePoint surfacePoint);

      ControlPoint *getRawControlPoint();

      // string format methods
      QString formatBundleOutputSummaryString(bool errorPropagation);
      QString formatBundleOutputDetailString(bool errorPropagation, double RTM);
      QString formatLatitudeAprioriSigmaString(int fieldWidth, int precision);
      QString formatLongitudeAprioriSigmaString(int fieldWidth, int precision);
      QString formatRadiusAprioriSigmaString(int fieldWidth, int precision);
      QString formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                bool errorPropagation);
      QString formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                 bool errorPropagation);
      QString formatRadiusAdjustedSigmaString(int fieldWidth, int precision, bool errorPropagation);

      bool isRejected();
      int numberMeasures();
      SurfacePoint getAdjustedSurfacePoint() const;
      QString getId() const;
      boost::numeric::ublas::bounded_vector< double, 3 >& corrections();
      boost::numeric::ublas::bounded_vector< double, 3 >& aprioriSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 >& adjustedSigmas() { return m_adjustedSigmas; }
      boost::numeric::ublas::bounded_vector< double, 3 >& weights();
      boost::numeric::ublas::bounded_vector<double, 3>& nicVector() { return m_nicVector; }         //!< array of NICs (see Brown, 1976)
      SparseBlockRowMatrix& cholmod_QMatrix() { return m_cholmod_QMatrix; }

    private:
      ControlPoint* m_controlPoint;

      boost::numeric::ublas::bounded_vector< double, 3 > m_corrections;                             //!< corrections to point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_aprioriSigmas;                           //!< apriori sigmas for point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_adjustedSigmas;                          //!< adjusted sigmas for point parameters
      boost::numeric::ublas::bounded_vector< double, 3 > m_weights;                                 //!< weights for point parameters

      boost::numeric::ublas::bounded_vector<double, 3> m_nicVector;
      SparseBlockRowMatrix m_cholmod_QMatrix;
  };
}

#endif // BundleControlPoint_h

