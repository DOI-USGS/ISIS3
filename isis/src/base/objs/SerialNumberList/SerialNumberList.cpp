#include "SerialNumberList.h"
#include "iException.h"
#include "FileList.h"
#include "Filename.h"
#include "SerialNumber.h"
#include "ObservationNumber.h"
#include "iString.h"
#include "Pvl.h"

namespace Isis {
  /**
   * Creates an empty SerialNumberList
   */
  SerialNumberList::SerialNumberList (bool checkTarget) {
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
  SerialNumberList::SerialNumberList (const std::string &listfile, bool checkTarget, Progress *progress) {
    p_checkTarget = checkTarget;
    p_target.clear();
    try {
      FileList flist(listfile);
      if (progress != NULL) {
        progress->SetText("Creating Isis 3 serial numbers from list file.");
        progress->SetMaximumSteps((int) flist.size()+1);
        progress->CheckStatus();
      }
      for (int i=0; i<(int)flist.size(); i++) {
        Add(flist[i]);
        if (progress != NULL){
          progress->CheckStatus();
        }
      }
    }
    catch (iException &e) {
      std::string msg = "Can't open or invalid file list [" + listfile + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }

  /**
   * Destructor
   */
  SerialNumberList::~SerialNumberList () {
  }
  

  /**
   * Adds a new filename / serial number pair to the
   * SerialNumberList
   * 
   * @param filename the filename to be added
   * 
   * @internal
   * @history 2007-06-04 Tracie Sucharski - Expand filename to include full
   *                        path before adding to list.
   */
  void SerialNumberList::Add (const std::string &filename) {
    
    Pvl p(Isis::Filename(filename).Expanded());
    try {
      PvlGroup instrument = p.FindGroup("Instrument",Isis::Pvl::Traverse);
      iString target = instrument["TargetName"][0];
      target.UpCase();

      // Test the target name if desired
      if (p_checkTarget && !p_target.empty()) {
        if (p_target != target) {
          std::string msg = "Target name of [" + target + "] from file [";
          msg += filename + "] does not match [" + p_target + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }
      }
      else {
        p_target = target;
      }

      // Create the SN
      std::string sn = SerialNumber::Compose(p);
      std::string on = ObservationNumber::Compose(p);
      if (sn == "Unknown") {
        std::string msg = "Invalid serial number [Unknown] from file [";
        msg += filename + "]";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }
      else if (HasSerialNumber(sn)) {
        int index = SerialNumberIndex(sn);
        std::string msg = "Duplicate, serial number [" + sn + "] from files [";
        msg += SerialNumberList::Filename(sn) + "] and [" + Filename(index) + "].";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }
      Pair nextpair;
      nextpair.filename = Isis::Filename(filename).Expanded();
      nextpair.serialNumber = sn;
      nextpair.observationNumber = on;
      p_pairs.push_back(nextpair);
      p_serialMap.insert(std::pair<std::string,int>(sn,(int)(p_pairs.size()-1)));
      p_fileMap.insert(std::pair<std::string,int>(nextpair.filename,(int)(p_pairs.size()-1)));
    }
    catch (iException &e) {
      std::string msg = "File [" + Isis::Filename(filename).Expanded() +
                          "] can not be added to ";
      msg += "serial number list";
      throw iException::Message(iException::User,msg,_FILEINFO_);
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
  bool SerialNumberList::HasSerialNumber (const std::string &sn) {
    if (p_serialMap.find(sn) == p_serialMap.end()) return false;
    return true;
  }


  /**
   * How many serial number / filename combos are in the list
   *
   * @return int Returns number of serial numbers current in the list
   */
  int SerialNumberList::Size () const {
    return p_pairs.size();
  }


  /**
   * Return a filename given a serial number
   * 
   * @param sn  The serial number of the desired filename
   * 
   * @return std::string The filename matching the input serial
   *         number
   */
  std::string SerialNumberList::Filename (const std::string &sn) {
    if (HasSerialNumber(sn)) {
      int index = p_serialMap.find(sn)->second;
      return p_pairs[index].filename;
    }
    else {
      std::string msg = "Requested serial number [" + sn + "] ";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }

  /**
   * return a serial number given a filename
   * 
   * @param filename The filename to be matched
   * 
   * @return std::string The serial number corresponding to the
   *         input filename
   * 
   * @internal
   * @history 2007-06-04 Tracie Sucharski - Expand filename to include full
   *                        path before searching list.
   */
  std::string SerialNumberList::SerialNumber(const std::string &filename){
    if (p_fileMap.find(Isis::Filename(filename).Expanded()) == p_fileMap.end()) {
      std::string msg = "Requested filename [" +
                          Isis::Filename(filename).Expanded() + "]";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    int index = FilenameIndex(filename);
    return p_pairs[index].serialNumber;
  }

  /**
   * Return a serial number given an index
   * 
   * @param index The index of the desired serial number
   * 
   * @return std::string The serial number returned
   */
  std::string SerialNumberList::SerialNumber (int index){
    if (index >= 0 && index < (int) p_pairs.size()) {
      return p_pairs[index].serialNumber;
    }
    else {
      iString num = iString(index);
      std::string msg = "Index [" + (std::string) num + "] is invalid";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }

  /**
   * Return a observation number given an index
   * 
   * @param index The index of the desired observation number
   * 
   * @return std::string The observation number returned
   */
  std::string SerialNumberList::ObservationNumber (int index){
    if (index >= 0 && index < (int) p_pairs.size()) {
      return p_pairs[index].observationNumber;
    }
    else {
      iString num = iString(index);
      std::string msg = "Index [" + (std::string) num + "] is invalid";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }

  /**
   * return a list index given a serial number
   * 
   * @param sn The serial number searched for
   * 
   * @return int The index of the serial number
   */
  int SerialNumberList::SerialNumberIndex(const std::string &sn) {
    if (HasSerialNumber(sn)) {
      return p_serialMap.find(sn)->second;
    }
    else {
      std::string msg = "Requested serial number [" + sn + "] ";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
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
  int SerialNumberList::FilenameIndex(const std::string &filename){

    std::map<std::string,int>::iterator  pos;
    if ((pos = p_fileMap.find(Isis::Filename(filename).Expanded())) == p_fileMap.end()) {
      std::string msg = "Requested filename [" +
                             Isis::Filename(filename).Expanded() + "]";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    return pos->second;
  }

  /**
   * Return the filename at the given index
   * 
   * @param index The index of the desired filename
   * 
   * @return std::string The filename at the given index
   */
  std::string SerialNumberList::Filename (int index) {
    if (index >=0 && index < (int) p_pairs.size()) {
      return p_pairs[index].filename;
    }
    else{
      iString num = iString(index);
      std::string msg = "Index [" + (std::string) num + "] is invalid";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }


  /**
   * Return possible serial numbers given an observation number
   * 
   * @param on  The observation number of the possible serial 
   *            number
   * 
   * @return vector<std::string> The list of possible serial 
   *         numbers matching the input observation number
   */
  std::vector<std::string> SerialNumberList::PossibleSerialNumbers (const std::string &on) {
    std::vector<std::string> numbers;
    for( unsigned index=0; index<p_pairs.size(); index++ ) {
      if( p_pairs[index].observationNumber == on ) {
        numbers.push_back( p_pairs[index].serialNumber );
      }
    }
    if( numbers.size() > 0 ) {
      return numbers;
    }
    else {
      std::string msg = "Requested observation number [" + on + "] ";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }


}
