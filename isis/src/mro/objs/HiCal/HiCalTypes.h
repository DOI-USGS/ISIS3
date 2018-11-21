#ifndef HiCalTypes_h
#define HiCalTypes_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2008/01/13 08:12:58 $
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
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "tnt_array1d.h"
#include "tnt_array1d_utils.h"
#include "tnt_array2d.h"
#include "tnt_array2d_utils.h"

#include "IString.h"
#include "PvlKeyword.h"

namespace Isis {

typedef TNT::Array1D<double> HiVector;       //!<  1-D Buffer
typedef TNT::Array2D<double> HiMatrix;       //!<  2-D buffer

class HiHistory {
  public:
    /**
     * @brief Output operator for history events
     * @param o Output stream
     * @param h History class to write contents of
     * 
     * @return std::ostream& Returns new state of stream
     */
    friend std::ostream &operator<<(std::ostream &o, const HiHistory &h) {
      std::vector<QString>::const_iterator it = h._events.begin();

      while (it != h._events.end()) {
        o << *it << "; ";
        it++;
      }
      return (o);
    }

  public:
    HiHistory() { }
    HiHistory(const HiHistory &h) : _events(h._events) { }
    virtual ~HiHistory() { }

    inline int size() const { return (_events.size()); }
    void add(const QString &event) { _events.push_back(event); }
    QString get(unsigned int index = 0) const { 
      if (index < _events.size()) { 
        return (_events[index]);
      }
      else {
        return (QString(""));
      }
    }

    void clear() { _events.clear(); }

    PvlKeyword makekey(const QString &name = "History") const {
      PvlKeyword key(name);
      for (unsigned int i = 0 ; i < _events.size() ; i++) {
        key.addValue(_events[i]);
      }
      return (key); 
    }

  private:
    std::vector<QString> _events;
};

};
#endif
