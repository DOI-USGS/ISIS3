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
#include "iException.h"
#include "Message.h"
#include "iString.h"
#include <iostream>
using namespace std;
namespace Isis{

  /**
   * Creates an ID object
   * @param name The string to be a base for the serial IDs
   * @param basenum The number to start the count at. Defaults to
   *                one.
   */
  ID::ID (const std::string &name, int basenum){
    p_current = basenum;
    p_namebase = name;
    if (p_namebase.find("?",0) == std::string::npos) {
      std::string msg = "No replacement set in string [" + p_namebase + "]";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    p_numStart = ((int) p_namebase.find("?",0));
    int endPos = (int)p_namebase.find_last_of("?",p_namebase.size());
    p_numLength = (endPos - p_numStart)+1;
    std::string sub = p_namebase.substr(p_numStart, p_numLength);
    for (int i=0; i<(int)sub.length(); i++) {
      if (sub[i] != '?') {
        std::string msg = "iString [" + p_namebase + "] contains more than one replacement set";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    }
    p_namebase.erase(p_numStart, p_numLength);
  }

  /**
   * Deconstructor
   */
  ID::~ID(){
  }

  /**
   * Returns the next ID in the sequence.
   * 
   * @return std::string The next ID in the sequence
   */
  std::string ID::Next(){
    iString num(p_current);
    if ((int)num.size() > p_numLength) {
      std::string replacement = "?";
      while ((int)replacement.size()<p_numLength) {
        replacement += "?";
      }
      std::string original = p_namebase;
      original.insert(p_numStart,replacement);
      std::string msg = "Maximum number reached for string [" + original + "]";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
    while ((int)num.size()<p_numLength) {
      num = "0" + num;
    }
    p_current++;
    std::string temp = p_namebase;
    return temp.insert((p_numStart),num.c_str());
  }

}
