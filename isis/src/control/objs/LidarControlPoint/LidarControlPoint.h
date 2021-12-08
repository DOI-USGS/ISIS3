#ifndef LidarControlPoint_h
#define LidarControlPoint_h

/**
 * @file
 * $Revision: 1.14 $
 * $Date: 2009/09/08 17:38:17 $
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

#include <algorithm>
#include <functional>

#include <QPointer>
#include <QSharedPointer>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "iTime.h"

namespace Isis {
  /**
   * @brief A lidar control ControlPoint
   * 
   * A lidar control point that extends from ControlPoint. Currently only works for LOLA data
   * 
   * @author 2018-01-29 Makayla Shepherd
   * 
   * @see ControlPoint
   *
   * @internal
   *   @history 2018-01-29 Makayla Shepherd Original version
   *   @history 2018-02-09 Ken Edmundson Added typedef forLidarControlPointQsp
   *   @history 2018-03-18 Debbie A. Cook Added Simultaneous measures 
   *   @history 2019-02-23 Debbie A. Cook Added Functor Predicate struct to sort
   *                                        based on Id.  This is needed for getting consistent 
   *                                        output for comparing test data. References #5343.
   *   @history 2019-04-28 Ken Edmundson Modified ComputeResiduals method to correct lidar measure
   *                                        by it's residuals, i.e. by simply moving the measure to
   *                                        it's current, back-projected location in the image.
   */
  
  class LidarControlPoint : public ControlPoint {
    
  public:

    LidarControlPoint();
    
    ~LidarControlPoint();
    
    ControlPoint::Status setRange(double range);
    ControlPoint::Status setSigmaRange(double sigmaRange);
    ControlPoint::Status setTime(iTime time);
    ControlPoint::Status addSimultaneous(QString newSerial);

    ControlPoint::Status ComputeResiduals();

    //  Functor predicate for sorting LidarControlPoints
    struct LidarControlPointLessThanFunctor :
      public std::binary_function<QSharedPointer<LidarControlPoint>,
                                                      QSharedPointer<LidarControlPoint>,
                                                      bool> {
      bool operator() ( QSharedPointer<LidarControlPoint> lcp1,
                                    QSharedPointer<LidarControlPoint> lcp2)
          {
           return (lcp1->GetId() < lcp2->GetId());
          }
    };

    double range();
    double sigmaRange();
    iTime time();
    QStringList snSimultaneous() const;
    bool isSimultaneous(QString serialNumber);

  private:
    double m_range;                //!< range
    double m_sigmaRange;           //!< range sigma
    iTime m_time;                  //!< time lidar point was acquired
    QStringList *m_snSimultaneous; //!< serial number(s) of simultaneous image(s)
    
  };

  // typedefs
  //! Definition for a shared pointer to a LidarControlPoint.
  typedef QSharedPointer<LidarControlPoint> LidarControlPointQsp;
}

#endif
    
