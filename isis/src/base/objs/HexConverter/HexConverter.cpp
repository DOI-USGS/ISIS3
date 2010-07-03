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
#include "HexConverter.h"

#include <iostream>

#include <QByteArray>

#include "iString.h"
#include "iException.h"

using namespace std;

namespace Isis {
  /**
   * This converts HEX to an iString. Not a good choice if you
   * expect binary data to come out of the hex code.
   *
   * @param hexCode
   *
   * @return iString
   */
  iString HexConverter::ToString(iString hexCode) {
    QByteArray binaryVersion;
    iString result;

    binaryVersion = ToBinary(hexCode);

    for(int i = 0; i < binaryVersion.size(); i++) {
      result += binaryVersion[i];
    }

    return result;
  }


  /**
   * This converts HEX to an array of bytes.
   *
   * @param hexCode
   *
   * @return QByteArray
   */
  QByteArray HexConverter::ToBinary(iString hexCode) {
    QByteArray result;

    unsigned int pos = 0;
    bool byteStarted = false;

    while(pos < hexCode.length()) {
      if(!IsHex(hexCode[pos])) {
        pos ++;
        continue;
      }

      if(!byteStarted) {
        result.push_back(HexToChar(hexCode[pos]) << 4);
        byteStarted = true;
      }
      else {
        result.data()[result.size() - 1] += HexToChar(hexCode[pos]);
        byteStarted = false;
      }

      pos ++;
    }

    if(byteStarted) {
      iString msg = "An even number of hex codes are required to decode into ";
      msg += "bytes";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    return result;
  }


  /**
   * This converts an iString to Hex
   *
   * @param str
   *
   * @return QByteArray
   */
  iString HexConverter::ToHex(iString str) {
    QByteArray stringArray;

    for(unsigned int strPos = 0; strPos < str.length(); strPos++) {
      stringArray.push_back(str[strPos]);
    }

    return ToHex(stringArray);
  }


  /**
   * This converts an array of bytes to Hex
   *
   * @param binary
   *
   * @return QByteArray
   */
  iString HexConverter::ToHex(QByteArray binary) {
    iString result;

    for(int bytePos = 0; bytePos < binary.size(); bytePos++) {
      char thisByte = binary[bytePos];
      result += CharToHex(thisByte);
    }

    return result;
  }


  /**
   * Returns true if the character is 0-9, a-f, A-F
   *
   * @author slambright (5/14/2010)
   *
   * @param hex
   *
   * @return bool
   */
  bool HexConverter::IsHex(char hex) {
    if(hex >= '0' && hex <= '9') return true;
    if(hex >= 'a' && hex <= 'f') return true;
    if(hex >= 'A' && hex <= 'F') return true;

    return false;
  }


  /**
   * Converts hex to binary (always in the first 4 bits)
   *
   * @param hex
   *
   * @return char
   */
  char HexConverter::HexToChar(char hex) {
    if(hex >= '0' && hex <= '9') {
      return (char)(hex - '0');
    }

    if(hex >= 'a' && hex <= 'f') {
      return (char)(10 + hex - 'a');
    }

    if(hex >= 'A' && hex <= 'F') {
      return (char)(10 + hex - 'A');
    }

    iString error = "Character [";
    error += hex;
    error += "] does not appear to be a hex digit";
    throw iException::Message(iException::Programmer, error, _FILEINFO_);
  }


  /**
   * Converts a binary char to hex (2 hex characters)
   *
   * @param binary
   *
   * @return iString
   */
  iString HexConverter::CharToHex(char binary) {
    // get the left and right parts of the character
    char bin1 = binary >> 4;
    char bin2 = binary & 0xF;

    char hex1 = NibbleToHex(bin1);
    char hex2 = NibbleToHex(bin2);

    iString result;
    result += hex1;
    result += hex2;

    return result;
  }


  /**
   * A nibble is 4 bits (half a byte). Converts half a byte to a
   * hex value (0-9,A-F)
   *
   * @param binary
   *
   * @return iString
   */
  char HexConverter::NibbleToHex(char binary) {
    binary &= 0xF;

    if(binary < 10) {
      return binary + '0';
    }
    else {
      return binary - 10 + 'A';
    }
  }
}
