#ifndef IsisIlluminator_h
#define IsisIlluminator_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SensorUtilities.h"

namespace Isis {
  class SpicePosition;

  /**
   * Implementation of SensorUtilities::Illuminator backed by an ISIS SpicePosition.
   */
  class IsisIlluminator : SensorUtilities::Illuminator {
    public:
      IsisIlluminator(SpicePosition* pos);

      SensorUtilities::Vec position(double time);
    private:
      SpicePosition* m_pos;
  };
};

#endif
