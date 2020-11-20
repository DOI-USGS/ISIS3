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
    p_nbytes = m_string.size();
  }


  /**
   * Writes blob data to a stream
   *
   * @param stream Output steam blob data will be written to
   *
   * @throws IException::Io - Error writing data to stream
   */
  void StringBlob::WriteData(std::fstream &os) {
    os.write(m_string.c_str(), p_nbytes);
    if (!os.good()) {
      QString msg = "Error writing data to " + p_type + " [" + p_blobName + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }
}
