/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2010/05/14 20:24:29 $
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

#ifndef HexConverter_h
#define HexConverter_h

class QByteArray;

namespace Isis {
  class iString;

  /**
   * @brief Convert to and from ASCII Hex Notation
   *
   * @ingroup Utility
   *
   * @author
   *
   * @internal
   */
  class HexConverter {
    public:
      static iString ToString(iString hexCode);
      static QByteArray ToBinary(iString hexCode);

      static iString ToHex(iString str);
      static iString ToHex(QByteArray binary);

    private:
      //! Not implemented, this class can only be used statically
      HexConverter();

      static bool IsHex(char hex);
      static char HexToChar(char hex);
      static iString CharToHex(char binary);
      static char NibbleToHex(char binary);
  };
};

#endif
