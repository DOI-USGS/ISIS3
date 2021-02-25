#ifndef GisBlob_h
#define GisBlob_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */           
                                      
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


