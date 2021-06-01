#ifndef JP2Error_h
#define JP2Error_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
   *  @history 2017-08-21 Tyler Wilson, Ian Humphrey, Summer Stapleton - Added
   *                        support for new kakadu libraries.  References #4809.
   *
   */

#if ENABLEJP2K
  class JP2Error : public kdu_core::kdu_thread_safe_message {
#else
  class JP2Error {
#endif
    public:
      //!<Save text from a Kakadu produced error
      void put_text(const char *message);

      //!<Place newline character between successive Kakadu produced error messages
      void add_text(const std::string &message);

      //!<Write Kakadu error messages using ISIS methods
      void flush(bool end_of_message = false);

      //!<Used to store accumulated Kakadu error messages
      std::string Message;
  };
};
#endif
