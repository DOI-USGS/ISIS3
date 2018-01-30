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
   * Empty LidarControlPoint constructor
   */
  LidarControlPoint::LidarControlPoint() {
    m_time = -1;
    m_range = -1;
    m_sigmaRange = -1;
  }
  
  
  /**
   * Constructs a LidarControlPoint with the given time, range, and sigma range.
   * 
   * @param time The time ths point was taken.
   * @param range The range of the point.
   * @param sigmaRange The sigmas od the range.
   */
  LidarControlPoint::LidarControlPoint(double time, double range, double sigmaRange) {
    m_time = iTime(QString::number(time));
    m_range = range;
    m_sigmaRange = sigmaRange;
  }
  
  
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
   * Constructs a LidarControlPoint with the given time, range, and sigma range.
   * 
   * @param time The time ths point was taken.
   * @param range The range of the point.
   * @param sigmaRange The sigmas od the range.
   */
  LidarControlPoint::LidarControlPoint(QString time, double range, double sigmaRange) {
    m_time = iTime(time);
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
  void LidarControlPoint::setTime(double time) {
    if (time < 0.0) {
      QString msg = "The time must be greater then 0.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
      return;
    }
    m_time = iTime(QString::number(time));
  }
  
  
  /**
   * Set the time of the LidarControlPoint
   * 
   * @param time The time to set
   */
  void LidarControlPoint::setTime(iTime time) {
    if (time < 0.0) {
      QString msg = "The time must be greater then 0.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
      return;
    }
    m_time = time;
  }
  
  
  /**
   * Set the time of the LidarControlPoint
   * 
   * @param time The time to set
   */
  void LidarControlPoint::setTime(QString time) {
    if (time.toDouble() < 0.0) {
      QString msg = "The time must be greater then 0.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
      return;
    }
    m_time = iTime(time);
  } 
  
  
  /**
   * Set the range of the LidarControlPoint
   * 
   * @param range The range to set
   */
  void LidarControlPoint::setRange(double range) {
    if (range < 0.0) {
      QString msg = "The range must be greater then 0.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
      return;
    }
    m_range = range;
  }
  
  
  /**
   * Sets the sigma range
   * 
   * @param sigmaRange The sigma range to set
   */
  void LidarControlPoint::setSigmaRange(double sigmaRange) {
    m_sigmaRange = sigmaRange;
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
