/**
 * @file
 * $Revision$
 * $Date$
 * $Id$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <string>
#include <vector>
#include <numeric>
#include <iostream>
#include <sstream>

#include "LoadCSV.h"
#include "Filename.h"
#include "IException.h"

using namespace std;

namespace Isis {


  LoadCSV::LoadCSV() : _base(), _csvSpecs("LoadCSV"), _data(0,0), _history() { }

  LoadCSV::LoadCSV(const std::string &base, const HiCalConf &conf,
                   const DbProfile &profile) : _base(), _csvSpecs("LoadCSV"),
                   _data(0,0), _history() {
    load(base, conf, profile);
  }

  void LoadCSV::load(const std::string &base, const HiCalConf &conf,
                     const DbProfile &profile) {

    //  Initialize the object with necessary info
    init(base, conf, profile);

    // Set up access to the CSV table.  Note that HiCalConf.getMatrixSource()
    // method is typically used, but the parsing has been broken up here for
    // implementation purposes.
    string csvfile(conf.filepath(getValue()));
    addHistory("File", csvfile);
    CSVReader csv;

    //  Retrieve information regarding the format within the CSV
    bool colHeader(IsEqual(ConfKey(_csvSpecs,makeKey("Header"), string("FALSE")), "TRUE"));
    bool rowHeader(colHeader);  // Both default to state of the {BASE}Header
    if (IsEqual(getValue("ColumnHeader"), "TRUE"))  colHeader = true;
    if (IsEqual(getValue("RowHeader"), "TRUE"))  rowHeader = true;
    if (_csvSpecs.exists(makeKey("ColumnName"))) colHeader = true;
    if (_csvSpecs.exists(makeKey("RowName")   )) rowHeader = true;

    // Skip lines, comment headers and separator
    int skip = ConfKey(_csvSpecs, makeKey("SkipLines"), 0);
    addHistory("SkipLines", ToString(skip));
    bool comments = IsEqual(ConfKey(_csvSpecs, makeKey("IgnoreComments"), string("TRUE")));
    string separator = ConfKey(_csvSpecs, makeKey("Separator"), string(","));
    if (separator.empty()) separator = ",";   // Guarantees content

    // Apply conditions
    csv.setComment(comments);
    csv.setSkip(skip);
    csv.setHeader(colHeader);
    csv.setDelimiter(separator[0]);
    if (separator[0] == ' ') csv.setSkipEmptyParts();

    //  Now read the file
    Filename csvF(csvfile);
    csvfile = csvF.Expanded();
    try {
      csv.read(csvfile);
    } catch (IException &ie) {
      string mess =  "Could not read CSV file \'" + csvfile + "\'";
      throw IException(IException::User, mess, _FILEINFO_);
    }

    //  Now get the data from the CSV table
    int ncols = csv.columns();
    int nrows = csv.rows();

    // Initial conditions selects all rows and columns
    int startColumn((rowHeader) ? 1 : 0), endColumn(ncols-1);
    int startRow(0), endRow(nrows-1);

    // Update columns
    string colName(getValue("ColumnName"));
    if (!colName.empty()) {
      addHistory("ColumnName", colName);
      CSVReader::CSVAxis chead = csv.getHeader();
      startColumn = getAxisIndex(colName, chead);
      if (startColumn < 0) {
        string mess = "Column name " + colName +
                      " not found in CSV file " + csvfile;
        throw IException(IException::User, mess, _FILEINFO_);
      }
      endColumn = startColumn;
      addHistory("ColumnIndex", ToString(startColumn));
    }
    else if (!(getValue("ColumnIndex").empty())) {
       startColumn = ToInteger(getValue("ColumnIndex")) +
                                 ((rowHeader) ? 1 : 0);
       endColumn = startColumn;
      addHistory("ColumnStart", ToString(startColumn));
      addHistory("ColumnEnd", ToString(endColumn));
    }

    // Update row indicies
    string rowName(getValue("RowName"));
    if (!rowName.empty()) {
      addHistory("RowName", rowName);
      if (!rowHeader) {
        string mess = "Row name given but config does not specify presence of row header!";
        throw IException(IException::User, mess, _FILEINFO_);
      }
      CSVReader::CSVAxis rhead = csv.getColumn(0);
      startRow = getAxisIndex(rowName, rhead);
      if (startRow < 0) {
        string mess = "Row name " + rowName + " not found in CSV file " + csvfile;
        throw IException(IException::User, mess, _FILEINFO_);
      }
      endRow = startRow;
      addHistory("RowIndex", ToString(startRow));
    }
    else if (!(getValue("RowIndex").empty())) {
       startRow = ToInteger(getValue("RowIndex"));
       if (rowHeader) startRow++;
       endRow = startRow;
      addHistory("RowStart", ToString(startRow));
      addHistory("RowEnd", ToString(endRow));
    }

    //  Now ready to read all row/columns and convert to matrix
    int drows(endRow-startRow+1);
    int dcols(endColumn-startColumn+1);
    HiMatrix d(drows, dcols);
    vector<string> errors;
    for (int r = startRow, hr = 0 ; r <= endRow ; r++, hr++) {
      CSVReader::CSVAxis row = csv.getRow(r);
      for (int c = startColumn, hc = 0 ; c <= endColumn ; c++, hc++) {
        try {
          d[hr][hc] = ToDouble(row[c]);
        }
        catch (...) {
          std::ostringstream mess;
          mess << "Invalid real value (" << row[c] << ") in row index " << r;
          if (!rowName.empty()) mess << " (Name:" << rowName << ")";
          mess << ", column index " << c;
          if (!colName.empty()) mess << " (Name:" << colName << ")";
          errors.push_back(mess.str());
          d[hr][hc] = Null;
        }
      }
    }

    // Save data anyway
    _data = d;

    // Check for errors
    if (errors.size() > 0) {
      //iException::Clear(); Not sure how this could ever do anything
      std::ostringstream mess;
      mess << "Conversion errors in CSV file " + csvfile + ": Errors: ";
      std::copy(errors.begin(), errors.end(),
                std::ostream_iterator<std::string>(mess,"; "));
      throw IException(IException::User, mess.str(), _FILEINFO_);
    }
    return;
  }


  std::string LoadCSV::filename() const {
    return (getValue());
  }

  int LoadCSV::size() const {
    return (_data.dim1() * _data.dim2());
  }

  bool LoadCSV::validateSize(const int &expected, const bool &throw_on_error)
                             const {
    if (expected != size()) {
      if (!throw_on_error) return (false);
      ostringstream mess;
      mess << "Invalid count (Expected: " << expected << ", Received: "
           << size() << ") in CSV file " << getValue();
      throw IException(IException::User, mess.str(), _FILEINFO_);
    }
    return (true);
  }

  HiVector LoadCSV::getVector() const {
    HiVector v(size(), const_cast<double *> (_data[0]));
    return (v.copy());
  }

  HiMatrix LoadCSV::getMatrix() const {
    return (_data.copy());
  }


  void LoadCSV::History(HiHistory &history) const {
    std::ostringstream mess;
    mess << "LoadCSV(";
    string comma("");
    for (unsigned int i = 0 ; i < _history.size() ; i++) {
      mess << comma << _history[i];
      comma = ",";
    }
    mess << ")";
    history.add(mess.str());
    return;
  }

  void LoadCSV::init(const std::string &base, const HiCalConf &conf,
                     const DbProfile &profile) {
    _base = base;
    _csvSpecs = ResolveKeys(base, conf, profile);
    _history.clear();
    return;
  }

  void LoadCSV::addHistory(const std::string &element,
                           const std::string &desc) {
    std::ostringstream mess;
    mess << element << "[" << desc << "]";
    _history.push_back(mess.str());
  }

  void LoadCSV::getKeyList(const std::string &base,
                           std::vector<std::string> &keys) const {
    keys.clear();
    keys.push_back(base);
    keys.push_back(base+"IgnoreComments");
    keys.push_back(base+"ColumnHeader");
    keys.push_back(base+"ColumnName");
    keys.push_back(base+"ColumnIndex");
    keys.push_back(base+"RowHeader");
    keys.push_back(base+"RowName");
    keys.push_back(base+"RowIndex");
    keys.push_back(base+"SkipLines");
    keys.push_back(base+"Header");
    keys.push_back(base+"Separator");
    return;
  }

  DbProfile LoadCSV::ResolveKeys(const std::string &base, const HiCalConf &conf,
                                 const DbProfile &prof) const {
    vector<string> keys;
    getKeyList(base, keys);
    DbProfile keyprof("LoadCSV");
    for (unsigned int i = 0 ; i < keys.size() ; i++) {
      string kvalue = ParsedKey(keys[i], conf, prof);
      if (!kvalue.empty())  keyprof.add(keys[i], kvalue);
    }
    return (keyprof);
  }

  std::string LoadCSV::ParsedKey(const std::string &key, const HiCalConf &conf,
                        const DbProfile &prof) const {
    string value("");
    if (prof.exists(key)) {
      value = conf.resolve(prof(key), prof);
    }
    return (value);
  }

  std::string LoadCSV::makeKey(const std::string &suffix) const {
    string key = _base + suffix;
    return (key);
  }

  std::string LoadCSV::getValue(const std::string &suffix) const {
    string key = makeKey(suffix);
    string value("");
    if (_csvSpecs.exists(key)) value = _csvSpecs(key);
    return (value);
  }

  int LoadCSV::getAxisIndex(const std::string &name,
                            const CSVReader::CSVAxis &header) const {
    std::string head = iString(name).Trim(" \r\n\t");
    for (int i = 0 ; i < header.dim() ; i++) {
      if (iString::Equal(head,iString(header[i]).Trim(" \r\n\t"))) {
        return (i);
      }
    }
    return (-1);
  }

} // namespace ISIS
