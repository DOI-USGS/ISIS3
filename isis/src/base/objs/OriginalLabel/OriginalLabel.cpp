/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <fstream>
#include <sstream>
#include <string>
#include "OriginalLabel.h"
#include "Application.h"
#include "PvlObject.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for creating an original blob with a given name
   */
  OriginalLabel::OriginalLabel() {
    m_originalLabel.setTerminator("");
  }


  /**
   * Constructor for creating an original label from a blob object.
   */
  OriginalLabel::OriginalLabel(Isis::Blob &blob) {
    stringstream os;
    char* dataBuffer = blob.getBuffer();
    for(int i = 0; i < blob.Size(); i++) {
        os << dataBuffer[i];
    }
    os >> m_originalLabel;
  }

  /**
   * Constructor for creating an original blob with a given name and file to
   * read labels from.
   *
   * @param file File to read labels from
   */
  OriginalLabel::OriginalLabel(const QString &file){
    Pvl pvl;
    stringstream os;
    Blob blob = Blob("IsisCube", "OriginalLabel");
    blob.Read(file);
    char *buff = blob.getBuffer();
    for(int i = 0; i < blob.Size(); i++){
      os << buff[i];
    }
    os >> pvl;
    m_originalLabel = pvl;
  }

  /**
   * Constructor for creating an original blob with a given name and Pvl
   * container.
   *
   * @param pvl  Pvl containing labels of the source
   */
  OriginalLabel::OriginalLabel(Pvl pvl){
    m_originalLabel = pvl;
  }

  // Destructor
  OriginalLabel::~OriginalLabel() {
  }

//  /**
//   * Prepare labels for writing to the output cube.
//   */
//  void OriginalLabel::WriteInit() {
//    ostringstream ostr;
//    if(p_nbytes > 0) ostr << std::endl;
//
//    // store labels
//    ostr << m_originalLabel;
//    string orglblStr = ostr.str();
//    int bytes = orglblStr.size();
//
//    // Copy label data to bytes variable
//    char *temp = p_buffer;
//    p_buffer = new char[p_nbytes+bytes];
//    if(temp != NULL) memcpy(p_buffer, temp, p_nbytes);
//    const char *ptr = orglblStr.c_str();
//    memcpy(&p_buffer[p_nbytes], (void *)ptr, bytes);
//    p_nbytes += bytes;
//
//    if(temp != NULL) delete [] temp;
//  }


  Isis::Blob *OriginalLabel::toBlob() {
    std::stringstream sstream;
    sstream << m_originalLabel;
    string orglblStr = sstream.str();

    Isis::Blob *blob = new Blob("IsisCube", "OriginalLabel");
    blob->setData(orglblStr.data(), orglblStr.length());
    return blob;
  }

  /**
   * Returns the labels in a Pvl object
   *
   * @return (Isis::Pvl) original labels
   */
  Pvl OriginalLabel::ReturnLabels() {
    Pvl pvl;
    stringstream os;
    os << m_originalLabel;
    os >> pvl;
    return pvl;
  }
}
