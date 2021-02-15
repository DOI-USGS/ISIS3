#ifndef StringBlob_h
#define StringBlob_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "Blob.h"

namespace Isis {
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
  class StringBlob : public Isis::Blob {
    public:
      StringBlob();
      StringBlob(const QString &file);
      StringBlob(std::string str, QString name);
      ~StringBlob();

      std::string string() {
        return m_string;
      }

    protected:
      // prepare data for writing
      void WriteInit();
      void ReadData(std::istream &stream);

    private:
      std::string m_string;
  };
};

#endif

