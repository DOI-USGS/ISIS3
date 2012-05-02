/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/04/14 01:56:24 $
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

#include "FileName.h"
#include "Pvl.h"
#include "IException.h"
#include "IException.h"
#include "Message.h"
#include "PvlFormat.h"
#include "PvlObject.h"

#include <QList>

#include <iostream>
#include <sstream>

using namespace std;
namespace Isis {

  //! Creates a blank PvlObject.
  PvlObject::PvlObject() : Isis::PvlContainer("Object") {
  }


  /**
   * Creates a PvlObject with a specified name.
   *
   * @param name The name of the PvlObject.
   */
  PvlObject::PvlObject(const std::string &name) :
    Isis::PvlContainer("Object", name) {
  }


  //! Copy constructor
  PvlObject::PvlObject(const PvlObject &other) :
    PvlContainer::PvlContainer(other) {
    p_objects = other.p_objects;
    p_groups = other.p_groups;
  }


  /**
   * Finds a group within the current PvlObject.
   *
   * @param name The name of the group to look for.
   * @param opts The FindOptions option (None or Traverse).
   *
   * @return The PvlGroup object sought for.
   *
   * @throws IException
   */
  Isis::PvlGroup &PvlObject::FindGroup(const std::string &name,
                                       PvlObject::FindOptions opts) {
    vector<PvlObject *> searchList;
    searchList.push_back(this);

    while(searchList.size() > 0) {
      PvlGroupIterator it =
        searchList[0]->FindGroup(name,
                                 searchList[0]->BeginGroup(),
                                 searchList[0]->EndGroup());
      if(it != searchList[0]->EndGroup()) return *it;
      if(opts == Traverse) {
        for(int i = 0; i < searchList[0]->Objects(); i++) {
          searchList.push_back(&searchList[0]->Object(i));
        }
      }
      searchList.erase(searchList.begin());
    }

    string msg = "Unable to find PVL group [" + name + "]";
    if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  /**
   * Finds a group within the current PvlObject.
   *
   * @param name The name of the group to look for.
   * @param opts The FindOptions option (None or Traverse).
   *
   * @return The PvlGroup object sought for.
   *
   * @throws IException
   */
  const Isis::PvlGroup &PvlObject::FindGroup(const std::string &name,
      PvlObject::FindOptions opts) const {
    vector<const PvlObject *> searchList;
    searchList.push_back(this);

    while(searchList.size() > 0) {
      ConstPvlGroupIterator it =
        searchList[0]->FindGroup(name,
                                 searchList[0]->BeginGroup(),
                                 searchList[0]->EndGroup());
      if(it != searchList[0]->EndGroup()) return *it;
      if(opts == Traverse) {
        for(int i = 0; i < searchList[0]->Objects(); i++) {
          searchList.push_back(&searchList[0]->Object(i));
        }
      }
      searchList.erase(searchList.begin());
    }

    string msg = "Unable to find PVL group [" + name + "]";
    if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  /**
   * Finds a keyword in the current PvlObject, or deeper inside
   * other PvlObjects and PvlGroups within this one. Note: This
   * member has the same name as the PvlContainer and hides those
   * implementations, but with the using statement the parents
   * FindKeyword members ar made visible. Note: If more than one
   * occurance of a Keyword appears below this Object no
   * guarantee is made as to which one is returned.
   *
   * @param kname The name of the keyword to look for.
   * @param opts The FindOptions option (None or Traverse).
   *
   * @return The keyword sought
   *
   * @throws IException
   */
  PvlKeyword &PvlObject::FindKeyword(const std::string &kname,
                                     FindOptions opts) {

    // Call the parent's version if they don't want to dig deeper
    if(opts == None) return FindKeyword(kname);

    // Search this PvlObject, and all PvlObjects and PvlContainers within
    // it for the first occurrence of the requested keyword.
    vector<PvlObject *> searchList;
    searchList.push_back(this);

    while(searchList.size() > 0) {
      PvlKeywordIterator it =
        searchList[0]->FindKeyword(kname, searchList[0]->Begin(),
                                   searchList[0]->End());
      if(it != searchList[0]->End()) {
        return *it;
      }

      // See if the keyword is inside a Group of this Object
      for(int g = 0; g < searchList[0]->Groups(); g++) {
        PvlKeywordIterator it =
          searchList[0]->Group(g).FindKeyword(kname,
                                              searchList[0]->Group(g).Begin(),
                                              searchList[0]->Group(g).End());
        if(it != searchList[0]->Group(g).End()) {
          return *it;
        }
      }

      // It's not in this Object or any Groups in this Object, so
      // add all Objects inside this Object to the search list
      for(int i = 0; i < searchList[0]->Objects(); i++) {
        searchList.push_back(&searchList[0]->Object(i));
      }

      // This Object has been searched to remove it from the list
      searchList.erase(searchList.begin());
    }

    // No where else to look for the Keyword so throw an error
    string msg = "Unable to find PVL keyword [" + kname + "]";
    if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  /**
   * See if a keyword is in the current PvlObject, or deeper inside
   * other PvlObjects and PvlGroups within this one. Note: This
   * member has the same name as the PvlContainer and hides those
   * implementations, but with the using statement the parents
   * FindKeyword members ar made visible.
   *
   * @param kname The name of the keyword to look for.
   * @param opts The FindOptions option (None or Traverse).
   *
   * @return True if the Keyword exists False otherwise.
   */
  bool PvlObject::HasKeyword(const std::string &kname,
                             FindOptions opts) const {

    // Call the parent's version if they don't want to dig deeper
    if(opts == None) return HasKeyword(kname);

    // Search this PvlObject, and all PvlObjects and PvlContainers within
    // it for the first occurrence of the requested keyword.
    vector<const PvlObject *> searchList;
    searchList.push_back(this);

    while(searchList.size() > 0) {
      ConstPvlKeywordIterator it =
        searchList[0]->FindKeyword(kname, searchList[0]->Begin(),
                                   searchList[0]->End());
      if(it != searchList[0]->End()) {
        return true;
      }

      // See if the keyword is inside a Group of this Object
      for(int g = 0; g < searchList[0]->Groups(); g++) {
        ConstPvlKeywordIterator it =
          searchList[0]->Group(g).FindKeyword(kname,
                                              searchList[0]->Group(g).Begin(),
                                              searchList[0]->Group(g).End());

        if(it != searchList[0]->Group(g).End()) {
          return true;
        }
      }

      // It's not in this Object or any Groups in this Object, so
      // add all Objects inside this Object to the search list
      for(int i = 0; i < searchList[0]->Objects(); i++) {
        searchList.push_back(&searchList[0]->Object(i));
      }

      // This Object has been searched to remove it from the list
      searchList.erase(searchList.begin());
    }
    return false;
  }


  /**
   * Find an object within the current PvlObject.
   *
   * @param name The object name to look for.
   * @param opts The FindOptions option (None or Traverse).
   *
   * @return The PvlObject sought for.
   *
   * @throws IException
   */
  PvlObject &PvlObject::FindObject(const std::string &name,
                                   PvlObject::FindOptions opts) {
    vector<PvlObject *> searchList;
    searchList.push_back(this);

    while(searchList.size() > 0) {
      PvlObjectIterator it =
        searchList[0]->FindObject(name,
                                  searchList[0]->BeginObject(),
                                  searchList[0]->EndObject());
      if(it != searchList[0]->EndObject()) return *it;
      if(opts == Traverse) {
        for(int i = 0; i < searchList[0]->Objects(); i++) {
          searchList.push_back(&searchList[0]->Object(i));
        }
      }
      searchList.erase(searchList.begin());
    }

    string msg = "Unable to find PVL object [" + name + "]";
    if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  /**
   * Find an object within the current PvlObject.
   *
   * @param name The object name to look for.
   * @param opts The FindOptions option (None or Traverse).
   *
   * @return The PvlObject sought for.
   *
   * @throws IException
   */
  const PvlObject &PvlObject::FindObject(const std::string &name,
                                         FindOptions opts) const {
    vector<const PvlObject *> searchList;
    searchList.push_back(this);

    while(searchList.size() > 0) {
      ConstPvlObjectIterator it =
        searchList[0]->FindObject(name,
                                  searchList[0]->BeginObject(),
                                  searchList[0]->EndObject());

      if(it != searchList[0]->EndObject()) {
        return *it;
      }

      if(opts == Traverse) {
        for(int i = 0; i < searchList[0]->Objects(); i++) {
          searchList.push_back(&searchList[0]->Object(i));
        }
      }

      searchList.erase(searchList.begin());
    }

    string msg = "Unable to find PVL object [" + name + "]";

    if(p_filename.size() > 0) {
      msg += " in file [" + p_filename + "]";
    }

    throw IException(IException::Unknown, msg, _FILEINFO_);
  }


  /**
   * Remove an object from the current PvlObject.
   *
   * @param name The name of the PvlObject to remove.
   *
   * @throws IException
   */
  void PvlObject::DeleteObject(const std::string &name) {
    PvlObjectIterator key = FindObject(name, BeginObject(), EndObject());
    if(key == EndObject()) {
      string msg = "Unable to find PVL object [" + name + "] in " + Type() +
                   " [" + Name() + "]";
      if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    p_objects.erase(key);
  }


  /**
   * Remove an object from the current PvlObject.
   *
   * @param index The index of the PvlObject to remove.
   *
   * @throws IException
   */
  void PvlObject::DeleteObject(const int index) {
    if(index >= (int)p_objects.size() || index < 0) {
      string msg = "The specified index is out of bounds in PVL " + Type() +
                   " [" + Name() + "]";
      if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    PvlObjectIterator key = BeginObject();
    for(int i = 0; i < index; i++)  key++;

    p_objects.erase(key);
  }


  /**
   * Remove a group from the current PvlObject.
   *
   * @param name The name of the PvlGroup to remove.
   *
   * @throws IException
   */
  void PvlObject::DeleteGroup(const std::string &name) {
    PvlGroupIterator key = FindGroup(name, BeginGroup(), EndGroup());
    if(key == EndGroup()) {
      string msg = "Unable to find PVL group [" + name + "] in " + Type() +
                   " [" + Name() + "]";
      if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    p_groups.erase(key);
  }


  /**
   * Remove a group from the current PvlObject.
   *
   * @param index The index of the PvlGroup to remove.
   *
   * @throws IException
   */
  void PvlObject::DeleteGroup(const int index) {
    if(index >= (int)p_groups.size() || index < 0) {
      string msg = "The specified index is out of bounds in PVL " + Type() +
                   " [" + Name() + "]";
      if(p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    PvlGroupIterator key = BeginGroup();
    for(int i = 0; i < index; i++)  key++;

    p_groups.erase(key);
  }


  /**
   * Return the group at the specified index.
   *
   * @param index The index of the group.
   *
   * @return The PvlGroup sought for.
   *
   * @throws IException
   */
  Isis::PvlGroup &PvlObject::Group(const int index) {
    if(index < 0 || index >= (int)p_groups.size()) {
      string msg = Message::ArraySubscriptNotInRange(index);
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return p_groups[index];
  }


  /**
   * Return the group at the specified index.
   *
   * @param index The index of the group.
   *
   * @return The PvlGroup sought for.
   *
   * @throws IException
   */
  const Isis::PvlGroup &PvlObject::Group(const int index) const {
    if(index < 0 || index >= (int)p_groups.size()) {
      string msg = Message::ArraySubscriptNotInRange(index);
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return p_groups[index];
  }

  /**
   * Return the object at the specified index.
   *
   * @param index The index of the object.
   *
   * @return The PvlObject sought for.
   *
   * @throws IException::Programmer
   */
  PvlObject &PvlObject::Object(const int index) {
    if(index < 0 || index >= (int)p_objects.size()) {
      string msg = Message::ArraySubscriptNotInRange(index);
      throw IException(Isis::IException::Programmer, msg, _FILEINFO_);
    }

    return p_objects[index];
  }

  /**
   * Return the object at the specified index.
   *
   * @param index The index of the object.
   *
   * @return The PvlObject sought for.
   *
   * @throws IException::Programmer
   */
  const PvlObject &PvlObject::Object(const int index) const {
    if(index < 0 || index >= (int)p_objects.size()) {
      string msg = Message::ArraySubscriptNotInRange(index);
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return p_objects[index];
  }


  /**
   * Outputs the PvlObject data to a specified output stream.
   *
   * @param os The output stream to write to.
   * @param object The PvlObject to send to the output stream.
   */
  ostream &operator<<(std::ostream &os, PvlObject &object) {

    // Set up a Formatter
    bool removeFormatter = false;
    if(object.GetFormat() == NULL) {
      object.SetFormat(new PvlFormat());
      removeFormatter = true;
    }

    Isis::PvlObject outTemplate("DEFAULT");
    if(object.HasFormatTemplate())
      outTemplate = *(Isis::PvlObject *)object.FormatTemplate();

    // Look for and process all include files and remove duplicates
    Isis::PvlObject newTemp(outTemplate.Name());

    // Make sure the new template has all the original's comments
    for(int i = 0; i < outTemplate.Comments(); i++) {
      newTemp.AddComment(outTemplate.Comment(i));
    }

    // Include files take precedence to all other objects and groups
    for(int i = 0; i < outTemplate.Keywords(); i++) {
      if(outTemplate[i].IsNamed("Isis:PvlTemplate:File")) {
        string filename = outTemplate[i];
        Isis::FileName file(filename);
        if(!file.fileExists()) {
          string message = "Could not open the following PVL template file: ";
          message += filename;
          throw IException(IException::Io, message, _FILEINFO_);
        }

        Isis::Pvl include(file.expanded());

        for(int j = 0; j < include.Keywords(); j++) {
          if(!newTemp.HasKeyword(include[j].Name()))
            newTemp.AddKeyword(include[j]);
        }

        for(int j = 0; j < include.Objects(); j++) {
          if(!newTemp.HasObject(include.Object(j).Name()))
            newTemp.AddObject(include.Object(j));
        }

        for(int j = 0; j < include.Groups(); j++) {
          if(!newTemp.HasGroup(include.Group(j).Name()))
            newTemp.AddGroup(include.Group(j));
        }
      }
      // If it is not an include file keyword add it in place
      else if(!newTemp.HasKeyword(outTemplate[i].Name()))
        newTemp.AddKeyword(outTemplate[i]);
    }

    // Copy over the objects
    for(int i = 0; i < outTemplate.Objects(); i++) {
      if(!newTemp.HasObject(outTemplate.Object(i).Name()))
        newTemp.AddObject(outTemplate.Object(i));
    }

    // Copy over the groups
    for(int i = 0; i < outTemplate.Groups(); i++) {
      if(!newTemp.HasGroup(outTemplate.Group(i).Name()))
        newTemp.AddGroup(outTemplate.Group(i));
    }

    outTemplate = newTemp;

    // Write out comments for this Object that were in the template
    if(outTemplate.Comments() > 0) {
      for(int k = 0; k < outTemplate.Comments(); k++) {
        for(int l = 0; l < object.Indent(); l++) os << " ";
        os << outTemplate.Comment(k) << object.GetFormat()->FormatEOL();
      }
      //os << object.GetFormat()->FormatEOL();
    }

    // Output the object comments and name
    os << object.GetNameKeyword() << object.GetFormat()->FormatEOL();
    object.SetIndent(object.Indent() + 2);

    // Output the keywords in this Object
    if(object.Keywords() > 0) {
      os << (Isis::PvlContainer &) object << object.GetFormat()->FormatEOL();
    }

    // This number keeps track of the number of objects have been written
    int numObjects = 0;

    // Output the Objects within this Object using the format template
    for(int i = 0; i < outTemplate.Objects(); i++) {
      for(int j = 0; j < object.Objects(); j++) {
        if(outTemplate.Object(i).Name() != object.Object(j).Name()) continue;
        if(j == 0 && object.Keywords() > 0)
          os << object.GetFormat()->FormatEOL();

        object.Object(j).SetIndent(object.Indent());
        object.Object(j).SetFormatTemplate(outTemplate.Object(i));
        object.Object(j).SetFormat(object.GetFormat());
        os << object.Object(j) << object.GetFormat()->FormatEOL();
        object.Object(j).SetFormat(NULL);
        object.Object(j).SetIndent(0);

        if(++numObjects < object.Objects())
          os << object.GetFormat()->FormatEOL();
      }
    }

    // Output the Objects within this Object that were not included in the
    // format template pvl
    for(int i = 0; i < object.Objects(); i++) {
      if(outTemplate.HasObject(object.Object(i).Name())) continue;
      if(i == 0 && object.Keywords() > 0)
        os << object.GetFormat()->FormatEOL();

      object.Object(i).SetIndent(object.Indent());
      object.Object(i).SetFormat(object.GetFormat());
      os << object.Object(i) << object.GetFormat()->FormatEOL();
      object.Object(i).SetFormat(NULL);
      object.Object(i).SetIndent(0);

      if(++numObjects < object.Objects())
        os << object.GetFormat()->FormatEOL();

    }

    // This number keeps track of the number of Groups that have been written
    int numGroups = 0;

    // Output the Groups within this Object using the format template
    for(int i = 0; i < outTemplate.Groups(); i++) {
      for(int j = 0; j < object.Groups(); j++) {
        if(outTemplate.Group(i).Name() != object.Group(j).Name()) continue;
        if((numGroups == 0) &&
            (object.Objects() > 0 || object.Keywords() > 0))
          os << object.GetFormat()->FormatEOL();

        object.Group(j).SetIndent(object.Indent());
        object.Group(j).SetFormatTemplate(outTemplate.Group(i));
        object.Group(j).SetFormat(object.GetFormat());
        os << object.Group(j) << object.GetFormat()->FormatEOL();
        object.Group(j).SetFormat(NULL);
        object.Group(j).SetIndent(0);
        if(++numGroups < object.Groups()) os << object.GetFormat()->FormatEOL();
      }
    }

    // Output the groups that were not in the format template
    for(int i = 0; i < object.Groups(); i++) {
      if(outTemplate.HasGroup(object.Group(i).Name())) continue;
      if((numGroups == 0) &&
          (object.Objects() > 0 || object.Keywords() > 0))
        os << object.GetFormat()->FormatEOL();

      object.Group(i).SetIndent(object.Indent());
      object.Group(i).SetFormat(object.GetFormat());
      os << object.Group(i) << object.GetFormat()->FormatEOL();
      object.Group(i).SetFormat(NULL);
      object.Group(i).SetIndent(0);

      if(++numGroups < object.Groups())
        os << object.GetFormat()->FormatEOL();
    }

    // Output the end of the object
    object.SetIndent(object.Indent() - 2);
    for(int i = 0; i < object.Indent(); i++) os << " ";
    os << object.GetFormat()->FormatEnd("End_Object", object.GetNameKeyword());

    if(removeFormatter) {
      delete object.GetFormat();
      object.SetFormat(NULL);
    }

    return os;
  }


  /**
   * This method reads a PvlObject from the input stream
   *
   */
  std::istream &operator>>(std::istream &is, PvlObject &result) {
    PvlKeyword termination("EndObject");

    PvlKeyword errorKeywords[] = {
      PvlKeyword("EndGroup")
    };

    PvlKeyword readKeyword;

    istream::pos_type beforeKeywordPos = is.tellg();
    is >> readKeyword;

    if(readKeyword != PvlKeyword("Object")) {
      if(is.eof() && !is.bad()) {
        is.clear();
      }

      is.seekg(beforeKeywordPos, ios::beg);

      string msg = "Expected PVL keyword named [Object], found keyword named [";
      msg += readKeyword.Name();
      msg += "]";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if(readKeyword.Size() == 1) {
      result.SetName(readKeyword[0]);
    }
    else {
      is.seekg(beforeKeywordPos, ios::beg);

      string msg = "Expected a single value for PVL object name, found [(";

      for(int i = 0; i < readKeyword.Size(); i++) {
        if(i != 0) msg += ", ";

        msg += readKeyword[i];
      }

      msg += ")]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    for(int comment = 0; comment < readKeyword.Comments(); comment++) {
      result.AddComment(readKeyword.Comment(comment));
    }

    readKeyword = PvlKeyword();
    beforeKeywordPos = is.tellg();

    is >> readKeyword;
    while(readKeyword != termination) {
      for(unsigned int errorKey = 0;
          errorKey < sizeof(errorKeywords) / sizeof(PvlKeyword);
          errorKey++) {
        if(readKeyword == errorKeywords[errorKey]) {
          if(is.eof() && !is.bad()) {
            is.clear();
          }

          is.seekg(beforeKeywordPos, ios::beg);

          string msg = "Unexpected [";
          msg += readKeyword.Name();
          msg += "] in PVL Object [";
          msg += result.Name();
          msg += "]";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }

      if(readKeyword == PvlKeyword("Group")) {
        is.seekg(beforeKeywordPos);
        PvlGroup newGroup;
        is >> newGroup;
        result.AddGroup(newGroup);
      }
      else if(readKeyword == PvlKeyword("Object")) {
        is.seekg(beforeKeywordPos);
        PvlObject newObject;
        is >> newObject;
        result.AddObject(newObject);
      }
      else {
        result.AddKeyword(readKeyword);
      }

      readKeyword = PvlKeyword();
      beforeKeywordPos = is.tellg();

      if(is.good()) {
        is >> readKeyword;
      }
      else {
        // eof found
        break;
      }
    }

    if(readKeyword != termination) {
      if(is.eof() && !is.bad()) {
        is.clear();
      }

      is.seekg(beforeKeywordPos, ios::beg);

      string msg = "PVL Object [" + result.Name();
      msg += "] EndObject not found before end of file";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return is;
  }


  //! This is an assignment operator
  const PvlObject &PvlObject::operator=(const PvlObject &other) {
    this->PvlContainer::operator=(other);

    p_objects = other.p_objects;
    p_groups = other.p_groups;

    return *this;
  }

  /**
   * Validate a PvlObject, comparing against corresponding Template PvlObject.
   * If the Objects are nested, it will recursively validate the PvlObject.
   *
   * Template PvlObject has the format:
   * Object = (objectName, optional/required)
   *
   * @author Sharmila Prasad (9/22/2010)
   *
   * @param pvlObj- PvlObject to be validated
   */
  void PvlObject::ValidateObject(PvlObject & pPvlObj)
  {
    // Validate the current object
    int iObjSize = Objects();

    for(int i=0; i<iObjSize; i++) {
      PvlObject & pvlTmplObj = Object(i);

      string sObjName = pvlTmplObj.Name();
      bool bObjFound = false;

      // Pvl contains the Object Name
      if(pPvlObj.HasObject(sObjName)) {
        PvlObject & pvlObj = pPvlObj.FindObject(sObjName);
        pvlTmplObj.ValidateObject(pvlObj);
        if(pvlObj.Objects()==0 && pvlObj.Groups()==0 && pvlObj.Keywords()==0) {
          pPvlObj.DeleteObject(pvlObj.Name());
        }
        bObjFound = true;
      }
      else {
        string sOption = sObjName + "__Required";
        bObjFound = true; // optional is the default
        if(pvlTmplObj.HasKeyword(sOption)) {
          PvlKeyword pvlKeyOption = pvlTmplObj.FindKeyword(sOption);
          if(pvlKeyOption[0] == "true") { // Required is true
            bObjFound = false;
          }
        }
      }
      if (bObjFound == false) {
        string sErrMsg = "Object \"" + sObjName + "\" Not Found in the Template File\n";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }
    }

    // Validate the Groups in the current object
    int iTmplGrpSize = Groups();
    for(int i=0; i<iTmplGrpSize; i++) {
      PvlGroup & pvlTmplGrp = Group(i);
      bool bGrpFound = false;
      string sGrpName = pvlTmplGrp.Name();

      // Pvl contains the Object Name
      if(pPvlObj.HasGroup(sGrpName)) {
        PvlGroup & pvlGrp = pPvlObj.FindGroup(sGrpName);
        pvlTmplGrp.ValidateGroup(pvlGrp);
        if(pvlGrp.Keywords()==0) {
          pPvlObj.DeleteGroup(pvlGrp.Name());
        }
        bGrpFound = true;
      }
      else {
        bGrpFound = true;
        string sOption = sGrpName + "__Required";
        if(pvlTmplGrp.HasKeyword(sOption)) {
          PvlKeyword pvlKeyOption = pvlTmplGrp.FindKeyword(sOption);
          if(pvlKeyOption[0] == "true") { // Required is true
            bGrpFound = false;
          }
        }
      }
      if (bGrpFound == false) {
        string sErrMsg = "Group \"" + sGrpName + "\" Not Found in the Template File\n";
        throw IException(IException::User, sErrMsg, _FILEINFO_);
      }
    }

    // Validate the Keywords in the current Object
    ValidateAllKeywords((PvlContainer &)pPvlObj);
  }

} // end namespace isis
