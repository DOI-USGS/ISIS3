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

#include "Preference.h"
#include "Filename.h"
#include "iException.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Preference object.
   */
  Preference::Preference() : Isis::Pvl() {
    atexit(Shutdown);
  }

  void Preference::Load(const std::string &file) {
    // Read the input PVL preference file
    Isis::Pvl pvl;

    if(Isis::Filename(file).Exists()) {
      pvl.Read(file);
    }

    // Override parameters each time load is called
    for(int i = 0; i < pvl.Groups(); i++) {
      Isis::PvlGroup &inGroup = pvl.Group(i);
      if(this->HasGroup(inGroup.Name())) {
        Isis::PvlGroup &outGroup = this->FindGroup(inGroup.Name());
        for(int k = 0; k < inGroup.Keywords(); k++) {
          Isis::PvlKeyword &inKey = inGroup[k];
          while(outGroup.HasKeyword(inKey.Name())) {
            outGroup.DeleteKeyword(inKey.Name());
          }
          outGroup += inKey;
        }
      }
      else {
        this->AddGroup(inGroup);
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
      Isis::Filename setup("$HOME/.Isis");
      if(!setup.Exists()) {
        QDir dir;
        QString dirName(iString(setup.Expanded()).ToQt());
        dir.mkdir(dirName);
      }

      // If its a unitTest then load with the unitTest preference file
      if(unitTest) {
        p_preference->Load("$ISISROOT/src/base/objs/Preference/TestPreferences");
      }
      // Otherwise load the Isis system and personal preferences.
      else {
        p_preference->Load("$ISISROOT/IsisPreferences");

        Isis::Filename userPref("$HOME/.Isis/IsisPreferences");
        if(userPref.Exists()) {
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
      p_preference->Clear();
      p_preference->Load("$ISISROOT/src/base/objs/Preference/TestPreferences");
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
