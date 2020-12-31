#ifndef XmlToJson_h
#define XmlToJson_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "QDomElement"
#include "QString"

#include <nlohmann/json.hpp>

namespace Isis {

nlohmann::ordered_json xmlToJson(QString xmlFile);
nlohmann::ordered_json xmlToJson(QDomDocument& doc);

// These are not intended to be used directly, helper functions
nlohmann::ordered_json convertXmlToJson(QDomElement& node, nlohmann::ordered_json& output);
nlohmann::ordered_json convertLastChildNodeToJson(QDomElement& element);
}

#endif
