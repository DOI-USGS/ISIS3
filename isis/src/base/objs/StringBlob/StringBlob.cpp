/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include "StringBlob.h"
#include "Blob.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for creating a string blob with no arguments
   */
  StringBlob::StringBlob() {
    m_string = "";
    m_name = "";
  }

  StringBlob::StringBlob(Blob &blob) {
    m_label = blob.Label();
    m_string = std::string(blob.getBuffer(), blob.Size());
    m_name = blob.Name();
  }

  /**
   * Constructor for creating a string blob with a standard string
   *
   * @param string String to read/write from the cube.
   */
  StringBlob::StringBlob(std::string str, QString name) {
    m_string = str;
    m_name = name;
  }

  // Destructor
  StringBlob::~StringBlob() {
  }

  Blob *StringBlob::toBlob() const {
    Blob *blob = new Blob(m_name, "String");

    char *buf = new char[m_string.size()];
    memcpy(buf, m_string.c_str(), m_string.size());
    blob->setData(buf, m_string.size());

    PvlObject &blobLabel = blob->Label();
    for (int i = 0; i < m_label.keywords(); i++) {
      if (!blobLabel.hasKeyword(m_label[i].name())) {
        blobLabel += m_label[i];
      }
    }
    for (int g = 0; g < m_label.groups(); g++) {
      blobLabel += m_label.group(g);
    }

    return blob;
  }
}
