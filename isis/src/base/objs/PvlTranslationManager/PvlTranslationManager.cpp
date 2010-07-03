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

#include "iString.h"
#include "Message.h"
#include "iException.h"
#include "PvlTranslationManager.h"

using namespace std;
namespace Isis {

  PvlTranslationManager::PvlTranslationManager (const std::string &transFile) {
    AddTable (transFile);
  }

  /** 
   * Constructs and initializes a TranslationManager object
   * 
   * @param inputLabel The Pvl holding the input label.
   * 
   * @param transFile The translation file to be used to tranlate keywords in  
   *                  the input label.
   */
   PvlTranslationManager::PvlTranslationManager (Isis::Pvl &inputLabel,
                                                  const std::string &transFile) {
    p_fLabel = inputLabel;

    // Internalize the translation table
    AddTable (transFile);
  }
  
 /** 
  * Constructs and initializes a TranslationManager object
  * 
  * @param inputLabel The Pvl holding the input label.
  * 
  * @param transStrm A stream containing the tranlation table to be used to  
  *                  tranlate keywords in the input label.
  */
  PvlTranslationManager::PvlTranslationManager (Isis::Pvl &inputLabel,
                                                std::istream &transStrm) {
    p_fLabel = inputLabel;

    // Internalize the translation table
    AddTable (transStrm);
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
  string PvlTranslationManager::Translate (const std::string nName, int findex) {
    const Isis::PvlContainer *con;
    int inst = 0;
    PvlKeyword grp;

    while ((grp = InputGroup(nName, inst++)).Name() != "") {
      if ((con = GetContainer(grp)) != NULL) {
        if (con->HasKeyword(InputKeywordName(nName))) {
          return PvlTranslationTable::Translate(nName,
                 (*con)[InputKeywordName(nName)][findex]);
        }
      }
    }

    return Isis::PvlTranslationTable::Translate(nName);
  }
  
 /** 
  * Translate the requested output name to output values using the input name  
  * and values or default value
  * 
  * @param nName The output name used to identify the input keyword to be 
  *              translated.
  * 
  * @return Isis::PvlKeyword
  */
  Isis::PvlKeyword PvlTranslationManager::DoTranslation(
      const std::string nName) {
    const Isis::PvlContainer *con = NULL;
    Isis::PvlKeyword key;

    int inst = 0;
    PvlKeyword grp;

    while ((grp = InputGroup(nName, inst++)).Name() != "") {
      if ((con = GetContainer(grp)) != NULL) {
        if (con->HasKeyword(InputKeywordName(nName))) {
          key.SetName(OutputName(nName));

          for (int v=0; v<(*con)[(InputKeywordName(nName))].Size(); v++) {
            key.AddValue(Isis::PvlTranslationTable::Translate(nName,
                         (*con)[InputKeywordName(nName)][v]),
                         (*con)[InputKeywordName(nName)].Unit(v));
          }

          return key;
        }
      }
    }

    return Isis::PvlKeyword (OutputName(nName),
                           PvlTranslationTable::Translate(nName, ""));
  }
  
  
  
  // Automatically translate all the output names found in the translation table
  // If a output name does not translate an error will be thrown by one
  // of the support members
  // Store the translated key, value pairs in the argument pvl
  void PvlTranslationManager::Auto (Isis::Pvl &outputLabel) {
    // Attempt to translate every group in the translation table
   for (int i=0; i<TranslationTable().Groups(); i++) {
      Isis::PvlGroup &g = TranslationTable().Group(i);
      if (IsAuto(g.Name())) {
        try {
          Isis::PvlContainer *con = CreateContainer(g.Name(), outputLabel);
          (*con) += PvlTranslationManager::DoTranslation (g.Name());
        }
        catch (iException &e){
          if (IsOptional(g.Name())) {
            e.Clear();
          }
          else {
//            e.Report();
            throw e;
          }
        }
      }
    }
  }
  
 /** 
  * Returns the ith input value assiciated with the output name argument. 
  * 
  * @param nName The output name used to identify the input keyword.
  * 
  * @param findex The index into the input keyword array.  Defaults to 0
  * 
  * @throws Isis::iException::Programmer
  */
  const PvlKeyword &PvlTranslationManager::InputKeyword (
      const std::string nName) const {

    int instanceNumber = 0;
    PvlKeyword inputGroupKeyword = InputGroup(nName, instanceNumber);
    bool anInputGroupFound = false;

    while(inputGroupKeyword.Name() != "") {
      const PvlContainer *containingGroup = GetContainer(inputGroupKeyword);
      if (containingGroup != NULL) {
        anInputGroupFound = true;

        if (containingGroup->HasKeyword(InputKeywordName(nName))) {
          return containingGroup->FindKeyword(InputKeywordName(nName));
        }
      }

      instanceNumber ++;
      inputGroupKeyword = InputGroup(nName, instanceNumber);
    }

    if(anInputGroupFound) {
      string msg = "Unable to find input keyword [" + InputKeywordName(nName) +
                   "] for output name [" + nName + "] in file [" + TranslationTable().Filename() +"]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
    else {
      string container = "";

      for(int i = 0; i < InputGroup(nName).Size(); i++) {
        if(i > 0) container += ",";

        container += InputGroup(nName)[i];
      }

      string msg = "Unable to find input group [" + container +
                   "] for output name [" + nName + "] in file [" + TranslationTable().Filename() +"]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  }

  
 /** 
  * Indicates if the input keyword corresponding to the output name exists in 
  * the label
  *  
  * @param nName The output name used to identify the input keyword.    
  */
  bool PvlTranslationManager::InputHasKeyword (const std::string nName) {
  
    // Set the current position in the input label pvl
    // by finding the input group corresponding to the output group
    const Isis::PvlContainer *con;
    int inst = 0;
    //while ((con = GetContainer(InputGroup(nName, inst++))) != NULL) {
    //if ((con = GetContainer (InputGroup(nName))) != NULL) {

    PvlKeyword grp;
    while ((grp = InputGroup(nName, inst++)).Name() != "") {
      if ((con = GetContainer(grp)) != NULL) {  
        if (con->HasKeyword(InputKeywordName(nName))) return true;
      }
    }

    return false;
  }
  
 /* 
  * Indicates if the input group corresponding to the output name exists in
  * the label 
  * 
  * @param nName The output name used to identify the input keyword. 
  
  bool PvlTranslationManager::InputHasGroup (const std::string nName) {
  
    if (GetContainer (InputGroup(nName)) != NULL) {
      return true;
    }
  
    return false;
  }
*/  
  
  // Return a container from the input label according tund
  const Isis::PvlContainer *PvlTranslationManager::GetContainer(
      const PvlKeyword &inputGroup) const {


    // Return the root container if "ROOT" is the ONLY thing in the list
    if (inputGroup.Size() == 1 && 
        PvlKeyword::StringEqual(inputGroup[0], "ROOT")) {
      return &p_fLabel;
    }

    const Isis::PvlObject *currentObject = &p_fLabel;

    // Search for object containing our solution
    int objectIndex;
    for(objectIndex = 0; 
         objectIndex < inputGroup.Size() - 1; 
         objectIndex ++) {
      if(currentObject->HasObject(inputGroup[objectIndex])) {
        currentObject = &currentObject->FindObject(inputGroup[objectIndex]);
      }
      else {
        return NULL;
      }
    }

    // Our solution can be an object or a group
    if(currentObject->HasObject(inputGroup[objectIndex])) {
      return &currentObject->FindObject(inputGroup[objectIndex]);
    }
    else if(currentObject->HasGroup(inputGroup[objectIndex])) {
      return &currentObject->FindGroup(inputGroup[objectIndex]);
    }
    else {
      return NULL;
    }
  }
  
  
  // Create the requsted container and any containers above it and
  // return a reference to the container
  // list is an Isis::PvlKeyword with an array of container types an their names
  Isis::PvlContainer *PvlTranslationManager::CreateContainer(const std::string nName,
                                                            Isis::Pvl &pvl) {
    
    // Get the array of Objects/Groups from the OutputName keyword
    Isis::PvlKeyword np = OutputPosition(nName);
  
    Isis::PvlObject *obj = &pvl;
  
    // Look at every pair in the output position
    for (int c=0; c<np.Size(); c+=2) {
      // If this pair is an object
      if (np[c].UpCase() == "OBJECT") {
        // If the object doesn't exist create it
        if (!obj->HasObject(np[c+1])) {
          obj->AddObject (np[c+1]);
        }
        obj = &(obj->FindObject(np[c+1]));
      }
      // If this pair is a group
      else if (np[c].UpCase() == "GROUP") {
        // If the group doesn't exist create it
        if (!obj->HasGroup (np[c+1])) {
          obj->AddGroup (np[c+1]);
        }
        return (Isis::PvlContainer *) &(obj->FindGroup(np[c+1]));
        
      }
    }
  
    return (Isis::PvlContainer *) obj;
  }
} // end namespace isis
