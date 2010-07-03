/**
 * @file
 * $Revision: 1.12 $
 * $Date: 2010/01/04 17:58:51 $
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

#include <fstream>
#include <sstream>

#include "iString.h"
#include "iException.h"
#include "iException.h"
#include "Message.h"
#include "Pvl.h"
#include "PvlTranslationTable.h"

using namespace std;
namespace Isis {

 /** 
  * Constructs and initializes a PvlTranslationTable object 
  * 
  * @param transFile The translation file to be used 
  * 
  * @throws iException::Io
  */
  PvlTranslationTable::PvlTranslationTable (Isis::Filename transFile) {
    AddTable(transFile.Expanded());
  }
  
 /** 
  * Constructs and initializes a PvlTranslationTable object  
  * 
  * @param istr The translation stream to be used to translate values 
  */
  PvlTranslationTable::PvlTranslationTable (std::istream &istr) {
    istr >> p_trnsTbl;
  }
  
  //! Construct an empty PvlTranslationTable
  PvlTranslationTable::PvlTranslationTable () {
  }
  
  /** 
   * Adds the contents of a translation table to the searchable groups/keys
   * 
   * @param transFile The name of the translation file to be added.
   * 
   * @throws iException::Io
   */
   void PvlTranslationTable::AddTable (const std::string &transFile) {
    p_trnsTbl.Read(Filename(transFile).Expanded());
  }
  
 /** 
  * Adds the contents of a translation table to the searchable groups/keys
  * Also performs a verification, to ensure that the translation table
  * is valid
  * 
  * @param transStm The stream to be added.
  */
  void PvlTranslationTable::AddTable (std::istream &transStm) {
    transStm >> p_trnsTbl;

    for (int i=0; i < p_trnsTbl.Groups(); i++) {
      PvlGroup currGrp = p_trnsTbl.Group(i);

      if (!currGrp.HasKeyword("InputKey")) {
        string message = "Unable to find InputKey for group ["
                         + currGrp.Name() + "] in file [" +
                         p_trnsTbl.Filename() + "]";
        throw iException::Message(iException::User, message, _FILEINFO_);
      }

      // pair< name, size > of acceptable keywords.
      // A size of -1 means non-zero size.
      vector< pair<string, int> > validKeywords;

      validKeywords.push_back( pair<string, int>("Translation",     2) );
      validKeywords.push_back( pair<string, int>("OutputName",      1) );
      validKeywords.push_back( pair<string, int>("InputGroup",     -1) );
      validKeywords.push_back( pair<string, int>("InputPosition",  -1) );
      validKeywords.push_back( pair<string, int>("OutputPosition", -1) );
      validKeywords.push_back( pair<string, int>("Auto",            0) );
      validKeywords.push_back( pair<string, int>("Optional",        0) );
      validKeywords.push_back( pair<string, int>("InputKey",        1) );
      validKeywords.push_back( pair<string, int>("InputDefault",   -1) );

      for (int j=0; j<currGrp.Keywords(); j++) {
        bool validKeyword = false;
        bool keywordSizeMismatch = false;

        const PvlKeyword &currKey = currGrp[j];

        // Test this keyword for validity
        for(int key = 0; 
            !validKeyword && key < (int)validKeywords.size(); 
            key++) {

          // If this is the right keyword (names match) then test sizes
          if(currKey.Name() == validKeywords[key].first) {

            // if -1 then test that size() > 0 
            if(validKeywords[key].second == -1) {
              if(currKey.Size() > 0) {
                validKeyword = true;
              }
            }
            // otherwise should exact match
            else if(currKey.Size() == validKeywords[key].second) {
              validKeyword = true;
            }
            else {
              keywordSizeMismatch = true;
            }
          }

        }

        // if we had an error report it
        if(!validKeyword) {
          if(!keywordSizeMismatch) {
            string message = "Keyword [" + currKey.Name();
            message += "] is not a valid keyword.";
            message += " Error in file [" + p_trnsTbl.Filename() +"]" ;
  
            throw iException::Message(iException::User,
                                            message,
                                            _FILEINFO_);
          }
          else {
            string message = "Keyword [" + currKey.Name();
            message += "] does not have the correct number of elements.";
            message += " Error in file [" + p_trnsTbl.Filename() +"]" ;
  
            throw iException::Message(iException::User,
                                            message,
                                            _FILEINFO_);
          }

        }
      }
    }
  }
  
 /** 
  * Translates the output name and input value.
  * 
  * @param nName The output name to be used to search the translation table.
  * 
  * @param fValue The input value to be translated
  * 
  * @return string The translated string
  *
  * @throws iException::Programmer
  */
  string PvlTranslationTable::Translate (const std::string nName,
                                         const std::string fValue) const {
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + 
                   nName + "] in file [" + p_trnsTbl.Filename() + "]";

      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }

    const PvlGroup &tgrp = p_trnsTbl.FindGroup(nName);
  
    // If no input value was passed in search using the input default
    string tmpFValue = fValue;
    if (tmpFValue.empty()) {
      if (tgrp.HasKeyword("InputDefault")) {
        tmpFValue = (string) tgrp["InputDefault"];
      }
      else {
        string msg = "No value or default value to translate for ";
        msg += "translation group [";
        msg += nName;
        msg += "] in file [" + p_trnsTbl.Filename() + "]";
        throw iException::Message(iException::Programmer,
                                        msg,
                                        _FILEINFO_);
      }
    }
  
    // Search the Translation keywords for a match to the input value
    Pvl::ConstPvlKeywordIterator it = tgrp.FindKeyword("Translation", 
                                                  tgrp.Begin(), 
                                                  tgrp.End()); 

    while (it != tgrp.End()) {
      const PvlKeyword &key = *it;
      if ((string) key[1] == tmpFValue) {
        return key[0];
      }
      else if ((string) key[1] == "*") {
        if ((string) key[0] == "*") {
          return tmpFValue;
        }
        else {
          return key[0];
        }
      }

      it = tgrp.FindKeyword("Translation",it+1,tgrp.End());
    }
  
    string msg = "Unable to find a translation value for [" +
                   nName +  ", " + Isis::iString(fValue) + "] in file [" + 
                   p_trnsTbl.Filename() + "]";

    throw iException::Message(iException::Programmer,
                                    msg,
                                    _FILEINFO_);
  }
  
   /** 
    * Returns the input group name from the translation table corresponding to 
    * the output name argument.
    *
    * @param nName The output name to be used to search the translation table.
    * @param inst The occurence number of the "InputGroup" keyword
    *             (first one is zero)
    * 
    * @return string The input group name
    * 
    * @throws iException::Programmer
    */
    PvlKeyword PvlTranslationTable::InputGroup (const std::string nName, 
                                                const int inst) const {
  
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" +
                   nName + "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }

    const PvlGroup &transGrp = p_trnsTbl.FindGroup(nName);

    //bool foundLegalInputGroup = false;

    Pvl::ConstPvlKeywordIterator it = transGrp.FindKeyword("InputPosition", 
                                                           transGrp.Begin(), 
                                                           transGrp.End());

    int currentInstance = 0;

    // If no InputGroup keywords exist, the answer is root
    if(inst == 0 && it == transGrp.End()) {
      PvlKeyword root("InputPosition");
      root += "ROOT";
      return root;
    }

    while (it != transGrp.End()) {
      const PvlKeyword &result = *it;

      // This check is to prevent backtracking to the old "value,value" way of
      //   doing translation file input groups for the new keyword. Flag it
      //   immediately to give a good error message.
      if(result.Size() == 1 && result[0].find(",") != string::npos) {
        iString msg = "Keyword [InputPosition] cannot have a comma [,] in ";
        msg += " the value [";
        msg += result[0];
        msg += "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
      else {
        //foundLegalInputGroup = true;

        if(currentInstance == inst) {
          return result;
        }

        currentInstance ++;
      }

      it = transGrp.FindKeyword("InputPosition", it+1, transGrp.End());
    }

    /* Error if no containers were listed
    if(!foundLegalInputGroup) {
      iString msg = "No input position found for translation [";
      msg += nName;
      msg += "] in translation file [";
      msg += p_trnsTbl.Filename();
      msg += "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }*/

    PvlKeyword empty;
    return empty;
  }
  
 /** 
  * Returns the input keyword name from the translation table corresponding to 
  * the output name argument.
  *
  * @param nName The output name to be used to search the translation table.
  * 
  * @return string The input keyword name
  * 
  * @throws iException::Programmer
  */
  string PvlTranslationTable::InputKeywordName (const std::string nName) const {
  
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + 
                   nName + "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
  
    Isis::PvlGroup tgrp = p_trnsTbl.FindGroup(nName);
    if (tgrp.HasKeyword("InputKey")) return tgrp["InputKey"];
      
    return "";
  }
  
 /** 
  * Returns the input default value from the translation table corresponding 
  * to the output name argument.
  * 
  * @param nName The output name to be used to search the translation table.
  * 
  * @return string The input default value
  *
  * @throws iException::Programmer
  */
  string PvlTranslationTable::InputDefault (const std::string nName) const {
  
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + 
                   nName + "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
  
    Isis::PvlGroup tgrp = p_trnsTbl.FindGroup(nName);
    if (tgrp.HasKeyword("InputDefault")) return tgrp["InputDefault"];
  
    return "";
  }
  
  bool PvlTranslationTable::IsAuto (const std::string nName) {
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + nName + 
                   "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
  
    Isis::PvlGroup &tgrp = p_trnsTbl.FindGroup(nName);
    if (tgrp.HasKeyword("Auto")) return true;
  
    return false;
  }

  bool PvlTranslationTable::IsOptional (const std::string nName) {
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + nName + 
                   "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }

    Isis::PvlGroup &tgrp = p_trnsTbl.FindGroup(nName);
    if (tgrp.HasKeyword("Optional")) return true;

    return false;
  }
  
  Isis::PvlKeyword &PvlTranslationTable::OutputPosition (
      const std::string nName) {
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + 
                   nName + "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
  
    Isis::PvlGroup &tgrp = p_trnsTbl.FindGroup(nName);
    if (!tgrp.HasKeyword("OutputPosition")) {
      string msg = "Unable to find translation keyword [OutputPostion] in [" + 
                   nName + "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
  
    }

    return tgrp["OutputPosition"];
  }
  
  
  string PvlTranslationTable::OutputName (const std::string nName) {
    if (!p_trnsTbl.HasGroup(nName)) {
      string msg = "Unable to find translation group [" + nName + 
                   "] in file [" + p_trnsTbl.Filename() + "]";
      throw iException::Message(iException::Programmer,
                                      msg,
                                      _FILEINFO_);
    }
  
    Isis::PvlGroup tgrp = p_trnsTbl.FindGroup(nName);
    if (tgrp.HasKeyword("OutputName")) {
      return tgrp["OutputName"];
    }
  
    return "";
  }
} // end namespace isis

