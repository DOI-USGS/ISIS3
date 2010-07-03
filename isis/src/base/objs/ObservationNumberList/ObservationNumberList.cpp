#include "ObservationNumberList.h"
#include "iException.h"
#include "FileList.h"
#include "Filename.h"
#include "SerialNumberList.h"
#include "iString.h"
#include "Pvl.h"

namespace Isis {
  /**
   * Creates an ObservationNumberList from a filename 
   *  
   * @param listfile The list of files to be given observation 
   *                 numbers
   * @param checkTarget Boolean value that specifies whether or not to check
   *                    to make sure the target names match between files added
   *                    to the observationnumber list
   */
  ObservationNumberList::ObservationNumberList (const std::string &listfile, bool checkTarget) :
                SerialNumberList( listfile, checkTarget ) {
    init ( this );
  }

  /**
   * Creates an ObservationNumberList from a SerialNumberList
   * 
   * @param snlist The serial number list from which to generate an observation number list
   */
  ObservationNumberList::ObservationNumberList (Isis::SerialNumberList *snlist) :
                SerialNumberList(*snlist) {
    init ( snlist );
  }

  /**
   * Initiates the ObservationNumberList 
   *  
   * @param snlist The already created SerialNumberList used to 
   *               create the ObservationNumberList object
   */
  void ObservationNumberList::init ( Isis::SerialNumberList *snlist ) {

    if (snlist->Size() == 0) {
      std::string msg = "Serial number list is empty";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    
    std::map<std::string,int> observationMap;
    std::string observationNumber;
    int currentIndex = 0;
    int observationIndex;

    // Fill the temporary map to generate observation sets
    for (int isn=0; isn < snlist->Size(); isn++) {
      observationNumber = snlist->ObservationNumber(isn);

      if (observationMap.find(observationNumber) == observationMap.end()) {
         observationMap.insert(std::pair<std::string,int>(observationNumber, currentIndex));
         observationIndex = currentIndex++;
       }
       else {
         observationIndex = observationMap.find(observationNumber)->second;
       }

       Add (isn, observationIndex, observationNumber);
    }
    p_numberObservations = currentIndex;
  }

  /**
   * Destructor
   */
  ObservationNumberList::~ObservationNumberList () {
  }

  /** 
   * Removes all of the listed serial numbers from the observation
   * 
   * @param snlist The list of SerialNumbers to remove
   */
  void ObservationNumberList::Remove( Isis::SerialNumberList *snlist ) {

    if (snlist->Size() == 0) {
       std::string msg = "Removing serial number list is empty";
       throw iException::Message(iException::User,msg,_FILEINFO_);
     }

    p_sets.clear();
    p_indexMap.clear();

    std::map<std::string,int> observationMap;
    std::string observationNumber;
    int currentIndex = 0;
    int observationIndex;

    // Fill the temporary map to generate observation sets
    for (int isn=0; isn < this->Size(); isn++) {
      if ((snlist->HasSerialNumber(this->SerialNumber(isn)))) continue;
      observationNumber = this->ObservationNumber(isn);

      if (observationMap.find(observationNumber) == observationMap.end()) {
         observationMap.insert(std::pair<std::string,int>(observationNumber, currentIndex));
         observationIndex = currentIndex++;
       }
       else {
         observationIndex = observationMap.find(observationNumber)->second;
       }

       Add (isn, observationIndex, observationNumber);
    }
    p_numberObservations = currentIndex;
  }


  /** 
   * Removes all of the listed serial numbers from the observation
   * 
   * @param listfile The list of SerialNumbers to remove
   */
  void ObservationNumberList::Remove (const std::string &listfile) {
    Isis::SerialNumberList snlist( listfile );
    Remove( &snlist );
  }
  

  /**
   * Adds a new serial number index / observation number index / observation
   * number to the SerialNumberList
   * 
   * @param isn The serial number index of the observation set to be added
   * @param observationIndex The observation number index of the observation set
   *                         to be added
   * @param observationNumber The observation number of the observation set to
   *                         be added
   * 
   */
  void ObservationNumberList::Add (const int isn, const int observationIndex, 
                              std::string observationNumber) {
    
      ObservationSet nextset;
      nextset.serialNumberIndex = isn;
      nextset.observationNumberIndex = observationIndex;
      nextset.observationNumber = observationNumber;

      p_sets.push_back(nextset);
      p_indexMap.insert(std::pair<int,int>(isn,observationIndex));
  }

  /**
   * How many unique observations are in the list?
   *
   * @return int Returns number of unique observations currently in the list
   */
  int ObservationNumberList::ObservationSize () const {
    return p_numberObservations;
  }


  /**
   * Determines whether or not the requested observation number 
   * exists in the list 
   * 
   * @param on The observation number to be checked for
   * 
   * @return bool
   */
  bool ObservationNumberList::HasObservationNumber (const std::string &on) {
    for ( unsigned index=0; index<p_pairs.size(); index++ ) {
      if( p_pairs[index].observationNumber == on ) {
        return true;
      }
    }
    return false;
  }

  /**
   * Return a observation index given a serial number index
   * 
   * @param serialNumberIndex The index of the serial number to map
   * 
   * @return int The observation index mapped to the serial number
   */
  int ObservationNumberList::ObservationNumberMapIndex (int serialNumberIndex){
    //    if (serialNumberIndex >= 0 && serialNumberIndex < (int) p_indexMap.size()) {
    if (serialNumberIndex >= 0 ) {
      return p_indexMap.find(serialNumberIndex)->second;
    }
    else {
      iString num = iString(serialNumberIndex);
      std::string msg = "Serial Number Index [" + (std::string) num + "] is invalid";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }


  /**
   * return an observation number given a filename
   * 
   * @param filename The filename to be matched
   * 
   * @return std::string The observation number corresponding to 
   *         the input filename
   */
  std::string ObservationNumberList::ObservationNumber(const std::string &filename){
    if (p_fileMap.find(Isis::Filename(filename).Expanded()) == p_fileMap.end()) {
      std::string msg = "Requested filename [" +
                          Isis::Filename(filename).Expanded() + "]";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    int index = FilenameIndex(filename);
    return p_pairs[index].observationNumber;
  }

  /**
   * Return a observation number given an index
   * 
   * @param index The index of the desired observation number
   * 
   * @return std::string The observation number returned
   */
  std::string ObservationNumberList::ObservationNumber (int index) {
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
   * Return possible filenames given an observation number
   * 
   * @param on  The observation number of the desired filename
   * 
   * @return vector<std::string> The list of possible filenames 
   *         matching the input observation number
   */
  std::vector<std::string> ObservationNumberList::PossibleFilenames (const std::string &on) {
    std::vector<std::string> filenames;
    for( unsigned index=0; index<p_pairs.size(); index++ ) {
      if( p_pairs[index].observationNumber == on ) {
        filenames.push_back( p_pairs[index].filename );
      }
    }
    if( filenames.size() > 0 ) {
      return filenames;
    }
    else {
      std::string msg = "Requested observation number [" + on + "] ";
      msg += "does not exist in the list";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
  }

}
