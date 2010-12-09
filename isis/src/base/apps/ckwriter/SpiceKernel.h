#if !defined(SpiceKernel_h)
#define SpiceKernel_h
/**                                                                       
 * @file                                                                  
 * $Revision$ 
 * $Date$
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
 *  
 *   $Id$
 */                                                                       

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "iString.h"
#include "iException.h"
#include "SpiceSegment.h"

namespace Isis {

class SpiceKernel {
  public:
    SpiceKernel();
    virtual ~SpiceKernel() { }

    /** Returns the number of segments */
    int size() const { return (_segments.size()); }
    const SpiceSegment &operator[](const int i) const;

    void add(const std::string &fname);
    void add(Cube &cube);

    std::string getSummary(const std::string &commfile = "") const;

    void write(const std::string &kname, const std::string &commfile = "",
               const int ckType = 3) const;

  private:
    typedef std::vector<SpiceSegment> Segments;
    Segments  _segments;

    void init();
    std::string getCkComment(const std::string &comFile = "") const;
};

};     // namespace Isis
#endif

