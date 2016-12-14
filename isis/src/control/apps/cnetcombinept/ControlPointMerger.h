#ifndef ControlPointMerger_h
#define ControlPointMerger_h
/**
 * @file
 * $Revision: 1.0 $ 
 * $Date: 2014/02/27 18:49:25 $ 
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

#include <QtGlobal>
#include <QList>
#include <QString>
#include <QScopedPointer>
#include <QSharedPointer>

#include <opencv2/opencv.hpp>
#include <boost/assert.hpp>

#include "ControlMeasureLogData.h"
#include "MeasurePoint.h"
#include "Statistics.h"

namespace Isis {

/**
 * @brief Combine control points based upon distance criteria 
 *  
 * This class will collect and compute ControlPoint candidates that are within 
 * a pixel tolerance for merging into a single control point. 
 *  
 * The criteria applied computes statistics on all common measures within the 
 * control point for image coordinate searches.
 *  
 * @author  2015-10-11 Kris Becker
 *  
 * @internal 
 *   @history 2015-10-11 Kris Becker - Original Version
 *   @history 2016-12-06 Jesse Mapel - Updated documentation.  References #4558.
 *   @history 2016-12-06 Jesse Mapel - Moved implementation to cpp file.  References #4558.
 */

class ControlPointMerger {
  public:

    ControlPointMerger();
    ControlPointMerger(const double image_tolerance);
    virtual ~ControlPointMerger();

    int size() const;
    void clear();
    int apply(ControlPoint *point, QList<MeasurePoint> &candidates);
    int merge(ControlPoint *source, ControlPoint *candidate, const Statistics &stats);

  private:
    double               m_image_tolerance; /**!< The image distance tolerance for determining
                                                  if control points should be merged.*/
    QList<MeasurePoint>  m_merged;          /**!< The number of control points that have been
                                                  merged by the ControlPointMerger.*/

    inline bool isValid(const ControlMeasure &m) const;
    inline double image_distance(const ControlMeasure &source, 
                                 const ControlMeasure &candidate) const;
};

}  // namespace Isis
#endif
