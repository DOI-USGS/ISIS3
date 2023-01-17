#ifndef CsmSensor_h
#define CsmSensor_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SensorUtilities.h"

#include "RasterGM.h"

#include "Orientations.h"

namespace Isis {
  /**
   * Implementation of the SensorUtilities::Sensor interface for a CSM RasterGM
   * model.
   *
   * This also incorporates an ALE Orientations object to hanlde the transformation
   * from object space, which CSM only operates in, to the universal J2000 reference
   * frame.
   */
  class CsmSensor : SensorUtilities::Sensor {
    public:
      CsmSensor(csm::RasterGM* cam, ale::Orientations *j2000Rot);

      virtual SensorUtilities::ObserverState getState(const SensorUtilities::ImagePt &imagePoint);
      virtual SensorUtilities::ObserverState getState(const SensorUtilities::GroundPt3D &groundPt);
    private:
      // The CSM model to dispatch to
      csm::RasterGM* m_cam;
      /**
       * The time dependent rotation from object space to J2000. This should
       * use the same time range as the CSM model.
       */
      ale::Orientations* m_j2000Rot;
  };
};

#endif
