#ifndef IsisSensor_h
#define IsisSensor_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SensorUtilities.h"

namespace Isis {
  class Camera;

  /**
   * Implementation of SensorUtilities::Sensor backed by an ISIS camera model.
   * This includes a full shape model and sun, but they are not used except when
   * getting the state from a ground point, the ground point is mapped back onto
   * the surface model prior to back-projection.
   */
  class IsisSensor : SensorUtilities::Sensor {
    public:
      IsisSensor(Camera* cam);

      virtual SensorUtilities::ObserverState getState(const SensorUtilities::ImagePt &imagePoint);
      virtual SensorUtilities::ObserverState getState(const SensorUtilities::GroundPt3D &groundPt);
    private:
      Camera* m_cam;
  };
};

#endif
