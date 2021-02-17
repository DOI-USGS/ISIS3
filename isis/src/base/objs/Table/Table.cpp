/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Table.h"

#include <fstream>
#include <string>

#include "Endian.h"
#include "IException.h"
#include "Pvl.h"
#include "TableField.h"

using namespace std;
namespace Isis {

  /**
   * This constructor creates a new table using the given name and record. 
   *  
   * Note that the record is not added to this table.  It is used to read the 
   * TableField names and set the record size (i.e. the number of bytes per 
   * record). Thus any records added to this table will be required to match 
   * this size. 
   *  
   * In this constructor, the Table::Association is set to None, the ByteOrder
   * keyword in the labels is set to NULL, and the record information is added 
   * to the table. 
   *  
   * This constructor also calls the parent constructor Blob(tableName, 
   * "Table"). 
   *
   * @param tableName Name of the Table to be read
   * @param rec Name of the TableRecord to be read into the Table
   */
  Table::Table(const QString &tableName, Isis::TableRecord &rec) :
    Blob(tableName, "Table") {
    p_assoc = Table::None;
    p_blobPvl += Isis::PvlKeyword("Records", 0);
    p_blobPvl += Isis::PvlKeyword("ByteOrder", "NULL");
    for (int f = 0; f < rec.Fields(); f++) p_blobPvl.addGroup(rec[f].pvlGroup());
    p_record = rec;
  }

  /**
   * This constructor creates an empty table from an existing table name 
   * to be read in when the Read() method is called. It should not be 
   * used to construct a new table object whose data will be filled 
   * in later since the record size will be set to 0. This constructor 
   * sets the Table::Association to None. 
   *  
   * This constructor also calls the parent constructor 
   * Blob(tableName, "Table").
   *
   * @param tableName Name of the Table to be read
   */
  Table::Table(const QString &tableName) :
    Isis::Blob(tableName, "Table") {
    p_assoc = Table::None;
  }

  /**
   * This constructor reads an existing table using the given table name and 
   * file containing the table. This constructor sets the Table::Association
   * to the Association keyword value in the Blob Pvl read from the file, if
   * the keyword exists.
   *  
   * This constructor also calls the parent constructor 
   * Blob(tableName, "Table").
   *  
   * @param tableName Name of the Table to be read
   * @param file Name of the file to be read into the Table 
   *  
   * @see Blob::Read() 
   */
  Table::Table(const QString &tableName, const QString &file) :
    Blob(tableName, "Table") {
    p_assoc = Table::None;
    Read(file);
  }

  /**
   * This constructor reads an existing table using the given table name and 
   * file containing the table and pvl labels. This constructor sets the 
   * Table::Association to the Association keyword value in the Blob Pvl 
   * read from the file, if the keyword exists. 
   *  
   * This constructor also calls the parent constructor 
   * Blob(tableName, "Table").
   *
   * @param tableName The name of the Table to be read
   * @param file The name of the file to be read into the Table
   * @param fileHeader Pvl labels.
   *  
   * @see Blob::Read() 
   */
  Table::Table(const QString &tableName, const QString &file,
      const Pvl &fileHeader) : Blob(tableName, "Table") {
    p_assoc = Table::None;
    Read(file, fileHeader);
  }


  /**
   * Copy constructor for an Table object.  This constructor copies TableRecords
   * and the member variable values for record, records, assoc, and swap. 
   *
   * @param other The table to copy from
   */
  Table::Table(const Table &other) : Blob(other) {
    p_record = other.p_record;
    p_records = other.p_records;
    p_assoc = other.p_assoc;
    p_swap = other.p_swap;

    for (unsigned int i = 0; i < other.p_recbufs.size(); i++) {
      char *data = new char[RecordSize()];

      for (int j = 0; j < RecordSize(); j++) {
        data[j] = other.p_recbufs[i][j];
      }

      p_recbufs.push_back(data);
    }
  }

  /**
   * Sets the Table equal to the input Table object.  This method copies 
   * TableRecords and the member variable values for record, records, assoc, and 
   * swap. 
   *
   * @param other The table to copy from 
   *  
   * @return @b Table The copied table.
   */
  Table &Table::operator=(const Isis::Table &other) {
    Clear();
    *((Isis::Blob *)this) = *((Isis::Blob *)&other);
    p_record = other.p_record;
    p_records = other.p_records;
    p_assoc = other.p_assoc;
    p_swap = other.p_swap;

    for (unsigned int i = 0; i < other.p_recbufs.size(); i++) {
      char *data = new char[RecordSize()];

      for (int j = 0; j < RecordSize(); j++) {
        data[j] = other.p_recbufs[i][j];
      }

      p_recbufs.push_back(data);
    }

    return *this;
  }

  //! Destroys the Table object
  Table::~Table() {
    Clear();
  }

  /**
   * Sets the association to the input parameter
   *
   * @param assoc Association type
   */
  void Table::SetAssociation(const Table::Association assoc) {
    p_assoc = assoc;
  }

  /**
   * Checks to see if association is Samples
   *
   * @return @b bool Returns true if association is Samples, and false if it is 
   *         not
   */
  bool Table::IsSampleAssociated() {
    return (p_assoc == Table::Samples);
  }

  /**
   * Checks to see if association is Lines
   *
   * @return @b bool Returns true if association is Lines, and false if it is 
   *         not
   */
  bool Table::IsLineAssociated() {
    return (p_assoc == Table::Lines);
  }

  /**
   * Checks to see if association is Bands
   *
   * @return @b bool Returns true if association is Bands, and false if it is 
   *         not
   */
  bool Table::IsBandAssociated() {
    return (p_assoc == Table::Bands);
  }

  /**
   * Returns the number of records
   *
   * @return @b int Number of records
   */
  int Table::Records() const {
    return p_recbufs.size();
  }

  /**
   * Returns the number of fields per record
   *
   * @return @b int Number of fields
   */
  int Table::RecordFields() const {
    return p_record.Fields();
  }

  /**
   * Returns the number of bytes per record
   *
   * @return @b int Number of bytes per record
   */
  int Table::RecordSize() const {
    return p_record.RecordSize();
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
    if (RecordSize() == 0) {
      IString msg = "Unable to add records to Isis Table [" 
                    + p_blobName + "]. Bytes per record = [0 bytes].";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

     if (RecordSize() != rec.RecordSize()) {
       QString msg = "Unable to add the given record with size = [" 
                     + Isis::toString(rec.RecordSize()) + " bytes] to to Isis Table [" 
                     + p_blobName + "] with record size = [" 
                     + Isis::toString(RecordSize()) + " bytes]. Record sizes must match.";
       throw IException(IException::Unknown, msg, _FILEINFO_);
     }
    char *newbuf = new char[RecordSize()];
    rec.Pack(newbuf);
    p_recbufs.push_back(newbuf);
  }

  /**
   * Updates a TableRecord
   *
   * @param rec TableRecord to update old TableRecord with
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
    for (int i = 0; i < index; i++, it++);
    delete [] p_recbufs[index];
    p_recbufs.erase(it);
  }

  /**
   * Clear the table of all records
   */
  void Table::Clear() {
    for (int i = 0; i < (int)p_recbufs.size(); i++) delete [] p_recbufs[i];
    p_recbufs.clear();
  }

  //! Virtual function to validate PVL table information
  void Table::ReadInit() {
    p_records = p_blobPvl["Records"];

    Isis::TableRecord rec;
    for (int g = 0; g < p_blobPvl.groups(); g++) {
      if (p_blobPvl.group(g).isNamed("Field")) {
        Isis::TableField f(p_blobPvl.group(g));
        rec += f;
      }
    }

    p_record = rec;

    if (p_blobPvl.hasKeyword("Association")) {
      QString temp = (QString) p_blobPvl["Association"];
      temp = temp.toUpper();
      if (temp == "SAMPLES") p_assoc = Table::Samples;
      if (temp == "LINES") p_assoc = Table::Lines;
      if (temp == "BANDS") p_assoc = Table::Bands;
    }

    // Determine if we need to swap stuff when we read the data
    Isis::ByteOrder bo = Isis::ByteOrderEnumeration(p_blobPvl["ByteOrder"]);
    p_swap = false;
    if (Isis::IsLsb() && (bo == Isis::Msb)) p_swap = true;
    if (Isis::IsMsb() && (bo == Isis::Lsb)) p_swap = true;

    // Cleanup in case of a re-read
    Clear();
  }

  /**
   * Virtual function to Read the data
   *
   * @param stream InputStream to read data in from
   *
   * @throws Isis::IException::Io - Error reading or preparing to read a record
   */
  void Table::ReadData(std::istream &stream) {
    for (int rec = 0; rec < p_records; rec++) {
      streampos sbyte = (streampos)(p_startByte - 1) +
                        (streampos)(rec * RecordSize());
      stream.seekg(sbyte, std::ios::beg);
      if (!stream.good()) {
        QString msg = "Error preparing to read record [" + Isis::toString(rec + 1) + 
                     "] from Table [" + p_blobName + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      char *buf = new char[RecordSize()];
      stream.read(buf, RecordSize());
      if (!stream.good()) {
        QString msg = "Error reading record [" + Isis::toString(rec + 1) + 
                      "] from Table [" + p_blobName + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      if (p_swap) p_record.Swap(buf);
      p_recbufs.push_back(buf);
    }
  }

  //! Virtual Function to prepare labels for writing
  void Table::WriteInit() {
    p_blobPvl["Records"] = Isis::toString(Records());
    p_nbytes = Records() * RecordSize();

    if (Isis::IsLsb()) {
      p_blobPvl["ByteOrder"] = Isis::ByteOrderName(Isis::Lsb);
    }
    else {
      p_blobPvl["ByteOrder"] = Isis::ByteOrderName(Isis::Msb);
    }

    if (p_blobPvl.hasKeyword("Association")) {
      p_blobPvl.deleteKeyword("Association");
    }
    if (p_assoc == Samples) {
      p_blobPvl += Isis::PvlKeyword("Association", "Samples");
    }
    else if (p_assoc == Lines) {
      p_blobPvl += Isis::PvlKeyword("Association", "Lines");
    }
    else if (p_assoc == Bands) {
      p_blobPvl += Isis::PvlKeyword("Association", "Bands");
    }
  }

  /**
   * Virtual function to write the data
   *
   * @param os Outputstream to write the data to
   */
  void Table::WriteData(std::fstream &os) {
    for (int rec = 0; rec < Records(); rec++) {
      os.write(p_recbufs[rec], RecordSize());
    }
  }


  QString Table::toString(Table table, QString fieldDelimiter) {
    QString tableValues;
    // add the first record with header, the given delimiter, and a new line after each record
    tableValues += TableRecord::toString(table[0], fieldDelimiter, true, true);
    // add remaining records without header, the same delimeter, and new line after each record
    for (int recordIndex = 1; recordIndex < table.Records(); recordIndex++) {
      tableValues += TableRecord::toString(table[recordIndex], fieldDelimiter);
    }
    return tableValues;
  }

}
