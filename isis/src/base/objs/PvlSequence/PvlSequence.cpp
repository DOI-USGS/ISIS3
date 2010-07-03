/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/01/30 22:12:23 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related 
 *   information.                            
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or 
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution 
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *
 *   For additional information, launch 
 *   $ISISROOT/doc/documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */

#include <sstream>
#include "PvlSequence.h"
#include "Pvl.h"

namespace Isis {
/**                                                                       
 * Load a sequence using a Pvl keyword.  Each value of the PvlKeyword will
 * be treated as an array for a sequence.  Typically, the values in the 
 * PvlKeyword should be enclosed in parens and comma separated.  
 * For example, (a,b,c).
 *                                                                        
 * @param key keyword containing sequence                    
 */                                                                       
  PvlSequence &PvlSequence::operator=(PvlKeyword &key) {
    for (int i=0; i<key.Size(); i++) {
      this->operator+=(key[i]);
    }
    return *this;
  }

/**                                                                       
 * Add a string array to the sequence.  The values in the string 
 * must be enclosed in parens and comma separated.  For example, 
 * (1,2,3).
 *                                                                        
 * @param  array  A string representing an array.
 */                                                                       
  PvlSequence &PvlSequence::operator+=(const std::string &array) {
    std::stringstream str;
    str << "temp = " << array;
    Pvl pvl;
    str >> pvl;
    PvlKeyword &key = pvl["temp"];
    std::vector<iString> temp;
    for (int i=0; i<key.Size(); i++) {
      temp.push_back(key[i]);
    }
    p_sequence.push_back(temp);
    return *this;
  }

/**                                                                       
 * Add a vector of strings to the sequence.  This adds another array to the
 * sequence whose values are all strings
 *                                                                        
 * @param  array  vector of strings
 */                                                                       
  PvlSequence &PvlSequence::operator+=(std::vector<std::string> &array) {
    std::vector<iString> temp;
    for (int i=0; i<(int)array.size(); i++) {
      temp.push_back(iString(array[i]));
    }
    p_sequence.push_back(temp);
    return *this;
  }

/**                                                                       
 * Add a vector of ints to the sequence.  This adds another array to the
 * sequence whose values are all integers.
 *                                                                        
 * @param  array  vector of integers
 */                                                                       
  PvlSequence &PvlSequence::operator+=(std::vector<int> &array) {
    std::vector<iString> temp;
    for (int i=0; i<(int)array.size(); i++) {
      temp.push_back(iString(array[i]));
    }
    p_sequence.push_back(temp);
    return *this;
  }

/**                                                                       
 * Add a vector of ints to the sequence.  This adds another array to the
 * sequence whose values are all doubles.
 *                                                                        
 * @param  array  vector of doubles
 */                                                                       
  PvlSequence &PvlSequence::operator+=(std::vector<double> &array) {
    std::vector<iString> temp;
    for (int i=0; i<(int)array.size(); i++) {
      temp.push_back(iString(array[i]));
    }
    p_sequence.push_back(temp);
    return *this;
  }
}
