/**
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ID.h"
#include "IException.h"
#include "Message.h"
#include "IString.h"
#include <iostream>
using namespace std;
namespace Isis {

  /**
   * Creates an ID object
   * @param name The string to be a base for the serial IDs
   * @param basenum The number to start the count at. Defaults to
   *                one.
   */
  ID::ID(const QString &name, int basenum) {
    p_current = basenum;
    p_namebase = name;
    if(!p_namebase.contains("?")) {
      QString msg = "No replacement set in string [" + p_namebase + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    p_numStart = ((int) p_namebase.indexOf("?", 0));
    int endPos = (int)p_namebase.lastIndexOf("?");
    p_numLength = (endPos - p_numStart) + 1;
    QString sub = p_namebase.mid(p_numStart, p_numLength);
    for(int i = 0; i < (int)sub.length(); i++) {
      if(sub[i] != '?') {
        QString msg = "IString [" + p_namebase + "] contains more than one replacement set";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    p_namebase.remove(p_numStart, p_numLength);
  }

  /**
   * Deconstructor
   */
  ID::~ID() {
  }

  /**
   * Returns the next ID in the sequence.
   *
   * @return QString The next ID in the sequence
   */
  QString ID::Next() {
    IString num(p_current);
    if((int)num.size() > p_numLength) {
      QString replacement = "?";
      while((int)replacement.size() < p_numLength) {
        replacement += "?";
      }
      QString original = p_namebase;
      original.insert(p_numStart, replacement);
      QString msg = "Maximum number reached for string [" + original + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    while((int)num.size() < p_numLength) {
      num = "0" + num;
    }
    p_current++;
    QString temp = p_namebase;
    return temp.insert((p_numStart), num.c_str());
  }

}
