/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2010/01/09 02:09:23 $
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
#include <fstream>
#include "PvlTokenizer.h"
#include "iException.h"
#include "Message.h"
#include "iString.h"

using namespace std;
namespace Isis {

  //! Constructs a Tokenizer with an empty token list
  PvlTokenizer::PvlTokenizer () {
    Clear ();
  }

  //! Destroys the Tokenizer object and token list
  PvlTokenizer::~PvlTokenizer () {
    Clear ();
  }

  //!  Empties the token list.
  void PvlTokenizer::Clear () {
    tokens.clear ();
  }

 /** 
  * Loads the Token list from a stream. The loading will be terminated upon  
  * reaching either 1) end-of-stream, or
  * 2) a programmer specified terminator string
  * 
  * @param stream The input stream to tokenize
  * 
  * @param terminator If the tokenizer see's this string as a token in the input 
  *                   stream it will cease tokenizing.  Defaults to "END"
  * 
  * @throws Isis::iException::Parse
  */
  void PvlTokenizer::Load (std::istream &stream, const std::string &terminator) {
    Isis::iString upTerminator(terminator);
    upTerminator.UpCase();
    string s;
    int c;
    bool newlineFound = false;
  
    while(true) {
      newlineFound = SkipWhiteSpace (stream);
      c = stream.peek ();
      ValidateCharacter (c);
      if (c == EOF) return;
  
      if (c == '#') {
        s = ReadComment (stream);
        Isis::PvlToken t("_COMMENT_");
        t.AddValue (s);

        if(newlineFound || tokens.size() == 0 || tokens[tokens.size()-1].ValueSize() == 0) {
          // applies to next pvl item
          tokens.push_back (t);
        }
        else {
          // applies to previous pvl item
          tokens.push_back(tokens[tokens.size()-1]);
          tokens[tokens.size()-2] = t;
        }
        
        continue;
      }
  
      if (c == '/') {
        c = stream.get ();
        c = stream.peek ();
        stream.unget ();
        ValidateCharacter (c);
        if (c == '*') {
          s = ReadComment (stream);
          Isis::PvlToken t("_COMMENT_");
          t.AddValue (s);

          if(newlineFound || tokens.size() == 0 || tokens[tokens.size()-1].ValueSize() == 0) {
            // applies to next pvl item
            tokens.push_back (t);
          }
          else {
            // applies to previous pvl item
            tokens.push_back(tokens[tokens.size()-1]);
            tokens[tokens.size()-2] = t;
          }

          continue;
        }
      }
  
      s = ReadToken (stream);
      Isis::PvlToken t(s);
  
      if (t.GetKeyUpper () == upTerminator) {
        tokens.push_back (t);
        return;
      }
  
      SkipWhiteSpace (stream);
      c = stream.peek ();
      ValidateCharacter (c);
      if (c == EOF) {
        tokens.push_back (t);
        return;
      }
  
      if (c != '=') {
        tokens.push_back (t);
        if (t.GetKeyUpper () == upTerminator) return;
        continue;
      }
  
      stream.ignore ();
      SkipWhiteSpace (stream);
  
      c = stream.peek ();
      ValidateCharacter (c);
      if (c == EOF) {
        tokens.push_back(t);
        return;
      }
      
      if (c == '(') {
        stream.ignore ();
        try {
          s = ReadToParen (stream);
          ParseCommaList (t,s);
        }
        catch (Isis::iException &e) {
          string message = Isis::Message::KeywordValueBad (t.GetKey());
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
        tokens.push_back(t);
        continue;
      }
  
      if (c == '{') {
        stream.ignore ();
        try {
          s = ReadToBrace (stream);
          ParseCommaList (t,s);
        }
        catch (Isis::iException &e) {
          string message = Isis::Message::KeywordValueBad (t.GetKey());
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
        tokens.push_back(t);
        continue;
      }
  
      if (c == '"') {
        stream.ignore ();
        try {
          s = ReadToDoubleQuote (stream);
        }
        catch (Isis::iException &e) {
          string message = Isis::Message::KeywordValueBad (t.GetKey());
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
        t.AddValue(s);
        tokens.push_back(t);
        continue;
      }
  
      if (c == '\'') {
        stream.ignore ();
        try {
          s = ReadToSingleQuote (stream);
        }
        catch (Isis::iException &e) {
          string message = Isis::Message::KeywordValueBad (t.GetKey());
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
        t.AddValue(s);
        tokens.push_back(t);
        continue;
      }
  
  
      s = ReadToken (stream);
      t.AddValue(s);
      tokens.push_back(t);
      continue;
    }
  }

 /** 
  * Reads and returns a comment from the stream.
  *
  * @param stream Input stream to read from
  * 
  * @return string
  */
  string PvlTokenizer::ReadComment (std::istream &stream) {
    string s;
    int c;
  
    c = stream.get ();
    while ((c != '\r') && (c != '\n') && (c != '\0')) {
      s += (char) c;
      c = stream.peek ();
      ValidateCharacter (c);
      if (c == EOF) return s;
      c = stream.get ();
    }
  
    stream.unget();
  
    return s;
  }

 /** 
  * Reads and returns a token from the stream. A token is delimited by either 
  * whitespace or an equal sign. In the case of whitespace the token will be 
  * considered valueless. That is, there will be no value in the value side of 
  * the token (e.g., KEYWORD=).
  *
  * @param stream Input stream to read from
  * 
  * @return string
  */
  string PvlTokenizer::ReadToken (std::istream &stream) {
    string s;
    int c;
  
    c = stream.get ();
    while ((!isspace (c)) && (c != '\0') && (c != '=')) {
      s += (char) c;
      c = stream.peek ();
      ValidateCharacter (c);
      if (c == EOF) return s;
      c = stream.get ();
    }
  
    stream.unget();
  
    return s;
  }

 /** 
  * Skips over whitespace so long as it is not inside quotes. Whitespace is 
  * tabs, blanks, line feeds, carriage returns, and NULLs.
  * 
  * @param stream Input stream to read from
  */
  bool PvlTokenizer::SkipWhiteSpace (std::istream &stream) {
    bool foundNewline = false;
    int c;
  
    c = stream.peek ();
    ValidateCharacter (c);
    while ((isspace (c)) || (c == '\0')) {
      if(c == '\n') {
        foundNewline = true;
      }

      c = stream.get ();
      c = stream.peek ();
      ValidateCharacter (c);
    }
  
    return foundNewline;
  }


  string PvlTokenizer::ReadToDoubleQuote (std::istream &stream) {
    string s;
    int c;

    do {
      c = stream.get ();
      ValidateCharacter (c);
      if (c == EOF) {
        string message = Isis::Message::MissingDelimiter ('"',s);
        throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
      }
      else if (c != '"') {
        s += (char) c;
      }
    } while (c != '"');
  
    std::string::size_type pos = s.find_first_of("\n\r");
    while (pos != std::string::npos) {
      Isis::iString first = s.substr(0,pos);
      bool addspace = false;
      if (first[pos-1] == ' ') addspace = true;
      first.TrimTail(" \t\n\r\f\0");
      Isis::iString second = s.substr(pos+1);
      if (second[0] == ' ') addspace = true;
      if (second[0] == '\r') addspace = true;
      if (second[0] == '\n') addspace = true;
      second.TrimHead(" \t\n\r\f\0");
      if (second[0] == ',') addspace = false;
      s = first;
      if (addspace) s += " ";
      s += second;
      pos = s.find_first_of("\n\r");
    }
    return s;
  }
  
  string PvlTokenizer::ReadToSingleQuote (std::istream &stream) {
    string s;
    int c;
  
    do {
      c = stream.get ();
      ValidateCharacter (c);
      if (c == EOF) {
        string message = Isis::Message::MissingDelimiter ('\'',s);
        throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
      }
      else if (c != '\'') {
        s += (char) c;
      }
    } while (c != '\'');
  
    std::string::size_type pos = s.find_first_of("\n\r");
    while (pos != std::string::npos) {
      Isis::iString first = s.substr(0,pos);
      bool addspace = false;
      if (first[pos-1] == ' ') addspace = true;
      first.TrimTail(" \t\n\r\f\0");
      Isis::iString second = s.substr(pos+1);
      if (second[0] == ' ') addspace = true;
      if (second[0] == '\r') addspace = true;
      if (second[0] == '\n') addspace = true;
      second.TrimHead(" \t\n\r\f\0");
      if (second[0] == ',') addspace = false;
      s = first;
      if (addspace) s += " ";
      s += second;
      pos = s.find_first_of("\n\r");
    }
 
    return s;
  }
  
  string PvlTokenizer::ReadToParen (std::istream &stream) {
    string s;
    int c;
    int leftParenCount = 1;
  
    do {
      c = stream.get ();
      ValidateCharacter (c);
      if (c == EOF) {
        string message = Isis::Message::MissingDelimiter (')',s);
        throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
      }
      else if (c == '"') {
        try {
          s += "\"" + ReadToDoubleQuote(stream) + "\"";
        }
        catch (Isis::iException &e) {
          e.Clear();
          string message = Isis::Message::MissingDelimiter ('"',s);
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
      }
      else if (c == '\'') {
        try {
          s += "'" + ReadToSingleQuote(stream) + "'";
        }
        catch (Isis::iException &e) {
          e.Clear();
          string message = Isis::Message::MissingDelimiter ('\'',s);
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
      }
      else if (c == ')') {
        leftParenCount--;
        if (leftParenCount > 0) s += (char) c;
      }
      else {
        s += (char) c;
        if (c == '(') leftParenCount++;
      }
    } while (leftParenCount > 0);
  
    return s;
  }
  
  string PvlTokenizer::ReadToBrace (std::istream &stream) {
    string s;
    int c;
    int leftBraceCount = 1;
  
    do {
      c = stream.get ();
      ValidateCharacter (c);
      if (c == EOF) {
        string message = Isis::Message::MissingDelimiter ('}',s);
        throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
      }
      else if (c == '"') {
        try {
          s += "\"" + ReadToDoubleQuote(stream) + "\"";
        }
        catch (Isis::iException &e) {
          e.Clear();
          string message = Isis::Message::MissingDelimiter ('"',s);
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
      }
      else if (c == '\'') {
        try {
          s += "'" + ReadToSingleQuote(stream) + "'";
        }
        catch (Isis::iException &e) {
          e.Clear();
          string message = Isis::Message::MissingDelimiter ('\'',s);
          throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
        }
      }
      else if (c == '}') {
        leftBraceCount--;
        if (leftBraceCount > 0) s += (char) c;
      }
      else {
        s += (char) c;
        if (c == '{') leftBraceCount++;
      }
    } while (leftBraceCount > 0);
  
    return s;
  }
  
 /** 
  * This routine parses a string containing a comma separated list. Each of the 
  * items in the list is stored as a value in the Token.
  * 
  * @param t Token to load the comma separated list
  * 
  * @param cl iString containing comma separated list
  */
  void PvlTokenizer::ParseCommaList (Isis::PvlToken &t, const std::string &cl) {
    stringstream stream(cl);
    int c;
    string s;
  
    do {
      SkipWhiteSpace(stream);
      c = stream.get ();
      if (c == '"') {
        s += ReadToDoubleQuote(stream);
      }
      else if (c == '\'') {
        s += ReadToSingleQuote(stream);
      }
      else if (c == '(') {
        s += "(";
        s += ReadToParen(stream);
        s += ")";
      }
      else if (c == '{') {
        s += "{";
        s += ReadToBrace(stream);
        s += "}";
      }
      else if (c == ',') {
        t.AddValue (s);
        s.erase();
      }
      else if (c != EOF) {
        s += (char) c;
      }
    } while (c != EOF);
  
    t.AddValue (s);
  }
  
  
  vector<Isis::PvlToken> & PvlTokenizer::GetTokenList () {
    return tokens;
  }

 /** 
  * Make sure a character is valid printable (non-control) character
  * 
  * @param c Character to be validated
  */
  void PvlTokenizer::ValidateCharacter (int c) {
    if (c == EOF) return;
    if (isprint (c)) return;
    if (isspace (c)) return;
    if (c == '\0') return;
  
    string message = "ASCII data expected but found unprintable (binary) data";
    throw Isis::iException::Message(Isis::iException::Parse,message,_FILEINFO_);
  }
} // end namespace isis
