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

#include "VirtisHK.h"

namespace Isis {


/**
 * Constructs a VIRTIS HK object.
 * 
 * @param hkName Name to be used for the housekeeping data
 * @param tableType The type the data is (double, int)
 * @param one first conversion coefficient
 * @param two second conversion coefficient
 * @param three third conversion coefficient
 */
 VirtisHK::VirtisHK(QString hkName, QString tableType, QString one, QString two, QString three) {
    m_name = hkName; 

    if (QString::compare(tableType.trimmed(), "double") == 0) {
      m_tableType = TableField::Double;
    }
    else if (QString::compare(tableType.trimmed(), "int") == 0) {
      m_tableType = TableField::Integer;
    }

    // Initialize equation 
    one = one.trimmed().remove(0, 1);
    three = three.trimmed().remove(three.size()-2, 1);

    m_coefficients.push_back(three.toDouble());
    m_coefficients.push_back(two.toDouble());
    m_coefficients.push_back(one.toDouble());
  }


 /**
 * Default destructor
 * 
 */
  VirtisHK::~VirtisHK() {}


/**
 * The name of the HK.
 * 
 * @return QString The name of the HK.
 */
  QString VirtisHK::name(){
    return m_name;
  }


/**
 * Returns the TableField Type for this HK.
 * 
 * @return TableField::Type The field type for this HK.
 */
  TableField::Type VirtisHK::tableType(){
    return m_tableType;
  }


/**
 * Creates and returns the appropriate TableField for this HK.
 * 
 * @return TableField The entire appropriate TableField for this HK
 */
  TableField VirtisHK::tableField(){
    return TableField(m_name, m_tableType);
  }


/**
 * Returns the coefficients used to convert the HK to physical units. 
 * 
 * @return std::vector<double> A vector of coefficients used to convert this HK to physical units
 */
  std::vector<double> VirtisHK::coefficients(){
    return m_coefficients;
  }

};
