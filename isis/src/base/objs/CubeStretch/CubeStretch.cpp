/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CubeStretch.h"

namespace Isis {

  /**
   * Constructs a CubeStretch object with default mapping of special pixel values to
   * themselves and a provided name, and a provided stretch type
   *  
   * @param name Name to use for Stretch 
   * @param type Type of stretch 
   */
  CubeStretch::CubeStretch(QString name, QString stretchType, int bandNumber) : m_name(name), 
    m_type(stretchType), m_bandNumber(bandNumber) {
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
    m_name = "DefaultStretch";
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


  /**
   * Check if the CubeStretches are equal
   * 
   * @param stretch2 The stretch to compare with
   * 
   * @return bool True if stretches are equal. Else, false.
   */
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


