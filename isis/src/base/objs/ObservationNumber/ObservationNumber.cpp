#include "ObservationNumber.h"
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
  ObservationNumber::ObservationNumber () {}

  /**
   * Destroy a SerialNumber object.
  */
  ObservationNumber::~ObservationNumber () {}

  /**
   * Compose a ObservationNumber from a PVL.
   * 
   * @param label A pvl formatted label to be used to generate the serial number
  */
  std::string ObservationNumber::Compose (Pvl &label, bool def2filename) {
    std::string sn;
    try {
      PvlGroup snGroup = FindObservationTranslation (label);
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
   * Compose a SerialNumber fPvlGroup snGrouprom a Cube.
   * 
   * @param cube An opened Isis cube
   */
  std::string ObservationNumber::Compose (Cube &cube, bool def2filename) {
    return Compose(*cube.Label(), def2filename);
  }

  /**
   * Compose a SerialNumber from a file.
   * 
   * @param filename a filename to open
   */
  std::string ObservationNumber::Compose (const std::string &filename, bool def2filename) {
    Pvl p(filename);
    return Compose(p, def2filename);
  }

  /**
   * Get Groups by translating from correct Translation table
   * 
   * @param label A pvl formatted label to be used to generate the serial number
   */
  PvlGroup ObservationNumber::FindObservationTranslation (Pvl &label) {
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
    //   ObservationNumberList if we keep the translation manager in memory, so re-reading
    //   from the disk is not necessary every time. To do this, we'll use a map to store
    //   the translation managers and observation number keys with a string identifier to find them. 
    //   This identifier needs to have the mission name and the instrument name.
    static std::map<std::string, std::pair<PvlTranslationManager, PvlKeyword> > missionTranslators;
    std::string key = mission + "_" + instrument;
    std::map<std::string, std::pair<PvlTranslationManager, PvlKeyword> >::iterator 
      translationIterator = missionTranslators.find(key);

    if(translationIterator == missionTranslators.end()) {
      // Get the file
      Filename snFile = (std::string) dataDir[mission] + "/translations/" + instrument + "SerialNumber????.trn";
      snFile.HighestVersion();

      // Delets the extra 
      Pvl translation(snFile.Expanded());
      PvlKeyword observationKeys;
      if (translation.HasKeyword("ObservationKeys")) {
        observationKeys = translation["ObservationKeys"];
      }

      // use the translation file to generate keywords
      missionTranslators.insert(
        std::pair<std::string, std::pair<PvlTranslationManager, PvlKeyword> >
        (key, std::pair<PvlTranslationManager, PvlKeyword>(PvlTranslationManager(snFile.Expanded()), observationKeys))
        );

      translationIterator = missionTranslators.find(key);
    }

    translationIterator->second.first.SetLabel(label);
    translationIterator->second.first.Auto(outLabel);
    PvlGroup snGroup = outLabel.FindGroup("SerialNumberKeywords");

    // Delets the extra 
    if (!translationIterator->second.second.Name().empty()) {
      snGroup += translationIterator->second.second;
    }
    else {
      snGroup += PvlKeyword("ObservationKeys", snGroup.Keywords());
    }

    return snGroup;
  }

  /**
   * Creates a vector of plasible SerialNumbers from a string 
   * representing the ObservationNumber and a SerialList. 
   *  
   * @param on the string representing the ObservationNumber 
   *  
   * @param list the SerialNumberList 
   */
  std::vector<std::string> ObservationNumber::PossibleSerial (const std::string &on, SerialNumberList &list ) {
    std::vector<std::string> sn;
    for( int i=0; i<list.Size(); i++ ) {
      if( list.SerialNumber(i).find(on) == 0 ) {
        sn.push_back( list.SerialNumber(i) );
      }
    }
    return sn;
  }



}
