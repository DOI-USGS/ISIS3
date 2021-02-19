#ifndef SerialNumber_h
#define SerialNumber_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include "SerialNumberList.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  class Cube;

  /**
   * @brief Serial Number composer
   *
   * A Serial Number is a unique identification for some object such as an
   * Isis cube. A Serial Number for a specific object must be repeatable. This
   * class is intended to be used to create all Serial Numbers for Isis.
   *
   * @ingroup ControlNetworks
   *
   * @author  2005-07-28 Stuart Sides
   *
   * @internal
   *
   *  @todo This is only a temporary version. The code needs to be modified
   *  to use a PVL file to determine which keywords to use to create the
   *  Serial Number
   *
   *  @history 2005-07-28 Stuart Sides Original version
   *  @history 2005-08-02 Jeff Anderson added new static methods for
   *                      serial number composition from a cube or filename
   *  @history 2006-01-24 Jacob Danton Updated the SerialNumber Compose method
   *                                   to use .trn files specific to the mission
   *  @history 2006-06-15 Kris Becker Added the return of the filename as the
   *                                  fallback default condition.  "Unknown" will
   *                                  still be returned if the input source is a
   *                                  label created in memory and does not have
   *                                  a valid serial number signature and it was
   *                                  not read in from a file.  Add a test for
   *                                  this to the unitTest.
   *  @history 2006-12-08 Stuart Sides Added parameter the the Compose methods
   *                                   to allow or disallow defaulting to the
   *                                   filename. This parameter has a default of
   *                                   false. Which will cause "Unknown" to be
   *                                   returned for files where a SN could not be
   *                                   correctly produced.
   *  @history 2007-07-09 Steven Lambright Changed missions translation filename from Missions.trn to
   *                                   MissionName2DataDir.trn
   *  @history 2007-09-11 Steven Koechle Added three ComposeObservation methods
   *                                   and made code reusable by seperating
   *                                   existing code into two private methods.
   *  @history 2007-09-13 Steven Koechle Fixed boolean paramaters passed into
   *           FindSerialTranslation
   *
   *  @history 2008-01-10 Christpher Austin Removed the use of the system default
   *           file in FindSerialTranslation() and detached ObservationNumber
   *           functionality into its own class
   *  @history 2008-05-09 Steven Lambright Optimized the FindSerialTranslation
   *           method
   *  @history 2008-05-18 Steven Lambright Fixed documentation
   *  @history 2020-03-03 Kristin Berry - Updated to use translation files moved into source.
   *                                      References #3727.
   */
  class SerialNumber {
    public:
      SerialNumber();

      virtual ~SerialNumber();

      static QString Compose(Pvl &label, bool def2filename = false);

      static QString Compose(Cube &cube, bool def2filename = false);

      static QString Compose(const QString &filename, bool def2filename = false);

      static QString ComposeObservation(const QString &sn, SerialNumberList &list, bool def2filename = false);

    protected:

      static QString CreateSerialNumber(PvlGroup &snGroup, int key);

    private:

      static PvlGroup FindSerialTranslation(Pvl &label);

  }; // End of Class
}; // End of namespace

#endif
