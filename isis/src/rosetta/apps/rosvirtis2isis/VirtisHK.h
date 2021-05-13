#ifndef VirtisHK_h
#define VirtisHK_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <QString>

#include "Table.h"

namespace Isis {
/**
 * Represents a Rosetta Virtis Housekeeping (HK) entry.
 *
 * @author 2017-05-27 Kristin Berry
 *
 * @internal
 * @history 2017-05-27 Kristin Berry - Original Version
 */
  class VirtisHK {
    public:

      VirtisHK(QString hkName, QString tableType, QString one, QString two, QString three);
      ~VirtisHK();

      QString name();
      TableField::Type tableType();
      TableField tableField();
      std::vector<double> coefficients();

    private:
      QString m_name; //!< The name of the housekeeping data, indicates what is stored
      TableField::Type m_tableType; //!< The type of the data (Double, Integer, etc)
      std::vector<double> m_coefficients; //!< The polynomial coefficients for the HK conversion

  };
};
#endif
