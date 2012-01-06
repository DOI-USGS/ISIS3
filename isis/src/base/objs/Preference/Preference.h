/**
 * @file
 * $Revision: 1.4 $
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

#ifndef Preference_h
#define Preference_h

#include <string>
#include "Pvl.h"

namespace Isis {
  class Application;

  /**
   * @brief Reads user preferences from a data file.
   *
   * This class is used to load, set, and obtain user preferences for
   * the operation of Isis. User preferences for Isis are items such
   * as the name of the log file, default data directories, and processing
   * status. This is essentially a specialized version of a Label object. It
   * reads a system-wide preference file and overwrites values in
   * the label with a user preference file. This object does not need
   * to be modified to add new prefences ... simply edit the system
   * wide preference file, add the new group and keywords, update the
   * preference document (user documentation) explaining the new
   * keywords.
   *
   * @ingroup ApplicationInterface
   *
   * @author 2002-03-13 Jeff Anderson
   *
   * @internal
   *   @todo The Preference class needs private methods & variables documented.
   *   The documentation shows methods that don't exist in the code. It also
   *   needs an example.
   *   @history 2003-04-23 Jeff Anderson - reworked the class to use the Label
   *                                       object.
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeoloy...
   *                                      isis.astrogeology
   *   @history 2003-12-03 Stuart Sides - added HasGroup member.
   *   @history 2005-10-03 Elizabeth Miller - changed @ingroup tag
   *   @history 2008-01-09 Steven Lambright - Made Application a friend
   *                          for destruction purposes
   *   @history 2008-07-08 Steven Lambright - Now uses atexit for destruction
   *   @history 2010-05-28 Steven Lambright - More resilient to $HOME/.Isis
   *                          problems
   *   @history 2010-07-19 Jeannie Walldren - Added FileCustomization group to
   *                          TestPreferences file
   */
  class Preference : public Pvl {

    public:
      void Load(const std::string &file);

      /**
       * Tests whether or not a file is a unitTest
       *
       * @return bool True if it is a unitTest, and false if it is not
       */
      inline bool IsUnitTest() {
        return p_unitTest;
      }

      static Preference &Preferences(bool unitTest = false);

    private:
      Preference();

      //! Destroys the Preference object
      ~Preference() {};

      Preference(const Preference &p);
      Preference &operator=(const Preference &p);

      static Preference *p_preference;   //!< Pointer to a Preference object
      static bool p_unitTest;            /**< Flag indicating whether the file
                                              is a unitTest or not.*/

      static void Shutdown();
  };
};

#endif
