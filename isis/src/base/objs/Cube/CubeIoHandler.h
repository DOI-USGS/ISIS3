/** 
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:06 $                                                                 
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

#if !defined(CubeIoHandler_h)
#define CubeIoHandler_h

#include "CubeDef.h"
#include "Buffer.h"
#include "Constants.h"

namespace Isis {
/**
 * @brief Pixel value mapper
 *
 * This class is used to stretch or remap pixel values. For example, it can be
 * used to apply contrast stretches, color code stretches, or remap from a
 * double range to 8-bit (0 to 255). The methodology used is straightforward.
 * The program must set up a list of stretch pairs, input-to-output mappings,
 * using the AddPair method. For example, (0,0) and (1,255) are two pairs which
 * would cause an input of 0 to be mapped to 0, 0.5 would be mapped to 127.5
 * and 1 would be mapped to 255. More than two pairs can be used which
 * generates piece-wise linear mappings. Special pixels are mapped to themselves
 * unless overridden with methods such as SetNull. Input values outside the
 * minimum and maximum input pair values are mapped to LRS and HRS respectively.
 *
 * If you would like to see Stretch being used in implementation,
 * see stretch.cpp
 *
 * @ingroup Low Level Cube I/O
 *
 * @author  ???
 *
 * @internal
 *  @history 2006-06-12 Tracie Sucharski - Clear stream bits before opening
 *                             cube.
 */
  class CubeIoHandler {
    public:
      CubeIoHandler(IsisCubeDef &cube);
      virtual ~CubeIoHandler();
      void Open();
      virtual void Create(bool overwrite);
      virtual void Close(const bool remove=false) = 0;
      virtual void Read(Isis::Buffer &rbuf) = 0;
      virtual void Write(Isis::Buffer &wbuf) = 0;
      void ToDouble(Isis::Buffer &buf);
      void ToRaw(Isis::Buffer &buf);
  
    protected:
      IsisCubeDef *p_cube;
      bool p_native;
  };
};

#endif
