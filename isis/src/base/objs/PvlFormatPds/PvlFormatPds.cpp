/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/09/15 21:13:25 $
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

#include <sstream>
#include <iomanip>

#include "iException.h"
#include "Message.h"
#include "iString.h"
#include "Filename.h"
#include "Constants.h"
#include "TextFile.h"

#include "PvlFormatPds.h"

using namespace std;

namespace Isis {

  /*
  * Constructs an empty PvlFormatPds
  */
  PvlFormatPds::PvlFormatPds() {
    Init();
  }


  /*
  * Constructs a PvlFormatPds using the file name to ingest to fill the keyword
  * to type map.
  * 
  * @param file A file name with keyword=type. Where KEYWORD is the name of a
  * keyword and TYPE is one of [string | integer | float | ...]
  */
  PvlFormatPds::PvlFormatPds(const std::string &file) : PvlFormat(file) {
    Init();
  }


  /*
  * Constructs a PvlFormatPds using the specified pre populated map of keyword name
  * (std::string) vs keyword type (KeywordType).
  * 
  * @param keywordType A map with keyword, type. Where keyword is the name of a
  * keyword in a PvlKeyword and type is one of [string | integer | float ]
  */
  PvlFormatPds::PvlFormatPds(Pvl &keywordType) : PvlFormat(keywordType) {
    Init();
  }


  //! Clears all PvlFormatPds specific data.
  void PvlFormatPds::Init() {
  }


  /*
  * Returns the keyword value formatted in "PDS" format
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormatPds::FormatValue (const PvlKeyword &keyword, int num) {

    iString name = keyword.Name();
    name.UpCase();
    if (name == "OBJECT" || (name == "GROUP")) {
      iString val = (string)keyword;
      return val.UpCase();
    }

    // Find out what type this keyword is
    KeywordType type = Type(keyword);

    switch (type) {
      case StringKeyword:
        return FormatString(keyword, num);
        break;

      case RealKeyword:
        return FormatReal(keyword, num, Accuracy(keyword));
        break;

      case IntegerKeyword:
        return FormatInteger(keyword, num, Accuracy(keyword));
        break;

      case HexKeyword:
        return FormatHex(keyword, num, Accuracy(keyword));
        break;

      case BinaryKeyword:
        return FormatBinary(keyword, num, Accuracy(keyword));
        break;

      case EnumKeyword:
        return FormatEnum(keyword, num);
        break;

      case BoolKeyword:
        return FormatBool(keyword, num);
        break;

      case NoTypeKeyword:
      default:
        return FormatUnknown(keyword,num);
        break;
    }
    return FormatUnknown(keyword,num);
  }


  /*
  * Returns the keyword value formatted as a "PDS" string
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword 
  * @internal 
  *   @history 2009-09-15 Jeannie Walldren - Moved the call to AddQuotes()
  *                                          inside the else-statement since
  *                                          the if portion of the code already
  *                                          adds quotes automatically
  */
  std::string PvlFormatPds::FormatString (const PvlKeyword &keyword, int num) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      val += keyword[num];
      val = AddQuotes (val);
    }


    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val = "(" + val;
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
      // For now PDS units are case sensitive, so we should not UpCase them
      //      unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // For now PDS units are case sensitive, so we should not UpCase them
      //      unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted as a "PDS" real number
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormatPds::FormatReal(const PvlKeyword &keyword, int num,
                                       int places) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val += "(";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else if (places >= 0) {
      stringstream out;
      out << setiosflags(ios::fixed) << setprecision(places) << (double)keyword[num];
      val += out.str();
    }
    else {
      val += keyword[num];
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted as a "PDS" enumeration
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormatPds::FormatEnum (const PvlKeyword &keyword, int num) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val += "(";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      val += keyword[num];
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
    // unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted without any knowledge of its type
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  * @internal 
  *   @history 2009-09-15 Jeannie Walldren - Moved the call to AddQuotes()
  *                                          inside the else-statement since
  *                                          the if portion of the code already
  *                                          adds quotes automatically
  */
  std::string PvlFormatPds::FormatUnknown (const PvlKeyword &keyword, int num) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      val += keyword[num];
      val = PvlFormat::AddQuotes (val);
    }


    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val = "(" + val;
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted as an integer
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormatPds::FormatInteger (const PvlKeyword &keyword, int num, int bytes) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val += "(";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      val += keyword[num];
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted as a binary value
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormatPds::FormatBinary (const PvlKeyword &keyword, int num, int bits) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    stringstream ss;
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val += "(";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      tmp.clear();
      BigInt value = (BigInt)keyword[num];
      string binDig = "01";
      do {
        tmp = binDig[value % 2] + tmp;
        value /= 2;
      } while (value);

      ss << right << setfill('0') << setw(bits) << tmp;
      tmp = ss.str();
      val += "2#" + tmp + "#";
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted as a hexidecimal value
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */


  std::string PvlFormatPds::FormatHex (const PvlKeyword &keyword, int num, int bytes) {

    iString val;
    val.clear();
    bool singleUnit = false;

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val += "(";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      stringstream ss;
      if (bytes == 2) {
        ss << hex << (unsigned short int)(int)keyword[num];
      }
      else if (bytes == 4) {
        ss << hex << (unsigned int)(int)keyword[num];
      }
      else {
        ss << hex << (BigInt)keyword[num];
      }
      iString h = ss.str();
      h.UpCase();
      val += "16#" + h + "#";
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) {
      iString unit = keyword.Unit(num);
      // unit.UpCase();
      val += " <" + unit + ">";
    }

    return val;
  }


  /*
  * Returns the keyword value formatted as a boolean value
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormatPds::FormatBool (const PvlKeyword &keyword, int num) {

    iString val;
    val.clear();

    // Create a Null value if the value index is greater than the number of values
    if ((num >= keyword.Size()) || (keyword[num].size() == 0)) {
      return "NULL";
    }

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val += "(";
    }

    // Handle PDS special values "N/A" "NULL" "UNK"
    iString tmp = keyword[num];
    tmp.UpCase();
    if ((tmp == "N/A") || (tmp == "NULL") || (tmp == "UNK")) {
      val += "\"" + tmp + "\"";
    }
    else {
      val += keyword[num];
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array, close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    return val;
  }




  /*
  * Format the name of this container
  * 
  * @param keyword The keyword (i.e., the Object or Group)
  */
  std::string PvlFormatPds::FormatName (const PvlKeyword &keyword) {
    iString text = keyword.Name();
    text.UpCase();
    return text;
  };


  /*
  * Format the end of a group or object
  * 
  * @param name A string representing the end text.
  * @param keyword The keyword (i.e., the Object or Group) that is ending
  */
  std::string PvlFormatPds::FormatEnd (const std::string name,
                                       const PvlKeyword &keyword) {
    iString left = name;
    left.UpCase();
    left += " = ";
    iString right = (string)keyword;
    right.UpCase();
    return left + right;
  };


  /*
  * Put quotes around the value of a keyword of type string according to PDS
  * standards. All keywords identified as "string" are quoted for PDS labels.
  * 
  * @param value The value of a PvlKeyword to be formatted. 
  * @internal 
  *   @history 2009-09-15 Jeannie Walldren - Added case to skip add quotes if
  *                                          the first character of the
  *                                          string is " or '
  */
  std::string PvlFormatPds::AddQuotes (const std::string value) {

    std::string val = value;

    bool quoteValue = true;
    bool singleQuoteValue = false;
    if (val.find(" ") != std::string::npos) {
      if (val.find("\"") != std::string::npos) {
        singleQuoteValue = true;
        quoteValue = false;
      }
    }

    // Turn the quoting back off if this value looks like a sequence
    // In this case the internal values should already be quoted.
    if (val[0] == '(') {
      singleQuoteValue = false;
      quoteValue = false;
    }
    else if (val[0] == '"') {
      singleQuoteValue = false;
      quoteValue = false;
    }
    else if (val[0] == '\'') {
      singleQuoteValue = false;
      quoteValue = false;
    }

    if (quoteValue) {
      val = "\"" + val + "\"";
    }
    else if (singleQuoteValue) {
      val = "'" + val + "'";
    }

    return val;
  }

}

