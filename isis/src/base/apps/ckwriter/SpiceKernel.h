#ifndef SpiceKernel_h
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

#include "IString.h"
#include "IException.h"
#include "SpiceSegment.h"

namespace Isis {

/**
 * @brief Container for SPICE kernel creation
 * 
 * This class serves as a container for ISIS cube files to prep for writing the
 * contents to a NAIF SPICE kernel.  Each file added is a CK segment.  When the
 * ISIS cube is added, the contents of the Table BLOB (InstrumentRotation for
 * CKs, InstrumentPosition for SPKs) are read and transformed to the appropriate
 * state intended to be compatible with kernels issued by each mission source.
 * 
 * It is designed for ease of use.  Here is an example to create the most basic
 * of CK kernel from a single ISIS file:
 * @code
 *   SpiceKernel kernel;
 *   kernel.add("mycube.cub");
 *   kernel.write("mycube.ck");  // Writes a type 3 CK kernel by default
 * @endcode
 * 
 * Note that processing ISIS cubes is expensive in terms of NAIF kernel
 * management.  Lots of NAIF kernel activity is incurred in resolving all the
 * necessary requirements to get the SPICE data in a form that satisfies NAIF
 * kernel specifications.
 * 
 * @ingroup Utility
 * 
 * @author 2010-11-22 Kris Becker
 * @internal
 * @history 2010-12-09 Kris Becker Add documentation and example 
 * @history 2013-07-10 Kris Becker Updated to better conform with coding 
 *                                 standards
 */
class SpiceKernel {
  public:
    SpiceKernel();
    virtual ~SpiceKernel() { }

    /** Returns the number of segments */
    int size() const { return (m_segments.size()); }
    const SpiceSegment &operator[](const int i) const;

    void add(const QString &fname);
    void add(Cube &cube);

    QString getSummary(const QString &commfile = "") const;

    void write(const QString &kname, const QString &commfile = "",
               const int ckType = 3) const;

  private:
    typedef std::vector<SpiceSegment> Segments;
    Segments  m_segments;

    void init();
    QString getCkComment(const QString &comFile = "") const;
};

};     // namespace Isis
#endif

