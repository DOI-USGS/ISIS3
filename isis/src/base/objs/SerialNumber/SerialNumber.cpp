/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <map>

#include "SerialNumber.h"
#include "ObservationNumber.h"
#include "SerialNumberList.h"
#include "IException.h"
#include "Pvl.h"
#include "Cube.h"
#include "Process.h"
#include "PvlToPvlTranslationManager.h"
#include "FileName.h"

namespace Isis {
  /**
  * Create an empty SerialNumber object.
  */
  SerialNumber::SerialNumber() {}

  /**
   * Destroy a SerialNumber object.
  */
  SerialNumber::~SerialNumber() {}

  /**
   * Compose a SerialNumber from a PVL.
   *
   * @param label A pvl formatted label to be used to generate the serial number
   * @param def2filename If a serial number could not be found, try to return the filename
   *
   * @return Calculated SerialNumber or FileName
  */
  QString SerialNumber::Compose(Pvl &label, bool def2filename) {

    QString sn;
    try {
      PvlGroup snGroup = FindSerialTranslation(label);
      sn = CreateSerialNumber(snGroup, (int)snGroup["ObservationKeys"]);
    }
    catch(IException &) {
      if(def2filename) {
        //  Try to return the filename if it exists in the label, otherwise use
        //  "Unknown" as a last resort.
        QString snTemp = label.fileName();
        if(!snTemp.isEmpty()) {
          sn = FileName(snTemp).name();
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
   * @return Calculated SerialNumber or FileName
  */
  QString SerialNumber::Compose(Cube &cube, bool def2filename) {
    return Compose(*cube.label(), def2filename);
  }

  /**
   * Compose a SerialNumber from a file.
   *
   * @param filename a filename to open
   * @param def2filename If a serial number could not be found, try to return the
   *                     filename
   *
   * @return Calculated SerialNumber or FileName
  */
  QString SerialNumber::Compose(const QString &filename, bool def2filename) {
    Pvl p(filename);
    return Compose(p, def2filename);
  }

  /**
   * Get Groups by translating from correct Translation table
   *
   * @param label A pvl formatted label to be used to generate the serial number
   *
   */
  PvlGroup SerialNumber::FindSerialTranslation(Pvl &label) {
    Pvl outLabel;

    // Determine the mission and instrument for the instrument-based serial number,
    // if the cube has an ISIS instrument label. These translations return a default
    // ("Unknown") for an unrecognized instrument, but can throw if the expected
    // groups/keywords are missing, so they are guarded.
    QString mission;
    QString instrument;
    if(label.findObject("IsisCube").hasGroup("Instrument")) {
      try {
        static QString missionTransFile = "$ISISROOT/appdata/translations/MissionName2DataDir.trn";
        static PvlToPvlTranslationManager missionXlater(missionTransFile);
        missionXlater.SetLabel(label);
        mission = missionXlater.Translate("MissionName");

        static QString instTransFile = "$ISISROOT/appdata/translations/Instruments.trn";
        static PvlToPvlTranslationManager instrumentXlater(instTransFile);
        instrumentXlater.SetLabel(label);
        instrument = instrumentXlater.Translate("InstrumentName");
      }
      catch (IException &e) {
        // The instrument label could not be translated. Fall back to the CSM
        // serial number below only if the cube has a CsmInfo group. Otherwise
        // there is no other source for a serial number, so re-throw the
        // original error rather than continuing with an empty mission and
        // instrument, which would fail later with a confusing message about a
        // missing translation file.
        if (!label.findObject("IsisCube").hasGroup("CsmInfo")) {
          throw IException(e, IException::Unknown,
                           "Unable to find a serial number translation for this cube.",
                           _FILEINFO_);
        }
        mission = "";
        instrument = "";
      }
    }

    // A usable instrument-based serial number requires a recognized mission and
    // instrument.
    bool instrumentUsable = !mission.isEmpty() && !instrument.isEmpty()
                            && instrument != "Unknown";

    // Prefer the instrument-based serial number when the cube has a recognized ISIS
    // instrument: it matches what spiceinit produces for the same cube, and unlike
    // the CSM serial number (built only from CSMPlatformID, CSMInstrumentId, and
    // ReferenceTime, which can be identical for multiple framelets of a framing
    // instrument) it is unique per image. Use the CSM serial number only when there
    // is no usable instrument serial (for example a generic CSM cube with no ISIS
    // instrument label).
    if(label.findObject("IsisCube").hasGroup("CsmInfo") && !instrumentUsable) {
      static QString csmTransFile = "$ISISROOT/appdata/translations/CsmSerialNumber.trn";
      PvlToPvlTranslationManager csmTranslator(label, csmTransFile);
      csmTranslator.Auto(outLabel);
    }
    else {
      // We want to use this instrument's translation manager. It's much faster for
      //   SerialNumberList if we keep the translation manager in memory, so re-reading
      //   from the disk is not necessary every time. To do this, we'll use a map to store
      //   the translation managers with a string identifier to find them. This identifier
      //   needs to have the mission name and the instrument name.

      //  Create the static map to keep the translation managers in memory
      static std::map<QString, PvlToPvlTranslationManager> missionTranslators;

      // Determine the key for this translation manager - must have both mission and instrument
      QString key = mission + "_" + instrument;

      // Try to find an existing translation manager with the key
      std::map<QString, PvlToPvlTranslationManager>::iterator translationIterator = missionTranslators.find(key);

      // If we don't succeed, create one
      if(translationIterator == missionTranslators.end()) {
        // Get the file

        FileName snFile((QString) "$ISISROOT/appdata/translations/" + mission + instrument + "SerialNumber.trn");

        // use the translation file to generate keywords
        missionTranslators.insert(
          std::pair<QString, PvlToPvlTranslationManager>(key, PvlToPvlTranslationManager(snFile.expanded()))
        );

        translationIterator = missionTranslators.find(key);
      }
      translationIterator->second.SetLabel(label);
      translationIterator->second.Auto(outLabel);
    }

    PvlGroup snGroup = outLabel.findGroup("SerialNumberKeywords");
    snGroup += PvlKeyword("ObservationKeys", toString(snGroup.keywords()));

    return snGroup;
  }

  /**
   * Create the SerialNumber string by concatenating the keywords in the label
   * with '/' in between serialNumber groups and the number of observationKeys
   *
   * @param snGroup A PvlGroup containing the keywords to concatenate
   * @param keys the number of strings to contatenate
   */
  QString SerialNumber::CreateSerialNumber(PvlGroup &snGroup, int keys) {
    QString sn = snGroup["Keyword1"][0];
    for(int i = 2; i <= keys; i++) {
      QString keyword = QString("Keyword%1").arg(i);
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
   * @return Calculated SerialNumber or FileName
  */
  QString SerialNumber::ComposeObservation(const QString &sn, SerialNumberList &list, bool def2filename) {
    QString filename = list.fileName(sn);
    return ObservationNumber::Compose(filename, def2filename);
  }
}
