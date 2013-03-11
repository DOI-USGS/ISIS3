/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2007/01/30 22:12:23 $
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
#include "PvlToken.h"
#include "IException.h"
#include "IString.h"
#include "Message.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a Token with k for keyword and NULL for the value list.
   *
   * @param k Value of the keyword
   */
  PvlToken::PvlToken(const QString &k) {
    setKey(k);
    valueClear();
  }

  //! Constructs a Token with NULL for both the keyword and value list.
  PvlToken::PvlToken() {
    valueClear();
  }

  //! Destroys the Token object
  PvlToken::~PvlToken() {
    m_key.clear();
    valueClear();
  }

  /**
   * Set the token keyword.
   *
   * @param k IString to load into the token keyword
   */
  void PvlToken::setKey(const QString &k) {
    m_key = k;
  };

  /**
   * Returns the token keyword
   *
   * @return QString
   */
  QString PvlToken::key() const {
    return m_key;
  };

  /**
   * Returns the token keyword in all uppercase characters
   *
   * @return QString
   */
  QString PvlToken::keyUpper() const {
    return m_key.toUpper();
  }

  //! Removes all elements from the value-vector
  void PvlToken::valueClear() {
    m_value.clear();
  }

  /**
   * Returns the number of elements in the value-vector
   *
   * @return int
   */
  int PvlToken::valueSize() const {
    return m_value.size();
  }

  /**
   * Adds a value to the value-vector. Successive calls add values to the end of
   * the vector.
   *
   * @param v IString add to the value-vector list
   */
  void PvlToken::addValue(const QString &v) {
    m_value.push_back(v);
  }

  /**
   * Returns one element of the value-vector.
   *
   * @param index Zero-based index of vector element to return. Defaults to 0
   *
   * @return QString
   *
   * @throws Isis::IException::Programmer
   */
  QString PvlToken::value(const int index) const {
    if((index < 0) || (index >= (int) m_value.size())) {
      QString message = Isis::Message::ArraySubscriptNotInRange(index);
      throw IException(IException::Programmer, message, _FILEINFO_);
    }
    return m_value[index];
  }

  /**
   * Returns one element of the value-vector in uppercase.
   *
   * @param index Zero-based index of vector element to return. Defaults to 0
   *
   * @return QString
   *
   * @throws Isis::IException::Programmer
   */
  QString PvlToken::valueUpper(int index) const {
    if((index < 0) || (index >= (int) m_value.size())) {
      QString message = Isis::Message::ArraySubscriptNotInRange(index);
      throw IException(IException::Programmer, message, _FILEINFO_);
    }

    return m_value[index].toUpper();
  }
} // end namespace isis
