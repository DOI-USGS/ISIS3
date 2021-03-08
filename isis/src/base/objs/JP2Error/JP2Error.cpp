/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <string>
#include <sstream>

#include "IException.h"
#include "JP2Error.h"

using namespace std;
namespace Isis {

  /**
   * This class is necessary to catch the error messages produced by
   * Kakadu so that they can be output using ISIS methods. If these
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
  void JP2Error::put_text(const char *message) {
    Message += message;
  }

  /**
   * Register add_text routine with Kakadu error and warning handling
   * facility. This routine places newline character between successive
   * Kakadu produced error messages.
   *
   */
  void JP2Error::add_text(const std::string &message) {
    if(!Message.empty()) Message += '\n';
    Message += message;
  }

  /**
   * Register flush routine with Kakadu error and warning handling
   * facility. This routine writes Kakadu error messages out using
   * ISIS preferred method.
   *
   */
  void JP2Error::flush(bool end_of_message) {
    throw IException(IException::User, Message.c_str(), _FILEINFO_);
  }
}
