/**
 *   Do we still need this??
 */

#include "CubeStretch.h"

namespace Isis {

  /**
   * Default constructor
   */
  CubeStretch::CubeStretch() {
    m_name = "DefaultStretch";
    m_type = "DefaultLinear";
    m_bandNumber = 1;
  }


  /**
   * Constructs a Stretch object with default mapping of special pixel values to
   * themselves and a provided name.
   *  
   * @param name Name to use for Stretch 
   */
  CubeStretch::CubeStretch(QString name) : m_name(name) {
    m_type = "DefaultLinear";
    m_bandNumber = 1;
  }


  /**
   * Constructs a Stretch object with default mapping of special pixel values to
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


  // semi-copy constructor
  CubeStretch::CubeStretch(Stretch const& stretch): Stretch(stretch) {
    m_name = "Unknown";
    m_bandNumber = 1;
    m_type = "Default";
  }



  // semi-copy constructor
  CubeStretch::CubeStretch(Stretch const& stretch, QString stretchType): Stretch(stretch), m_type(stretchType) {
    m_name = "Unknown";
    m_bandNumber = 1;
  }

  /**
   * Get the Type of Stretch. This is only used by the AdvancedStretchTool.
   * 
   * @return QString Type of Stretch. 
   */
  QString CubeStretch::getType(){
    return m_type;
  }


  void CubeStretch::setType(QString stretchType){
    m_type = stretchType;
  }

  void CubeStretch::setName(QString name){
    m_name = name;
  }

  QString CubeStretch::getName(){
    return m_name;
  }

  int CubeStretch::getBandNumber() {
    return m_bandNumber;
  }

  void CubeStretch::setBandNumber(int bandNumber) {
    m_bandNumber = bandNumber;
  }
} // end namespace isis


