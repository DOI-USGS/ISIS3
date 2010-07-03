#ifndef JP2Error_h
#define JP2Error_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/02/22 02:12:38 $
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

#include <string>

#if ENABLEJP2K
#include "jp2.h"
#endif

namespace Isis {
/**                                                                       
 * @brief  Kakadu error messaging class
 *                                                                        
 * This class is used to register a Kakadu error handler. It is
 * necessary to register the routines put_text, add_text, and
 * flush with the Kakadu error handler in order for the Kakadu
 * error messages to get reported to the user.
 * 
 * @ingroup HighLevelCubeIO                                                  
 *                                                                        
 * @author 2009-12-18 Janet Barrett
 *                                                                        
 * @internal
 *  @history 2009-12-18 Janet Barrett - Original version.
 * 
 */                                                                       

#if ENABLEJP2K
class JP2Error : public kdu_thread_safe_message {
#else
class JP2Error {
#endif
  public:
    //!<Save text from a Kakadu produced error
    void put_text (const char *message);
 
    //!<Place newline character between successive Kakadu produced error messages
    void add_text (const std::string &message);

    //!<Write Kakadu error messages using ISIS3 methods
    void flush (bool end_of_message = false);
 
    //!<Used to store accumulated Kakadu error messages 
    std::string Message;
};
};
#endif
