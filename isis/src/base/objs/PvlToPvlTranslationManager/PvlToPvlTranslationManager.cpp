/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2010/01/04 18:01:31 $
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
#include "LabelTranslationManager.h"

#include "IException.h"
#include "IString.h"
#include "Message.h"
#include "Pvl.h"
#include "PvlContainer.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "PvlToPvlTranslationManager.h"

using namespace std;
namespace Isis {

  /**
   * Constructs and initializes a TranslationManager object from given the
   * Pvl translation file. If this constructor is used, the user will need to
   * set the input label before translating. This may be done by using
   * SetLabel(Pvl inputLabel) or Auto(Pvl inputLabel, Pvl outputLabel).
   *
   * @param transFile The translation file to be used to tranlate keywords in
   *                  the input label.
   */
  PvlToPvlTranslationManager::PvlToPvlTranslationManager(const QString &transFile)
      : LabelTranslationManager(transFile) {
  }

  /**
   * Constructs and initializes a TranslationManager object from the given
   * input stream. If this constructor is used, the user will need to set the
   * input label before translating. This may be done by using SetLabel(Pvl
   * inputLabel) or Auto(Pvl inputLabel, Pvl outputLabel).
   *
   * @param transStrm A stream containing the tranlation table to be used to
   *                  tranlate keywords in the input label.
   */
  PvlToPvlTranslationManager::PvlToPvlTranslationManager(std::istream &transStrm)
      : LabelTranslationManager(transStrm) {
  }


  /**
   * Constructs and initializes a TranslationManager object
   *
   * @param inputLabel The Pvl holding the input label.
   *
   * @param transFile The translation file to be used to tranlate keywords in
   *                  the input label.
   */
  PvlToPvlTranslationManager::PvlToPvlTranslationManager(Pvl &inputLabel,
                                               const QString &transFile)
      : LabelTranslationManager(transFile) {
    p_fLabel = inputLabel;
  }


  /**
   * Constructs and initializes a TranslationManager object
   *
   * @param inputLabel The Pvl holding the input label.
   *
   * @param transStrm A stream containing the tranlation table to be used to
   *                  tranlate keywords in the input label.
   */
  PvlToPvlTranslationManager::PvlToPvlTranslationManager(Pvl &inputLabel,
      std::istream &transStrm)
      : LabelTranslationManager(transStrm) {
    p_fLabel = inputLabel;
  }


  //! Destroys the TranslationManager object.
  PvlToPvlTranslationManager::~PvlToPvlTranslationManager() {
  }


  void PvlToPvlTranslationManager::SetLabel(Pvl &inputLabel) {
    p_fLabel = inputLabel;
  }


  /**
   * Returns a translated value. The output name is used to find the input
   * group, keyword, default and tranlations in the translation table. If the
   * keyword does not exist in the input label, the input default if
   * available will be used as the input value. This input value
   * is then used to search all of the translations. If a match is
   * found the translated value is returned.
   *
   * @param nName The output name used to identify the input keyword to be
   *              translated.
   *
   * @param findex The index into the input keyword array.  Defaults to 0
   *
   * @return string
   */
  QString PvlToPvlTranslationManager::Translate(QString nName, int findex) {
    const PvlContainer *con;
    int inst = 0;
    PvlKeyword grp;

    while((grp = InputGroup(nName, inst++)).name() != "") {
      if((con = GetContainer(grp)) != NULL) {
        if(con->hasKeyword(InputKeywordName(nName))) {
          return PvlTranslationTable::Translate(nName,
                                                (*con)[InputKeywordName(nName)][findex]);
        }
      }
    }

    return PvlTranslationTable::Translate(nName);
  }


  /**
   * Translate the requested output name to output values using the input name
   * and values or default value
   *
   * @param nName The output name used to identify the input keyword to be
   *              translated.
   *
   * @return PvlKeyword
   */
   PvlKeyword PvlToPvlTranslationManager::DoTranslation(const QString nName) {
     const PvlContainer *con = NULL;
     PvlKeyword key;

     int inst = 0;
     PvlGroup transGroup;
     PvlKeyword grp;

     while((grp = InputGroup(nName, inst++)).name() != "") {
       if((con = GetContainer(grp)) != NULL) {
         transGroup = TranslationTable().findGroup(nName);
         Pvl::ConstPvlKeywordIterator it = transGroup.findKeyword("InputKey",
                                           transGroup.begin(),
                                           transGroup.end());
         // Loop through potential InputKeys in the translation file group currently beginning
         // translated.
         while(it != transGroup.end()) {
           const PvlKeyword &result = *it;
           if(con->hasKeyword(result[0])) {
             key.setName(OutputName(nName));

             for(int v = 0; v < (*con)[(result[0])].size(); v++) {
               key.addValue(PvlTranslationTable::Translate(nName,
                            (*con)[result[0]][v]),
                            (*con)[result[0]].unit(v));
             }

             return key;
           }
           it = transGroup.findKeyword("InputKey", it + 1, transGroup.end());
         }
       }
     }

     return PvlKeyword(OutputName(nName),
                             PvlTranslationTable::Translate(nName, ""));
   }


  /**
   * Automatically translate all the output names found in the translation table
   * If a output name does not translate an error will be thrown by one
   * of the support members
   * Store the translated key, value pairs in the argument pvl
   */
  void PvlToPvlTranslationManager::Auto(Pvl &inputLabel, Pvl &outputLabel) {
    p_fLabel = inputLabel;
    Auto(outputLabel);
  }


  /**
   * Automatically translate all the output names found in the translation table
   * If a output name does not translate an error will be thrown by one
   * of the support members
   * Store the translated key, value pairs in the argument pvl
   */
  void PvlToPvlTranslationManager::Auto(Pvl &outputLabel) {
    // Attempt to translate every group in the translation table
    for(int i = 0; i < TranslationTable().groups(); i++) {
      PvlGroup &g = TranslationTable().group(i);
      if(IsAuto(g.name())) {
        try {
          PvlContainer *con = CreateContainer(g.name(), outputLabel);
          (*con) += DoTranslation(g.name());
        }
        catch(IException &e) {
          if(!IsOptional(g.name())) {
            throw;
          }
        }
      }
    }
  }


  /**
   * Returns the ith input value associated with the output name argument.
   *
   * @param nName The output name used to identify the input keyword.
   *
   * @param findex The index into the input keyword array.  Defaults to 0
   *
   * @throws IException::Programmer
   */
  const PvlKeyword &PvlToPvlTranslationManager::InputKeyword(const QString nName) const {

    int instanceNumber = 0;
    PvlKeyword inputGroupKeyword = InputGroup(nName, instanceNumber);
    bool anInputGroupFound = false;

    while(inputGroupKeyword.name() != "") {
      const PvlContainer *containingGroup = GetContainer(inputGroupKeyword);
      if(containingGroup != NULL) {
        anInputGroupFound = true;

        if(containingGroup->hasKeyword(InputKeywordName(nName))) {
          return containingGroup->findKeyword(InputKeywordName(nName));
        }
      }

      instanceNumber ++;
      inputGroupKeyword = InputGroup(nName, instanceNumber);
    }

    if(anInputGroupFound) {
      QString msg = "Unable to find input keyword [" + InputKeywordName(nName) +
                   "] for output name [" + nName + "] in file [" + TranslationTable().fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    else {
      QString container = "";

      for(int i = 0; i < InputGroup(nName).size(); i++) {
        if(i > 0) container += ",";

        container += InputGroup(nName)[i];
      }

      QString msg = "Unable to find input group [" + container +
                   "] for output name [" + nName + "] in file [" + TranslationTable().fileName() + "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Indicates if the input keyword corresponding to the output name exists in
   * the label
   *
   * @param nName The output name used to identify the input keyword.
   */
  bool PvlToPvlTranslationManager::InputHasKeyword(const QString nName) {

    // Set the current position in the input label pvl
    // by finding the input group corresponding to the output group
    const PvlContainer *con;
    int inst = 0;
    //while ((con = GetContainer(InputGroup(nName, inst++))) != NULL) {
    //if ((con = GetContainer (InputGroup(nName))) != NULL) {

    PvlKeyword grp;
    while((grp = InputGroup(nName, inst++)).name() != "") {
      if((con = GetContainer(grp)) != NULL) {
        if(con->hasKeyword(InputKeywordName(nName))) return true;
      }
    }

    return false;
  }


  //! Return a container from the input label according tund
  const PvlContainer *PvlToPvlTranslationManager::GetContainer(const PvlKeyword &inputGroup) const {


    // Return the root container if "ROOT" is the ONLY thing in the list
    if(inputGroup.size() == 1 &&
        PvlKeyword::stringEqual(inputGroup[0], "ROOT")) {
      return &p_fLabel;
    }

    const PvlObject *currentObject = &p_fLabel;

    // Search for object containing our solution
    int objectIndex;
    for(objectIndex = 0;
        objectIndex < inputGroup.size() - 1;
        objectIndex ++) {
      if(currentObject->hasObject(inputGroup[objectIndex])) {
        currentObject = &currentObject->findObject(inputGroup[objectIndex]);
      }
      else {
        return NULL;
      }
    }

    // Our solution can be an object or a group
    if(currentObject->hasObject(inputGroup[objectIndex])) {
      return &currentObject->findObject(inputGroup[objectIndex]);
    }
    else if(currentObject->hasGroup(inputGroup[objectIndex])) {
      return &currentObject->findGroup(inputGroup[objectIndex]);
    }
    else {
      return NULL;
    }
  }


  /**
   * Create the requsted container and any containers above it and
   * return a reference to the container
   * list is an PvlKeyword with an array of container types an their names
   */
  PvlContainer *PvlToPvlTranslationManager::CreateContainer(const QString nName, Pvl &pvl) {
    return LabelTranslationManager::CreateContainer(nName, pvl);
  }
} // end namespace isis
