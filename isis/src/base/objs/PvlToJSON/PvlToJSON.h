#ifndef PvlToJSON_h
#define PvlToJSON_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <nlohmann/json.hpp>

namespace Isis {
  class Pvl;
  class PvlContainer;
  class PvlGroup;
  class PvlKeyword;
  class PvlObject;

  nlohmann::json pvlToJSON(Pvl &pvl);
  nlohmann::json pvlContainerToJSON(PvlContainer &container);
  nlohmann::json pvlKeywordToJSON(PvlKeyword &keyword);
  nlohmann::json pvlGroupToJSON(PvlGroup &group);
  nlohmann::json pvlObjectToJSON(PvlObject &object);
}

#endif
