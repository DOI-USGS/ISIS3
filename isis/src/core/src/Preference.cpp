/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/05/28 17:57:03 $
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

#include <fstream>
#include <iostream>
#include <iomanip>

#include <QDir>
#include <QThreadPool>

#include "Preference.h"
#include "FileName.h"
#include "IException.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Preference object.
   */
  Preference::Preference() : Isis::Pvl() {
    atexit(Shutdown);
  }

  void Preference::Load(const QString &file) {
    // Read the input PVL preference file
    Isis::Pvl pvl;

    if(Isis::FileName(file).fileExists()) {
      pvl.read(file);
    }
    else {
      // Throw an exception if the preference file isn't found
      throw IException(IException::User,
                       QString("The preference file %1 was not found or does not exist").arg(file),
                       _FILEINFO_);
    }


    // Override parameters each time load is called
    for(int i = 0; i < pvl.groups(); i++) {
      Isis::PvlGroup &inGroup = pvl.group(i);
      if(this->hasGroup(inGroup.name())) {
        Isis::PvlGroup &outGroup = this->findGroup(inGroup.name());
        for(int k = 0; k < inGroup.keywords(); k++) {
          Isis::PvlKeyword &inKey = inGroup[k];
          while(outGroup.hasKeyword(inKey.name())) {
            outGroup.deleteKeyword(inKey.name());
          }
          outGroup += inKey;
        }
      }
      else {
        this->addGroup(inGroup);
      }
    }

    // Configure Isis to use the user performance preferences where
    //   appropriate.
    if (hasGroup("Performance")) {
      PvlGroup &performance = findGroup("Performance");
      if (performance.hasKeyword("GlobalThreads")) {
        IString threadsPreference = performance["GlobalThreads"][0];

        if (threadsPreference.DownCase() != "optimized") {
          // We need a no-iException conversion here
          int threads = threadsPreference.ToQt().toInt();

          if (threads > 0) {
            QThreadPool::globalInstance()->setMaxThreadCount(threads);
          }
        }
      }
    }
  }


  // Instantiation and initialization of static member variables
  Preference *Preference::p_preference = NULL;
  bool Preference::p_unitTest = false;


  Preference &Preference::Preferences(bool unitTest) {
    if(p_preference == NULL) {
      p_unitTest = unitTest;
      // Create the singleton
      p_preference = new Preference();

      // Make sure the user has a .Isis directory
      Isis::FileName setup("$HOME/.Isis");
      if(!setup.fileExists()) {
        QDir dir;
        QString dirName(IString(setup.expanded()).ToQt());
        dir.mkdir(dirName);
      }

      // If its a unitTest then load with the unitTest preference file
      if(unitTest) {
        p_preference->Load("$ISISROOT/TestPreferences");
      }
      // Otherwise load the Isis system and personal preferences.
      else {
        p_preference->Load("$ISISROOT/IsisPreferences");

        Isis::FileName userPref("$HOME/.Isis/IsisPreferences");
        if(userPref.fileExists()) {
          p_preference->Load("$HOME/.Isis/IsisPreferences");
        }
      }
    }
    // Note: Keep this here
    // During unitTests some other class may call Preferences before the
    // unitTest does. This would cause the preferences to be loaded with
    // the system and user preferences instead of the TestPreferences.
    else if(unitTest) {
      p_unitTest = unitTest;
      p_preference->clear();
      p_preference->Load("$ISISROOT/TestPreferences");
    }

    return *p_preference;
  }

  void Preference::Shutdown() {
    if(p_preference) {
      delete p_preference;
      p_preference = NULL;
    }
  }
} // end namespace isis
