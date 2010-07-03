/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2010/02/08 22:48:32 $
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

#include <iostream>
#include <string>
#include <sstream>

#include "iException.h"
#include "JP2Error.h"

using namespace std;
namespace Isis {

 /**
  * This class is necessary to catch the error messages produced by 
  * Kakadu so that they can be output using ISIS3 methods. If these
  * routines are not registered with the Kakadu error handling facility,
  * then the Kakadu errors will be lost and not reported to the user.
  *
  */

 /**
  * Register put_text routine with Kakadu error and warning handling
  * facility. This routine saves the text from a Kakadu produced error
  * message.
  *
  */
  void JP2Error::put_text (const char *message) {
    Message += message;
  }

 /**
  * Register add_text routine with Kakadu error and warning handling
  * facility. This routine places newline character between successive
  * Kakadu produced error messages.
  *
  */
  void JP2Error::add_text (const std::string &message) {
    if (!Message.empty()) Message += '\n';
    Message += message;
  }

 /**
  * Register flush routine with Kakadu error and warning handling
  * facility. This routine writes Kakadu error messages out using
  * ISIS3 preferred method.
  *
  */
  void JP2Error::flush (bool end_of_message) {
    throw iException::Message(iException::User,Message,_FILEINFO_);
  }
}
