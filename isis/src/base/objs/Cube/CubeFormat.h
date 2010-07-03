/**                                                                     
 * @file                                                                
 * $Revision: 1.2 $                                                           
 * $Date: 2007/01/30 22:12:22 $                                                               
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
#if !defined(CubeFormat_h)
#define CubeFormat_h

#include "iException.h"

namespace Isis {
  enum CubeFormat {
    Bsq,
    Tile
  };

  inline std::string CubeFormatName (CubeFormat cubeFormat) {
    if (cubeFormat == Tile) return "Tile";
    if (cubeFormat == Bsq) return "BandSequential";

    std::string msg = "Invalid cube format name [" + Isis::iString(cubeFormat) + "]";
    throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
  }

  inline CubeFormat CubeFormatEnumeration(const std::string &cubeFormat) {
    Isis::iString temp(cubeFormat);
    temp = temp.UpCase();
    if (temp == "TILE") return Tile;
    if (temp == "BSQ") return Bsq;
    if (temp == "BANDSEQUENTIAL") return Bsq;

    std::string msg = "Invalid cube format string [" + cubeFormat + "]";
    throw Isis::iException::Message(Isis::iException::Parse,msg, _FILEINFO_);
  }
}

#endif
