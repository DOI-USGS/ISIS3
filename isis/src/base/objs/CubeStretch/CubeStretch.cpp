/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "CubeStretch.h"

namespace Isis {

  /**
   * Default constructor
   */
  CubeStretch::CubeStretch() {
    m_name = "DefaultStretch";
    m_type = "Default";
    m_bandNumber = 1;
  }


  /**
   * Constructs a Stretch object with default mapping of special pixel values to
   * themselves and a provided name.
   *  
   * @param name Name to use for CubeStretch 
   */
  CubeStretch::CubeStretch(QString name) : m_name(name) {
    m_type = "Default";
    m_bandNumber = 1;
  }


  /**
   * Constructs a CubeStretch object with default mapping of special pixel values to
   * themselves and a provided name, and a provided stretch type
   *  
   * @param name Name to use for Stretch 
   * @param type Type of stretch 
   */
  CubeStretch::CubeStretch(QString name, QString stretchType, int bandNumber) : m_name(name), m_type(stretchType) {
    m_bandNumber = bandNumber;
  }


  // Default destructor
  CubeStretch::~CubeStretch() {
  }


  /**
   * Constructs a CubeStretch object from a normal Stretch.
   *  
   * @param Stretch Stretch to construct the CubeStretch from.
   */
  CubeStretch::CubeStretch(Stretch const& stretch): Stretch(stretch) {
    m_name = "DefaultName";
    m_bandNumber = 1;
    m_type = "Default";
  }


  /**
   * Constructs a CubeStretch object from a normal Stretch.
   *  
   * @param Stretch Stretch to construct the CubeStretch from.
   */
  CubeStretch::CubeStretch(Stretch const& stretch, QString stretchType): Stretch(stretch), m_type(stretchType) {
    m_name = "DefaultName";
    m_bandNumber = 1;
  }


  bool CubeStretch::operator==(CubeStretch& stretch2) {
    return (getBandNumber() == stretch2.getBandNumber()) && 
           (getName() == stretch2.getName()) &&
           (Text() == stretch2.Text());
  }

  /**
   * Get the Type of Stretch.
   * 
   * @return QString Type of Stretch. 
   */
  QString CubeStretch::getType(){
    return m_type;
  }


  /**
   * Set the type of Stretch.
   * 
   * @param QString Type of Stretch. 
   */
  void CubeStretch::setType(QString stretchType){
    m_type = stretchType;
  }


  /**
   * Set the Stretch name.
   * 
   * @param QString name for stretch
   */
  void CubeStretch::setName(QString name){
    m_name = name;
  }


  /**
   * Get the Stretch name.
   * 
   * @return QString name of stretch
   */
  QString CubeStretch::getName(){
    return m_name;
  }


  /**
   * Get the band number for the stretch.
   * 
   * @return int band number
   */
  int CubeStretch::getBandNumber() {
    return m_bandNumber;
  }


  /**
   * Set the band number for the stretch.
   * 
   * @param int band number
   */
  void CubeStretch::setBandNumber(int bandNumber) {
    m_bandNumber = bandNumber;
  }
} // end namespace isis


