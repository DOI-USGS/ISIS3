/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Blob.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <gdal_priv.h>

#include <QDebug>

#include "FileName.h"
#include "IException.h"
#include "Message.h"
#include "Pvl.h"

namespace fs = std::filesystem;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
using namespace std;

namespace Isis {
  /**
   * Constructs an Blob object using a name and type.
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
   * This copies the Blob object.
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
   * This makes the two Blob objects exactly the same (copies the Blob)
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
   *  @return @b string Type of Blob.
   */
  QString Blob::Type() const {
    return p_type;
  }

  /**
   *  Accessor method that returns a string containing the Blob name.
   *
   *  @return @b string The name of the Blob.
   */
  QString Blob::Name() const {
    return p_blobName;
  }

  /**
   * Create a unique key for the blob based on name and type
   *
   *  @return @b string The key of the blob.
   */
  QString Blob::Key() const {
    QString key = Type() + "_" + Name();
    return key;
  }

  /**
   *  Accessor method that returns the start byte of the blob data.
   *
   *  @return @b int Start byte of the blob data.
   */
  int Blob::StartByte() const {
    return p_startByte;
  }


  /**
   *  Accessor method that returns the number of bytes in the blob data.
   *
   *  @return @b int Number of bytes in the Blob data.
   */
  int Blob::Size() const {
    return p_nbytes;
  }

  /**
   *  Accessor method that returns a PvlObject containing the Blob label.
   *
   *  @return @b PvlObject The label of the Blob.
   */
  PvlObject &Blob::Label() {
    return p_blobPvl;
  }

  /**
   *  This method searches the given Pvl for the Blob by the Blob's type and
   *  name. If found, the start byte, number of bytes are read from the Pvl.
   *  Also, if a keyword label pointer is found, the filename for the detached
   *  Blob is stored and the pointer is removed from the Blob pvl.
   *
   *  @param pvl The Pvl to be searched
   *  @param keywords A list of keyword, value pairs to match inside the Blob's
   *  PVL object. Only if all the keyword match is the Blob processed. This is used
   *  when there are multiple Blobs with the same name, but different keywords that
   *  define the exact Blob (see Stretch with a band number)
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
   *  This method reads a blob from a gdal GDALDataset
   *
   *  @param GDALDataset An open gdal dataset 
   */
  void Blob::ReadGdal(GDALDataset *dataset) {
    try {
      std::string key = Key().toStdString();

      const char* metadata = dataset->GetMetadataItem(key.c_str(), "json:ISIS3");

      if (metadata == nullptr) {
        QString msg = "The key [" + QString::fromStdString(key) + "] does not exist on the geodata set.";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
      
      ordered_json jsonblob = {};
      jsonblob[key] = nlohmann::ordered_json::parse(metadata);

      std::string blobData = jsonblob[key]["_data"];
      jsonblob[key].erase("_data");

      Pvl pvl;
      Pvl::readObject(pvl, jsonblob);

      p_blobName = QString::fromStdString(jsonblob[key]["Name"]);
      if (jsonblob[key].contains("_container_name")) {
        p_type = QString::fromStdString(jsonblob[key]["_container_name"]);
      }

      Find(pvl);
      ReadData(blobData);
    }
    catch(exception &e) { 
      QString msg = "Failed to read blob [" + p_blobName + "]: " + e.what();
      throw IException(IException::Io, msg, _FILEINFO_);
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
   * @param istm The input file stream containing the Blob data to be read.
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
    p_buffer = new char[p_nbytes + 1];
    p_buffer[p_nbytes] = '\0';

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
   * Set the data stored in the Blob.
   *
   * This function will copy the data buffer, if you want to avoid that use
   * takeData which will take ownership of the buffer.
   *
   * @param buffer The buffer of data. The Blob does not take ownership of this,
   *               the caller is still responsible for cleaning it up.
   * @param nbytes The amount of data in the buffer
   */
  void Blob::setData(const char *buffer, int nbytes) {
    char *buf;
    if (buffer[nbytes - 1] != '\0') {
      buf = new char[nbytes + 1];
      memcpy(buf, buffer, nbytes);
      buf[nbytes] = '\0';
    }
    else {
      buf = new char[nbytes];
      memcpy(buf, buffer, nbytes);
    }
    takeData(buf, nbytes);
  }


  /**
   * Set the data stored in the Blob without copying it.
   *
   * This function takes ownership of the data buffer and will delete is when the Blob
   * is cleaned up.
   *
   * @param buffer The buffer of data. The Blob takes ownership of this.
   * @param nbytes The amount of data in the buffer
   */
  void Blob::takeData(char *buffer, int nbytes) {
    p_nbytes = nbytes;

    if (p_buffer != NULL) {
      delete [] p_buffer;
    }

    p_buffer = buffer;
  }

  /**
   * Write the blob out to a json blob
   *
   * @param json String to write the json data to
   */
  void Blob::WriteGdal(std::string &json) {
    try {
      stringstream stream;
      WriteData(stream);

      p_blobPvl["Bytes"] = toString(p_nbytes);
      p_blobPvl["StartByte"] = toString(1);
      if(p_blobPvl.hasKeyword("_data")) { 
        p_blobPvl["_data"] = QString::fromStdString(stream.str()); 
      }
      else { 
        p_blobPvl += PvlKeyword("_data", QString::fromStdString(stream.str()));
      }

      if(p_blobPvl.hasKeyword("Name")) { 
        p_blobPvl["Name"] = p_blobName; 
      }
      else { 
        p_blobPvl += PvlKeyword("Name", p_blobName);
      }

      // update metadata
      Pvl pvl;
      pvl.addObject(p_blobPvl);

      std::string key = Key().toStdString();

      ordered_json jsonblob = ordered_json::parse(json);
      jsonblob[key] = pvl.toJson()["Root"][key];
      json = jsonblob.dump();
    }
    catch(exception &e) {
      QString msg = "Failed to write blob [" + p_blobName + "]: " + QString(e.what());
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }


  /**
   * Write the Blob data out to a file.
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
   * Write the Blob data out to a Pvl object.
   * @param pvl The pvl object to update
   * @param stm stream to write data to
   */
  void Blob::Write(Pvl &pvl, std::fstream &stm) {
                    
    // Handle 64-bit I/O
    WriteInit();

    // Find out where they wanted to write the Blob
    streampos sbyte = stm.tellp();

    p_blobPvl["StartByte"] = toString((BigInt) sbyte  + 1);
    p_blobPvl["Bytes"] = toString(p_nbytes);
    bool found = false;
    for (int i = 0; i < pvl.objects(); i++) {
      if (pvl.object(i).name() == p_blobPvl.name()) {
        PvlObject &obj = pvl.object(i);
        if ((QString)obj["Name"] == (QString)p_blobPvl["Name"]) {
          found = true;
          obj = p_blobPvl;
        }
      }
    }
    if (!found) {
      pvl.addObject(p_blobPvl);
    }
    WriteData(stm);
  }

  /**
   * Get the internal data buff of the Blob.
   *
   * @return @b char* A data buffer containing Blob::Size bytes
   */
  char *Blob::getBuffer() {
    return p_buffer;
  }

  /**
   * This virtual method for classes that inherit Blob. It is not defined in
   * the Blob class.
   */
  void Blob::WriteInit(){
  }

  /**
   * Writes Blob data to a stream
   *
   * @param stream Output steam Blob data will be written to
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
   * Writes blob data to a stream
   *
   * @param stream Output stream blob data will be written to
   *
   * @throws IException::Io - Error writing data to stream
   */
  void Blob::WriteData(std::stringstream &stream) {
    stream << std::hex << std::setfill('0');
    int copy = 0;
    for (int i = 0; i < p_nbytes; ++i) {
      memcpy(&copy, &(p_buffer[i]), 1);
      stream << std::setw(2) << copy;
    }

    if (!stream.good()) {
      QString msg = "Error writing data to " + p_type + " [" + p_blobName + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * Writes blob data to a stream
   *
   * @param stream Output steam blob data will be written to
   *
   * @throws IException::Io - Error writing data to stream
   */
  void Blob::ReadData(string &hexdata) {
    // Read the binary data
    if (p_buffer != NULL) delete [] p_buffer;
    p_buffer = new char[p_nbytes + 1];
    p_buffer[p_nbytes] = '\0';

    // Loop through the hex string and bytes, hex is two characters at a time 
    for (size_t i=0,j=0; i < static_cast<size_t>(p_nbytes); i++,j+=2) {
        string byteString = hexdata.substr(j, 2);
  
        char byteValue = static_cast<char>( 
            stoi(byteString, nullptr, 16));
  
        // Add the byte to the byte array 
        p_buffer[i] = byteValue;
    }
  }



  /**
   * Checks pvl object and returns whether or not it is a Blob
   *
   * @param obj Pvl object
   *
   * @return bool Returns true if the object is a Blob, and false if it is not
   */
  bool IsBlob(PvlObject &obj) {
    if (obj.isNamed("TABLE")) return true;
    return false;
  }
} // end namespace isis
