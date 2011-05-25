/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/05/14 19:17:09 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <fstream>
#include <string>
#include "Table.h"
#include "iException.h"
#include "Endian.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for creating a table
   *
   * @param tableName Name of the Table to be read
   *
   * @param rec Name of the TableRecord to be read into the Table
   */
  Table::Table(const std::string &tableName, Isis::TableRecord &rec) :
    Blob(tableName, "Table") {
    p_assoc = Table::None;
    p_blobPvl += Isis::PvlKeyword("Records", 0);
    p_blobPvl += Isis::PvlKeyword("ByteOrder", "NULL");
    for(int f = 0; f < rec.Fields(); f++) p_blobPvl.AddGroup(rec[f].PvlGroup());
    p_record = rec;
  }

  /**
   * Constructor for reading a table
   *
   * @param tableName Name of the Table to be read
   *
   * @param file Name of the file to be read into the Table
   */
  Table::Table(const std::string &tableName, const std::string &file) :
    Blob(tableName, "Table") {
    p_assoc = Table::None;
    Read(file);
  }


  Table::Table(const std::string &tableName, const std::string &file,
      const Pvl &fileHeader) : Blob(tableName, "Table") {
    p_assoc = Table::None;
    Read(file, fileHeader);
  }


  /**
   * Copy constructor for table
   *
   * @param other The table to copy from
   */
  Table::Table(const Table &other) : Blob(other) {
    p_record = other.p_record;
    p_records = other.p_records;
    p_assoc = other.p_assoc;
    p_swap = other.p_swap;

    for(unsigned int i = 0; i < other.p_recbufs.size(); i++) {
      char *data = new char[RecordSize()];

      for(int j = 0; j < RecordSize(); j++) {
        data[j] = other.p_recbufs[i][j];
      }

      p_recbufs.push_back(data);
    }
  };

  Table &Table::operator=(const Isis::Table &other) {
    *((Isis::Blob *)this) = *((Isis::Blob *)&other);
    p_record = other.p_record;
    p_records = other.p_records;
    p_assoc = other.p_assoc;
    p_swap = other.p_swap;

    for(unsigned int i = 0; i < other.p_recbufs.size(); i++) {
      char *data = new char[RecordSize()];

      for(int j = 0; j < RecordSize(); j++) {
        data[j] = other.p_recbufs[i][j];
      }

      p_recbufs.push_back(data);
    }

    return *this;
  }

  /**
   * Constructor for reading a table
   *
   * @param tableName Name of the Table to be read
   */
  Table::Table(const std::string &tableName) :
    Isis::Blob(tableName, "Table") {
    p_assoc = Table::None;
  }

  //! Destroys the Table object
  Table::~Table() {
    Clear();
  }

  /**
   * Reads a TableRecord from the Table
   *
   * @param index Index where desired TableRecord is located
   *
   * @return Returns the TableRecord at specific index
   */
  Isis::TableRecord &Table::operator[](const int index) {
    p_record.Unpack(p_recbufs[index]);
    return p_record;
  }

  /**
   * Adds a TableRecord to the Table
   *
   * @param rec The record to be added to the table
   */
  void Table::operator+=(Isis::TableRecord &rec) {
    char *newbuf = new char[RecordSize()];
    rec.Pack(newbuf);
    p_recbufs.push_back(newbuf);
  }

  /**
   * Updates a TableRecord
   *
   * @param rec TableRecord to update old TableRecord with
   *
   * @param index Index of TableRecord to be updated
   */
  void Table::Update(const Isis::TableRecord &rec, const int index) {
    rec.Pack(p_recbufs[index]);
  }

  /**
   * Deletes a TableRecord from the Table
   *
   * @param index Index of TableRecord to be deleted
   */
  void Table::Delete(const int index) {
    vector<char *>::iterator it = p_recbufs.begin();
    for(int i = 0; i < index; i++, it++);
    delete [] p_recbufs[index];
    p_recbufs.erase(it);
  }

  //! Virtual function to validate PVL table information
  void Table::ReadInit() {
    p_records = p_blobPvl["Records"];

    Isis::TableRecord rec;
    for(int g = 0; g < p_blobPvl.Groups(); g++) {
      if(p_blobPvl.Group(g).IsNamed("Field")) {
        Isis::TableField f(p_blobPvl.Group(g));
        rec += f;
      }
    }

    p_record = rec;

    if(p_blobPvl.HasKeyword("Association")) {
      Isis::iString temp = (string) p_blobPvl["Association"];
      temp.UpCase();
      if(temp == "SAMPLES") p_assoc = Table::Samples;
      if(temp == "LINES") p_assoc = Table::Lines;
      if(temp == "BANDS") p_assoc = Table::Bands;
    }

    // Determine if we need to swap stuff when we read the data
    Isis::ByteOrder bo = Isis::ByteOrderEnumeration(p_blobPvl["ByteOrder"]);
    p_swap = false;
    if(Isis::IsLsb() && (bo == Isis::Msb)) p_swap = true;
    if(Isis::IsMsb() && (bo == Isis::Lsb)) p_swap = true;

    // Cleanup in case of a re-read
    Clear();
  }

  /**
   * Virtual function to Read the data
   *
   * @param stream InputStream to read data in from
   *
   * @throws Isis::iException::Io - Error reading or preparing to read a record
   */
  void Table::ReadData(std::istream &stream) {
    for(int rec = 0; rec < p_records; rec++) {
      streampos sbyte = (streampos)(p_startByte - 1) +
                        (streampos)(rec * RecordSize());
      stream.seekg(sbyte, std::ios::beg);
      if(!stream.good()) {
        string msg = "Error preparing to read record [" + Isis::iString(rec + 1) +
                     "] from Table [" + p_blobName + "]";
        throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
      }

      char *buf = new char[RecordSize()];
      stream.read(buf, RecordSize());
      if(!stream.good()) {
        string msg = "Error reading record [" + Isis::iString(rec + 1) +
                     "] from Table [" + p_blobName + "]";
        throw Isis::iException::Message(Isis::iException::Io, msg, _FILEINFO_);
      }

      if(p_swap) p_record.Swap(buf);
      p_recbufs.push_back(buf);
    }
  }

  //! Virtual Function to prepare labels for writing
  void Table::WriteInit() {
    p_blobPvl["Records"] = Records();
    p_nbytes = Records() * RecordSize();

    if(Isis::IsLsb()) {
      p_blobPvl["ByteOrder"] = Isis::iString(Isis::ByteOrderName(Isis::Lsb));
    }
    else {
      p_blobPvl["ByteOrder"] = Isis::iString(Isis::ByteOrderName(Isis::Msb));
    }

    if(p_blobPvl.HasKeyword("Association")) {
      p_blobPvl.DeleteKeyword("Association");
    }
    if(p_assoc == Samples) {
      p_blobPvl += Isis::PvlKeyword("Association", "Samples");
    }
    else if(p_assoc == Lines) {
      p_blobPvl += Isis::PvlKeyword("Association", "Lines");
    }
    else if(p_assoc == Bands) {
      p_blobPvl += Isis::PvlKeyword("Association", "Bands");
    }
  }

  /**
   * Virtual function to write the data
   *
   * @param os Outputstream to write the data to
   */
  void Table::WriteData(std::fstream &os) {
    for(int rec = 0; rec < Records(); rec++) {
      os.write(p_recbufs[rec], RecordSize());
    }
  }

  /**
   * Clear the table of all records
   */
  void Table::Clear() {
    for(int i = 0; i < (int)p_recbufs.size(); i++) delete [] p_recbufs[i];
    p_recbufs.clear();
  }
}
