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

#include "IException.h"
#include "IString.h"
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
  PvlTranslationTable::PvlTranslationTable(FileName transFile) {
    AddTable(transFile.expanded());
  }


  /**
   * Constructs and initializes a PvlTranslationTable object
   *
   * @param istr The translation stream to be used to translate values
   */
  PvlTranslationTable::PvlTranslationTable(std::istream &istr) {
    istr >> p_trnsTbl;
  }


  //! Construct an empty PvlTranslationTable
  PvlTranslationTable::PvlTranslationTable() {
  }


  //! Destroys the PvlTranslationTable object.
  PvlTranslationTable::~PvlTranslationTable() {
  }


  //! Protected accessor for pvl translation table passed into class.
  Pvl &PvlTranslationTable::TranslationTable() {
    return p_trnsTbl;
  }


  //! Protected accessor for const pvl translation table passed into class.
  const Pvl &PvlTranslationTable::TranslationTable() const {
    return p_trnsTbl;
  }


  /**
   * Adds the contents of a translation table to the searchable groups/keys
   *
   * @param transFile The name of the translation file to be added.
   *
   * @throws IException::Io
   */
  void PvlTranslationTable::AddTable(const QString &transFile) {
    p_trnsTbl.read(FileName(transFile).expanded());
    validateTable();
  }


  /**
   * Adds the contents of a translation table to the searchable groups/keys
   *
   * @param transStm The stream to be added.
   */
  void PvlTranslationTable::AddTable(std::istream &transStm) {
    transStm >> p_trnsTbl;
    validateTable();
  }


  /**
  * Performs verification to ensure that p_trnsTbl is valid
  *
  */
  void PvlTranslationTable::validateTable() {
    // pair< name, size > of acceptable keywords.
    // A size of -1 means non-zero size.
    vector< pair<QString, int> > validKeywordSizes = validKeywords();

    for(int i = 0; i < p_trnsTbl.groups(); i++) {
      PvlGroup currGrp = p_trnsTbl.group(i);

      if(!currGrp.hasKeyword("InputKey")) {
        QString message = "Unable to find InputKey for group ["
                         + currGrp.name() + "] in file [" +
                         p_trnsTbl.fileName() + "]";
        throw IException(IException::User, message, _FILEINFO_);
      }

      for(int j = 0; j < currGrp.keywords(); j++) {
        bool validKeyword = false;
        bool keywordSizeMismatch = false;

        const PvlKeyword &currKey = currGrp[j];

        // Test this keyword for validity
        for(int key = 0;
            !validKeyword && key < (int)validKeywordSizes.size();
            key++) {

          // If this is the right keyword (names match) then test sizes
          if(currKey.name() == validKeywordSizes[key].first) {

            // if -1 then test that size() > 0
            if(validKeywordSizes[key].second == -1) {
              if(currKey.size() > 0) {
                validKeyword = true;
              }
            }
            // otherwise should exact match
            else if(currKey.size() == validKeywordSizes[key].second) {
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
            QString message = "Keyword [" + currKey.name();
            message += "] is not a valid keyword.";
            message += " Error in file [" + p_trnsTbl.fileName() + "]" ;

            throw IException(IException::User, message, _FILEINFO_);
          }
          else {
            QString message = "Keyword [" + currKey.name();
            message += "] does not have the correct number of elements.";
            message += " Error in file [" + p_trnsTbl.fileName() + "]" ;

            throw IException(IException::User, message, _FILEINFO_);
          }

        }
      }
    }
  }


  /**
   * Returns a vector of valid keyword names and their sizes.  A size of -1
   * indicates that the keyword can be any size.
   *
   * @return @b vector<pair<QString,int>> A vector of valid keyword names and their sizes.
   */
  vector< pair<QString, int> > PvlTranslationTable::validKeywords() const {

    vector< pair<QString, int> > validKeywords;
    validKeywords.push_back(pair<QString, int>("Translation",     2));
    validKeywords.push_back(pair<QString, int>("OutputName",      1));
    validKeywords.push_back(pair<QString, int>("InputGroup",     -1));
    validKeywords.push_back(pair<QString, int>("InputPosition",  -1));
    validKeywords.push_back(pair<QString, int>("OutputPosition", -1));
    validKeywords.push_back(pair<QString, int>("Auto",            0));
    validKeywords.push_back(pair<QString, int>("Optional",        0));
    validKeywords.push_back(pair<QString, int>("InputKey",        1));
    validKeywords.push_back(pair<QString, int>("InputDefault",   -1));

    return validKeywords;
  }


  /**
   * Translates the output name and input value.
   *
   * @param nName The output name to be used to search the translation table.
   * @param fValue The input value to be translated
   *
   * @return QString The translated QString
   *
   * @throws iException::Programmer
   */
  QString PvlTranslationTable::Translate(const QString nName,
                                         const QString fValue) const {
    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" +
                   nName + "] in file [" + p_trnsTbl.fileName() + "]";

      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    const PvlGroup &tgrp = p_trnsTbl.findGroup(nName);

    // If no input value was passed in search using the input default
    QString tmpFValue = fValue;
    if(tmpFValue.isEmpty()) {
      if(tgrp.hasKeyword("InputDefault")) {
        tmpFValue = (QString) tgrp["InputDefault"];
      }
      else {
        QString msg = "No value or default value to translate for ";
        msg += "translation group [";
        msg += nName;
        msg += "] in file [" + p_trnsTbl.fileName() + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    // Search the Translation keywords for a match to the input value
    Pvl::ConstPvlKeywordIterator it = tgrp.findKeyword("Translation",
                                      tgrp.begin(),
                                      tgrp.end());

    while(it != tgrp.end()) {
      const PvlKeyword &key = *it;
      // compare the value from the input file to the second value of each Translation in the trans file.
      // ignore cases for input values
      if(QString::compare((QString) key[1], tmpFValue, Qt::CaseInsensitive) == 0) {
        return key[0];
      }
      else if((QString) key[1] == "*") {
        if((QString) key[0] == "*") {
          return tmpFValue;
        }
        else {
          return key[0];
        }
      }

      it = tgrp.findKeyword("Translation", it + 1, tgrp.end());
    }

    QString msg = "Unable to find a translation value for [" +
                 nName +  ", " + fValue + "] in file [" +
                 p_trnsTbl.fileName() + "]";

    throw IException(IException::Programmer, msg, _FILEINFO_);
  }


  /**
   * Returns the input group name from the translation table corresponding to
   * the output name argument.
   *
   * @param nName The output name to be used to search the translation table.
   * @param inst The occurence number of the "InputGroup" keyword
   *             (first one is zero)
   *
   * @return QString The input group name
   *
   * @throws iException::Programmer
   */
  PvlKeyword PvlTranslationTable::InputGroup(const QString nName,
      const int inst) const {

    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" +
                   nName + "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    const PvlGroup &transGrp = p_trnsTbl.findGroup(nName);

    //bool foundLegalInputGroup = false;

    Pvl::ConstPvlKeywordIterator it = transGrp.findKeyword("InputPosition",
                                      transGrp.begin(),
                                      transGrp.end());

    int currentInstance = 0;

    // If no InputPosition keyword exists, the answer is root
    if(inst == 0 && it == transGrp.end()) {
      PvlKeyword root("InputPosition");
      root += "ROOT";
      return root;
    }

    while(it != transGrp.end()) {
      const PvlKeyword &result = *it;

      // This check is to prevent backtracking to the old "value,value" way of
      //   doing translation file input groups for the new keyword. Flag it
      //   immediately to give a good error message.
      if(result.size() == 1 && result[0].contains(",")) {
        QString msg = "Keyword [InputPosition] cannot have a comma [,] in ";
        msg += " the value [";
        msg += result[0];
        msg += "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      else {
        //foundLegalInputGroup = true;

        if(currentInstance == inst) {
          return result;
        }

        currentInstance ++;
      }

      it = transGrp.findKeyword("InputPosition", it + 1, transGrp.end());
    }

    /* Error if no containers were listed
    if(!foundLegalInputGroup) {
      QString msg = "No input position found for translation [";
      msg += nName;
      msg += "] in translation file [";
      msg += p_trnsTbl.FileName();
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
   * @return QString The input keyword name
   *
   * @throws iException::Programmer
   */
  QString PvlTranslationTable::InputKeywordName(const QString nName) const {

    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" +
                   nName + "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup tgrp = p_trnsTbl.findGroup(nName);
    if(tgrp.hasKeyword("InputKey")) return tgrp["InputKey"];

    return "";
  }


  /**
   * Returns the input default value from the translation table corresponding
   * to the output name argument.
   *
   * @param nName The output name to be used to search the translation table.
   *
   * @return QString The input default value
   *
   * @throws iException::Programmer
   */
  QString PvlTranslationTable::InputDefault(const QString nName) const {

    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" +
                   nName + "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup tgrp = p_trnsTbl.findGroup(nName);
    if(tgrp.hasKeyword("InputDefault")) return tgrp["InputDefault"];

    return "";
  }

  bool PvlTranslationTable::hasInputDefault(const QString nName) {
    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" + nName +
                   "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup &tgrp = p_trnsTbl.findGroup(nName);
    if(tgrp.hasKeyword("InputDefault")) return true;

    return false;
  }

  bool PvlTranslationTable::IsAuto(const QString nName) {
    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" + nName +
                   "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup &tgrp = p_trnsTbl.findGroup(nName);
    if(tgrp.hasKeyword("Auto")) return true;

    return false;
  }

  bool PvlTranslationTable::IsOptional(const QString nName) {
    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" + nName +
                   "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup &tgrp = p_trnsTbl.findGroup(nName);
    if(tgrp.hasKeyword("Optional")) return true;

    return false;
  }

  PvlKeyword &PvlTranslationTable::OutputPosition(const QString nName) {
    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" +
                   nName + "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup &tgrp = p_trnsTbl.findGroup(nName);
    if(!tgrp.hasKeyword("OutputPosition")) {
      QString msg = "Unable to find translation keyword [OutputPostion] in [" +
                   nName + "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);

    }

    return tgrp["OutputPosition"];
  }


  QString PvlTranslationTable::OutputName(const QString nName) {
    if(!p_trnsTbl.hasGroup(nName)) {
      QString msg = "Unable to find translation group [" + nName +
                   "] in file [" + p_trnsTbl.fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    PvlGroup tgrp = p_trnsTbl.findGroup(nName);
    if(tgrp.hasKeyword("OutputName")) {
      return tgrp["OutputName"];
    }

    return "";
  }
} // end namespace isis
