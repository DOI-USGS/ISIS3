#ifndef GisBlob_h
#define GisBlob_h
/**                                                                       
 * @file                                                                  
 * $Revision: 6150 $ 
 * $Date: 2015-04-19 23:40:55 -0700 (Sun, 19 Apr 2015) $ 
 * $Id: GisBlob.h 6150 2015-04-20 06:40:55Z jwbacker@GS.DOI.NET $
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
                                      
// parent class
#include "Blob.h"

// Qt library
#include <QString>

namespace Isis {
  class Cube;

  /** 
   * This class creates a polygon-type Isis Blob named "Footprint". It inherits 
   * from the Isis Blob class. This Blob may be read from a given cube or the 
   * polygon maybe set using a wkt polygon string. 
   *  
   * @author 2012-07-15 Kris Becker 
   * @internal 
   *   @history 2012-07-15 Kris Becker - Original version.
   *   @history 2015-03-18 Jeannie Backer - Brought class files closer to ISIS coding standards.
   *   @history 2015-03-31 Jeannie Backer - Updated documentation.
   *   @history 2016-03-02 Ian Humphrey - Updated for coding standards compliance in preparation
   *                           for adding this class to ISIS. Fixes #2398.
   */
  class GisBlob : public Blob {
    public:
      GisBlob();
  
      GisBlob(Cube &cube);
  
      ~GisBlob();
  
      QString polygon() const;
  
      void setPolygon(const QString &wkt);
  
    private:
      QString scrub(const char *rawbuf, int rbytes)  const;
  
      QString m_wkt; //!< Well-known text string containing the polygon defintion for this GIS blob.
  };
} // Namespace Isis

#endif


