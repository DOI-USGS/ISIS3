#ifndef PvlToken_h
#define PvlToken_h
/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:09 $
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

#include <vector>
#include <cctype>

#include <QString>

namespace Isis {
  /**
   * @brief Container for Keyword-value pair
   *
   * This class is used for internalizing keyword-value(s) pairs. For example,
   * SPACECRAFT=MARS_GLOBAL_SURVEYOR or FROM=file.cub. This is useful when parsing
   * ASCII files such as PDS labels or command lines.
   *
   * @ingroup Parsing
   *
   * @author 2002-03-18 Jeff Anderson
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2005-02-14 Elizabeth Ribelin - Modified file to support Doxygen
   *  @history 2013-03-11 Steven Lambright and Mathew Eis - Brought method names and member variable
   *                          names up to the current Isis 3 coding standards. Fixes #1533.
   *                                          documentation
   *
   *  @todo 2005-02-14 Jeff Anderson - add coded and implemetation example to
   *                                   class documentation
   */

  class PvlToken {
    private:
      QString m_key;                 //!< Storage for the keyword name
      std::vector<QString> m_value;   /**<Vector storage for a list of values.
                                            See the standard template library
                                            for more information on vectors.*/

    public:
      PvlToken(const QString &k);
      PvlToken();
      ~PvlToken();

      void setKey(const QString &k);
      QString key() const;
      QString keyUpper() const;

      void addValue(const QString &v);
      QString value(const int index = 0) const;
      QString valueUpper(const int index = 0) const;
      int valueSize() const;
      void valueClear();

      inline const std::vector<QString> &valueVector() const {
        return m_value;
      };
  };
};

#endif
