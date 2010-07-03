/**                                                                     
 * @file                                                                
 * $Revision: 1.2 $                                                           
 * $Date: 2008/10/30 16:52:30 $                                                               
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

#ifndef IsisCubeDef_h
#define IsisCubeDef_h

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "Constants.h"
#include "Pvl.h"
#include "PixelType.h"
#include "Endian.h"
#include "CubeFormat.h"

struct IsisCubeDef {
  std::string labelFile;
  Isis::Pvl label;
  int labelBytes;
  bool attached;
  bool history;
  
  std::string dataFile;
  Isis::BigInt startByte;
  std::fstream stream;
  enum Access { ReadOnly, ReadWrite } access;

  int samples;
  int lines;
  int bands;
  Isis::PixelType pixelType;
  Isis::CubeFormat cubeFormat;

  Isis::ByteOrder byteOrder;

  double base;
  double multiplier;
    
  std::vector<int> virtualBandList;

  std::streampos dataBytes;
};

#endif
