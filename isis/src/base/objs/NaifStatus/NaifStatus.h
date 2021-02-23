#ifndef NaifStatus_h
#define NaifStatus_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
namespace Isis {
  /**
   * @brief Class for checking for errors in the NAIF library
   *
   * The Naif Status class looks for errors that have occurred in NAIF calls. If
   * an error has occurred, it will be converted to an iException.
   *
   * @author 2008-06-13 Steven Lambright
   *
   * @internal
   */
  class NaifStatus {
    public:
      static void CheckErrors(bool resetNaif = true);
    private:
      static bool initialized;
  };
};

#endif
