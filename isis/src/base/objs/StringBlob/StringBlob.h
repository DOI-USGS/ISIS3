#ifndef StringBlob_h
#define StringBlob_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include <QString>

#include "Pvl.h"

namespace Isis {
  class Blob;
  /**
   * @brief Read and store std::strings on the cube.
   *
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2020-11-19 Kristin Berry - Original Version
   *
   * @internal
   *   @history 2020-11-19 Kristin Berry - Original Version
    */
  class StringBlob {
    public:
      StringBlob();
      StringBlob(Blob &blob);
      StringBlob(std::string str, QString name);
      virtual ~StringBlob();

      virtual Blob *toBlob() const;

      std::string string() const {
        return m_string;
      }

      QString name() const {
        return m_name;
      }

      PvlObject &Label() {
        return m_label;
      }

    private:
      std::string m_string;
      QString m_name;
      PvlObject m_label;
  };
};

#endif

