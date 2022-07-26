#include "LidarControlPoint.h"

#include <QString>
#include <QVector>

#include "Camera.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
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
   * @return QStringList The list of serial numbers
   */
  QStringList LidarControlPoint::snSimultaneous() const{
    return *m_snSimultaneous;
  }


  /**
   * Determines if input serial number is in list of simultaneous measure serial numbers.
   *
   * @param serialNumber Serial number to check.
   *
   * @return bool true if serialNumber is contained in m_snSimultaneous.
   */
  bool LidarControlPoint::isSimultaneous(QString serialNumber) {

    if (m_snSimultaneous->contains(serialNumber)) {
      return true;
    }

    return false;
  }


  /**
   * TODO: clean up code and document why this is different from the ComputeResiduals method for a
   *       normal photogrammetric control point.
   * TODO: compute residuals is a misnomer, we are really moving the measure to it's new back-
   *       projected location in the image
   * TODO: should code be special for radar?
   *
   *     *** Warning:  Only BundleAdjust and its applications should be
   *                   using this method.
   *
   * @history 2019-02-26 Ken Edmundson, initial version.
   */
  ControlPoint::Status LidarControlPoint::ComputeResiduals() {
    if (IsIgnored()) {
      return Failure;
    }

    ControlPoint::PointModified();

    double newSampleDistorted = 0.0;
    double newLineDistorted = 0.0;

    // Loop for each measure to compute the error
    QList<QString> keys = measures->keys();

    for (int j = 0; j < keys.size(); j++) {
      ControlMeasure *measure = (*measures)[keys[j]];
      if (measure->IsIgnored()) {
        continue;
      }

      // back project the adjusted surface point with SPICE updated from current bundle iteration
      Camera* camera = measure->Camera();
      if (camera->SetGround(GetAdjustedSurfacePoint())) {
        newSampleDistorted = camera->Sample();
        newLineDistorted = camera->Line();
        measure->SetRejected(false);
      }
      // what happens if SetGround fails (e.g. if back-projection is off the image?
      else {
        measure->SetRejected(true);
        measure->SetResidual(0.0, 0.0);
        continue;
      }

      // set measures sample, line to the back-projected location
      measure->SetCoordinate(newSampleDistorted, newLineDistorted);

      double measuredUndistortedFPx = camera->DistortionMap()->UndistortedFocalPlaneX();
      double measuredUndistortedFPy = camera->DistortionMap()->UndistortedFocalPlaneY();
      measure->SetFocalPlaneMeasured(measuredUndistortedFPx,measuredUndistortedFPy);
      measure->SetFocalPlaneComputed(measuredUndistortedFPx, measuredUndistortedFPy);

      measure->SetResidual(0.0, 0.0);
    }

    return Success;
  }
}
