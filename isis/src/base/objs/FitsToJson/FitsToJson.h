#ifndef FitsToJson_h
#define FitsToJson_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "FileName.h"

#include <nlohmann/json.hpp>
#include <fstream>

namespace Isis {
  nlohmann::json fitsToJson(FileName fitsFile);
  nlohmann::json fitsToJson(std::ifstream &fileStream);
}

#endif
