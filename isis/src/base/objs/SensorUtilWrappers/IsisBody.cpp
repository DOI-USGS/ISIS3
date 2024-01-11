/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IsisBody.h"

#include "SpiceRotation.h"

using namespace std;

namespace Isis {
  /**
   * Create an IsisBody that wraps a SpiceRotation.
   */
  IsisBody::IsisBody (SpiceRotation* rot) {
    m_rot = rot;
  }


  std::vector<double> IsisBody::rotation(double time) {
    double oldTime = m_rot->EphemerisTime();
    bool timeChanged = oldTime != time;
    if (timeChanged) {
      m_rot->SetEphemerisTime(time);
    }
    vector<double> bodyRotMat = m_rot->Matrix();
    if (timeChanged) {
      m_rot->SetEphemerisTime(oldTime);
    }
    // ISIS uses Km, so convert to meters
    for (int i = 0; i < 9; i++ ){
      bodyRotMat[i] = bodyRotMat[i] * 1000.0;
    }
    return bodyRotMat;
  }

  SensorUtilities::Vec IsisBody::fixedVector(SensorUtilities::Vec pos) {
    std::vector<double> instPos(pos);
    std::vector<double> sB = m_rot->ReferenceVector(instPos);

    return {sB[0] * 1000.0, sB[1] * 1000.0, sB[2] * 1000.0};
  }
}