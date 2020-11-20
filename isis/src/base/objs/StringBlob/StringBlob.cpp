/**
 */
#include <string>
#include "StringBlob.h"
#include "Application.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for creating an string blob with no arguments
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

    char *temp = p_buffer;
    p_buffer = new char[p_nbytes+bytes];
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
