/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/02/04 22:36:41 $
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

#include "iException.h"
#include "Message.h"
#include "iString.h"
#include "Filename.h"
#include "PvlKeyword.h"
#include "TextFile.h"
#include "Pvl.h"
#include "PvlFormat.h"

using namespace std;

namespace Isis {

  /*
  * Constructs an empty PvlFormat
  */
  PvlFormat::PvlFormat() {
    Init();
  }


  /*
  * Constructs a PvlFormat using the file name to ingest as the keyword to type
  * mapping. This is provided as a convience for child objects. The map is not
  * used for output of PvlKeywords in Normal Isis format.
  * 
  * @param file A file name with keyword=type. Where KEYWORD is the name of a
  * keyword in this PvlKeyword and TYPE is one of [string | integer | float ]
  */
  PvlFormat::PvlFormat(const std::string &file) {
    Init();
    Add(file);
  }


  /*
  * Constructs a PvlFormat using the specified pre populated Pvl map of keyword
  * name (std::string) vs keyword type (KeywordType).
  * 
  * @param keywordType A Pvl with keyword=type. Where keyword is the name of a
  * keyword in a PvlKeyword and type is one of [string | integer | float ]
  */
  PvlFormat::PvlFormat(Pvl &keywordType) {
    Init();
    Add(keywordType);
  }


  //! Clears all PvlFormat data.
  void PvlFormat::Init() {
    p_keywordMap.Clear();
    p_keywordMapFile.clear();
    p_charLimit = 80;
  }


  /*
  * Add the contents of a file to the keyword type mapping. The file should
  * contain KEYWORD=TYPE (one per line), where TYPE is one of the strings
  * KeywordType can convert.
  */
  void PvlFormat::Add(const std::string &file) {
    p_keywordMapFile = file;
  
    // Open the file and internalize it into the Pvl map
    try {
      Pvl pvl(file);
      Add(pvl);
    }
    catch (iException &e) {
      string msg;
      msg += "Unable to open or read keyword to type mapping file [";
      msg += file + "]";
      throw iException::Message (Isis::iException::Programmer, msg, _FILEINFO_);
    }
  }


  /*
  * Add the contents of a Pvl to the keyword type mapping. The pvl should
  * contain KEYWORD=TYPE, where TYPE is one of the strings KeywordType can
  * convert.
  */
  void PvlFormat::Add(Pvl &pvl) {
    for (int i=0; i<pvl.Keywords(); ++i) {
      PvlKeyword &key = pvl[i];
      iString name = key.Name();
      name.UpCase();
      iString type = key[0];
      type.UpCase();
      PvlKeyword newKey(name, type);
      for (int j=1; j< key.Size(); ++j) newKey.AddValue(key[j]);
      p_keywordMap.AddKeyword(newKey);
    }
  }


  /*
  * Returns the type of the keyword from the supplied map if any
  * 
  * @param keyword The PvlKeyword to have its type returned
  */
  KeywordType PvlFormat::Type(const PvlKeyword &keyword) {
    iString name = keyword.Name();
    name.UpCase();
    if (p_keywordMap.HasKeyword(name)) {
      PvlKeyword &key = p_keywordMap.FindKeyword(name);
      return ToKeywordType(key[0]);
    }
    return NoTypeKeyword;
  }


  /*
  * Returns the number of digits of accuracy (right of decimal place) this
  * keyword should be output with
  * 
  * @param keyword The PvlKeyword the accuracy is need for
  * @return The number of decimal places to be output. If this number is not
  *         available in keyword map return -1.
  */
  int PvlFormat::Accuracy(const PvlKeyword &keyword) {
    iString name = keyword.Name();
    name.UpCase();
    if (p_keywordMap.HasKeyword(name)) {
      PvlKeyword &key = p_keywordMap.FindKeyword(name);
      if (key.Size() > 1) {
        return (int)key[1];
      }
    }
    return -1;
  }


  /*
  * Returns the keyword name and value(s) formatted in "Normal" Isis format
  * 
  * @param keyword The PvlKeyword to be formatted
  * @param num Use the ith value of the keyword
  */
  std::string PvlFormat::FormatValue(const PvlKeyword &keyword, int num) {

    string val;
    val.clear();

    // Find out if the units are the same for all values
    bool singleUnit = IsSingleUnit(keyword);

    // Create a Null value if the value index is greater than the number of values
    if (num >= keyword.Size()) {
      return "Null";
    }

    // Create a Null value if the requested index is an empty string
    if (keyword[num].size() == 0) {
      val += "Null";
    }
    else {
      val += keyword[num];
    }

    val = AddQuotes (val);

    // If it is an array start it off with a paren
    if ((keyword.Size() > 1) && (num == 0)) {
        val = "(" + val;
    }

    // Add the units to this value
    if ((!singleUnit) && (keyword.Unit(num).size() > 0)) { 
      val += " <" + keyword.Unit(num) + ">";
    }

    // Add a comma for arrays
    if (num != keyword.Size()-1) {
      val += ", ";
    }
    // If it is an array close it off
    else if (keyword.Size() > 1) {
      val += ")";
    }

    // Add the units to the end if all values have the same units
    if ((singleUnit) && (num == keyword.Size()-1) && 
        (keyword.Unit(num).size() > 0)) { 
      val += " <" + keyword.Unit(num) + ">";
    }

    return val;
  }


  /*
  * Format the name of the container
  * 
  * @param keyword The PvlContainer being closed.
  */
  std::string PvlFormat::FormatName(const PvlKeyword &keyword) {
    return keyword.Name();
  }


  /*
  * Format the end of a container
  * 
  * @param name The text used to signify the end of a container
  * @param keyword The PvlContainer being closed.
  */
  std::string PvlFormat::FormatEnd(const std::string name,
                                   const PvlKeyword &keyword) {
    return "End_" + FormatName(keyword);
  };


  /*
  * Add single or double quotes around a value if necessary. The Isis definition
  * of when quotes are necessary is used.
  * 
  * @param value The PvlKeyword value to be quoted if necessary.
  */
  std::string PvlFormat::AddQuotes(const std::string value) {
    std::string val = value;

    bool needQuotes = false;
    
    // find out if we need quotes and what kind of quotes might already exist
    char existingQuoteType = '\0';
    for(unsigned int pos = 0; !needQuotes && pos < val.size(); pos++) {
      // check for values indicating we need quotes, if we have a sequence
      //   it should already be properly quoted...
      if(pos == 0) {
        if(val[pos] == '(' && val[val.size() - 1] == ')') break;
        if(val[pos] == '{' && val[val.size() - 1] == '}') break;
      }

      if(val[pos] == ' ' || val[pos] == '(' ||
         val[pos] == '(' || val[pos] == ')' ||
         val[pos] == '{' || val[pos] == '}' ||
         val[pos] == ',') {
         needQuotes = true;
      }

      // remember if we are a quote, what quote type we are
      if(existingQuoteType == '\0') {
        if(val[pos] == '"') {
          existingQuoteType = '"';
        }
        else if(val[pos] == '\'') {
          existingQuoteType = '\'';
        }
      }
      else {
        // make sure we dont have mixing of our outside quote type
        if(val[pos] == '"' || val[pos] == '\'') {
          val[pos] = existingQuoteType;
        }
      }
    }

    // figure out what kind of quotes we want to add
    char quoteValue = '"';

    if(existingQuoteType == '"') {
      quoteValue = '\''; 
    }

    if(needQuotes) {
      val = quoteValue + val + quoteValue;
    }

    return val;
  }


 /**
  * Returns true if the units are the same for all value in the keyword
  * otherwise it returns false
  * 
  * @param keyword The PvlKeyword to be formatted
  */
  bool PvlFormat::IsSingleUnit(const PvlKeyword &keyword) {

    // See if the units are all the same
    bool singleUnit = true;
    for (int i = 0; i < keyword.Size(); i ++) {
      if (!keyword.StringEqual(keyword.Unit(i), keyword.Unit(0))) {
        singleUnit = false;
      }
    }
    
    return singleUnit;
  }
}

