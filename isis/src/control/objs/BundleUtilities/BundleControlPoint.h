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
   *   @history 2014-05-22 Ken Edmundson - Original version.
   *   @history 2015-02-20 Jeannie Backer - Added unitTest.  Reformatted output
   *                           strings. Brought closer to ISIS coding standards.
   *   @history 2015-08-13 Jeannie Backer - Added some documentation to class variables.
   *                           Changed intitial value of aprioriSigmas to be Null instead of
   *                           zero to be consistent with other ISIS bundle classes.
   */
  class BundleControlPoint : public QVector<BundleMeasure*> {

    public:
      BundleControlPoint(ControlPoint *point); // default constructor
      BundleControlPoint(const BundleControlPoint &src);
      ~BundleControlPoint();

      // copy
      BundleControlPoint &operator=(const BundleControlPoint &src);// ??? not implemented
      void copy(const BundleControlPoint &src);

      // mutators
      BundleMeasure *addMeasure(ControlMeasure *controlMeasure);
      void setWeights(const BundleSettings *settings, double metersToRadians);
      void setAdjustedSurfacePoint(SurfacePoint surfacePoint);

      // accessors
      ControlPoint *rawControlPoint() const;
      bool isRejected() const;
      int numberMeasures() const;
      SurfacePoint getAdjustedSurfacePoint() const; // TODO: Rename this method without "get" to meet coding standards
      QString getId() const; // TODO: Rename this method without "get" to meet coding standards
      boost::numeric::ublas::bounded_vector< double, 3 > &corrections();
      boost::numeric::ublas::bounded_vector< double, 3 > &aprioriSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 > &adjustedSigmas();
      boost::numeric::ublas::bounded_vector< double, 3 > &weights();
      boost::numeric::ublas::bounded_vector<double, 3> &nicVector(); // array of NICs (see Brown, 1976)
      SparseBlockRowMatrix &cholmod_QMatrix();

      // string format methods
      QString formatBundleOutputSummaryString(bool errorPropagation) const;
      QString formatBundleOutputDetailString(bool errorPropagation, double RTM) const;
      QString formatValue(double value, int fieldWidth, int precision) const;
      QString formatAprioriSigmaString(int type, int fieldWidth, int precision) const;
      QString formatLatitudeAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatLongitudeAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatRadiusAprioriSigmaString(int fieldWidth, int precision) const;
      QString formatAdjustedSigmaString(int type, int fieldWidth, int precision,
                                        bool errorPropagation) const;
      QString formatLatitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                bool errorPropagation) const;
      QString formatLongitudeAdjustedSigmaString(int fieldWidth, int precision,
                                                 bool errorPropagation) const;
      QString formatRadiusAdjustedSigmaString(int fieldWidth, int precision, 
                                              bool errorPropagation) const;

    private:
      ControlPoint *m_controlPoint;

      boost::numeric::ublas::bounded_vector< double, 3 > m_corrections;    /**< corrections to point
                                                                                parameters.*/
      boost::numeric::ublas::bounded_vector< double, 3 > m_aprioriSigmas;  /**< a priori sigmas for
                                                                                point parameters.*/
      boost::numeric::ublas::bounded_vector< double, 3 > m_adjustedSigmas; /**< adjusted sigmas for
                                                                                point parameters.*/
      boost::numeric::ublas::bounded_vector< double, 3 > m_weights;        /**< weights for point
                                                                                parameters.*/

      boost::numeric::ublas::bounded_vector<double, 3> m_nicVector;// array of NICs (see Brown, 1976)

      SparseBlockRowMatrix m_cholmod_QMatrix;
  };
}

#endif // BundleControlPoint_h

