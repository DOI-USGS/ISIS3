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
 
#ifndef PvlSequence_h
#define PvlSequence_h

#include <vector>
#include "PvlKeyword.h"
#include "iString.h"

namespace Isis {
/**                                                                       
 * @brief Parse and return elements of a Pvl sequence.
 *                                                                        
 * A Pvl sequence is essentially an array of arrays.  For example,
 * @code
 * Keyword = ((a,b,c), (d,e))
 * @endcode
 * To extract the invidual arrays from a PvlKeyword you must use a
 * PvlSequence.
 * 
 * Here is an example of how to use PvlSequence                           
 * @code                                                                  
 * PvlKeyword k;
 * k += "(a,b,c)";
 * k += "(d,e)";
 * 
 * PvlSequence s = k;
 * cout << s.Size() << endl; // should be 2
 * cout << s[0][0] << endl;  // should be a
 * cout << s[1][1] << endl;  // should be e
 * @endcode                                                               
 * @ingroup Parsing
 *                                                                        
 * @author 2005-02-16 Jeff Anderson                                   
 *                                                                        
 * @internal                                                              
 *  @todo Add PvlSequence(PvlKeyword) constructor
 *  so that we can code PvlSequence s = k;
 *  where k is a PvlKeyword.
 */                                                                       
  class PvlSequence {  
    public:
      //! Construct an empty sequence
      PvlSequence () {};

      //! Destruct sequence
      ~PvlSequence () {};

      PvlSequence &operator=(PvlKeyword &key);

      PvlSequence &operator+=(const std::string &array);

      PvlSequence &operator+=(std::vector<std::string> &array);

      PvlSequence &operator+=(std::vector<int> &array);

      PvlSequence &operator+=(std::vector<double> &array);

      //! Return the ith Array of the sequence
      std::vector<iString> &operator[](int i) { return p_sequence[i]; };

      //! Number of arrays in the sequence
      inline int Size() const { return p_sequence.size(); };

      //! Clear the sequence
      inline void Clear() { p_sequence.clear(); };

    private:
      std::vector<std::vector<iString> > p_sequence; /**<A vector of Strings
                                                        that contains the values
                                                        for the keyword. */
                                                        
  };
};

#endif
