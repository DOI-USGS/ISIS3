/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Message.h"

using namespace std;

QString Isis::Message::KeywordValueNotInList(const QString &key, const QString &value,
                                             const vector<QString> &list) {
  QString message;
  message = "Keyword [" + key + "=" + value + "] must be one of [";
  for(unsigned int i = 0; i < list.size(); i++) {
    message += list[i];
    if(i < (list.size() - 1)) message += ",";
  }
  message += "]";

  return message;
}
