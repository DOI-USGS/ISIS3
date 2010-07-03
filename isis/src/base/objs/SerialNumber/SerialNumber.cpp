#include <map>

#include "SerialNumber.h"
#include "ObservationNumber.h"
#include "SerialNumberList.h"
#include "iException.h"
#include "Pvl.h"
#include "Cube.h"
#include "Process.h"
#include "PvlTranslationManager.h"
#include "Filename.h"

namespace Isis {
  /**
  * Create an empty SerialNumber object.
  */
  SerialNumber::SerialNumber () {}

  /**
   * Destroy a SerialNumber object.
  */
  SerialNumber::~SerialNumber () {}

  /**
   * Compose a SerialNumber from a PVL.
   * 
   * @param label A pvl formatted label to be used to generate the serial number
   * @param def2filename If a serial number could not be found, try to return the filename
   *
   * @return Calculated SerialNumber or Filename
  */
  std::string SerialNumber::Compose (Pvl &label, bool def2filename) {

    std::string sn;
    try {
      PvlGroup snGroup = FindSerialTranslation (label);
      sn = CreateSerialNumber (snGroup,(int)snGroup["ObservationKeys"]);
    }
    catch (iException &e) {
      e.Clear();
      if (def2filename) {
        //  Try to return the filename if it exists in the label, otherwise use
        //  "Unknown" as a last resort.
        std::string snTemp = label.Filename();
        if (!snTemp.empty()) {
          sn = Filename(snTemp).Name();
        }
        else {
          sn = "Unknown";
        }
      }
      else {
        sn = "Unknown";
      }
    }

    return sn;
  }

  /**
   * Compose a SerialNumber from a Cube.
   * 
   * @param cube An opened Isis cub 
   * @param def2filename If a serial number could not be found, try to return the filename
   *
   * @return Calculated SerialNumber or Filename
  */
  std::string SerialNumber::Compose (Cube &cube, bool def2filename) {
    return Compose(*cube.Label(), def2filename);
  }

  /**
   * Compose a SerialNumber from a file.
   * 
   * @param filename a filename to open 
   * @param def2filename If a serial number could not be found, try to return the 
   *                     filename
   *
   * @return Calculated SerialNumber or Filename
  */
  std::string SerialNumber::Compose (const std::string &filename, bool def2filename) {
    Pvl p(filename);
    return Compose(p, def2filename);
  }

  /**
   * Get Groups by translating from correct Translation table
   * 
   * @param label A pvl formatted label to be used to generate the serial number 
   *  
   */
  PvlGroup SerialNumber::FindSerialTranslation (Pvl &label) {
    Pvl outLabel;
    static PvlGroup dataDir (Preference::Preferences().FindGroup("DataDirectory"));

    // Get the mission name
    static std::string missionTransFile = (std::string) dataDir["base"] + "/translations/MissionName2DataDir.trn";
    static PvlTranslationManager missionXlater (missionTransFile);
    missionXlater.SetLabel(label);
    std::string mission = missionXlater.Translate ("MissionName");

    // Get the instrument name
    static std::string instTransFile = (std::string) dataDir["base"] + "/translations/Instruments.trn";
    static PvlTranslationManager instrumentXlater (instTransFile);
    instrumentXlater.SetLabel(label);
    std::string instrument = instrumentXlater.Translate ("InstrumentName");

    // We want to use this instrument's translation manager. It's much faster for
    //   SerialNumberList if we keep the translation manager in memory, so re-reading
    //   from the disk is not necessary every time. To do this, we'll use a map to store
    //   the translation managers with a string identifier to find them. This identifier
    //   needs to have the mission name and the instrument name.

    //  Create the static map to keep the translation managers in memory
    static std::map<std::string, PvlTranslationManager> missionTranslators;

    // Determine the key for this translation manager - must have both mission and instrument
    std::string key = mission + "_" + instrument;

    // Try to find an existing translation manager with the key
    std::map<std::string, PvlTranslationManager>::iterator translationIterator = missionTranslators.find(key);

    // If we don't succeed, create one
    if(translationIterator == missionTranslators.end()) {
      // Get the file
      Filename snFile = (std::string) dataDir[mission] + "/translations/" + instrument + "SerialNumber????.trn";
      snFile.HighestVersion();
  
      // use the translation file to generate keywords
      missionTranslators.insert(
        std::pair<std::string, PvlTranslationManager>(key, PvlTranslationManager(snFile.Expanded()))
        );

      translationIterator = missionTranslators.find(key);
    }

    translationIterator->second.SetLabel(label);
    translationIterator->second.Auto(outLabel);

    PvlGroup snGroup = outLabel.FindGroup("SerialNumberKeywords");
    snGroup += PvlKeyword("ObservationKeys", snGroup.Keywords());

    return snGroup;
  }

  /**
   * Create the SerialNumber string by concatenating the keywords in the label 
   * with '/' in between serialNumber groups and the number of observationKeys 
   *  
   * @param snGroup A PvlGroup containing the keywords to concatenate 
   * @param keys the number of strings to contatenate 
   */
  std::string SerialNumber::CreateSerialNumber (PvlGroup &snGroup, int keys ) {
    std::string sn = snGroup["Keyword1"][0];
    for (int i=2; i<= keys; i++) {
      iString keyword = "Keyword"+ (iString)i;
      sn += "/" + snGroup[keyword][0];
    }
    return sn;
  }

  /**
   * Creates the ObservationNumber from a string representing the 
   * SerialNumber and a SerialList. 
   *  
   * @param sn the string representing the SerialNumber 
   *  
   * @param list the SerialNumberList 
   * @param def2filename If a serial number could not be found, try to return the 
   *                     filename
   *
   * @return Calculated SerialNumber or Filename
  */
  std::string SerialNumber::ComposeObservation (const std::string &sn, SerialNumberList &list, bool def2filename) {
    std::string filename = list.Filename(sn);
    return ObservationNumber::Compose(filename,def2filename);
  }
}
