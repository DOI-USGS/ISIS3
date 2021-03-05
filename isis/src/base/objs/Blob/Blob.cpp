/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Blob.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include <QDebug>

#include "FileName.h"
#include "IException.h"
#include "Message.h"
#include "Pvl.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Blob object using a name and type.
   *
   * @param name The blob name
   * @param type The blob type
   */
  Blob::Blob(const QString &name, const QString &type) {
    p_blobName = name;
    p_buffer = NULL;
    p_labelFile = "";
    p_nbytes = 0;
    p_type = type;

    p_blobPvl.setName(p_type);
    p_blobPvl += PvlKeyword("Name", p_blobName);
    p_blobPvl += PvlKeyword("StartByte", "0");
    p_blobPvl += PvlKeyword("Bytes", "0");
  }

  /**
   * Constructs a Blob object using a name, type, and reading Pvl values from a
   * file.
   *
   * @param name The blob name
   * @param type The blob type
   * @param file The filename to read from.
   */
  Blob::Blob(const QString &name, const QString &type, 
             const QString &file) {
    p_blobName = name;
    p_buffer = NULL;
    p_nbytes = 0;
    p_type = type;
    p_labelFile = FileName(file).expanded();

    Read(file);
  }

  /**
   * This copies the blob object.
   *
   * @param other Blob to be copied
   */
  Blob::Blob(const Blob &other) {
    p_blobPvl = other.p_blobPvl;
    p_blobName = other.p_blobName;
    p_startByte = other.p_startByte;
    p_nbytes = other.p_nbytes;
    p_type = other.p_type;
    p_detached = other.p_detached;
    p_labelFile = other.p_labelFile;

    p_buffer = NULL;

    if (other.p_buffer) {
      p_buffer = new char[p_nbytes];

      for (int i = 0; i < p_nbytes; i++) {
        p_buffer[i] = other.p_buffer[i];
      }
    }
  }

  /**
   * This makes the two blob objects exactly the same (copies the blob)
   *
   * @param other Blob to be copied
   *
   * @return Copied Blob 
   */
  Blob &Blob::operator=(const Blob &other) {
    p_blobPvl = other.p_blobPvl;
    p_blobName = other.p_blobName;
    p_startByte = other.p_startByte;
    p_nbytes = other.p_nbytes;
    p_type = other.p_type;
    p_detached = other.p_detached;
    p_labelFile = other.p_labelFile;

    p_buffer = NULL;

    if (other.p_buffer) {
      p_buffer = new char[p_nbytes];

      for (int i = 0; i < p_nbytes; i++) {
        p_buffer[i] = other.p_buffer[i];
      }
    }

    return *this;
  }

  //! Destroys the Blob object.
  Blob::~Blob() {
    if (p_buffer != NULL) delete [] p_buffer;
  }

  /** 
   *  Accessor method that returns a string containing the Blob type.
   *  
   *  @return @b string Type of blob.
   */ 
  QString Blob::Type() const {
    return p_type;
  }

  /** 
   *  Accessor method that returns a string containing the Blob name.
   *  
   *  @return @b string The name of the blob.
   */ 
  QString Blob::Name() const {
    return p_blobName;
  }

  /** 
   *  Accessor method that returns the number of bytes in the blob data.
   *  
   *  @return @b int Number of bytes in the blob data.
   */ 
  int Blob::Size() const {
    return p_nbytes;
  }

  /** 
   *  Accessor method that returns a PvlObject containing the Blob label.
   *  
   *  @return @b PvlObject The label of the blob.
   */ 
  PvlObject &Blob::Label() {
    return p_blobPvl;
  }

  /** 
   *  This method searches the given Pvl for the Blob by the Blob's type and
   *  name. If found, the start byte, number of bytes are read from the Pvl.
   *  Also, if a keyword label pointer is found, the filename for the detached
   *  blob is stored and the pointer is removed from the blob pvl.
   *  
   *  @param pvl The Pvl to be searched
   *  @param keywords A list of keyword, value pairs to match inside the blob's
   *  PVL object. Only if all the keyword match is the blob processed. This is used
   *  when there are multiple blobs with the same name, but different keywords that 
   *  define the exact blob (see Stretch with a band number)
   */ 
  void Blob::Find(const Pvl &pvl, const std::vector<PvlKeyword> keywords) {
    bool found = false;
    try {
      // Search for the blob name
      QString blobName = p_blobName.toUpper();
      for (int o = 0; o < pvl.objects(); o++) {
        const PvlObject &obj = pvl.object(o);
        if (obj.isNamed(p_type)) {
          QString curName = obj["Name"];
          curName = curName.toUpper();
          if (blobName == curName) {
            // If there are keywords supplied, check that those match, too!
            if (!keywords.empty()){
              bool keywordFound = true;
              for (PvlKeyword keyword : keywords) {
                if (obj.hasKeyword(keyword.name())) {
                  PvlKeyword blobKeyword = obj.findKeyword(keyword.name());
                  if (blobKeyword == keyword && !blobKeyword.isEquivalent(keyword[0])) {
                    keywordFound = false;
                    break;
                  }
                }
                else {
                  keywordFound = false;
                  break;
                }
              }
              if (keywordFound) {
                p_blobPvl = obj;
                found = true;
                break;
              }
            }
            else {
              p_blobPvl = obj;
              found = true;
              break;
            }
          }
          else {
            if (p_type == "OriginalLabel" && curName == "ORIGINALLABEL") {
              p_blobPvl = obj;
              found = true;
              break;
            }
          }
        }
      }
    }
    catch (IException &e) {
      QString msg = "Invalid " + p_type + " label format";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }

    // Did we find it?
    if (!found) {
      QString msg = "Unable to find " + p_type + " [" + p_blobName + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    // Ok the blob exists so we need to prep for reading the binary data
    try {
      p_startByte = p_blobPvl["StartByte"];
      p_nbytes = p_blobPvl["Bytes"];
      p_detached = "";
      if (p_blobPvl.hasKeyword("^" + p_type)) {
        QString path = "";
        if (p_labelFile != "") {
          path = FileName(p_labelFile).path() + "/";
        }
        p_detached = path + (QString) p_blobPvl["^"+p_type];
        p_blobPvl.deleteKeyword("^" + p_type);
      }
    }
    catch (IException &e) {
      QString msg = "Invalid " + p_type + " label format";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }

  /**
   * This method reads Pvl values from a specified file.
   *
   * @param file The filename to read from.
   *
   * @throws iException::Io - Unable to open file
   * @throws iException::Pvl - Invalid label format
   */
  void Blob::Read(const QString &file, const std::vector<PvlKeyword> keywords) {
    // Expand the filename
    QString temp(FileName(file).expanded());

    // Get the pvl
    Pvl pvl;
    try {
      pvl.read(temp);
    }
    catch (IException &e) {
      QString msg = "Invalid " + p_type + " label format";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
    Read(file, pvl, keywords);
  }

  /**
   * This method reads the given a file and labels.
   *
   * @param file The filename to read from.
   * @param pvlLabels A Pvl containing the label information.
   *
   * @throws iException::Io - Unable to open file
   */
  void Blob::Read(const QString &file, const Pvl &pvlLabels, const std::vector<PvlKeyword> keywords) {
    // Expand the filename
    QString temp(FileName(file).expanded());

    // Open the file
    fstream istm;
    istm.open(temp.toLatin1().data(), std::ios::in);
    if (!istm) {
      QString message = Message::FileOpen(temp);
      throw IException(IException::Io, message, _FILEINFO_);
    }

    try {
      // Check pvl and read from the stream
      Read(pvlLabels, istm, keywords);
    }
    catch (IException &e) {
      istm.close();
      QString msg = "Unable to open " + p_type + " [" + p_blobName +
                   "] in file [" + temp + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    istm.close();
  }

  /**
   * This method reads the Blob data from an open input file stream. 
   *
   * @param pvl A Pvl containing the label information.
   * @param istm The input file stream containing the blob data to be read.
   *
   * @throws iException::Io - Unable to open file
   */
  void Blob::Read(const Pvl &pvl, std::istream &istm, const std::vector<PvlKeyword> keywords){
    try {
      Find(pvl, keywords);
      ReadInit();
      if (p_detached != "") {
        fstream dstm;
        dstm.open(p_detached.toLatin1().data(), std::ios::in);
        if (!dstm) {
          QString message = Message::FileOpen(p_detached);
          throw IException(IException::Io, message, _FILEINFO_);
        }
        ReadData(dstm);
      }
      else {
        ReadData(istm);
      }
    }
    catch (IException &e) {
      QString msg = "Unable to read " + p_type + " [" + p_blobName + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }
  }


 /**
  * This virtual method for classes that inherit Blob. It is not defined in 
  * the Blob class.
  */ 
  void Blob::ReadInit(){
  }

  /**
   * Read binary data from an input stream into the Blob object.
   *
   * @param stream The input stream to read from.
   *
   * @throws IException::Io - Error reading data from stream
   */
  void Blob::ReadData(std::istream &stream) {
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
  }

  /**
   * Write the blob data out to a file.
   *
   * @param file The filename to write to.
   *
   * @throws IException::Io - Unable to open file
   * @throws IException::Io - Error preparing to write data to file
   * @throws IException::Io - Error creating file
   */
  void Blob::Write(const QString &file) {
    // Determine the size of the label and write it out
    try {
      WriteInit();
      Pvl pvl;
      pvl.addObject(p_blobPvl);
      ostringstream os;
      os << pvl << endl;
      os.seekp(0, std::ios::end);
      BigInt nbytes = (BigInt) os.tellp() + (BigInt) 64;
      p_startByte = nbytes + 1 + 1; // 1-based;
      pvl.findObject(p_type)["StartByte"] = toString(p_startByte);
      pvl.findObject(p_type)["Bytes"] = toString(p_nbytes);
      pvl.write(file);

      // Prepare and write the binary data
      fstream stream;
      ios::openmode flags = std::ios::in | std::ios::binary | std::ios::out;
      stream.open(file.toLatin1().data(), flags);
      if (!stream) {
        QString message = "Unable to open [" + file + "]";
        throw IException(IException::Io, message, _FILEINFO_);
      }

      streampos sbyte = p_startByte - 1;
      stream.seekp(sbyte, std::ios::beg);
      if (!stream.good()) {
        stream.close();
        QString msg = "Error preparing to write data to " +
                     p_type + " [" + p_blobName + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      WriteData(stream);
      stream.close();
    }
    catch (IException &e) {
      QString msg = "Unable to create " + p_type + " file [" + file + "]";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }
  }

  /**
   * Write the blob data out to a Pvl object.
   * @param pvl The pvl object to update
   * @param stm stream to write data to
   * @param detachedFileName If the stream is detached from the labels give
   * the name of the file
   */
  void Blob::Write(Pvl &pvl, std::fstream &stm,
                   const QString &detachedFileName, bool overwrite) {
    // Handle 64-bit I/O
    WriteInit();

    // Find out where they wanted to write the blob
    streampos sbyte = stm.tellp();
    sbyte += 1;

    // Find out where the end-of-file is
    stm.seekp(0, std::ios::end);
    streampos eofbyte = stm.tellp();
    eofbyte += 1;

    // Handle detached blobs
    if (detachedFileName != "") {
      p_blobPvl += PvlKeyword("^" + p_type, detachedFileName);
    }


    p_blobPvl["StartByte"] = toString((BigInt)sbyte);
    p_blobPvl["Bytes"] = toString(p_nbytes);

    
    // See if the blob is already in the file
    bool found = false; 
    if (overwrite) {

      for (int i = 0; i < pvl.objects(); i++) {
        if (pvl.object(i).name() == p_blobPvl.name()) {
          PvlObject &obj = pvl.object(i);
          if ((QString)obj["Name"] == (QString)p_blobPvl["Name"]) {
            found = true;

            BigInt oldSbyte = obj["StartByte"];
            int oldNbytes = (int) obj["Bytes"];

            // Does it fit in the old space
            if (p_nbytes <= oldNbytes) {
              p_blobPvl["StartByte"] = obj["StartByte"];
              sbyte = oldSbyte;
            }

            // Was the old space at the end of the file
            else if (((oldSbyte + oldNbytes) == eofbyte) &&
                    (eofbyte >= sbyte)) {
              p_blobPvl["StartByte"] = obj["StartByte"];
              sbyte = oldSbyte;
            }

            // Put it at the requested position/end of the file
            else {
              // Leave this here for clarity
            }

            obj = p_blobPvl;
          }
        }
      }
    }

    // Didn't find the same blob, or don't want to overwrite, so add it to the labels
    if (!found || !overwrite) {
      pvl.addObject(p_blobPvl);
    }

    stm.seekp((BigInt) sbyte - (BigInt)1);
    WriteData(stm);

    // Handle detached blobs
    if (detachedFileName != "") {
      p_blobPvl.deleteKeyword("^" + p_type);
    }
  }

  /**
   * This virtual method for classes that inherit Blob. It is not defined in 
   * the Blob class.
   */ 
  void Blob::WriteInit(){
  }

  /**
   * Writes blob data to a stream
   *
   * @param stream Output steam blob data will be written to
   *
   * @throws IException::Io - Error writing data to stream
   */
  void Blob::WriteData(std::fstream &stream) {
    stream.write(p_buffer, p_nbytes);
    if (!stream.good()) {
      QString msg = "Error writing data to " + p_type + " [" + p_blobName + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * Checks pvl object and returns whether or not it is a Blob
   *
   * @param obj Pvl object
   *
   * @return bool Returns true if the object is a blob, and false if it is not
   */
  bool IsBlob(PvlObject &obj) {
    if (obj.isNamed("TABLE")) return true;
    return false;
  }
} // end namespace isis

