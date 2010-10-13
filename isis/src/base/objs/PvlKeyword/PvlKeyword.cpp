/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/06/25 20:42:35 $
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

#include "IsisDebug.h"

#include "PvlKeyword.h"
#include "iException.h"
#include "Message.h"
#include "iString.h"
#include "PvlSequence.h"
#include "PvlFormat.h"

using namespace std;
namespace Isis {
  //! Constructs a blank PvlKeyword object.
  PvlKeyword::PvlKeyword() {
    Init();
  }


  /**
   * Constructs a PvlKeyword object with a name.
   *
   * @param name The keyword name
   */
  PvlKeyword::PvlKeyword(const std::string &name) {
    Init();
    SetName(name);
  }


  /**
   * Constructs a PvlKeyword object with a name, value and units.
   * Defaults to unit="".
   *
   * @param name The keyword name.
   * @param value The keyword values.
   * @param unit The units the values are given in.
   */
  PvlKeyword::PvlKeyword(const std::string &name, const Isis::iString value,
                         const std::string unit) {
    Init();
    SetName(name);
    AddValue(value, unit);
  }


  //! Copy constructor
  PvlKeyword::PvlKeyword(const PvlKeyword &other) {
    *this = other;
  }


  /**
   * Destructs a PvlKeyword object.
   */
  PvlKeyword::~PvlKeyword() {}


  //! Clears all PvlKeyword data.
  void PvlKeyword::Init() {
    Clear();
    ClearComments();
    SetName("");
    p_width = 0;
    p_indent = 0;
    p_formatter = NULL;
  }

  /**
   * Decides whether a value is null or not at a given index.
   * Defaults to index = 0.
   *
   * @param index The value index
   * @return <B>bool</B> True if the value is null, false if it's
   *         not.
   */
  bool PvlKeyword::IsNull(const int index) const {
    if(Size() == 0) return true;
    if(index < 0 || index >= (int)p_values.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
    if(StringEqual("NULL", p_values[index])) return true;
    if(StringEqual("", p_values[index])) return true;
    if(StringEqual("\"\"", p_values[index])) return true;
    if(StringEqual("''", p_values[index])) return true;
    return false;
  }

  /**
   * Sets the keyword name.
   *
   * @param name The new keyword name.
   */
  void PvlKeyword::SetName(const std::string &name) {
    iString final(name);
    final.Trim("\n\r\t\f\v\b ");
    if(final.find_first_of("\n\r\t\f\v\b ") != std::string::npos) {
      string msg = "[" + name + "] is invalid. Keyword name cannot ";
      msg += "contain whitespace.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
    p_name = final;
  }

  /**
   * Sets new values.
   *
   * If no current value exists, this method sets the given value
   * to the PvlKeyword.  Otherwise, it clears any existing values
   * and resets to the value given using AddValue(). Defaults to
   * unit = "" (empty string).
   *
   * @param value New value to be assigned.
   * @param unit Units of measurement corresponding to the value.
   *
   * @see AddValue()
   * @see operator=
   * @see operator+=
   */
  void PvlKeyword::SetValue(const Isis::iString value, const std::string unit) {
    Clear();
    AddValue(value, unit);
  }


  /**
   * Sets the unit of measure for all current values if any exist
   *
   * @param units New units to be assigned.
   */
  void PvlKeyword::SetUnits(const iString &units) {
    p_units.clear();
    for(unsigned int i = 0; i < p_values.size(); i++) {
      p_units.push_back(units);
    }
  }


  /**
   * Sets the unit of measure for a given value
   *
   * @param value The value to match
   * @param units New units to be assigned.
   *
   * @throws Isis::iException::Programmer - Given value must exist
   */
  void PvlKeyword::SetUnits(const iString &value, const iString &units) {

    bool found = false;
    int i = -1;
    while(!found && ++i < (int) p_values.size()) {
      if(value == p_values[i]) {
        found = true;
      }
    }

    if(found) {
      ASSERT(i < (int) p_units.size());

      p_units[i] = units;
    }
    else {
      iString msg = "PvlKeyword::SetUnits called with value [" + value +
                    "] which does not exist in this Keyword";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   *
   * Sets new values.
   *
   * Overwrites the '=' operator to add a new value using AddValue(). Like
   * SetValue(), this method clears any previously existing values and resets to
   * the given value with unit = "" (empty string).
   *
   * @param value The value to be added.
   * @return <B>PvlKeyword&</B> Reference to PvlKeyword object.
   *
   * @see AddValue()
   * @see SetValue()
   * @see operator+=
   */
  PvlKeyword &PvlKeyword::operator=(const Isis::iString value) {
    Clear();
    AddValue(value);
    return *this;
  }

  /**
   * Adds a value with units.
   *
   * If no current value exists, this method sets the given value.
   * Otherwise, it retains any current values and adds the value
   * given to the array of values for this PvlKeyword object.
   * Defaults to unit = "" (empty string).
   *
   * @param value New value to be assigned.
   * @param unit Units of measurement corresponding to the value.
   *
   * @see SetValue()
   * @see operator=
   * @see operator+=
   */
  void PvlKeyword::AddValue(const Isis::iString value, const std::string unit) {
    p_values.push_back(value);
    p_units.push_back(unit);
  }

  /**
   * Adds a value.
   *
   * Overwrites the '+=' operators to add a new value. Like
   * AddValue(), this method keeps any previously existing values
   * and adds the new value with unit = "" (empty string) to the
   * array of values for this PvlKeyword object.
   *
   * @param value The new value.
   * @return <B>PvlKeyword&</B> Reference to PvlKeyword object.
   *
   * @see AddValue()
   * @see SetValue()
   * @see operator=
   */
  PvlKeyword &PvlKeyword::operator+=(const Isis::iString value) {
    AddValue(value);
    return *this;
  }

  //! Clears all values and units for this PvlKeyword object.
  void PvlKeyword::Clear() {
    p_values.clear();
    p_units.clear();
  }

  /**
   * Gets value for this object at specified index.
   *
   * Overrides the '[]' operator to return the element in the
   * array of values at the specified index.
   *
   * @param index The index of the value.
   * @return <B>iString</B> The value at the index.
   * @throws iException ArraySubscriptNotInRange (index) Index out of bounds.
   *
   * @see const operator[]
   */
  Isis::iString &PvlKeyword::operator[](const int index) {
    if(index < 0 || index >= (int)p_values.size()) {
      string msg = (Isis::Message::ArraySubscriptNotInRange(index)) +
                   "for Keyword [" + p_name + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
    return p_values[index];
  }

  /**
   * Gets value for this object at specified index.
   *
   * Overrides the '[]' operator to return the element in the
   * array of values at the specified index.
   *
   * @param index The index of the value.
   * @return <b>iString</b> The value at the index.
   * @throws iException ArraySubscriptNotInRange (index) Index out of bounds.
   *
   * @see operator[]
   */
  const Isis::iString &PvlKeyword::operator[](const int index) const {
    if(index < 0 || index >= (int)p_values.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
    return p_values[index];
  }

  /**
   * Returns the units of measurement of the element of the array
   * of values for the object at the specified index. Defaults to
   * index = 0.
   *
   * @param index The index of the unit.
   * @return <B>string</B> The unit at the index.
   * @throws iException ArraySubscriptNotInRange (index) Index out of bounds.
   */
  string PvlKeyword::Unit(const int index) const {
    if(index < 0 || index >= (int)p_units.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
    return p_units[index];
  }

  /**
   * Add a comment to the PvlKeyword.
   *
   * @param comment The new comment.
   *
   * @see AddCommentWrapped()
   * @see AddComments()
   * @see ClearComments()
   */
  void PvlKeyword::AddComment(const std::string &comment) {
    if(comment.size() == 0) {
      p_comments.push_back("#");
    }
    if(comment[0] == '#') {
      p_comments.push_back(comment);
    }
    else if(comment.size() == 1) {
      p_comments.push_back("# " + comment);
    }
    else if((comment[0] == '/') && (comment[1] == '*')) {
      p_comments.push_back(comment);
    }
    else if((comment[0] == '/') && (comment[1] == '/')) {
      p_comments.push_back(comment);
    }
    else {
      p_comments.push_back("# " + comment);
    }
  }

  /**
   * Automatically wraps and adds long comments to the PvlKeyword
   *
   * @param comment The new comment to add
   *
   * @see AddComment()
   * @see AddComments()
   * @see ClearComments()
   */
  void PvlKeyword::AddCommentWrapped(const std::string &comment) {
    iString cmt = comment;
    string token = cmt.Token(" ");
    while(cmt != "") {
      string temp = token;
      token = cmt.Token(" ");
      int length = temp.size() + token.size() + 1;
      while((length < 72) && (token.size() > 0)) {
        temp += " " + token;
        token = cmt.Token(" ");
        length = temp.size() + token.size() + 1;
      }
      AddComment(temp);
    }
    if(token.size() != 0) AddComment(token);
  }

  //! Clears the current comments.
  void PvlKeyword::ClearComments() {
    p_comments.clear();
  }

  /**
   * Return a comment at the specified index.
   * @param index The index of the comment.
   * @return <B>string</B> The comment at the index.
   * @throws iException ArraySubscriptNotInRange (index) Index out of bounds.
   */
  string PvlKeyword::Comment(const int index) const {
    if(index < 0 || index >= (int)p_comments.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
    return p_comments[index];
  };

  /**
   * Checks if the value needs to be converted to PVL or iPVL and returns it in
   * the correct format.
   * @param value The value to be converted.
   * @return <B>string</B> The value in its proper format (iPVL or
   *         PVL).
   */
  string PvlKeyword::Reform(const std::string &value) const {
#if 0
    static bool firstTime = true;
    static bool iPVL = true;
    if(firstTime) {
      firstTime = false;
      Isis::PvlGroup &g = Isis::Preference::Preferences().FindGroup(
                            "UserInterface", Isis::Pvl::Traverse);

      Isis::iString s = (string) g["PvlFormat"];
      s.UpCase();
      if(s == "PVL") iPVL = false;
    }

    if(iPVL) return ToIPvl(value);
#endif
    return ToPvl(value);
  }

  /**
   * Converts a value to iPVL format.
   * @param value The value to be converted.
   * @return <B>string</B> The value in iPVL format.
   */
  string PvlKeyword::ToIPvl(const std::string &value) const {
    string out;
    bool upcase = true;
    bool lastlower = true;
    for(unsigned int i = 0; i < value.size(); i++) {
      if((lastlower) && (isupper(value[i]))) upcase = true;
      if(value[i] == '_') {
        upcase = true;
      }
      else if(upcase) {
        out += toupper(value[i]);
        lastlower = false;
        upcase = false;
      }
      else {
        out += tolower(value[i]);
        if(islower(value[i])) lastlower = true;
        upcase = false;
      }
    }
    return out;
  }

  /**
   * Converts a value to PVL format.
   * @param value The value to be converted.
   * @return <B>string</B> The value in PVL format.
   */
  string PvlKeyword::ToPvl(const std::string &value) const {
    string out;
    bool lastlower = false;
    for(unsigned int i = 0; i < value.size(); i++) {
      if((lastlower) && (isupper(value[i]))) out += "_";
      if(value[i] == '_') {
        out += "_";
        lastlower = false;
      }
      else {
        out += toupper(value[i]);
        if(islower(value[i])) lastlower = true;
      }
    }
    return out;
  }

  /**
   * Checks to see if two strings are equal. Each is converted to uppercase
   * and removed of underscores and whitespaces.
   * @param string1 The first string
   * @param string2 The second string
   * @return <B>bool</B> True or false, depending on whether
   *          the string values are equal.
   */
  bool PvlKeyword::StringEqual(const std::string &string1,
                               const std::string &string2) {
    Isis::iString s1(string1);
    Isis::iString s2(string2);

    s1.ConvertWhiteSpace();
    s2.ConvertWhiteSpace();

    s1.Remove(" _");
    s2.Remove(" _");

    s1.UpCase();
    s2.UpCase();

    if(s1 == s2) return true;
    return false;
  }

  /**
   * Checks to see if a value with a specified index is equivalent to another
   * string.
   * @param string1 The string to compare the value to.
   * @param index The index of the existing value.
   * @return <B>bool</B> True if the two strings are equivalent,
   *         false if they're not.
   * @throws iException ArraySubscriptNotInRange (index) Index out of bounds.
   */
  bool PvlKeyword::IsEquivalent(const std::string &string1, int index) const {
    if(index < 0 || index >= (int)p_values.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange(index);
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }

    return StringEqual(p_values[index], string1);
  }

  /**
   * Add values and units from a PvlSequence. (Clears current values and units)
   * @param seq The PvlSequence to add from.
   * @return <B>PvlKeyword&</B> Reference to PvlKeyword object.
   */
  PvlKeyword &PvlKeyword::operator=(Isis::PvlSequence &seq) {
    this->Clear();
    for(int i = 0; i < seq.Size(); i++) {
      string temp = "(";
      for(int j = 0; j < (int)seq[i].size(); j++) {
        string val = seq[i][j];
        if(val.find(" ") != std::string::npos) {
          temp += "\"" + val + "\"";
        }
        else {
          temp += val;
        }
        if(j < (int) seq[i].size() - 1) temp += ", ";
      }
      temp += ")";
      this->operator+=(temp);
    }

    return *this;
  }

  /**
   * Wraps output so that length doesn't exceed the character
   * limit.  By default, the character limit is set to 80, and can
   * be changed with the method SetCharLimit.  Used as a helper
   * method for output of PvlKeyword.
   *
   * @param os Designated output stream
   * @param textToWrite The text to be written
   * @param startColumn The starting column after the "=" sign.
   * @param endOfLine The EOL character
   *
   * @return <B>ostream&</B> Reference to ostream.
   * @see operator<<
   */
  ostream &PvlKeyword::WriteWithWrap(std::ostream &os,
                                     const std::string &textToWrite,
                                     int startColumn,
                                     PvlFormat &format) const {

    /*
    http://pds.jpl.nasa.gov/tools/standards-reference.shtml

    pds.jpl.nasa.gov/documents/sr/Chapter12.pdf

    Object Description Language Specification and Usage
    The following provides a complete specification for Object Description Language
    (ODL), the language used to encode data labels for the Planetary Data System
    (PDS) and other NASA data systems. This standard contains a formal definition of
    the grammar semantics of the language. PDS specific implementation notes and
    standards are referenced in separate sections.

    12.5.3.1 Implementation of String Values
    A text string read in from a label is reassembled into a string of characters.
    The way in which the string is broken into lines in a label does not affect the
    format of the string after it has been reassembled. The following rules are used
              when reading text strings: If a format effector or a sequence of
              format effectors is encountered within a text string,
              the effector (or sequence of effectors) is replaced by a single space
              character, unless the last character is a hyphen (dash) character. Any
              spacing characters at the end of the line are removed and any spacing
              characters at the beginning of the following line are removed. This
              allows a text string in a label to appear with the left and right
              margins set at arbitrary points without changing the string value. For
                       example, the following two strings are the same: "To be or
                       not to be" and
                       "To be or
                       not to be"
              If the last character on a line prior to a format effector is a hyphen
              (dash) character, the hyphen is removed with any spacing characters at
              the beginning of the following line. This follows the standard
              convention in English of using a hyphen to break a word across lines.
              For example, the following two strings are the same:
                        "The planet Jupiter is very big" and
                       "The planet Jupi-
                       ter is very big"
              Control codes, other than the horizontal tabulation character and
              format effectors, appearing within a text string are removed.
        */

    /*
      We will be adding a condition for human-readable purposes:
        If a quoted string of text does not fit on the current line,
        but will fit on the next line, use the next line.
    */

    // Position set
    string remainingText = textToWrite;
    int spaceForText = format.CharLimit() - 1 - format.FormatEOL().length() - startColumn;

    // find quote positions to better determine which line to put the
    //  string on. Data structure: vector< startPos, endPos > where
    //  remainingText[startPos] and remainingText[endPos] must both be quotes.
    vector< pair<int, int> > quotedAreas;
    char endQuoteChar = '\0';
    int  quoteStart = -1;

    // if its an array, indent subsequent lines 1 more
    if(textToWrite[0] == '(' || textToWrite[0] == '"') {
      startColumn ++;
    }

    /* Standard 12.3.3.1 ->
      A quoted text string may not contain the quotation mark, which is reserved
      to be the text string delimiter.

      So we don't have to worry about escaped quotes.
    */

    vector< pair<char, char> > quoteStartEnds;
    quoteStartEnds.push_back(pair<char, char>('"', '"'));
    quoteStartEnds.push_back(pair<char, char>('\'', '\''));
    quoteStartEnds.push_back(pair<char, char>('<', '>'));

    // clean up any EOL characters, they mustn't interfere, remove sections of
    //   multiple spaces (make them into one), and find quoted areas
    for(unsigned int pos = 0; pos < remainingText.size(); pos++) {
      // remove \r and \n from string
      if(remainingText[pos] == '\n' || remainingText[pos] == '\r') {
        if(pos != remainingText.length() - 1) {
          remainingText = remainingText.substr(0, pos) +
                          remainingText.substr(pos + 1);
        }
        else {
          remainingText = remainingText.substr(0, pos);
        }
      }

      // convert "      " to " " if not quoted
      if(quoteStart == -1) {
        while(pos > 0 &&
              remainingText[pos-1] == ' ' &&
              remainingText[pos] == ' ') {
          remainingText = remainingText.substr(0, pos) +
                          remainingText.substr(pos + 1);
        }
      }

      // Find quotes
      for(unsigned int i = 0;
          (quoteStart < 0) && i < quoteStartEnds.size();
          i++) {
        if(quoteStartEnds[i].first == remainingText[pos]) {
          endQuoteChar = quoteStartEnds[i].second;
          quoteStart = pos;
        }
      }


      //bool mismatchQuote = false;

      // Check to see if we're ending a quote if we didn't just
      //   start the quote and we are inside a quote
      if(quoteStart != (int)pos && quoteStart != -1) {
        for(unsigned int i = 0; i < quoteStartEnds.size(); i++) {
          if(quoteStartEnds[i].second == remainingText[pos]) {
            if(quoteStartEnds[i].first != remainingText[quoteStart]) {
              continue;
              //  mismatchQuote = true;
            }

            quotedAreas.push_back(pair<int, int>(quoteStart, pos));

            quoteStart = -1;
            endQuoteChar = '\0';
          }
        }
      }

      //if(mismatchQuote) {
      //  iString msg = "Pvl keyword values [" + textToWrite +
      //    "] can not have embedded quotes";
      //  throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      //}
    }

    int charsLeft = spaceForText;
    int printedSoFar = 0;

    // while we have something to write, keep going
    while(!remainingText.empty()) {
      // search backwards for the last space or comma *in the limit* (80 chars)
      int lastSpacePosition = charsLeft;

      // if everything fits into our remaining space, consider the last
      //   spot in the string to be printed still the split position.
      if(lastSpacePosition >= (int)remainingText.length()) {
        lastSpacePosition = remainingText.length();
      }
      else {
        // Everything does not fit; use good space for mediocre splits (inside
        //  quoted strings), and excellent space for good splits (between array
        //  values for example)
        int goodSpace = -1;
        int excellentSpace = -1;
        int searchPosition = lastSpacePosition;
        bool doneSearching = false;

        while(!doneSearching) {
          bool currentPosQuoted = false;

          for(unsigned int i = 0; i < quotedAreas.size(); i++) {
            if(searchPosition + printedSoFar >= quotedAreas[i].first &&
                searchPosition + printedSoFar <= quotedAreas[i].second) {
              currentPosQuoted = true;
            }
          }

          if(remainingText[searchPosition] == ' ') {
            bool validSpace = true;

            // this really isn't a good space if the previous character is a
            // '-' though - then it would be read wrong when re-imported.
            if(searchPosition > 0 && remainingText[searchPosition - 1] == '-') {
              validSpace = false;
            }

            if(validSpace && goodSpace < 0) {
              goodSpace = searchPosition;
            }

            // An excellent space is the prefential break - not quoted and
            //   not units next.
            // we were already done if we had an excellent space
            if(validSpace && !currentPosQuoted) {
              if((unsigned)searchPosition < remainingText.size() - 1 &&
                  remainingText[searchPosition+1] != '<') {
                excellentSpace = searchPosition;
              }
            }
          }

          doneSearching = (excellentSpace >= 0 || searchPosition <= 1);
          searchPosition --;
        }

        // Use the best breaking point we have
        if(excellentSpace > 0) {
          lastSpacePosition = excellentSpace;
        }
        else if(goodSpace > 0) {
          lastSpacePosition = goodSpace;
        }
        else {
          lastSpacePosition = -1;
        }
      }

      // we found a space or comma in our limit, write to that chatacter
      //   and repeat the loop
      if(lastSpacePosition >= 0) {
        os << remainingText.substr(0, lastSpacePosition);

        remainingText = remainingText.substr(lastSpacePosition);
        printedSoFar += lastSpacePosition;
      }
      // we failed to find a space or a comma in our limit,
      //   use a hyphen (-)
      else {
        os << remainingText.substr(0, charsLeft - 1);
        os << "-";
        remainingText = remainingText.substr(charsLeft - 1);
        printedSoFar += charsLeft - 1;
      }

      // we wrote as much as possible, do a newline and repeat
      if(!remainingText.empty()) {
        os << format.FormatEOL();
        WriteSpaces(os, startColumn);

        // dont allow spaces to begin the next line inside what we're printing
        if(remainingText[0] == ' ') {
          remainingText = remainingText.substr(1);
          printedSoFar += 1;
        }
      }

      charsLeft = spaceForText;
    }

    return os;
  }


  /**
   * This writes numSpaces spaces to the ostream.
   *
   * @param os Stream to write to
   * @param numSpaces number of spaces to write
   */
  void PvlKeyword::WriteSpaces(std::ostream &os, int numSpaces) const {
    for(int space = 0; space < numSpaces; space ++) {
      os << " ";
    }
  }


  /**
   *
   * Set the PvlFormatter used to format the keyword name and value(s)
   *
   * @param formatter A pointer to the formatter to be used
   */
  void PvlKeyword::SetFormat(PvlFormat *formatter) {
    p_formatter = formatter;
  }


  /**
   *
   * Get the current PvlFormat or create one
   * @return <B>PvlFormat*</B> Pointer to PvlFormat.
   *
   */
  PvlFormat *PvlKeyword::GetFormat() {
    return p_formatter;
  };

  /**
   * Read in a keyword
   *
   * http://pds.jpl.nasa.gov/tools/standards-reference.shtml
   *
   * @param is The input stream
   * @param result The keyword to read into (OUTPUT)
   *
   */
  std::istream &operator>>(std::istream &is, PvlKeyword &result) {
    result = PvlKeyword();
    string line;
    iString keywordString;

    bool keywordDone = false;
    bool multiLineComment = false;
    bool error = !is.good();

    while(!error && !keywordDone) {
      istream::pos_type beforeLine = is.tellg();

      line = PvlKeyword::ReadLine(is, multiLineComment);

      // We read an empty line (failed to read next non-empty line)
      // and didnt complete our keyword, essentially we hit the implicit
      // keyword named "End"
      if(line.empty() && !is.good()) {
        if(keywordString.empty() ||
            keywordString[keywordString.size()-1] == '\n') {
          line = "End";

          if(multiLineComment) {
            error = true;
          }
        }
        else {
          error = true;
        }
      }

      bool comment = false;

      if(!multiLineComment) {
        if(line.size() > 0 && line[0] == '#') {
          comment = true;
        }

        if(line.size() > 1 && line[0] == '/' &&
            (line[1] == '*' || line[1] == '/')) {
          comment = true;

          if(line[1] == '*') {
            multiLineComment = true;
            keywordString += line.substr(0, 2);
            line = iString(line.substr(2)).Trim(" \r\n\t");
          }
        }
      }

      if(multiLineComment) {
        comment = true;

        if(line.find("/*") != string::npos) {
          iString msg = "Cannot have ['/*'] inside a multi-line comment";
          throw iException::Message(iException::Pvl, msg, _FILEINFO_);
        }

        if(line.find("*/") != string::npos) {
          multiLineComment = false;

          line = iString(
                   line.substr(0, line.find("*/"))
                 ).Trim(" \r\n\t") + " */";
        }
      }

      if(line.empty()) {
        continue;
      }
      // comment line
      else if(comment) {
        keywordString += line + '\n';
        continue;
      }
      // first line of keyword data
      else if(keywordString.empty()) {
        keywordString = line;
      }
      // concatenation
      else if(!comment && keywordString[keywordString.size()-1] == '-') {
        keywordString = keywordString.substr(0, keywordString.size() - 1) + line;
      }
      // Non-commented and non-concatenation -> put in the space
      else {
        keywordString += " " + line;
      }
      // if this line concatenates with the next, read the next
      if(line[line.size()-1] == '-') {
        continue;
      }

      std::vector< std::string > keywordComments;
      std::string keywordName;
      std::vector< std::pair<std::string, std::string> > keywordValues;

      bool attemptedRead = false;

      try {
        attemptedRead = PvlKeyword::ReadCleanKeyword(keywordString,
                        keywordComments,
                        keywordName,
                        keywordValues);
      }
      catch(iException &e) {
        if(is.eof() && !is.bad()) {
          is.clear();
          is.unget();
        }

        is.seekg(beforeLine, ios::beg);

        string msg = "Unable to read keyword [";
        msg += keywordString;
        msg += "]";

        throw iException::Message(iException::Pvl, msg, _FILEINFO_);
      }

      // Result valid?
      if(attemptedRead) {
        // if the next line starts with '<' then it should be read too...
        // it should be units
        // however, you can't have units if there is no value
        if(is.good() && is.peek() == '<' && !keywordValues.empty()) {
          continue;
        }

        result.SetName(keywordName);
        result.AddComments(keywordComments);

        for(unsigned int value = 0; value < keywordValues.size(); value++) {
          result.AddValue(keywordValues[value].first,
                          keywordValues[value].second);
        }

        keywordDone = true;
      }

      if(!attemptedRead) {
        error = error || !is.good();
      }
      // else we need to keep reading
    }

    if(error) {
      // skip comments
      while(keywordString.find('\n') != string::npos) {
        keywordString = keywordString.substr(keywordString.find('\n') + 1);
      }

      string msg;

      if(keywordString.empty() && !multiLineComment) {
        msg = "Input contains no Pvl Keywords";
      }
      else if(multiLineComment) {
        msg = "Input ends while still in a multi-line comment";
      }
      else {
        msg = "The keyword [" + keywordString + "] does not appear to be";
        msg += " a valid Pvl Keyword";
      }

      throw iException::Message(iException::Pvl, msg, _FILEINFO_);
    }

    if(!keywordDone) {
      // skip comments
      while(keywordString.find('\n') != string::npos)
        keywordString = keywordString.substr(keywordString.find('\n') + 1);

      string msg;

      if(keywordString.empty()) {
        msg = "Error reading keyword";
      }
      else {
        msg = "The keyword [" + keywordString + "] does not appear to be";
        msg += " complete";
      }

      throw iException::Message(iException::Pvl, msg, _FILEINFO_);
    }

    return is;
  }

  /**
   * This method adds multiple comments at once by calling AddComments on each
   * element in the vector.
   *
   * @param comments Comments to associate with this keyword
   */
  void PvlKeyword::AddComments(const std::vector<std::string> &comments) {
    for(unsigned int i = 0; i < comments.size(); i++) {
      AddComment(comments[i]);
    }
  }

  /**
   * This reads a keyword compressed back to 1 line of data (excluding comments,
   * which are included on separate lines of data before the keyword). Line
   * concatenations must have already been handled. This returns the data of the
   * keyword (if valid) and its status.
   *
   * @param keyword Pvl "#COMMENT\n//COMMENT\nKeyword = (Value1,Value2,...)"
   *                string
   * @param keywordComments Output: Lines of data that are comments
   * @param keywordName Output: Name of keyword
   * @param keywordValues Output: vector< pair<Value, Units> >
   *
   * @return bool false if it is invalid but could become valid given more data,
   *              true if it is a valid keyword and successful
   */
  bool PvlKeyword::ReadCleanKeyword(std::string keyword,
                                    std::vector< std::string > &keywordComments,
                                    std::string &keywordName,
                                    std::vector< std::pair<std::string, std::string> > &keywordValues) {
    // Reset outputs
    keywordComments.clear();
    keywordName = "";
    keywordValues.clear();

    // This is in case a close quote doesn't exist
    bool explicitIncomplete = false;

    // Possible (known) comment starts in pvl
    iString comments[] = {
      "#",
      "//"
    };

    // Need more data if nothing is here!
    if(keyword.empty()) return 0;

    /*
      Step 1: Read Comments

      Theoretically, we should have an input that looks like this:
        #Comment
        //Comment
        / * Comment
            Comment * /
        Keyword = Data

      So we could just grab all of the first lines; however, this method
      needs to be as error-proof as possible (it is the basis of reading
      all PVLs after all), so verify we have things that look like comments
      first, strip them & store them.
     */

    // While we have newlines, we have comments
    while(keyword.find("\n") != string::npos) {
      // Make sure we strip data every loop of comment types; otherwise we
      // have no comment and need to error out.
      bool noneStripped = true;

      // Check every comment type now and make sure this line (it isn't the last
      // line since a newline exists) starts in a comment
      string keywordStart = keyword.substr(0, 2);

      // Handle multi-line comments
      if(keywordStart == "/*") {
        noneStripped = false;
        bool inComment = true;

        while(inComment && keyword.find("*/") != string::npos) {
          // Correct the */ to make sure it has a \n after it,
          // without this we risk an infinite loop
          size_t closePos = keyword.find("*/\n");

          if(closePos == string::npos) {
            closePos = keyword.find("*/") + 2;
            keyword = keyword.substr(0, closePos) + "\n" +
                      keyword.substr(closePos);
          }

          string comment = keyword.substr(0, keyword.find("\n"));
          comment = iString(comment).Trim(" \r\n\t");

          // Set these to true if too small, false if not (they need if
          //   cant currently fit).
          bool needsStart = (comment.size() < 2);
          bool needsStartSpace = comment.size() < 3;

          int commentEndPos = comment.size() - 2;
          bool needsEnd = (commentEndPos < 0);
          bool needsEndSpace = comment.size() < 3;

          // Needs are currently set based on string size, apply real logic
          //   to test for character sequences (try to convert them from false
          //   to true).
          if(!needsStart) {
            needsStart = (comment.substr(0, 2) != "/*");
          }

          if(!needsEnd) {
            needsEnd = (comment.substr(commentEndPos, 2) != "*/");
          }

          if(!needsStartSpace) {
            needsStartSpace = (comment.substr(0, 3) != "/* ");
          }

          if(!needsEndSpace) {
            needsEndSpace = (comment.substr(commentEndPos - 1, 3) != " */");
          }

          if(needsStart) {
            comment = "/* " + comment;
          }
          else if(needsStartSpace) {
            comment = "/* " + comment.substr(2);
          }

          if(needsEnd) {
            comment = comment + " */";
          }
          else if(needsEndSpace) {
            comment = comment.substr(0, comment.size() - 2) + " */";;
          }

          inComment = needsEnd;

          keywordComments.push_back(comment);

          if(keyword.find("\n") != string::npos) {
            keyword = iString(
                        keyword.substr(keyword.find("\n") + 1)
                      ).Trim(" \r\n\t");
          }

          // Check for another comment start
          if(!inComment) {
            if(keyword.size() >= 2)
              keywordStart = keyword.substr(0, 2);

            inComment = (keywordStart == "/*");
          }
        }

        // So we have a bunch of multi-line commands... make them the same size
        //   Find longest
        unsigned int longest = 0;
        for(unsigned int index = 0; index < keywordComments.size(); index++) {
          iString comment = keywordComments[index];

          if(comment.size() > longest)
            longest = comment.size();
        }

        // Now make all the sizes match longest
        for(unsigned int index = 0; index < keywordComments.size(); index++) {
          iString comment = keywordComments[index];

          while(comment.size() < longest) {
            // This adds a space to the end of the comment
            comment = comment.substr(0, comment.size() - 2) + " */";
          }

          keywordComments[index] = comment;
        }
        // They should all be the same length now
      }

      // Search for single line comments
      for(unsigned int commentType = 0;
          commentType < sizeof(comments) / sizeof(iString);
          commentType++) {

        if(keywordStart.find(comments[commentType]) == 0) {
          // Found a comment start; strip this line out and store it as a
          // comment!
          string comment = keyword.substr(0, keyword.find("\n"));
          keywordComments.push_back(iString(comment).Trim(" \r\n\t"));

          noneStripped = false;

          if(keyword.find("\n") != string::npos) {
            keyword = iString(
                        keyword.substr(keyword.find("\n") + 1)
                      ).Trim(" \r\n\t");
          }
        }
      }

      // Does it look like Name=Value/*comm
      //                              mment*/ ?
      if(noneStripped && keyword.find("/*") != string::npos &&
          keyword.find("*/") != string::npos) {
        iString firstPart = keyword.substr(0, keyword.find("\n"));
        iString lastPart = keyword.substr(keyword.find("\n") + 1);

        keyword = firstPart.Trim(" \r\n\t") + " " + lastPart.Trim(" \r\n\t");
        noneStripped = false;
      }

      if(noneStripped) {
        string msg = "Expected a comment but found [";
        msg += keyword;
        msg += "]";
        throw iException::Message(iException::Pvl, msg, _FILEINFO_);
      }
    }

    // Do we have a keyword at all?
    if(keyword.empty()) {
      return false; // need more data
    }

    /*
      Step 2: Determine Keyword Format

      Make sure we have a keyword after the comments first. We expect
      one of three formats:
        KEYWORD                             PROCESSED IN STEP 3.1
        KEYWORD = (VALUE,VALUE,...)         PROCESSED IN STEP 3.2
        KEYWORD = VALUE                     PROCESSED IN STEP 3.3
     */

    // Get the keyword name
    keywordName = ReadValue(keyword, explicitIncomplete);

    // we have taken the name; if nothing remains then it is value-less
    // and we are done.
    if(keyword.empty()) {
      /*
        Step 3.1

        Format is determined to be:
          KEYWORD

        Well, no value/units may exist so we're done processing this keyword.
       */
      return 1; // Valid & Successful
    }

    // if we don't have an equal, then we have a problem - an invalid symbol.
    // Our possible remaining formats both are KEYWORD = ...
    if(keyword[0] != '=') {
      string msg = "Expected an assignment operator [=], but found [";
      msg += keyword[0];
      msg += "]";

      throw iException::Message(iException::Pvl, msg, _FILEINFO_);
    }

    keyword = iString(keyword.substr(1)).Trim(" \t");

    if(keyword.empty()) {
      return false;
    }

    // now we need to split into two possibilities: array or non-array
    if(keyword[0] == '(' || keyword[0] == '{') {
      /*
        Step 3.2

        Our format is confirmed as:
          KEYWORD = (...)

        We need to read each value/unit in the array.
      */

      char closingParen = ((keyword[0] == '(') ? ')' : '}');
      char wrongClosingParen = ((keyword[0] == '(') ? '}' : ')');
      bool closedProperly = false;

      vector< pair<char, char> > extraDelims;
      extraDelims.push_back(pair<char, char>('(', ')'));
      extraDelims.push_back(pair<char, char>('{', '}'));

      // strip '(' - onetime, this makes every element in the array the same
      // (except the last)
      keyword = iString(keyword.substr(1)).Trim(" \t");

      // handle empty arrays: KEYWORD = ()
      if(!keyword.empty() && keyword[0] == closingParen) {
        closedProperly = true;
      }

      // Each iteration of this loop should consume 1 value in the array,
      // including the comma, i.e. we should start out with:
      //  'VALUE,VALUE,...)' until we hit ')' (our exit condition)
      while(!keyword.empty() && keyword[0] != closingParen) {
        // foundComma delimits the end of this element in the array (remains
        //  false for last value which has no comma at the end)
        bool foundComma = false;
        // keyword should be of the format: VALUE <UNIT>,....)
        // Read VALUE from it
        string nextItem = ReadValue(keyword, explicitIncomplete, extraDelims);

        if(!keyword.empty() && keyword[0] == wrongClosingParen) {

          string msg = "Incorrect array close; expected [";
          msg += closingParen;
          msg += "] but found [";
          msg += wrongClosingParen;
          msg += "] in keyword named [";
          msg += keywordName;
          msg += "]";
          throw iException::Message(iException::Pvl, msg, _FILEINFO_);
        }

        // This contains <VALUE, UNIT>
        pair<string, string> keywordValue;

        // Store VALUE
        keywordValue.first = nextItem;

        // Now there's 2 possibilities: units or no units ->
        //   if we have units, read them
        if(!keyword.empty() && keyword[0] == '<') {
          string unitsValue = ReadValue(keyword, explicitIncomplete);
          keywordValue.second = unitsValue;
        }

        // Now we should* have a comma, strip it
        if(!keyword.empty() && keyword[0] == ',') {
          foundComma = true;
          keyword = iString(keyword.substr(1)).Trim(" \t");
        }

        // No comma and nothing more in string - we found
        //  KEYWORD = (VALUE,VALUE\0
        //  we need more information to finish this keyword
        if(!foundComma && keyword.empty()) {
          return false; // could become valid later
        }

        bool foundCloseParen = (!keyword.empty() && keyword[0] == closingParen);

        if(foundCloseParen) {
          closedProperly = true;
        }

        // Check for the condition of:
        //  keyword = (VALUE,VALUE,)
        // which is unrecoverable
        if(foundComma && foundCloseParen) {
          string msg = "Unexpected close of keyword-value array";
          throw iException::Message(iException::Pvl, msg, _FILEINFO_);
        }

        // Check for (VALUE VALUE
        if(!foundComma && !foundCloseParen) {
          // We have ("VALUE VALUE
          if(explicitIncomplete) return false;

          // We have (VALUE VALUE
          string msg = "Found extra data after [";
          msg += nextItem;
          msg += "] in array";
          throw iException::Message(iException::Pvl, msg, _FILEINFO_);
        }

        // we're good with this element of the array, remember it
        keywordValues.push_back(keywordValue);
      }

      if(!closedProperly) {
        return false;
      }

      // Trim off the closing paren
      if(!keyword.empty()) {
        keyword = iString(keyword.substr(1)).Trim(" \t");
      }

      // Read units here if they exist and apply them
      //  case: (A,B,C) <unit>
      if(!keyword.empty() && keyword[0] == '<') {
        string units = ReadValue(keyword, explicitIncomplete);
        for(unsigned int val = 0; val < keywordValues.size(); val++) {
          if(keywordValues[val].second.empty()) {
            keywordValues[val].second = units;
          }
        }
      }
    }
    else {
      /*
        Step 3.3

        Our format is confirmed as:
          "KEYWORD = VALUE <UNIT>"

        We need to read the single value/unit in the keywordValues array.
       */
      pair<string, string> keywordValue;
      keywordValue.first = ReadValue(keyword, explicitIncomplete);

      if(!keyword.empty() && keyword[0] == '<') {
        keywordValue.second = ReadValue(keyword, explicitIncomplete);
      }

      keywordValues.push_back(keywordValue);
    }

    /*
      This is set when a quote is opened somewhere and never closed, it means
      we need more information
     */
    if(explicitIncomplete) {
      return false; // unclosed quote at end... need more information
    }

    /*
      See if we have a comment at the end of the keyword...
     */
    if(!keyword.empty()) {
      // if the data left is a comment, we can handle it probably
      if(keyword[0] == '#' ||
          ((keyword.size() > 1 && keyword[0] == '/') &&
           (keyword[1] == '/' || keyword[1] == '*'))) {
        keywordComments.push_back(keyword);

        if(keyword.size() > 1 && keyword.substr(0, 2) == "/*") {
          if(keyword.substr(keyword.size() - 2, 2) != "*/")
            return false; // need more comment data
        }

        keyword = "";
      }
    }

    /*
      If more data remains, it is unrecognized.
    */
    if(!keyword.empty()) {
      string msg = "Keyword has extraneous data [";
      msg += keyword;
      msg += "] at the end";
      throw iException::Message(iException::Pvl, msg, _FILEINFO_);
    }

    // We've parsed this keyword! :)
    return true;
  }


  std::string PvlKeyword::ReadValue(std::string &keyword, bool &quoteProblem) {
    std::vector< std::pair<char, char> > otherDelims;

    return ReadValue(keyword, quoteProblem, otherDelims);
  }

  /**
   * This method looks for a data element in the string. A data element is a
   * quoted string, a units value, or one value of an array (not including
   * units). As an example, each value in the following string is quoted:
   *
   * 'VALUE' '=' ('VALUE','VALUE',  'VALUE' '<VALUE>')
   *
   *  The returned values of each of these elements is VALUE. Explicitly defined
   *  quotes (', ", <>) are stripped from the return value.
   *
   * @param keyword Input/Output: The keyword to get the next value from
   *                (DESTRUCTIVE)
   * @param quoteProblem Output: The string has an unclosed quote character
   *
   * @return std::string The stripped out token.
   */
  std::string PvlKeyword::ReadValue(std::string &keyword, bool &quoteProblem,
                                    const std::vector< std::pair<char, char> > &
                                    otherDelimiters) {
    string value = "";

    // This method ignores spaces except as delimiters; let's trim the string
    // to start
    keyword = iString(keyword).Trim(" \t");

    if(keyword.empty()) {
      return "";
    }

    // An implied quote is one that is started without a special character, for
    //  example HELLO WORLD has HELLO and WORLD separately as implied quotes for
    //  PVLs. However, "HELLO WORLD" has an explicit (not implied) double quote.
    //  We do consider <> as quotes.
    bool impliedQuote = true;
    char quoteEnd = ' ';
    bool keepQuotes = false;
    size_t currentDelimPos = string::npos;

    if(keyword[0] == '\'' || keyword[0] == '"') {
      quoteEnd = keyword[0];
      impliedQuote = false;
      currentDelimPos = 0;
    }
    else if(keyword[0] == '<') {
      quoteEnd = '>';
      impliedQuote = false;
      currentDelimPos = 0;
    }
    else {
      // we're not quoted, look for alternative delimiters.
      char implicitQuotes[] = {
        ')',
        '}',
        ',',
        ' ',
        '\t',
        '<',
        '='
      };

      bool foundImplicitQuote = false;

      unsigned int currentPos = 0;
      while(!foundImplicitQuote && currentPos != keyword.size()) {
        for(unsigned int quote = 0;
            quote < sizeof(implicitQuotes) / sizeof(char);
            quote ++) {
          if(keyword[currentPos] == implicitQuotes[quote]) {
            currentDelimPos = currentPos;
            quoteEnd = implicitQuotes[quote];
            foundImplicitQuote = true;
          }
        }

        if(!foundImplicitQuote) {
          currentPos ++;
        }
      }
    }

    for(unsigned int delim = 0; delim < otherDelimiters.size(); delim ++) {
      if(keyword[0] == otherDelimiters[delim].first) {
        currentDelimPos = 0;
        quoteEnd = otherDelimiters[delim].second;
        keepQuotes = true;
        impliedQuote = false;
      }
    }

    string startQuote;
    // non-implied delimeters need the opening delimiter ignored. Remember
    //  startQuote in case of error later on we can reconstruct the original
    //  string.
    if(!impliedQuote) {
      startQuote += keyword[0];
      keyword = keyword.substr(1);
    }

    // Do we have a known quote end?
    size_t quoteEndPos = keyword.find(quoteEnd);

    if(quoteEndPos != string::npos) {
      value = keyword.substr(0, quoteEndPos);

      // Trim keyword 1 past end delimiter (if end delimiter is last, this
      // results in empty string). If the close delimiter is ')' or ',' then
      // leave it be however, since we need to preserve that to denote this was
      //  an end of a valuein array keyword.
      if(!impliedQuote) {
        keyword = keyword.substr(quoteEndPos + 1);
      }
      else {
        keyword = keyword.substr(quoteEndPos);
      }

      // Make sure we dont have padding
      keyword = iString(keyword).Trim(" \t");

      if(keepQuotes) {
        value = startQuote + value + quoteEnd;
      }

      return value;
    }
    // implied quotes terminate at end of keyword; otherwise we have a problem
    //   (which is this condition)
    else if(!impliedQuote) {
      // restore the original string
      keyword = startQuote + keyword;
      quoteProblem = true;

      return "";
    }

    // we have an implied quote but no quote character, the rest must be the
    //  value.
    value = keyword;
    keyword = "";

    return value;
  }


  /**
   * This method reads one line of data from the input stream.
   *
   * All spaces, newlines, returns and tabs are trimmed from the result.
   * Once a newline is encountered, if the line we read is blank, we keep
   * reading. Once a line with data is encountered, that is the result. All
   * newlines, spaces, returns and tabs are consumed past this line of data
   * until the next different character (seeks to next valid data).
   *
   * @param is The stream to read from
   *
   * @return std::string The first encountered line of data
   */
  std::string PvlKeyword::ReadLine(std::istream &is, bool insideComment) {
    iString lineOfData;

    while(is.good() && lineOfData.empty()) {

      // read until \n (works for both \r\n and \n) or */
      while(is.good() &&
            (!lineOfData.size() || lineOfData[lineOfData.size() - 1] != '\n')) {
        char next = is.get();

        // if non-ascii found then we're done... immediately
        if(next <= 0) {
          is.seekg(0, ios::end);
          is.get();
          return lineOfData;
        }

        // if any errors (i.e. eof) happen in the get operation then don't
        //   store this data
        if(is.good()) {
          lineOfData += next;
        }

        if(insideComment &&
            lineOfData.size() >= 2 && lineOfData[lineOfData.size() - 2] == '*' &&
            lineOfData[lineOfData.size() - 1] == '/') {
          // End of multi-line comment = end of line!
          break;
        }
        else if(lineOfData.size() >= 2 &&
                lineOfData[lineOfData.size() - 2] == '/' &&
                lineOfData[lineOfData.size() - 1] == '*') {
          insideComment = true;
        }
      }

      // Trim off non-visible characters from this line of data
      lineOfData = lineOfData.Trim(" \r\n\t");

      // read up to next non-whitespace in input stream
      while(is.good() &&
            (is.peek() == ' ' ||
             is.peek() == '\r' ||
             is.peek() == '\n')) {
        is.get();
      }

      // if lineOfData is empty (line was empty), we repeat
    }

    return lineOfData;
  }


  /**
   * Write out the keyword.
   *
   * @param os The output stream.
   * @param keyword The PvlKeyword object to output.
   * @return <B>ostream&</B> Reference to ostream.
   * @see WriteWithWrap()
   */
  ostream &operator<<(std::ostream &os, const Isis::PvlKeyword &keyword) {
    // Set up a Formatter
    PvlFormat *tempFormat = keyword.p_formatter;
    bool removeFormatter = false;
    if(tempFormat == NULL) {
      tempFormat = new PvlFormat();
      removeFormatter = true;
    }

    // Write out the comments
    for(int i = 0; i < keyword.Comments(); i++) {
      for(int j = 0; j < keyword.Indent(); j++) os << " ";
      os << keyword.Comment(i) << tempFormat->FormatEOL();
    }

    // Write the keyword name & add length to startColumn.
    int startColumn = 0;
    for(int i = 0; i < keyword.Indent(); i++) {
      os << " ";
      ++startColumn;
    }
    string keyname = tempFormat->FormatName(keyword);
    os << keyname;
    startColumn += keyname.length();

    // Add padding and then write equal sign.
    for(int i = 0; i < keyword.Width() - (int)keyname.size(); ++i) {
      os << " ";
      ++startColumn;
    }
    os << " = ";
    startColumn += 3;

    // If it has no value then write a NULL
    if(keyword.Size() == 0) {
      os << tempFormat->FormatValue(keyword);
    }

    // Loop and write each array value
    string stringToWrite;
    for(int i = 0; i < keyword.Size(); i ++) {
      stringToWrite += tempFormat->FormatValue(keyword, i);
    }

    keyword.WriteWithWrap(os,
                          stringToWrite,
                          startColumn,
                          *tempFormat);

    if(removeFormatter) delete tempFormat;

    return os;
  }


  //! This is an assignment operator
  const PvlKeyword &PvlKeyword::operator=(const PvlKeyword &other) {
    p_formatter = other.p_formatter;
    p_name = other.p_name;
    p_values = other.p_values;
    p_units = other.p_units;
    p_comments = other.p_comments;
    p_width = other.p_width;
    p_indent = other.p_indent;

    return *this;
  }

  /**
   * Validate a Keyword, comparing against corresponding Template Keyword.
   *  
   * Template Keyword has the format: 
   * keyName = (valueType, optional/required, Values allowed separated by comma)  
   * 
   * @author Sharmila Prasad (9/22/2010)
   * 
   * @param pvlKwrd - Keyword to be validated
   */
  void PvlKeyword::ValidateKeyword(PvlKeyword & pvlKwrd)
  {
    int iTmplKwrdSize = Size();
    int iSize = pvlKwrd.Size();
    
    string sType = iString::DownCase(p_values[0]);
    
    // Type integer
    if(sType == "integer") {
      for(int i=0; i<iSize; i++) {
        string sValue = iString::DownCase(pvlKwrd[i]);
        if(sValue != "null"){
          iString::ToInteger(sValue);
        }
      }
      return;
    }
    
    // Type double
    if(sType == "double") {
      for(int i=0; i<iSize; i++) {
        string sValue = iString::DownCase(pvlKwrd[i]);
        if(sValue != "null"){
          iString::ToDouble(sValue);
        }
      }
      return;
    }
    
    // Type boolean
    if(sType == "boolean") {
      for(int i=0; i<iSize; i++) {
        string sValue = iString::DownCase(pvlKwrd[i]);
        if(sValue != "null" && sValue != "true" && sValue != "false"){
          string sErrMsg = "Wrong Type of value in the Keyword \"" + Name() + "\" \n";
          throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_);
        }
      }
      return;
    }
    
    // Type String
    if(sType == "string" && iTmplKwrdSize > 1) {
      for(int i=0; i<iSize; i++) {
        string sValue = iString::DownCase(pvlKwrd[i]);
        bool bValFound = false;
        
        for(int j=1; j<iTmplKwrdSize; j++) {
          string sTmplValue = iString::DownCase(p_values[j]);
          if (sValue == sTmplValue) {
            bValFound = true;
            break;
          }
        }
        if(bValFound == false) {
          string sErrMsg = "Wrong Type of value in the Keyword \"" + Name() + "\" \n";
          throw Isis::iException::Message(Isis::iException::User, sErrMsg, _FILEINFO_);
        }
      }
    }
  }
}
