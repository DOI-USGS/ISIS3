/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                              
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
      p_buffer[i] = wkt[i].toLatin1();
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
    return (QString::fromLatin1(&rawbuf[i], nbytes));
  }

} // Namespace Isis
