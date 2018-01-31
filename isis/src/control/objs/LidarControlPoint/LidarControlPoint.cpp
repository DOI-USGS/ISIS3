#include "LidarControlPoint.h"

#include <QString>

#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "IException.h"
#include "iTime.h"

using namespace std;

namespace Isis {
  

  /**
   * Constructs a LidarControlPoint with the given time, range, and sigma range.
   * 
   * @param time The time ths point was taken.
   * @param range The range of the point.
   * @param sigmaRange The sigmas od the range.
   */
  LidarControlPoint::LidarControlPoint(iTime time, double range, double sigmaRange) {
    m_time = time;
    m_range = range;
    m_sigmaRange = sigmaRange;
  }
  
  
  /**
   * Destructor
   */
  LidarControlPoint::~LidarControlPoint() {
  }
  
  
  /**
   * Set the time of the LidarControlPoint
   * 
   * @param time The time to set
   */
  ControlPoint::Status LidarControlPoint::setTime(iTime time) {
    if (IsEditLocked()) {
      return ControlPoint::Status::PointLocked;
    }
    m_time = time;
    return ControlPoint::Status::Success;
  }

  
  /**
   * Set the range of the LidarControlPoint
   * 
   * @param range The range to set
   */
  ControlPoint::Status LidarControlPoint::setRange(double range) {
    if (IsEditLocked()) {
      return ControlPoint::Status::PointLocked;
    }
    m_range = range;
    return ControlPoint::Status::Success;
  }
  
  
  /**
   * Sets the sigma range
   * 
   * @param sigmaRange The sigma range to set
   */
  ControlPoint::Status LidarControlPoint::setSigmaRange(double sigmaRange) {
    if (IsEditLocked()) {
      return ControlPoint::Status::PointLocked;
    }
    m_sigmaRange = sigmaRange;
    return ControlPoint::Status::Success;
  }
  
  
  /**
   * Returns the range of the point
   * 
   * @return double The range
   */
  double LidarControlPoint::range() {
    return m_range;
  }
  
  
  /**
   * Returns the time of the point
   * 
   * @return double The time
   */
  iTime LidarControlPoint::time() {
    return m_time;
  }
  
  
  /**
   * Returns the sigma range of the point
   * 
   * @return double The sigma range
   */
  double LidarControlPoint::sigmaRange() {
    return m_sigmaRange;
  }
}
