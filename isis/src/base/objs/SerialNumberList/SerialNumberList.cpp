#include "SerialNumberList.h"

#include <QString>

#include "IException.h"
#include "FileList.h"
#include "FileName.h"
#include "SerialNumber.h"
#include "ObservationNumber.h"
#include "IString.h"
#include "Pvl.h"

namespace Isis {
  /**
   * Creates an empty SerialNumberList
   */
  SerialNumberList::SerialNumberList(bool checkTarget) {
    p_checkTarget = checkTarget;
    p_target.clear();
  }


  /**
   * Creates a SerialNumberList from a list of filenames
   *
   * @param listfile The list of files to be given serial numbers
   * @param checkTarget Boolean value that specifies whether or not to check
   *                    to make sure the target names match between files added
   *                    to the serialnumber list
   * @internal
   *   @history 2009-10-20 Jeannie Walldren - Added Progress flag
   *   @history 2009-11-05 Jeannie Walldren - Modified number
   *                          of maximum steps for Progress flag
   */
  SerialNumberList::SerialNumberList(const QString &listfile, bool checkTarget, Progress *progress) {
    p_checkTarget = checkTarget;
    p_target.clear();
    try {
      FileList flist(listfile);
      if(progress != NULL) {
        progress->SetText("Creating Isis 3 serial numbers from list file.");
        progress->SetMaximumSteps((int) flist.size() + 1);
        progress->CheckStatus();
      }
      for(int i = 0; i < flist.size(); i++) {
        Add(flist[i].toString());
        if(progress != NULL) {
          progress->CheckStatus();
        }
      }
    }
    catch(IException &e) {
      QString msg = "Can't open or invalid file list [" + listfile + "]";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }

  /**
   * Destructor
   */
  SerialNumberList::~SerialNumberList() {
  }


  /**
   * Delete a serial number off of the list given the Serial Number
   *
   * @author Sharmila Prasad (9/9/2010)
   *
   * @param sn - serial number
   */
  void SerialNumberList::Delete(const QString &sn)
  {
    int index = SerialNumberIndex(sn);
    QString sFileName = FileName(sn);

    // Delete the reference to this serial number in the
    // vector and the maps
    p_pairs.erase(p_pairs.begin() + index);
    p_serialMap.erase(sn);
    p_fileMap.erase(sFileName);
  }



  /**
   * Adds a new filename / serial number pair to the
   * SerialNumberList
   *
   * @param filename the filename to be added
   * @param def2filename If a serial number could not be found, try to return the
   *                     filename
   *
   * @internal
   * @history 2007-06-04 Tracie Sucharski - Expand filename to include full
   *                        path before adding to list.
   * @history 2010-11-24 Tracie Sucharski - Added bool def2filename parameter.
   *                        This will allow level 2 images to be added to a
   *                        serial number list.
   * @history 2010-11-29 Tracie Sucharski - Only read the Instrument group
   *                        if p_checkTarget is True.  If def2filename is True,
   *                        check for Mapping group if Target does not exist.
   */
  void SerialNumberList::Add(const QString &filename, bool def2filename) {

    Pvl p(Isis::FileName(filename).expanded());
    PvlObject cubeObj = p.FindObject("IsisCube");
    try {
      // Test the target name if desired
      if (p_checkTarget) {
        QString target;
        PvlGroup targetGroup;
        if (cubeObj.HasGroup("Instrument")) {
          targetGroup = cubeObj.FindGroup("Instrument");
        }
        else if (def2filename) {
          // No instrument, try Mapping
          if (cubeObj.HasGroup("Mapping")) {
            targetGroup = cubeObj.FindGroup("Mapping");
          }
          else {
            QString msg = "Unable to find Instrument or Mapping group in ";
            msg += filename + " for comparing target";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }
        else {
          // No Instrument group
          QString msg = "Unable to find Instrument group in " + filename;
          msg += " for comparing target";
          throw IException(IException::User, msg, _FILEINFO_);
        }

        target = targetGroup["TargetName"][0];
        target = target.toUpper();
        if (p_target.isEmpty()) {
          p_target = target;
        }
        else if (p_target != target) {
          QString msg = "Target name of [" + target + "] from file [";
          msg += filename + "] does not match [" + p_target + "]";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      // Create the SN
      QString sn = SerialNumber::Compose(p, def2filename);
      QString on = ObservationNumber::Compose(p, def2filename);
      if(sn == "Unknown") {
        QString msg = "Invalid serial number [Unknown] from file [";
        msg += filename + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      else if(HasSerialNumber(sn)) {
        int index = SerialNumberIndex(sn);
        QString msg = "Duplicate, serial number [" + sn + "] from files [";
        msg += SerialNumberList::FileName(sn) + "] and [" + FileName(index) + "].";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      Pair nextpair;
      nextpair.filename = Isis::FileName(filename).expanded();
      nextpair.serialNumber = sn;
      nextpair.observationNumber = on;
      p_pairs.push_back(nextpair);
      p_serialMap.insert(std::pair<QString, int>(sn, (int)(p_pairs.size() - 1)));
      p_fileMap.insert(std::pair<QString, int>(nextpair.filename, (int)(p_pairs.size() - 1)));
    }
    catch(IException &e) {
      QString msg = "File [" + Isis::FileName(filename).expanded() +
                        "] can not be added to ";
      msg += "serial number list";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }


  void SerialNumberList::Add(const char *serialNumber, const char *filename) {

    Add((QString)serialNumber, (QString)filename);
  }


  /**
   * Adds a new filename / and pre-composed serial number pair to the SerialNumberList
   *
   * @param serialNumber the serial number to be added
   * @param filename the filename to be added
   *  
   * @author 2012-07-12 Tracie Sucharski 
   *  
   * @internal
   */
  void SerialNumberList::Add(const QString &serialNumber, const QString &filename) {

    Pvl p(Isis::FileName(filename).expanded());
    PvlObject cubeObj = p.FindObject("IsisCube");
    try {
      // Test the target name if desired
      if (p_checkTarget) {
        QString target;
        PvlGroup targetGroup;
        if (cubeObj.HasGroup("Instrument")) {
          targetGroup = cubeObj.FindGroup("Instrument");
        }
        else if (cubeObj.HasGroup("Mapping")) {
          // No instrument, try Mapping
          if (cubeObj.HasGroup("Mapping")) {
            targetGroup = cubeObj.FindGroup("Mapping");
          }
        else {
            QString msg = "Unable to find Instrument or Mapping group in ";
            msg += filename + " for comparing target";
            throw IException(IException::User, msg, _FILEINFO_);
          }
        }

        target = targetGroup["TargetName"][0];
        target = target.toUpper();
        if (p_target.isEmpty()) {
          p_target = target;
        }
        else if (p_target != target) {
          QString msg = "Target name of [" + target + "] from file [";
          msg += filename + "] does not match [" + p_target + "]";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      QString observationNumber = "Unknown";
      if (serialNumber == "Unknown") {
        QString msg = "Invalid serial number [Unknown] from file [";
        msg += filename + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      else if (HasSerialNumber(serialNumber)) {
        int index = SerialNumberIndex(serialNumber);
        QString msg = "Duplicate, serial number [" + serialNumber + "] from files [";
        msg += SerialNumberList::FileName(serialNumber) + "] and [" + FileName(index) + "].";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      Pair nextpair;
      nextpair.filename = Isis::FileName(filename).expanded();
      nextpair.serialNumber = serialNumber;
      nextpair.observationNumber = observationNumber;
      p_pairs.push_back(nextpair);
      p_serialMap.insert(std::pair<QString, int>(serialNumber, (int)(p_pairs.size() - 1)));
      p_fileMap.insert(std::pair<QString, int>(nextpair.filename, (int)(p_pairs.size() - 1)));
    }
    catch(IException &e) {
      QString msg = "File [" + Isis::FileName(filename).expanded() +
                        "] can not be added to ";
      msg += "serial number list";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Determines whether or not the requested serial number exists
   * in the list
   *
   * @param sn The serial number to be checked for
   *
   * @return bool
   */
  bool SerialNumberList::HasSerialNumber(QString sn) {
    if(p_serialMap.find(sn) == p_serialMap.end()) return false;
    return true;
  }


  /**
   * How many serial number / filename combos are in the list
   *
   * @return int Returns number of serial numbers current in the list
   */
  int SerialNumberList::Size() const {
    return p_pairs.size();
  }


  /**
   * Return a filename given a serial number
   *
   * @param sn  The serial number of the desired filename
   *
   * @return QString The filename matching the input serial
   *         number
   */
  QString SerialNumberList::FileName(const QString &sn) {
    if(HasSerialNumber(sn)) {
      int index = p_serialMap.find(sn)->second;
      return p_pairs[index].filename;
    }
    else {
      QString msg = "Requested serial number [" + sn + "] ";
      msg += "does not exist in the list";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * return a serial number given a filename
   *
   * @param filename The filename to be matched
   *
   * @return QString The serial number corresponding to the
   *         input filename
   *
   * @internal
   * @history 2007-06-04 Tracie Sucharski - Expand filename to include full
   *                        path before searching list.
   */
  QString SerialNumberList::SerialNumber(const QString &filename) {
    if(p_fileMap.find(Isis::FileName(filename).expanded()) == p_fileMap.end()) {
      QString msg = "Requested filename [" +
                        Isis::FileName(filename).expanded() + "]";
      msg += "does not exist in the list";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    int index = FileNameIndex(filename);
    return p_pairs[index].serialNumber;
  }

  /**
   * Return a serial number given an index
   *
   * @param index The index of the desired serial number
   *
   * @return QString The serial number returned
   */
  QString SerialNumberList::SerialNumber(int index) {
    if(index >= 0 && index < (int) p_pairs.size()) {
      return p_pairs[index].serialNumber;
    }
    else {
      QString num = QString(index);
      QString msg = "Index [" + (QString) num + "] is invalid";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * Return a observation number given an index
   *
   * @param index The index of the desired observation number
   *
   * @return QString The observation number returned
   */
  QString SerialNumberList::ObservationNumber(int index) {
    if(index >= 0 && index < (int) p_pairs.size()) {
      return p_pairs[index].observationNumber;
    }
    else {
      QString num = QString(index);
      QString msg = "Index [" + (QString) num + "] is invalid";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * return a list index given a serial number
   *
   * @param sn The serial number searched for
   *
   * @return int The index of the serial number
   */
  int SerialNumberList::SerialNumberIndex(const QString &sn) {
    if(HasSerialNumber(sn)) {
      return p_serialMap.find(sn)->second;
    }
    else {
      QString msg = "Requested serial number [" + sn + "] ";
      msg += "does not exist in the list";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * Return a list index given a filename
   *
   * @param filename The filename to be searched for
   *
   * @return int The index of the input filename
   *
   *  @internal @history 2007-06-04 Tracie Sucharski - Expand filename to include
   *                        full path before searching list.
   *  @internal @history 2007-07-11 Stuart Sides - Fixed bug where
   *                        the correct index was not returned.
   */
  int SerialNumberList::FileNameIndex(const QString &filename) {

    std::map<QString, int>::iterator  pos;
    if((pos = p_fileMap.find(Isis::FileName(filename).expanded())) == p_fileMap.end()) {
      QString msg = "Requested filename [" +
                        Isis::FileName(filename).expanded() + "]";
      msg += "does not exist in the list";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    return pos->second;
  }

  /**
   * Return the filename at the given index
   *
   * @param index The index of the desired filename
   *
   * @return QString The filename at the given index
   */
  QString SerialNumberList::FileName(int index) {
    if(index >= 0 && index < (int) p_pairs.size()) {
      return p_pairs[index].filename;
    }
    else {
      QString num = toString(index);
      QString msg = "Index [" + num + "] is invalid";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Return possible serial numbers given an observation number
   *
   * @param on  The observation number of the possible serial
   *            number
   *
   * @return vector<QString> The list of possible serial
   *         numbers matching the input observation number
   */
  std::vector<QString> SerialNumberList::PossibleSerialNumbers(const QString &on) {
    std::vector<QString> numbers;
    for(unsigned index = 0; index < p_pairs.size(); index++) {
      if(p_pairs[index].observationNumber == on) {
        numbers.push_back(p_pairs[index].serialNumber);
      }
    }
    if(numbers.size() > 0) {
      return numbers;
    }
    else {
      QString msg = "Requested observation number [" + on + "] ";
      msg += "does not exist in the list";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


}
