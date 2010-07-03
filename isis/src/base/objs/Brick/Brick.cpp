/**
 * 
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
#include "Brick.h"

namespace Isis {
 /**                                                                       
  * Resizes the memory buffer to the specified number of samples, lines, and 
  * bands.                                                          
  *                                                                        
  * @param nsamps Number of samples
  * @param nlines Number of lines
  * @param nbands Number of bands                                                          
  */                                                                       
  void Brick::Resize(const int nsamps, const int nlines, const int nbands) {
    delete [] p_buf;
    delete [] (char *) p_rawbuf;
    p_nsamps = nsamps;
    p_nlines = nlines;
    p_nbands = nbands;
    p_npixels = p_nsamps * p_nlines * p_nbands;
    Allocate();
  }

  /** 
  * Sets the current brick as requested
  * 
  * @param brick  The brick number within a cube. This number starts with the 
  *              upper left corner of the cube and proceedes across the samples 
  *              then down the lines and lastly through the bands. The first
  *              brick starts at (1,1,1). The last brick contains the point
  *              (cubeSamples,cubeLines,cubeBands).
  * 
  * @return bool
  * 
  * @throws Isis::iException::Programmer - invalid argument value
  */
  bool Brick::SetBrick (const int brick) {
    if (brick < 1) {
      std::string message = "Invalid value for argument [brick]";
      throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_); 
    }

    return setpos(brick-1);
  }
} // end namespace isis
