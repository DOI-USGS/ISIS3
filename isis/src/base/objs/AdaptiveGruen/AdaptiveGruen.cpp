/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AdaptiveGruen.h"

/**
 * @brief AdaptiveGruen algorithm entry point for the plugin
 *
 * @param pvl Input registration config file containing algorithm parameters
 * @return Isis::AutoReg* Returns a pointer to the plugin registration object
 */
extern "C" Isis::AutoReg *AdaptiveGruenPlugin(Isis::Pvl &pvl) {
  return new Isis::AdaptiveGruen(pvl);
}


