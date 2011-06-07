/**
 * @file
 * $Revision: 1.15 $
 * $Date: 2010/03/19 20:38:01 $
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

#include <string>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include "iString.h"
#include "iException.h"
#include "SpecialPixel.h"

using namespace std;
namespace Isis {

  /**
   * Constructs an empty iString object
   */
  iString::iString() : string() {
  }

  /**
   * Constructs a iString object with initial value set to the string
   * argument.
   *
   * @param str The initial value of the iString
   */
  iString::iString(const std::string &str) : string(str) {
  }

  /**
   * Constructs a iString with initial value set to the iString argument.
   *
   * @param str The initial value of the iString
   */
  iString::iString(const iString &str) : string(str) {
  }

  /**
   * Constructs a iString with initial value set to the argument
   *
   * @param str The inital value of the iString
   */
  iString::iString(const char *str) : string(str) {
  }

  /**
   * Constructs a iString object with its initial value set to the string
   * representation of the int argument.
   *
   * @param num The initial value of the iString. The integer value is
   *            converted to a string representation and stored as the value.
   */
  iString::iString(const int &num) : string() {
    ostringstream str;
    str << num;
    assign(str.str());
  }

  /**
   * Constructs a iString object with its initial value set to the string
   * representation of the BigInt argument.
   *
   * @param num The initial value of the iString. The integer value is
   *            converted to a string representation and stored as the value.
   */
  iString::iString(const BigInt &num) : string() {
    ostringstream str;
    str << num;
    assign(str.str());
  }

  /**
   * Constructs a iString object with its initial value set to the string
   * representation of the double argument.
   *
   * @param num The initial value of the iString. The double value is converted to
   *            a string representation and stored as the value. The conversion
   *            is handled in the following manner: If (abs(num) < 0.1) it is
   *            presented in scientific notation If (abs(log10(num)) < 16) it is
   *            presented in normal notation if (abs(log10(num)) >= 16) it is
   *            presented in scientific notation Trailing zeros are removed such
   *            that 5.000 is presented as 5.0
   */
  iString::iString(const double &num, const int piPrecision) : string() {
    SetDouble(num, piPrecision);
  }


  /**
   * Performs the conversion necessary to represent a floating-point value as a
   * string. See iString (const double &num) for details
   *
   * @param num The input value to be stored
   */
  void iString::SetDouble(const double &num, const int piPrecision) {
    // This is the original code and was replaced by the good stuff below
    //  ostringstream str;
    //  str << num;
    //  assign (str.str());

    // If num is zero, then it is not valid to do a log10 on it. To avoid this,
    // check for zero ahead of time and handle it.
    if(num == 0.0) {
      assign("0.0");
      return;
    }

    if(num > DBL_MAX) {
      SetDouble(DBL_MAX, piPrecision);
      return;
    }

    if(num < -DBL_MAX) {
      SetDouble(-DBL_MAX, piPrecision);
      return;
    }

    if(isnan(num)) {
      assign("nan");
      return;
    }

    // First determine the number of digits preceding the decimal point
    // Numbers of the form 0.ABCDEFG where A is non-zero are assumed to
    // have a leading digit of zero.  Numbers of the form 0.0ABCDEFG,
    // 0.00ABCEFEG and so on are not considered to have a leading digit
    // (pre = 0).

    //cout << "istring\n";
    //printf("%.15f\n",num);

    double temp = abs(num);
    int pre = (int)(log10(temp)) + 1;

    //printf("%.15f\n",temp);
    //cout << "tempI " << iString(temp) << endl;

    // If preceding number of digits is too large then we will need to create a
    // scientific notation string.  We will need 14 spaces for numbers, 2 spaces
    // for exponents, 2 spaces for signs, and 1 for the letter E, 1 for a decimal
    // place, and 1 extra in case of a leading 0.  A grand total
    // of 21 spaces is required.  Therefore our format can be %22e

    // If the preceding number of digits is zero then we likely have a
    // really small number (e.g., 0.000331236236). So let's put those in
    // scientific notation as well

    // Finally, remove any zeroes before the E and if the exponent is zero
    // then strip it off as well.

    char dblstr[22];

    if((log10(temp) > 13.0) || (log10(temp) < -3.0)) {
      char fmt[8], buff[8];
      sprintf(buff, "%de", piPrecision);
      strcpy(fmt, "%21.");
      strcat(fmt, buff);
      sprintf(dblstr, fmt, num);

      char *e = strchr(dblstr, 'e');
      char *sptr = e - 1;

      while(*sptr == '0') sptr--;
      if(*sptr == '.') sptr++;
      sptr++;
      char tmp[8];
      strcpy(tmp, e);
      strcpy(sptr, tmp);

      e = strchr(dblstr, 'e');
      int allzero = 1;
      for(sptr = e + 2; *sptr; sptr++) if(*sptr != '0') allzero = 0;

      if(allzero) *e = 0;
    }

    // Ok it can be presented as a normal floating point number.  So we will need
    // 14 spaces for numbers, 1 for the sign, 1 for the decimal, and 1 more
    // for a possible leading 0.  A grand total of 17 spaces.  Therefore our
    // format can be "%17.postf".  Finally remove any trailing zeroes.

    else {
      if(temp < 1.0) pre--;
      int post = piPrecision - pre;

      char tempstr[3], fmt[8];
      strcpy(fmt, "%17.");
      sprintf(tempstr, "%d", post);
      strcat(fmt, tempstr);
      strcat(fmt, "f");

      sprintf(dblstr, fmt, num);

      if(post > 0) {
        char *sptr = dblstr + strlen(dblstr) - 1;
        while((*sptr == '0') && (*(sptr - 1) != '.')) *sptr-- = 0;
      }
    }

    while(dblstr[0] == ' ') {
      for(unsigned int i = 0; i < strlen(dblstr); i++) {
        dblstr[i] = dblstr[i+1];
      }
    }
    assign(dblstr);
  }

  /**
   * Constructs a iString object with initial value set to the input QString
   * @param str
   */
  iString::iString(const QString &str) : string() {
    assign(str.toStdString());
  }

  /**
   * Destructor
   */
  iString::~iString() {}

  /**
   * Removes characters from the beginning and end of the iString. The order
   * of the characters makes no difference.
   *
   * @param chars The string of characters to be trimmed
   *
   * @return iString
   */
  iString iString::Trim(const std::string &chars) {
    TrimHead(chars);
    TrimTail(chars);
    return *this;
  }

  /**
   * Removes all occurences of the input characters from the beginning and
   * end of the input string
   *
   * @param chars The string of characters to be removed. Order makes no
   * difference
   *
   * @param str The input string to be trimmed
   *
   * @return string The result of the trimming operation
   */
  std::string iString::Trim(const std::string &chars, const std::string &str) {
    //string result = str;
    //result.replace (0,str.find_first_not_of (chars), "");
    return TrimTail(chars, TrimHead(chars, str));
  }

  /**
   * Trims The input characters from the beginning of the object iString
   *
   * @param chars The string of characters to be trimmed. Order makes no
   *              difference
   */
  iString iString::TrimHead(const std::string &chars) {
    *this = replace(0, find_first_not_of(chars), "");
    return *this;
  }

  /**
   * Trims the input characters from the beginning of the input string
   *
   * @param chars The input characters to be removed. Order makes no difference
   * @param str The string to be trimmed
   *
   * @return string The resulting string
   */
  std::string iString::TrimHead(const std::string &chars, const std::string &str) {
    string result = str;
    result.replace(0, str.find_first_not_of(chars), "");
    return result;
  }

  /**
   * Trims the input characters from the end of the object iString
   *
   * @param chars The string of characters to be removed. Order is irrelevant
   *
   */
  iString iString::TrimTail(const std::string &chars) {
    *this = erase(find_last_not_of(chars) + 1);
    return *this;
  }

  /**
   * Trims the input characters from the end of the input string
   *
   * @param chars The characters to be removed from the input string. Order does
   *              not matter, all characters are treated individually.
   *
   * @param str The string to be trimmed
   *
   * @return string The result of the trimming
   */
  std::string iString::TrimTail(const std::string &chars, const std::string &str) {
    string result = str;
    result.erase(str.find_last_not_of(chars) + 1);
    return result;
  }

  /**
   * Converst any lower case characters in the object iString with uppercase
   * characters
   *
   */
  iString iString::UpCase() {
    string temp = *this;
    *this = UpCase(temp);
    return *this;
  }

  /**
   * Converts lower case characters in the input string to upper case characters
   *
   * @param str The string to be converted
   *
   * @return string The result of the conversion
   */
  std::string iString::UpCase(const std::string &str) {
    string sOut(str);
    transform(str.begin(), str.end(), sOut.begin(), (int ( *)(int)) toupper);
    return(sOut);
  }

  /**
   * Converts all upper case letters in the object iString into lower case
   * characters
   *
   */
  iString iString::DownCase() {
    *this = DownCase((string) * this);
    return *this;
  }

  /**
   * Converts all upper case letters in the input string into lower case
   * characters
   *
   * @param str
   *
   * @return string
   */
  std::string iString::DownCase(const std::string &str) {
    string sOut(str);
    transform(str.begin(), str.end(), sOut.begin(), (int ( *)(int))tolower);
    return sOut;
  }

  /** Compare two characters without regard to case
   *
   * This small, internal function compares two characters while ignoring
   * case.  This is used in the STL @b equal function to compare the contents
   * of two STL strings.
   *
   * @param c1 First character to compare
   * @param c2 Second character to compare
   * @return true if the two characters are the same, false otherwise
   */
  static bool nocase_compare(const char c1, const char c2) {
    return(toupper(c1) == toupper(c2));
  }

  /**
   * Compare a string to the object iString
   *
   * @param str The string with which the comparison is made
   *
   * @return bool True if they are equal, false if they are not.
   */
  bool iString::Equal(const std::string &str) const {
    string temp = *this;
    return Equal(str, temp);
  }

  /**
   * Compare two strings, case-insensitive
   *
   * @param str1 [in] The first string to compare
   * @param str2 [in] The second string to compare
   *
   * @return True if the two input strings are identical otherwise false
   */
  bool iString::Equal(const std::string &str1, const std::string &str2) {
    if(str1.size() != str2.size()) return(false);
    return(std::equal(str1.begin(), str1.end(), str2.begin(), nocase_compare));
  }


  /**
   * Returns the object string as an integer.
   *
   * @return int The integer te string represents
   */
  int iString::ToInteger() const {
    return ToInteger(*this);
  }

  /**
   * Returns the integer representation of the input string
   *
   * @param str The string representing an integer value
   *
   * @return int The integer value represented by the string
   */
  int iString::ToInteger(const std::string &str) {
    int v_out;
    try {
      stringstream s;
      s << str;               // Put the string into a stream
      s.seekg(0, ios::beg);     // Move the input pointer to the beginning
      s >> v_out;               // read/get "type T" out of the stream
      ios::iostate state = s.rdstate();
      if((state & ios::failbit) || (state & ios::badbit) ||
          (!(state & ios::eofbit))) {  // Make sure the stream is empty
        throw(int) - 1;
      }
    }
    catch(...) {
      string message = "Cannot convert (" + str + ") to an integer";
      throw Isis::iException::Message(Isis::iException::Parse, message, _FILEINFO_);
    }
    return(v_out);
  }

  /**
   * Returns the BigInt representation of the object iString
   *
   * @return BigInt The Big Integer representation of the iString
   */
  BigInt iString::ToBigInteger() const {
    return ToBigInteger(*this);
  }

  /**
   * Returns the Big Integer representation of the input string
   *
   * @param str The string representing an integer value
   *
   * @return BigInt The string as a BigInt
   */
  BigInt iString::ToBigInteger(const std::string &str) {
    BigInt v_out;
    try {
      stringstream s;
      s << str;            // Put the string into a stream
      s.seekg(0, ios::beg);     // Move the input pointer to the beginning
      s >> v_out;               // read/get "type T" out of the stream
      ios::iostate state = s.rdstate();
      if((state & ios::failbit) || (state & ios::badbit) ||
          (!(state & ios::eofbit))) {  // Make sure the stream is empty
        throw(int) - 1;
      }
    }
    catch(...) {
      string message = "Cannot convert (" + str + ") to a big integer";
      throw Isis::iException::Message(Isis::iException::Parse, message, _FILEINFO_);
    }
    return(v_out);
  }

  /**
   * Returns the floating point value the iString represents
   *
   * @return double The iString as a double
   */
  double iString::ToDouble() const {
    return ToDouble(*this);
  }

  /**
   * Returns the floating-point value represented by the input string
   *
   * @param str The string representing the numeric value
   *
   * @return double The number the string represents
   */
  double iString::ToDouble(const std::string &str) {

    double v_out;

    // Convert a hex value
    if(str.substr(0, 3) == "16#" && str.substr(str.length() - 1, 1) == "#") {
      try {
        stringstream s;
        s << str.substr(3, str.find_last_of("#") - 3);
        s.seekg(0, ios::beg);

        union {
          unsigned int i;
          float f;
        } u;

        s >> hex >> u.i;

        ios::iostate state = s.rdstate();
        if((state & ios::failbit) || (state & ios::badbit)) {
          throw(int) - 1;
        }
        v_out = u.f;
      }
      catch(...) {
        string message = "Cannot convert HEX value [" + str + "] to a double";
        throw Isis::iException::Message(Isis::iException::Parse, message, _FILEINFO_);
      }
    }
    // Convert a decimal value
    else {
      try {
        stringstream s;
        s << str;               // Put the string into a stream
        s.seekg(0, ios::beg);     // Move the input pointer to the beginning
        s >> v_out;               // read/get "type T" out of the stream
        ios::iostate state = s.rdstate();
        if((state & ios::failbit) || (state & ios::badbit) ||
            (!(state & ios::eofbit))) {  // Make sure the stream is empty
          throw(int) - 1;
        }
      }
      catch(...) {
        string message = "Cannot convert [" + str + "] to a double";
        throw Isis::iException::Message(Isis::iException::Parse, message, _FILEINFO_);
      }
    }

    return(v_out);
  }

  /**
   * Retuns the object string as a QString
   */
  QString iString::ToQt() const {
    return QString::fromStdString(*this);
  }

  /**
   * Resturns the input string as a QString
   *
   * @param s [in] The standard string to be converted to a Qt string
   */
  QString iString::ToQt(const std::string &s) {
    return(QString::fromStdString(s));
  }

  /**
   * Returns the first token in the iString. A token is defined as a string of
   * characters from the beginning of the string to, but not including, the first
   * character matching any character in the separator string. The token is
   * removed from the original string along with the separator.
   *
   * @param separator The string of characters used to separate tokens. The order
   *                  of the characters is not important.
   *
   * @return iString
   */
  iString iString::Token(const iString &separator) {
    iString retstr = "" ;

    for(unsigned int i = 0; i < size(); i++) {
      for(unsigned int sep = 0; sep < separator.size(); sep++) {
        if(separator[sep] == at(i)) {
          retstr = substr(0, i);
          replace(0, i + 1, "");
          return retstr;
        }
      }
    }

    if(retstr == "") {
      retstr = (*this);
      replace(0, size(), "");
    }

    return retstr;
  }

  /**
   * @brief Find separators between characters and split them into strings
   *
   * This method will break up the input string into tokens that are separated by
   * one or more of the specified character.  If allowEmptyEntries == true, then
   * one or separator characters are deem a single separator and the string is
   * split into two different sections.  If allowEmptyEntries == false, then
   * should more than one separator character occur in succession, this will
   * result in the number of separator characters less one empty strings returned
   * to the caller.
   *
   * @param separator  A single character that separates each substring
   * @param str  The string to break into separate fields or tokens
   * @param tokens  A vector of strings that will receive the tokens as separated
   *                by the separator character.
   * @param allowEmptyEntries If true, treat successive separator characters as a
   *                          single separator.  If false, successive separator
   *                          characters result in empty strings/tokens.
   * @return int The number of fields/tokens found in str
   */
  int iString::Split(const char separator, const std::string &str,
                     std::vector<std::string> &tokens,
                     bool allowEmptyEntries) {
    string::size_type idx(0), idx2(0);
    unsigned int ksize = str.size();
    tokens.clear();

    if(ksize > 0) {
      if(str[idx] == separator) idx++;
      while((idx2 = str.find(separator, idx)) != string::npos) {
        if((idx2 == idx)) {
          if(allowEmptyEntries) tokens.push_back("");
        }
        else {
          string::size_type len(idx2 - idx);
          tokens.push_back(str.substr(idx, len));
        }
        idx = idx2 + 1;
      }
      if(idx < ksize) tokens.push_back(str.substr(idx));
    }
    return(tokens.size());
  }


  /**
   * Collapses multiple spaces into single spaces
   *
   * @param force Determines whether to compress inside quotes (single and
   *              double)
   *
   */
  iString iString::Compress(bool force) {
    *this =  Compress((string) * this, force);
    return *this;
  }

  /**
   * Returns the input string with multiple spaces collapsed into single spaces
   *
   * @param str The string to be compressed
   *
   * @param force Determines whether to compress inside quotes
   *
   * @return string The compressed version of the input string
   */
  std::string iString::Compress(const std::string &str, bool force) {
    string result(str);
    if(force == false) {
      int spaces = 0;
      int leftquote = result.find_first_of("\"\'");
      while((spaces = result.find("  ", spaces)) >= 0) {
        int rightquote = result.find_first_of("\"\'", leftquote + 1);
        if(spaces < leftquote) { //spaces are before quotation
          result.erase(spaces, 1);
          leftquote = result.find_first_of("\"\'", spaces);
        }
        else if((spaces > leftquote) && (spaces < rightquote)) {   //spaces are within quotation
          spaces = rightquote + 1;
          leftquote = result.find_first_of("\"\'", rightquote + 1);
        }
        else if(leftquote == (int)npos) {   //there are no quotations
          result.erase(spaces, 1);
        }
        else {  //spaces are after quotation
          leftquote = result.find_first_of("\"\'", rightquote + 1);
        }
      }
      return result;
    }
    else {
      int spaces = 0;
      while((spaces = result.find("  ", spaces)) >= 0) {
        result.erase(spaces, 1);
      }
      return result;
    }
  }

  /**
   * Replaces all instances of the first input string with the second input
   * string
   *
   * For more information, see iString::Replace(const string, const string,
   * const string, int)
   *
   * @param from Search string that when found in str, it is replaced with to
   * @param to iString that will replace every occurance of from in str.
   * @param maxReplaceCount  Maximum number of replacements to allow per call
   *
   */
  iString iString::Replace(const std::string &from, const std::string &to,
                           int maxReplaceCount) {
    *this = iString(Replace((string) * this, from, to, maxReplaceCount));
    return *this;
  }

  /**
   * @brief Replace specified substring with replacement substring in a string
   *
   * This method accepts a string, a target substring and a replacement substring
   * with the intent to find all occurances of the \b subTarg substring in \b s
   * and replace them with the substring \b subRep.  The \b maxReplaceCount
   * parameter is so that a should the replacement substring contain the target
   * substring, an infinite loop would occur.
   *
   * Note that the search for strings are implemented as a loop that always starts
   * at the begining of \b s.  So should the above scenario occur, it will be
   * limited.
   *
   * I have found this useful for formulating database SQL queries in a loop.  The
   * following example illustrates this usage:
   * @code
   *   string pntDist  = "distance(giscpt,UPCPoint(%longitude,%latitude))";
   *   string pntQuery = "SELECT pointid, latitude, longitude, radius, "
   *                     " %distance AS Distance FROM "  + pntTable +
   *                     " WHERE (%distance <= " + iString(maxDist) + ")";
   *
   * SqlQuery finder;  // Uses whatever the default database is
   * while (!theEndOfTime()) {
   *    iString longitude(source.getLongitude());
   *    iString latitude(source.getLatitude());
   *
   *    string qDist = StringTools::replace(pntDist,"%longitude", longitude);
   *    qDist = StringTools::replace(qDist,"%latitude", latitude);
   *    string query = StringTools::replace(pntQuery, "%distance", qDist);
   *    finder.exec(query);
   *    ... //  Do what you will with the results!
   * }
   * @endcode
   *
   * This routine \b is case sensitive and will only replace exact matches.
   *
   * To prevent infinite recursion, where the replace string contains the search
   * string, use the \b maxReplaceCount to adjust appropriately.
   *
   * @param str Input string to search and replace substrings
   * @param from Search string that when found in str, it is replaced with to
   * @param to iString that will replace every occurance of from in str.
   * @param maxReplaceCount  Maximum number of replacements to allow per call
   *
   * @return std::string NEw string with from replaced with to
   */
  std::string iString::Replace(const std::string &str, const std::string &from,
                               const std::string &to, int maxReplaceCount) {
    if(str.empty()) return(str);
    if(from.empty()) return(str);
    string sRet(str);
    string::size_type pos;
    int nReplaced = 0;
    while((nReplaced < maxReplaceCount) &&
          (pos = sRet.find(from)) != string::npos) {
      sRet.replace(pos, from.size(), to);
      nReplaced++;
    }
    return(sRet);
  }


  /**
   * Replaces all instances of the first input string with the second input
   * string. Honoring quotes if requested by the boolean
   *
   * @param from Search string that when found in str, it is
   *                replaced with to.
   * @param to iString that will replace every occurance of
   *          from in str.
   * @param honorquotes  Set to true to honor quotes and not
   *                     replace inside them
   *
   * @return iString New string with subTarg replaced with subRep
   *
   */
  iString iString::Replace(const std::string &from, const std::string &to,
                           bool honorquotes) {
    *this = Replace((string) * this, from, to, honorquotes);
    return *this;
  }

  /**
   * Replace specified substring with replacement substring in a string honoring
   * quotes if requested. This routine is case sensitive and will only replace
   * exact matches.
   *
   * @param str Input string to search and replace substrings in
   * @param from Search string that when found in str, it is
   *                replaced with to.
   * @param to iString that will replace every occurance of
   *          from in str.
   * @param honorquotes  Set to true to honor quotes and not
   *                     replace inside them
   *
   * @return iString New string with subTarg replaced with
   *         subRep
   */
  iString iString::Replace(const std::string &str, const std::string &from,
                           const std::string &to , bool honorquotes) {

    string result = str;
    if(honorquotes == true) {
      int instances = 0;
      int quote = result.find_first_of("\"\'");
      while((instances = result.find(from, instances)) >= 0) {

        int nextquote = result.find_first_of("\"\'", quote + 1);
        if(instances < quote) {
          result.replace(instances, from.length(), to);
          quote = result.find_first_of("\"\'", instances);
        }
        else if((instances > quote) && (instances < nextquote)) {
          instances = nextquote + 1;
          quote = result.find_first_of("\"\'", nextquote);
        }
        else if(quote == (int)npos) {
          result.replace(instances, from.length(), to);
        }
        else {
          quote = result.find_first_of("\"\'", nextquote);
        }
      }
      return (iString) result;
    }
    else {
      int instances = 0;
      while((instances = result.find(from, instances)) >= 0) {
        result.replace(instances, from.length(), to);
      }
      return (iString) result;
    }
  }

  /**
   * Returns the string with all occurrences of any character in the "from"
   * argument converted to the "to" argument. The original string is modified.
   *
   * @param listofchars The string of characters to be replaced. The order of the
   *             characters is not important.
   *
   * @param to The single character used as the replacement.
   *
   * @return iString
   */
  iString iString::Convert(const std::string &listofchars, const char &to) {
    *this = Convert((string) * this, listofchars, to);
    return *this;
  }

  /**
   * Converts all occurences in the input string of any character in the "from"
   * string to the "to" character
   *
   * @param str The input string
   *
   * @param listofchars The string of characters to be replaced. The order of the
   *             characters is unimportant
   *
   * @param to The single character used as replacement
   *
   * @return string The converted string (the input string is unmodified)
   */
  string iString::Convert(const std::string &str, const std::string &listofchars,
                          const char &to) {
    std::string::size_type pos = 0;
    string result = str;
    string tmp;
    tmp = to;
    while((pos = result.find_first_of(listofchars, pos)) != npos) {
      result.replace(pos, 1, tmp);
      pos++;
    }
    return result;
  }

  /**
   * Returns the string with all "new lines", "carriage returns", "tabs", "form
   * feeds", "vertical tabs" and "back spaces" converted to single spaces. All
   * quotes are ignored. The original string is modified.
   *
   * @return iString
   */
  iString iString::ConvertWhiteSpace() {
    *this = ConvertWhiteSpace((string) * this);
    return *this;
  }

  /**
   * Converts all forms of whitespace in the input string into single spaces
   *
   * @param str
   *
   * @return string
   */
  std::string iString::ConvertWhiteSpace(const std::string &str) {
    return Convert(str, "\n\r\t\f\v\b", ' ');
  }

  /**
   * Remove all instances of any character in the string from the iString
   *
   * @param del The characters to be removed from the iString. The character is
   *            unimportant
   *
   * @return iString
   */
  iString iString::Remove(const std::string &del) {
    std::string::size_type pos;
    while((pos = find_first_of(del)) != npos) this->erase(pos, 1);
    return *this;
  }

  /**
   * Remove all instances of any character in the "del" argument from the input
   * string
   *
   * @param str The string from which characters are to be removed
   *
   * @param del The string of characters to be removed. Order is unimportant.
   *
   * @return string The string with the characters removed. The original string
   *         is unmodified
   */
  std::string iString::Remove(const std::string &str, const std::string &del) {
    string::size_type pos;
    string result(str);
    while((pos = result.find_first_of(del)) != npos) result.erase(pos, 1);
    return result;
  }

  /**
   * Attempts to convert a 32 bit integer into its string representation
   *
   * @param value [in] The 32 bit integer to be converted to a string
   *
   * @return The Isis::iString representation of the int
   */
  iString &iString::operator= (const int &value) {
    ostringstream str;
    str << value;
    assign(str.str());
    return *this;
  }

  /**
   * Attempts to convert a 64 bit integer into its string representation
   *
   * @param value [in] The 64 bit integer to be converted to a string
   *
   * @return The Isis::iString representation of the BigInt
   */
  iString &iString::operator= (const BigInt &value) {
    ostringstream str;
    str << value;
    assign(str.str());
    return *this;
  }

  /**
   * Converts a Qt string into a std::string
   *
   * @param str [in] The Qt string to be converted to a std::string
   *
   * @return The std::string representation of the Qt string
   */
  std::string iString::ToStd(const QString &str) {
    return(str.toStdString());
  }

  /**
   * Converts a vector of strings into a QStringList
   *
   * @param sl STL vector of strings
   *
   * @return QStringList
   */
  QStringList iString::ToQt(const std::vector<std::string> &sl) {
    QStringList Qsl;
    for(unsigned int i = 0 ; i < sl.size() ; i++) {
      Qsl << ToQt(sl[i]);
    }
    return Qsl;
  }

  /**
   * Converts a QStringList into a vector of strings
   *
   * @param sl
   *
   * @return vector<string>
   */
  std::vector<std::string> iString::ToStd(const QStringList &sl) {
    std::vector<std::string> Stdsl;
    for(int i = 0 ; i < sl.size() ; i++) {
      Stdsl.push_back(ToStd(sl.at(i)));
    }

    return(Stdsl);
  }
}
