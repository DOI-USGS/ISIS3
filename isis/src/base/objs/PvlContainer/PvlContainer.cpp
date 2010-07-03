/**
 * @file
 * $Revision: 1.12 $
 * $Date: 2010/04/14 00:21:53 $
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

#include <algorithm>

#include "PvlContainer.h"
#include "Pvl.h"
#include "Filename.h"
#include "iException.h"
#include "iException.h"
#include "Message.h"
#include "PvlFormat.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a PvlContainer object with a type.
   * @param type The type of the container.
   */
  PvlContainer::PvlContainer(const std::string &type) {
    Init();
    p_name.SetName(type);
  }


  /**
   * Constructs a PvlContainer object with a keyword name and a container name.
   * @param type The type of container.
   * @param name The name of the container.
   */
  PvlContainer::PvlContainer(const std::string &type, const std::string &name) {
    Init();
    p_name.SetName(type);
    SetName(name);
  }
  
  
  PvlContainer::PvlContainer(const PvlContainer & other)
  {
    *this = other;
  }



  //! Sets the filename to blank.
  void PvlContainer::Init () {
    p_filename = "";
    p_formatTemplate = NULL;
  }


  /**
   * Find a keyword with a specified name.
   * @param name The name of the keyword to look for.
   * @return The PvlKeyword object.
   * @throws iException::Pvl The keyword doesn't exist.
   */
  Isis::PvlKeyword &PvlContainer::FindKeyword(const std::string &name) {
    PvlKeywordIterator key = FindKeyword(name,Begin(),End());
    if (key == End()) {
      string msg = "Keyword [" + name + "] does not exist in [" + 
                   Type() + " = " + Name() + "]";
      if (p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg,_FILEINFO_);
    }

    return *key;
  }

  /**
   * Find a keyword with a specified name.
   * @param name The name of the keyword to look for.
   * @return The PvlKeyword object.
   * @throws iException::Pvl The keyword doesn't exist.
   */
  const Isis::PvlKeyword &PvlContainer::FindKeyword(const std::string &name) const {
    ConstPvlKeywordIterator key = FindKeyword(name,Begin(),End());
    if (key == End()) {
      string msg = "Keyword [" + name + "] does not exist in [" + 
                   Type() + " = " + Name() + "]";
      if (p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg,_FILEINFO_);
    }

    return *key;
  }

  /**
   * Remove a specified keyword.
   * @param name The name of the keyword to remove.
   * @throws iException::Pvl Keyword doesn't exist.
   */
  void PvlContainer::DeleteKeyword (const std::string &name) {
    PvlKeywordIterator key = FindKeyword(name,Begin(),End());
    if (key == End()) {
      string msg = "Keyword [" + name + "] does not exist in [" + 
                   Type() + " = " + Name() + "]";
      if (p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg,_FILEINFO_);
    }

    p_keywords.erase(key);
  }


  /**
   * Remove the specified keyword.
   * @param index The index of the keyword to remove.
   * @throws iException::Pvl Keyword doesn't exist.
   */
  void PvlContainer::DeleteKeyword (const int index) {
    if (index >= (int)p_keywords.size() || index < 0) {
      string msg = "The specified index is out of bounds in [" + 
                   Type() + " = " + Name() + "]";
      if (p_filename.size() > 0) msg += " in file [" + p_filename + "]";
      throw Isis::iException::Message(Isis::iException::Pvl,msg,_FILEINFO_);
    }

    PvlKeywordIterator key = Begin();
    for(int i=0; i<index; i++) key++;

    p_keywords.erase(key);
  }


  /**
   * Removes keywords from the container that have BOTH the same name and value. 
   * 
   * @return bool True if one or more keywords were deleted; False if no keywords 
   *         were deleted.
   */
  bool PvlContainer::CleanDuplicateKeywords() {
    bool keywordDeleted = false;

    for (unsigned int index = 0; index < p_keywords.size(); index ++) {
      PvlKeyword & current = p_keywords[index];

      for (PvlKeywordIterator key = Begin() + index + 1; key < End(); key ++) {
        if (current == *key) {
          p_keywords.erase(key);
          keywordDeleted = true;
        }
      }
    }
    
    return keywordDeleted;
  }


  /**
   * Check to see if a keyword exists.
   * @param name The name of the keyword to check for.
   * @return True if the keyword exists, false if it doesn't.
   */
  bool PvlContainer::HasKeyword(const std::string &name) const {
    ConstPvlKeywordIterator key = FindKeyword(name,Begin(),End());
    if (key == End()) return false;
    return true;
  }


  /**
   * Return the PvlKeyword object at the specified index.
   * @param index The index to use.
   * @return The PvlKeyword at the specified index.
   * @throws iException::Message The index is out of bounds.
   */
  Isis::PvlKeyword &PvlContainer::operator[](const int index) { 
    if (index < 0 || index >= (int)p_keywords.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange (index);
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return *(p_keywords.begin()+index); 
  };


  /**
   * Return the PvlKeyword object at the specified index.
   * @param index The index to use.
   * @return The PvlKeyword at the specified index.
   * @throws iException::Message The index is out of bounds.
   */
  const Isis::PvlKeyword &PvlContainer::operator[](const int index) const {
    if (index < 0 || index >= (int)p_keywords.size()) {
      string msg = Isis::Message::ArraySubscriptNotInRange (index);
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return *(p_keywords.begin()+index); 
  }

  /**
   * Add a keyword to the PvlContainer object.
   * @param key The PvlKeyword object to add.
   * @param mode The enum InsertMode has two possible values, Append or Replace.
   * Use Append if you just want to add it to the end, Replace if you want to 
   * replace it.
   */
  void PvlContainer::AddKeyword(const Isis::PvlKeyword &key,
                                const InsertMode mode) { 
    if (mode == Append) {
      p_keywords.push_back(key);
    } else if (HasKeyword(key.Name())) {
      Isis::PvlKeyword &outkey = FindKeyword(key.Name());
      outkey = key;
    } else {
      p_keywords.push_back(key);
    }
  }

  /**
   * @brief Insert a keyword at the specified iterator position
   *  
   * This method provides the capability to insert a keyword at the specified 
   * iterator position.  The process follows the description of the STL vector 
   * definition along with all the caveats (e.g., invalidation of iterators upon
   * insert operations). 
   *  
   * This method will not perform any checks for the existance of the keyword. 
   * This could lead to multiple instances of the same keyword in the same 
   * container.  It is up to the caller to manage this issue. 
   * 
   * @param key Keyword to insert
   * @param pos Iterator position where to insert the new keyword
   * @return PvlContainer::PvlKeywordIterator Returns the position of the
   *          inserted keyword per the STL vector documentation.
   */
   PvlContainer::PvlKeywordIterator PvlContainer::AddKeyword(const Isis::PvlKeyword &key,
                                                             PvlKeywordIterator pos) { 
      return (p_keywords.insert(pos, key));
  }

  /**
   * Output the PvlContainer information.
   * 
   * @param os The preferred output stream.
   * @param container The PvlContainer object to output.
   */
  ostream& operator<<(std::ostream &os, PvlContainer &container) {

    // Set up a Formatter (This should not be necessary for a container because
    // Object or Group should have done this already, but just in case.
    bool removeFormatter = false;
    if (container.GetFormat() == NULL) {
      container.SetFormat(new PvlFormat());
      removeFormatter = true;
    }

    Isis::PvlContainer outTemplate("DEFAULT_TEMPLATE");
    if (container.HasFormatTemplate()) outTemplate = *(container.FormatTemplate());

    // Look for and process all include files inside the template
    Isis::PvlContainer newTemp(outTemplate.Type());

    // Include files take precedence over all other objects and groups
    for (int i=0; i<outTemplate.Keywords(); i++) {
      if (outTemplate[i].IsNamed("Isis:PvlTemplate:File")) {
        string filename = outTemplate[i];
        Isis::Filename file(filename);
        if (!file.Exists()) {
          string message = "Could not open the template file [" + filename + "]";
          throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
        }
        Isis::Pvl include(file.Expanded());

        for (int j=0; j<include.Keywords(); j++) {
          if (!newTemp.HasKeyword(include[j].Name()))
            newTemp.AddKeyword(include[j]);
        }
      }
      // If it is not an include file keyword add it in place
      else if (!newTemp.HasKeyword(outTemplate[i].Name())) {
        newTemp.AddKeyword(outTemplate[i]);
      }
    }

    outTemplate = newTemp;

    // Figure out the longest keyword
    unsigned int width = 0;
    for (int i=0; i<container.Keywords(); i++) {
      if (container[i].Name().length() > width) width = container[i].Name().length();
    }

    // This number keeps track of the number of keywords written
    int numKeywords = 0;

    // Write out the container using the output format template
    for (int i=0; i<outTemplate.Keywords(); i++) {
      for (int j=0; j<container.Keywords(); j++) {
        if (outTemplate[i].Name() != container[j].Name()) continue;
        container[j].SetIndent(container.Indent());
        container[j].SetWidth(width);
        container[j].SetFormat(container.GetFormat());
        // Add a blank line before keyword comments
        if (outTemplate[i].Comments() + container[j].Comments() > 0) os << container.GetFormat()->FormatEOL();
        if (outTemplate[i].Comments() > 0) {
          for (int k=0; k<outTemplate[i].Comments(); k++) {
            for (int l=0; l<outTemplate[i].Indent()+container[j].Indent(); l++) os << " ";
              os << outTemplate[i].Comment(k) << container.GetFormat()->FormatEOL();
          }
        }
        os << container[j];
        container[j].SetFormat(NULL);
        container[j].SetIndent(0);
        container[j].SetWidth(0);
        if (++numKeywords < container.Keywords()) {
//          if (j+1 < container.Keywords() && container[j+1].Comments() > 0) os << container.GetFormat()->FormatEOL();
          os << container.GetFormat()->FormatEOL();
        }
      }
    }

    // Output the keywords in the container that were not specified in the template
    for (int i=0; i<container.Keywords(); i++) {
      if (outTemplate.HasKeyword(container[i].Name())) continue;
      container[i].SetIndent(container.Indent());
      container[i].SetWidth(width);
      container[i].SetFormat(container.GetFormat());
      os << container[i];
      container[i].SetFormat(NULL);
      container[i].SetIndent(0);
      container[i].SetWidth(0);
      if (++numKeywords < container.Keywords()) {
        if (i+1 < container.Keywords() && container[i+1].Comments() > 0) os << container.GetFormat()->FormatEOL();
        os << container.GetFormat()->FormatEOL();
      }
    }

    if (removeFormatter) {
      delete container.GetFormat();
      container.SetFormat(NULL);
    }

    return os;
  }


  /**
   * Find the index of a keyword, using iterators.
   * @param name The name of the keyword.
   * @param beg The beginning iterator.
   * @param end The ending iterator.
   * @return The keyword index.
   */
  PvlContainer::PvlKeywordIterator PvlContainer::FindKeyword(const std::string &name,
                                 PvlContainer::PvlKeywordIterator beg,
                                 PvlContainer::PvlKeywordIterator end) {
    PvlKeyword temp(name);
    return find(beg,end,temp);
  };


  /**
   * Find the index of a keyword, using iterators.
   * @param name The name of the keyword.
   * @param beg The beginning iterator.
   * @param end The ending iterator.
   * @return The keyword index.
   */
  PvlContainer::ConstPvlKeywordIterator PvlContainer::FindKeyword(const std::string &name,
                                 PvlContainer::ConstPvlKeywordIterator beg,
                                 PvlContainer::ConstPvlKeywordIterator end) const {
    PvlKeyword temp(name);
    return find(beg,end,temp);
  };
  
  
  //! This is an assignment operator
  const PvlContainer & PvlContainer::operator=(const PvlContainer & other)
  {
    p_filename = other.p_filename;
    p_name = other.p_name;
    p_keywords = other.p_keywords;
    p_formatTemplate = other.p_formatTemplate;
    
    return *this;
  }

} // end namespace isis
