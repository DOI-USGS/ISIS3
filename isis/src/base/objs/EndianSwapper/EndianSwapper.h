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

#ifndef EndianSwapper_h
#define EndianSwapper_h

#include "iException.h"

namespace Isis {

/**                                                                       
 * @brief Byte swapper                
 *                                                                        
 * This class is used to swap bytes on data that is from a different machine 
 * architecture.              
 *                                                                        
 * @ingroup Utility                                                  
 *                                                                        
 * @author 2002-07-10 Tracie Sucharski                                                                                
 *                                                                        
 * @internal                                                              
 *   @todo This class needs an example.                                                               
 *   @history 2003-05-16 Stuart Sides modified schema from 
 *   astrogeology...isis.astrogeology.                                     
 *   @history 2004-03-18 Stuart Sides used Endian.h instead of the linux gcc 
 *   endian.h to figure the system's endian type.
 *   @history 2008-08-14 Christopher Austin - Added ExportFloat() for exporting
 *   real data to the non-native endians. i.e. exporting to msb on a lsb system
 *   @history 2009-04-16 Steven Lambright - Added Int and LongLongInt. Long was
 *            not added because it is 4 bytes on 32-bit linux and 8 bytes on
 *            64-bit linux.
 */                                                                       
  class EndianSwapper {
    private:
      //! Indicates whether bytes need to be swapped.
      bool p_needSwap;
      /**
       *  Indicates which direction to increment the pointer for swapping.
       *  (Possible values: -1,1)
       */
      int p_swapDirection;

      /**
       * Union containing the output double precision value, floating point
       * value, short integer value, unsigned short integer value and
       * byte format - all with swapped bytes.
       */
      union {
      //! Union containing the output double precision value with swapped bytes.
        double p_double;
      //! Union containing the output floating point value with swapped bytes.
        float p_float;
      //! Union containing the output 4 byte integer value with swapped bytes.
        int p_int;
      //! Union containing the output 8 byte integer value with swapped bytes.
        long long int p_longLongInt;
      //! Union containing the output 2 byte integer value with swapped bytes.
        short int p_shortInt;
      /**
       * Union containing the output unsigned short integer value with swapped
       * bytes.
       */
        unsigned short int p_uShortInt;
        //! Union containing the output value in byte format.
        char p_char[8];
        } p_swapper;
        
    public:
      EndianSwapper (std::string inputEndian);
      ~EndianSwapper ();
      double Double (void *buf);      
      float Float (void *buf);
      int ExportFloat (void *buf);
      int Int (void *buf);
      long long int LongLongInt (void *buf);
      short int ShortInt (void *buf);
      unsigned short int UnsignedShortInt (unsigned short int *buf);
      
  };
};

#endif
