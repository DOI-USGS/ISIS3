
#ifndef ObservationNumber_h
#define ObservationNumber_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <string>

#include "SerialNumber.h"
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
   * @author  2008-??-?? Christopher Austin
   *
   * @internal
   *
   *  @todo This is only a temporary version. The code needs to be modified
   *  to use a PVL file to determine which keywords to use to create the
   *  Serial Number
   *
   *  @history 2008-01-08 Christpher Austin - Original Version,
   *           a derivative of the previous SerialNumber class
   *
   *  @history 2008-05-09 Steven Lambright - Optimized the
   *           FindObservationTranslation method
   *  @history 2020-03-03 Kristin Berry - Updated to use translation files migrated into source.
   *                                      See #3727
   */
  class ObservationNumber : public Isis::SerialNumber {
    public:
      ObservationNumber();

      virtual ~ObservationNumber();

      static QString Compose(Pvl &label, bool def2filename = false);

      static QString Compose(Cube &cube, bool def2filename = false);

      static QString Compose(const QString &filename, bool def2filename = false);

      std::vector<QString> PossibleSerial(const QString &on, SerialNumberList &list);

    private:

      static PvlGroup FindObservationTranslation(Pvl &label);

  }; // End of Class
}; // End of namespace

#endif
