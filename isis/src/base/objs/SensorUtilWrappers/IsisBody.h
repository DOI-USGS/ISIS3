#ifndef IsisBody_h
#define IsisBody_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SensorUtilities.h"

namespace Isis {
  class SpiceRotation;

  /**
   * Implementation of SensorUtilities::Body backed by an ISIS SpiceRotation.
   */
  class IsisBody : SensorUtilities::Body {
    public:
      IsisBody(SpiceRotation* rot);

      std::vector<double> rotation(double time);

      SensorUtilities::Vec fixedVector(SensorUtilities::Vec pos);
    private:
      SpiceRotation* m_rot;
  };
};

#endif