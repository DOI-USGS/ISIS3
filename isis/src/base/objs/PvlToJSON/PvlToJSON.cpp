/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "PvlToJSON.h"

#include <nlohmann/json.hpp>

#include "Pvl.h"
#include "PvlKeyword.h"

using json = nlohmann::json;
using namespace std;

namespace Isis {

  /**
   * Convert the contents of a PvlKeyword to a JSON object.
   * All values from the keyword will be stored in "Value", all units will be
   * stored in "Units", and all comments will be stored in "Comment".
   *
   * @param keyword The keyword to convert
   * @return @b json The contents of the keyword as a JSON object
   */
  json pvlKeywordToJSON(PvlKeyword &keyword) {
    json jsonKeyword;

    // Convert values
    if (keyword.size() == 1) {
      jsonKeyword["Value"] = keyword[0].toStdString();
    }
    else if (keyword.size() > 1) {
      json valueList;
      for (int i = 0; i < keyword.size(); i++) {
        valueList.push_back(keyword[i].toStdString());
      }
      jsonKeyword["Value"] = valueList;
    }

    // Optionally convert units
    if (keyword.size() == 1 && !keyword.unit(0).isEmpty()) {
      jsonKeyword["Units"] = keyword.unit(0).toStdString();
    }
    else if (keyword.size() > 1 && !keyword.unit(0).isEmpty()) {
      json valueList;
      for (int i = 0; i < keyword.size(); i++) {
        valueList.push_back(keyword.unit(i).toStdString());
      }
      jsonKeyword["Units"] = valueList;
    }

    // Optionally convert comments
    if (keyword.comments() == 1) {
      jsonKeyword["Comment"] = keyword.comment(0).toStdString();
    }
    else if (keyword.comments() > 1) {
      json commentList;
      for (int i = 0; i < keyword.comments(); i++) {
        commentList.push_back(keyword.comment(i).toStdString());
      }
      jsonKeyword["Comment"] = commentList;
    }
    return jsonKeyword;
  }


  /**
   * Convert the contents of a PvlContainer to a JSON object.
   * Any comments in the container will be stored in "Comment".
   * Comments associated with keywords will be stored inside their json object.
   *
   * @param container The container to convert
   * @return @b json The contents of the container as a JSON object
   */
  json pvlContainerToJSON(PvlContainer &container) {
    json jsonContainer;

    // Convert keywords
    PvlContainer::PvlKeywordIterator keywordIt;
    for (keywordIt = container.begin(); keywordIt != container.end(); keywordIt++) {
      jsonContainer[keywordIt->name().toStdString()] = pvlKeywordToJSON(*keywordIt);
    }

    // Optionally convert comments
    if (container.comments() == 1) {
      jsonContainer["Comment"] = container.comment(0).toStdString();
    }
    else if (container.comments() > 1) {
      json commentList;
      for (int i = 0; i < container.comments(); i++) {
        commentList.push_back(container.comment(i).toStdString());
      }
      jsonContainer["Comment"] = commentList;
    }
    return jsonContainer;

  }


  /**
   * Convert the contents of a PvlGroup to a JSON object.
   * Any comments in the group will be stored in "Comment".
   * Comments associated with keywords will be stored inside their json object.
   *
   * @param group The group to convert
   * @return @b json The contents of the group as a JSON object
   */
  json pvlGroupToJSON(PvlGroup &group) {
    // PvlGroups are just PvlContainers with extra input/output options
    // so we can just use the PvlContainer conversion directly.
    return pvlContainerToJSON(group);;
  }


  /**
   * Convert the contents of a PvlObject to a JSON object.
   * Any comments in the base object will be stored in "Comment".
   * Comments associated with keywords, groups, or nested objects will be
   * stored inside their associated JSON object.
   *
   * @param object The object to convert
   * @return @b json The contents of the object as a JSON object
   */
  json pvlObjectToJSON(PvlObject &object) {
    json jsonObject = pvlContainerToJSON(object);
    PvlObject::PvlGroupIterator groupIt;
    for (groupIt = object.beginGroup(); groupIt != object.endGroup(); groupIt++) {
      jsonObject[groupIt->name().toStdString()] = pvlGroupToJSON(*groupIt);
    }
    PvlObject::PvlObjectIterator objectIt;
    for (objectIt = object.beginObject(); objectIt != object.endObject(); objectIt++) {
      jsonObject[objectIt->name().toStdString()] = pvlObjectToJSON(*objectIt);
    }
    return jsonObject;
  }


  /**
   * Convert the contents of a Pvl to a JSON object.
   * Any comments in the root of the Pvl will be stored in "Comment".
   * Comments associated with keywords, groups, or objects will be stored
   * inside their associated JSON object.
   *
   * @param pvl The Pvl to convert
   * @return @b json The contents of the Pvl as a JSON object
   */
  json pvlToJSON(Pvl &pvl) {
    return pvlObjectToJSON(pvl);
  }

}
