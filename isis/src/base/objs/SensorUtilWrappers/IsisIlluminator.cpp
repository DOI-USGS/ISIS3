/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IsisIlluminator.h"

#include "SpicePosition.h"

using namespace std;

namespace Isis {
  /**
   * Create an IsisIlluminator that wraps a SpicePosition.
   */
  IsisIlluminator::IsisIlluminator (SpicePosition* pos) {
    m_pos = pos;
  }


  /**
   * Get the position in meters at a given time.
   */
  SensorUtilities::Vec IsisIlluminator::position(double time) {
    double oldTime = m_pos->EphemerisTime();
    bool timeChanged = oldTime != time;
    if (timeChanged) {
      m_pos->SetEphemerisTime(time);
    }
    vector<double> coord = m_pos->Coordinate();
    if (timeChanged) {
      m_pos->SetEphemerisTime(oldTime);
    }
    // ISIS uses Km, so convert to meters
    return {coord[0] * 1000.0, coord[1] * 1000.0, coord[2] * 1000.0};
  }
}