#ifndef VirtisHK_h
#define VirtisHK_h
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
