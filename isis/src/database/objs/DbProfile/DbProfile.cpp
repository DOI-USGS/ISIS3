/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $
 * $Date: 2008/09/06 06:47:48 $
 * 
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

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using std::string;
using std::ostringstream;

#include "DbProfile.h"
#include "PvlGroup.h"

namespace Isis {

/**
 * @brief Creates a DbProfile from a Pvl entity
 * 
 * This constructor will create a DbProfile by reading keywords from a Pvl
 * container.  Pvl Containers can be an Object or a Group, or simply a list of
 * keywords.  This PvlContainer is expected to point to the appropriate keywords
 * that the caller has pre-established by normal Isis Pvl methods.
 * 
 * @param pvl Pvl container of keywords to make a DbProfile from
 */
DbProfile::DbProfile(PvlContainer &pvl) : _name("Profile"), _keys() {
  loadkeys(pvl);
  if (_keys.exists("Name")) {
    _name = value("Name");
  }
}


/**
 * @brief Creates a profile from the merging of two a DbProfiles
 * 
 * This constructor will create a new DbProfile from two existing DbProfiles.
 * Its intended use is for the merging of keys in an ordered fashion.  It is
 * useful when a higher level class inherits this class and a specific
 * implementations calls for the keys to come from both the parent and the child
 * DbProfile.
 * 
 * It should be noted that this is a convenient way to merge high level
 * database access parameters with individual profile access where the last keys
 * take precedence over the first.  Thus, in the case where the same keys exist
 * in both the first and second profiles, keys in the second profile take
 * precedence.  This is great for defining general database parameters in the
 * top level database configure section and having individual profiles redefine
 * certain access parameters.
 * 
 * @param name   Name of new profile
 * @param prof1  First profile to merge
 * @param prof2  Second profile to merge
 */
DbProfile::DbProfile(const DbProfile &prof1, const DbProfile &prof2,
                     const std::string &name) : _name(prof1.Name()),
                     _keys(prof1._keys) {
  for (int nk = 0 ; nk < prof2._keys.size() ; nk++) {
    _keys.add(prof2._keys.key(nk),prof2._keys.getNth(nk));
  }

  if (!name.empty()) { _name = name; }
}


/**
 * Adds a keyword and value pair to the profile
 * 
 * This method adds a keyword and value pair to the profile if it doesn't exist.
 *  If the keyword already exists,it appends the value to the existing keyword.
 * 
 * @param key   Keyword to add or ammend
 * @param value Value to add to the keyword
 */
void DbProfile::add(const std::string &key, const std::string &value) {
  if (_keys.exists(key)) {
    _keys.get(key).AddValue(value);
  }
  else {
    _keys.add(key, PvlKeyword(key,value));
  }
}

/**
 * Adds a keyword and value pair to the profile
 * 
 * This method adds a keyword and value pair to the profile if 
 * it doesn't exist. 
 *  
 * If the keyword already exists, it is deleted and replaced 
 * with this new keyword and value. 
 * 
 * @param key   Keyword to replace
 * @param value Value to add to the keyword
 */
void DbProfile::replace(const std::string &key, const std::string &value) {
  _keys.add(key, PvlKeyword(key,value));
}


/**
 * Removes a keyword from the profile
 * 
 * @param key   Keyword to remove
 */
void DbProfile::remove(const std::string &key) {
  _keys.remove(key);
}

/**
 * Report number of values in keyword
 * 
 * This method will return the number of values in the specified keyword.  If
 * the keyword does not exist, 0 is returned.
 * 
 * @param key  Name of key to get value count for
 * 
 * @return int Number values in key, or 0 if the key does not exist
 */
int DbProfile::count(const std::string &key) const {
  if (_keys.exists(key)) {
    return (_keys.get(key).Size());
  }
  return (0);
}

/**
 * Returns the specified value for the given keyword
 * 
 * This method returns a value from the specified keyword.  If the keyword or
 * the specified value does not exist, an exception is thrown.
 * 
 * @param key Name of keyword to return value for.
 * 
 * @param nth  Specifies the nth value in the keyword
 * 
 * @return std::string  The requested value in the keyword
 */
std::string DbProfile::value(const std::string &key, int nth) const {
  try {
    return (_keys.get(key)[nth]);
  } catch ( iException &ie ) {
    ostringstream mess;
    mess << "Error fetching value from key " << key;
    if ( nth != 0 ) {
      mess << " (index=" << nth << ")";
    }
    ie.Message(iException::Programmer, mess.str(), _FILEINFO_);
    throw;
  }
}

/**
 * Returns the specified value for the given keyword
 * 
 * This method returns a value from the specified keyword.  If the keyword or
 * the specified value does not exist, an exception is thrown.
 * 
 * @param key Name of keyword to return value for.
 * 
 * @param nth  Specifies the nth value in the keyword
 * 
 * @return std::string  The requested value in the keyword
 */
std::string DbProfile::operator()(const std::string &key, int nth) const {
  return (value(key,nth));
}

/**
 * @brief Loads DbProfile keys from the given Pvl construct
 * 
 * This method iterates through all keywords in the Pvl container and loads them
 * into this property.
 * 
 * @param pvl Container of keywords that will be loaded
 */
void DbProfile::loadkeys(PvlContainer &pvl) {
  PvlContainer::PvlKeywordIterator key;
  for (key = pvl.Begin() ; key != pvl.End() ; ++key) {
    _keys.add(key->Name(), *key);
  }
}

#if 0
void DbProfile::printOn(std::ostream &o) const {

  // Create the ordered list of keywords
  typedef CollectorMap<long, const PvlKeyword *> OrderedKeys;
  OrderedKeys okeys;
  DbProfileKeyList::CollectorConstIter keys;
  for (keys = begin() ; keys != end() ; ++keys) {
    const DbProfileKey &pk = keys->second;
    okeys.add(pk._index, &pk.key);
  }

  //  Now write the keys in order
  PvlGroup propGroup(_group);
  OrderedKeys::CollectorConstIter okItr;
  for (okItr = okeys.begin() ; okItr != okeys.end() ; ++okItr) {
    propGroup.AddKeyword(*(okItr->second));
  }

  o << propGroup << std::endl;
}
#endif


}
