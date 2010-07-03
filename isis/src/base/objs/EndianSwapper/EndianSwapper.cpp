/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2009/04/16 17:37:59 $                                                                 
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
#include "Endian.h"
#include "EndianSwapper.h"
#include "iException.h"
#include "Message.h"
#include <string>

#include <iostream>

using namespace std;
namespace Isis {
/**                                                                       
 * Constructs an EndianSwapper object, determining whether swapping of bytes
 * actually needs to occur and sets the direction of swapping.
 *                                                                        
 * @param inputEndian Byte order of input value (MSB or LSB).                                                          
 */                                                                         
  EndianSwapper::EndianSwapper (std::string inputEndian) {
    
  	if (inputEndian != "LSB" && inputEndian != "MSB") {
  		string message = "Invalid parameter-InputEndian must be LSB or MSB";
  		throw Isis::iException::Message(Isis::iException::Programmer,message,_FILEINFO_);
  	}
  
    if ( (Isis::IsLsb() &&	inputEndian == "LSB") ||
  			 (Isis::IsMsb()  && inputEndian == "MSB") ) {
  		p_needSwap = false;
  		p_swapDirection = 1;
  	}
  	else {
  		p_needSwap = true;
  		p_swapDirection = -1;
  	}
  	
  }
  	
  
/**                                                                       
 * Destroys the EndianSwapper object.                                                         
 */
  EndianSwapper::~EndianSwapper () {
  }
  
  
/**                                                                       
 * Swaps a double precision value.                                                          
 *                                                                        
 * @param buf Input double precision value to swap.                                
 */                                                                       
  double EndianSwapper::Double (void *buf) {
    char *ptr = (char *)buf + (sizeof (double) -1) * p_needSwap;
  
    for (unsigned int i=0; i<sizeof (double); i++) {
      p_swapper.p_char[i] = *ptr;
      ptr += p_swapDirection;
    }
    return p_swapper.p_double;
  }
  
  
/**                                                                       
 * Swaps a floating point value.                                                         
 *                                                                        
 * @param buf Input floating point value to swap.                                 
 */                                                                       
  float EndianSwapper::Float (void *buf) {
    char *ptr = (char *)buf + (sizeof (float) -1) * p_needSwap;

    for (unsigned int i=0; i<sizeof (float); i++) {
      p_swapper.p_char[i] = *ptr;
      ptr += p_swapDirection;
    }
    return p_swapper.p_float;
  }


/**
 * Swaps a floating point value for Exporting.
 */
  int EndianSwapper::ExportFloat (void *buf) {
    return Int(buf);
  }

/**                                                                       
 * Swaps a 4 byte integer value.                                                
 *                                                                        
 * @param buf Input integer value to swap.   
 */ 
  int EndianSwapper::Int(void *buf) {
    char *ptr = (char *)buf + (sizeof (int) -1) * p_needSwap;

    for (unsigned int i=0; i<sizeof (int); i++) {
      p_swapper.p_char[i] = *ptr;
      ptr += p_swapDirection;
    }

    return p_swapper.p_int;
  }

/**                                                                       
 * Swaps an 8 byte integer value.                                               
 *                                                                        
 * @param buf Input integer value to swap.   
 */ 
  long long int EndianSwapper::LongLongInt(void *buf) {
    char *ptr = (char *)buf + (sizeof (long long int) -1) * p_needSwap;

    for (unsigned int i=0; i<sizeof (long long int); i++) {
      p_swapper.p_char[i] = *ptr;
      ptr += p_swapDirection;
    }

    return p_swapper.p_longLongInt;
  }
  
/**                                                                       
 * Swaps a short integer value.                                                         
 *                                                                        
 * @param buf Input short integer value to swap.                                
 */ 
  short int EndianSwapper::ShortInt (void *buf) {
    char *ptr = (char *)buf + (sizeof (short int) -1) * p_needSwap;
  
    for (unsigned int i=0; i<sizeof (short int); i++) {
      p_swapper.p_char[i] = *ptr;
      ptr += p_swapDirection;
    }
    return p_swapper.p_shortInt;
  }
  
  
/**                                                                       
 * Swaps an unsigned short integer value                                                         
 *                                                                        
 * @param buf Input unsigned short integer value to swap.                                  
 */ 
  unsigned short int EndianSwapper::UnsignedShortInt (unsigned short int *buf) {
    char *ptr = (char *)buf + (sizeof (unsigned short int) -1) * p_needSwap;
  
    for (unsigned int i=0; i<sizeof (unsigned short int); i++) {
      p_swapper.p_char[i] = *ptr;
      ptr += p_swapDirection;
    }
    return p_swapper.p_uShortInt;
  }
}

