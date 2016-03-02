/**                                                                       
 * @file                                                                  
 * $Revision: 6084 $ 
 * $Date: 2015-03-04 18:17:45 -0700 (Wed, 04 Mar 2015) $ 
 * $Id: GisBlob.h 6084 2015-03-05 01:17:45Z kbecker@GS.DOI.NET $
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
#include "GisBlob.h"

// Qt library
#include <QDebug>
#include <QString>

// other ISIS
#include "Blob.h"
#include "Cube.h"

namespace Isis {

  /** 
   * Constructs an Isis polygon-type Blob named "Footprint." 
   */
  GisBlob::GisBlob() : Blob("Footprint", "Polygon"), m_wkt() {
  }


  /** 
   * Constructs an Isis polygon-type Blob named "Footprint" and sets the
   * well-known text string that defines the polygon by reading the 
   * given cube.
   * @param cube 
   */
  GisBlob::GisBlob(Cube &cube) : Blob("Footprint", "Polygon"), m_wkt() {
    cube.read(*this);
    m_wkt = scrub(p_buffer, p_nbytes);
  }   


  /** 
   * Destroys the GisBlob object. 
   */
  GisBlob::~GisBlob() { 
  }


  /** 
   * Accesses the well-known text string that defines the polygon. 
   *  
   * @return QString A well-known text string containing the polygon definition.
   */
  QString GisBlob::polygon() const { 
    return (m_wkt); 
  }


  /** 
   * Sets the polygon using the given well-known text string. 
   *  
   * @param wkt A string containing the well-known text that defines a polygon.
   */
  void GisBlob::setPolygon(const QString &wkt) {
    delete [] p_buffer;
    p_nbytes = wkt.size();
    p_buffer = new char[p_nbytes+1];
    for (int i = 0 ; i < p_nbytes ; i++) {
      p_buffer[i] = wkt[i].toAscii();
    }
    p_buffer[p_nbytes] = 0;
    m_wkt = scrub(p_buffer, p_nbytes);
    return;
  }


  /** 
   * This method will scrub all zeros that prefix the given buffer and convert 
   * it to a string using the number of allocated bytes given.
   *  
   * @param rawbuf A pointer containing the raw buffer to be converted.
   * @param rbytes The number of bytes to be read in. 
   *  
   * @return QString A string converted from the given raw buffer.
   */
  QString GisBlob::scrub(const char *rawbuf, int rbytes) const {
    int i;
    for (i = 0 ; i < rbytes ; i++) {
      if (rawbuf[i] != 0) break;
    }
    int nbytes =  rbytes - i;
    return (QString::fromAscii(&rawbuf[i], nbytes));
  }

} // Namespace Isis
