/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CsmBundleObservationSolveSettings.h"

namespace Isis {

  /**
   * Create the solution settings for a CSM observation
   *
   * @param solveSet the CSM set of parameters to solve for
   */
  CsmBundleObservationSolveSettings::CsmBundleObservationSolveSettings(csm::param::Set solveSet)
    : m_solveSet(solveSet) {}

  /**
   * Return the set of parameters to solve for
   *
   * @returns @b csm::param::Set
   */
  csm::param::Set CsmBundleObservationSolveSettings::solveSet() {
    return m_solveSet;
  }
}