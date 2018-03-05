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
   */
  LidarControlPoint::LidarControlPoint() {
    m_time = iTime();
    m_range = -1.0;
    m_sigmaRange = -1.0;
    m_snSimultaneous = NULL;

    m_snSimultaneous = new QStringList;
  }
  
  
  /**
   * Destructor
   */
  LidarControlPoint::~LidarControlPoint() {
    if (m_snSimultaneous) {
      delete m_snSimultaneous;
      m_snSimultaneous = NULL;
    }
    
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
   * Add a measure to the list of simultaneous images of a  LidarControlPoint
   * 
   * @param time The serial number of the simultaneous image to add
   */
  ControlPoint::Status LidarControlPoint::addSimultaneous(QString newSerial) {
    m_snSimultaneous->append(newSerial);
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
  
  
  /**
   * Returns the list of serial numbers of simultaneous images of the Lidar point
   * 
   * @return QList The list of serial numbers
   */
  QList < QString > LidarControlPoint::snSimultaneous() const{
    return *m_snSimultaneous;
  }
}
