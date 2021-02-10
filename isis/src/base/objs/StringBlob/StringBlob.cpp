/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>
#include "StringBlob.h"
#include "Application.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for creating a string blob with no arguments
   */
  StringBlob::StringBlob() : Isis::Blob("IsisCube", "String") {
    m_string = "";
  }

  /**
   * Constructor for creating a string blob with a file to
   * read labels from.
   *
   * @param file File to read labels from
   */
  StringBlob::StringBlob(const QString &file) :
    Isis::Blob("IsisCube", "String") {
    Blob::Read(file);
  }

  /**
   * Constructor for creating a string blob with a standard string
   *
   * @param string String to read/write from the cube.
   */
  StringBlob::StringBlob(std::string str, QString name) : Isis::Blob(name, "String") {
    m_string = str;
  }

  // Destructor
  StringBlob::~StringBlob() {
  }

  /**
   * Prepare to write string to output cube
   */
  void StringBlob::WriteInit() {
    int bytes = m_string.size();
    std::cout << "String size in StringBlob writeinit" << bytes << std::endl;
    char *temp = p_buffer;
//    p_nbytes = bytes
    p_buffer = new char[p_nbytes+bytes];
    std::cout << "nbytes in StringBlob writeinit" << p_nbytes << std::endl;
    if (temp != NULL) memcpy(p_buffer, temp, p_nbytes);
    const char *ptr = m_string.c_str();
    memcpy(&p_buffer[p_nbytes], (void *)ptr, bytes);
    p_nbytes += bytes;

    if (temp != NULL) delete [] temp;
  }

  /**
   * Read binary data from an input stream into the string.
   *
   * @param stream The input stream to read from.
   *
   * @throws IException::Io - Error reading data from stream
   */
  void StringBlob::ReadData(std::istream &stream) {
    // Read the binary data
    if (p_buffer != NULL) delete [] p_buffer;
    p_buffer = new char[p_nbytes];

    streampos sbyte = p_startByte - 1;
    stream.seekg(sbyte, std::ios::beg);
    if (!stream.good()) {
      QString msg = "Error preparing to read data from " + p_type +
                   " [" + p_blobName + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    stream.read(p_buffer, p_nbytes);
    if (!stream.good()) {
      QString msg = "Error reading data from " + p_type + " [" + p_blobName + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    m_string = std::string(p_buffer, p_nbytes);
  }
}
