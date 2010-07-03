#if !defined(MdisEdrKeys_h)
#define MdisEdrKeys_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.5 $
 * $Date: 2008/09/22 20:46:27 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "Pvl.h"
#include "CollectorMap.h"
#include "MdisGeometry.h"
#include "OriginalLabel.h"
#include "iException.h"

namespace Isis {

  /**
   * @brief MDIS EDR keyword container class
   * 
   * This class ingests a PDS EDR label and stages all keywords found in a
   * table.  Keywords can be added through the Pvl class or updated explicitly
   * by adding a PvlKeyword.
   * 
   * Any add or update operation will replace existing completely so no more
   * than on keyword will exist in the table at a time.
   * 
   * Keyword names are not case sensitive.  Also, any Object or Group hierarchy
   * is not honored in this class.  Meaning that if a keyword of the same name
   * exists in more than one Object or Group, only the last occurring one is
   * retained.
   * 
   * @ingroup Utility
   * @author 2007-08-14 Kris Becker
   * @history 2008-09-22 Kris Becker - corrected formatting of units that were 
   *          missing in some cases where the first value units is not defined.
   */
class MdisEdrKeys {
    public:
      /** Benign constuctor */
      MdisEdrKeys() { }
      /** Constuctor adds a Pvl label to the table */
      MdisEdrKeys(Pvl &edrlab) : _edrLabel(edrlab), _keys() { 
        LoadKeys(edrlab, _keys); 
      }

      /**
       * @brief Add a Pvl file (label) to the table
       * 
       * This constuctor will ingest a Pvl labelled file to the table.  It can
       * be any Pvl file that is supported by the Pvl class, including most all
       * PDS formatted labels.  This particular interface assumes a MDIS PDS EDR
       * label as its parameter.
       * 
       * @param edrfile  File containing the PDS EDR label
       */
      MdisEdrKeys(const std::string &edrfile) {
        _edrLabel = Pvl(edrfile);
        LoadKeys(_edrLabel, _keys);
      }

      /**
       * @brief Construct from an Isis Cube file
       * 
       * This constructor will ingest an ISIS cube file label.  Note that there
       * is no regard for the Object/Group structure so all keywords are
       * included and keywords that repeat in other Objects or Groups replace
       * any existing definition.
       * 
       * @param cube  A valid ISIS Cube object with a label
       */
      MdisEdrKeys(Cube &cube) {
        _edrLabel = OriginalLabel(cube.Filename()).ReturnLabels();
        LoadKeys(_edrLabel, _keys);
      }

      /** Destroys the object */
      virtual ~MdisEdrKeys() { }

      /**
       * @brief Returns the number of keywords in the container
       * 
       * @return int Number of keys in container
       */
      inline int size() const { return _keys.size(); }

      /**
       * @brief Adds a keyword to the container directly
       * 
       * This method is provided for adding keywords directly to the contained
       * that may not be contained within a Pvl object.
       * 
       * @param key PvlKeyword to add to the container
       * @param name  Optional name other than what the keyword is named
       */
      void add(const PvlKeyword &key, const std::string &name = "") {
        if (name.empty()) {
          _keys.add(key.Name(), key);
        }
        else {
          _keys.add(name, key);
        }
        return;
      }

      /**
       * @brief Returns the nth keyword in the container
       * 
       * This method allows one to retrieve the nth keyword in the container as
       * a means to iterate through the complete contents for direct
       * interogation/use.
       * 
       * If the keyword at specified index does not exist, an exception is
       * thrown.
       * 
       * @param index Index of the keyword ranging from 0 to size()-1
       * 
       * @return const PvlKeyword&  Returns a reference to the keyword at index
       */
      const PvlKeyword &operator[](const int index) const throw (iException &) { 
        return (_keys.getNth(index));
      }

      /**
       * @brief Return the specified keyword by name
       * 
       * Retrieves a keyword by name without regard for case.  An exception is
       * thrown if it does not exist.
       * 
       * @param name  Name of keyword to retrieve
       * 
       * @return PvlKeyword& Reference to named keyword
       */
      PvlKeyword &get(const std::string &name) throw (iException &) {
        return (_keys.get(name));
      }

      /**
       * @brief Returns the specified keyword by name
       * 
       * Retrieves a keyword by name without regard for case.  An exception is
       * thrown if it does not exist.
       * 
       * @param name  Name of keyword to retrieve
       * 
       * @return const PvlKeyword& Returns a const reference to the named
       *         keyword
       */
      const PvlKeyword &get(const std::string &name) const throw (iException &) {
        return (_keys.get(name));
      } 

      /**
       * @brief Replaces or adds keywords in a Pvl class
       * 
       * This method will add all the keywords found in the Pvl keys object to
       * the container.  Any keywords that already exist in the container are
       * replaced with the new one.
       * 
       * @param keys Pvl class containing the keywords to add
       */
      void updateKeys(Pvl &keys) {
        LoadKeys(keys, _keys);
      }

      /**
       * @brief Return a formatted list of keyword values
       * 
       * This method takes in a list of named keywords and formats them
       * according to PDS standards in a comma delineated string.  This is the
       * defined format of the team for its population of PDS EDR label
       * keywords.
       * 
       * If any one of the keywords is not found, an exception is thrown.
       * 
       * @param keylist List of keywords to extract
       * @param NAstr String used to specify no value (used mainly so a unit
       *              qualifer is not added to the value)
       * @return std::string Returns a comma delineated string of keyword values
       */
      std::string extract(const std::vector<std::string> &keylist, 
                          const std::string &NAstr, PvlGroup *group = 0) {
        std::ostringstream out;

        std::string loopSep("");
        int nbad(0);
        for (unsigned int i = 0 ; i < keylist.size() ; i++) {
          iString keyname(keylist[i]);
          keyname.Trim(" \n\t");
          try {
            PvlKeyword &key = _keys.get(keyname);
            if (group) group->AddKeyword(key);
            if ((key.Size() == 0) || (key.IsNull())) {
              out << loopSep << "NULL";
            }
            else if (key.Size() == 1) {
              out << loopSep << key[0] << formatUnit(key.Unit(0));
            }
            else {
              out << loopSep << "(";
              std::string vsep("");
              for (int iv = 0 ; iv < key.Size() ; iv++) {
                 out << vsep << key[iv];
                 if (key[iv] != NAstr) {
                   out << formatUnit(key.Unit(iv));
                 }
                 vsep = ",";
              }
              out << ")";
            }
            loopSep = ";";
          } catch (iException &ie) {
            nbad++;
            std::string mess = "Keyword \"" + keyname + "\" does not exist!";
            iException::Message(iException::User, mess.c_str(), _FILEINFO_);
          }
        }

        // Check to see if all keywords are found
        if (nbad > 0) {
          std::string mess = "One or more keywords in list do not exist!";
          throw iException::Message(iException::User, mess.c_str(), _FILEINFO_);
        }

        return (out.str());
      }

  private:
//!< Define the keyword container for systematic processing
      typedef CollectorMap<std::string,PvlKeyword,NoCaseStringCompare> KeyList;
      Pvl     _edrLabel;  //!< Label used to populate the container
      KeyList _keys;      //!<  The container

      /**
       * @brief Parse the contents of a (generic) container
       * 
       * This method iterates through all keywords in the PvlContainer and adds
       * them KeyList.
       * 
       * @param p  Pvl container of keywords
       * @param keys Container to receive the keywords
       */
      void MapKeys(PvlContainer &p, KeyList &keys, 
                   const std::string prefix= "") {
        std::string prekey(prefix);
        if (!prefix.empty()) prekey += "/";
        PvlContainer::PvlKeywordIterator keyIter = p.Begin();
        for ( ; keyIter != p.End() ;  ++keyIter) {
          std::string keyname = prefix + keyIter->Name();
          PvlKeyword key = *keyIter;
          key.SetName(keyname);
          keys.add(keyname, key);
        }
        return;
      }

      /**
       * @brief Adds keywords in groups within an object
       * 
       * This metod iterates through all the keywords in the given group and
       * addes them to the keys container.
       * 
       * @param obj  Object containing Groups to add to container
       * @param keys  Container to add the group keywords to
       */
      void LoadGroups(PvlObject &obj, KeyList &keys) {
        PvlObject::PvlGroupIterator current = obj.BeginGroup();
        for ( ; current != obj.EndGroup() ; ++current) {
          MapKeys(*current, keys);
        } 
        return;
      }

      /**
       * @brief Add object keywords and all keywords in the Object hierachy
       * 
       * This method adds all keywords found in the Object and then any Groups
       * that exist in the object.
       * 
       * @param Object to extract keywords from and add to container
       * @param keys The container to add keywords to
       */
      void LoadKeys(PvlObject &obj, KeyList &keys, 
                    const std::string prefix = "") {
        MapKeys(obj, keys, prefix);   // Load object level keywords
        LoadGroups(obj, keys);  // Load all groups in this object
      
      //  Now load all the rest of the object keywords ingnoring
      //  all SUBFRAME[12345]_PARAMETERS since they are unsupported.
        PvlObject::PvlObjectIterator objIter = obj.BeginObject();
        for ( ; objIter != obj.EndObject() ; ++objIter) {
          iString objname(objIter->Name());
          objname.UpCase();
          std::string::size_type gotSubframe = objname.find("SUBFRAME");
          if (gotSubframe != std::string::npos) {
            LoadKeys(*objIter, keys, objIter->Name()); 
          }
          else {
            LoadKeys(*objIter, keys);
          }
        }
        return;
      }

      /**
       * @brief Returns a properly formated unit
       * 
       * 
       * @param unit value of unit.  If empty returns empty string otherwise 
       *             places "<" and ">" on the ends.
       * 
       * @return std::string  The formatted unit
       */
      std::string formatUnit(const std::string &unit) const {
        if (!unit.empty()) {
          return (" <"+unit+">");
        }
        return (unit);
      }
};

}     // namespace Isis
#endif

