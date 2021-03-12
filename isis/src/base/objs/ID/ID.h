#ifndef ID_h
#define ID_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QString>

namespace Isis {
  /**
   * @brief   Creates sequential IDs
   *
   * This class generates IDs in numerical sequence, from an input
   * string. The input must contain one, and only one, series of
   * question marks, which will be replaced with numbers in the
   * generation of IDs. The default start value is 1, but this can
   * be changed.
   *
   * @ingroup Utility
   *
   * @author 2006-07-05 Brendan George
   *
   * @internal
   */
  class ID {
    public:
      ID(const QString &name, int basenum = 1);

      ~ID();

      QString Next();

    private:
      QString p_namebase;
      int p_current;
      int p_numLength;
      int p_numStart;
  };
}

#endif
