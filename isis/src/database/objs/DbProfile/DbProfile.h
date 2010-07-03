#if !defined(DbProfile_h)
#define DbProfile_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $
 * $Date: 2008/09/06 06:47:48 $
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

#include "PvlKeyword.h"
#include "PvlContainer.h"
#include "CollectorMap.h"
#include "iException.h"

namespace Isis {

/**                                          
 * @brief A DbProfile is a container for access parameters to a database
 * 
 * This class provides a utility for (keyword) parameter management to access a
 * database system.  Profiles can be used to specify all parameters necessary
 * to apply to programatic interfaces to database software such as Qt's SQL
 * Module class, QSqlDatabase.
 * 
 * Using QSqlDatabase as an example, keywords such as \a User, \a Host, \a Port,
 * and \a Dbname can be stored in this object and retrieved easily.
 * 
 * It can be easily adapted to any database software API by externally managing
 * the contents of a configuration file.  See DbAccess for additional details on
 * how this scheme can be utilized.
 * 
 * @ingroup Utility
 * @author 2006-11-09 Kris Becker
 * 
 * @internal
 *   @history 2007-02-15 Kris Becker - Added key() method so one can iterate
 *            through all keys in the profile
 *   @history 2007-12-19 Kris Becker - Added getKeyList method for additional
 *            flexibility to class developers
 *   @history 2008-03-11 Kris Becker - Added more information to exception
 *            caught when attempting to get values from non-existant keys
 *   @history 2008-09-05 Kris Becker - Added the replace and remove methods to
 *            the class.
 */                                                                       
  class DbProfile {

    protected:
      /* Container for multi-valued keywords in profiles */
       typedef CollectorMap<std::string,PvlKeyword,NoCaseStringCompare> KeyList;

    public: 
      //  Constructors and Destructor
      DbProfile() : _name("Profile"), _keys() { }
      DbProfile(const std::string &name) : _name(name), _keys() { }
      DbProfile(const DbProfile &prof1,const DbProfile &prof2,
                const std::string &name= "");
      DbProfile(PvlContainer &pvl);


      /** Destructor ensures everything is cleaned up properly */
      virtual ~DbProfile () { }

      /**
       * @brief Reports if this is a valid profile
       * 
       * A valid profile is simply defined to contain keys.  If there are no
       * keys defined for the profile, it is deemed invalid.
       * 
       * @return bool True if the profile is valid, therefore containing keys
       */
      bool isValid() const { return (size() > 0); }

      /**
       * Reports the number of keywords in this user profile
       * 
       * @return int Number keywords found in profile
       */
      int size() const { return (_keys.size()); }

      /**
       * Set the name of this profile
       * 
       * @param name iString used to set the name of this profile
       */
      void setName(const std::string &name) { _name = name; }

      /**
       * @brief Returns the name of this property
       * 
       * @return std::string Name of this property
       */
      std::string Name() const { return (_name); }

      /**
       * Checks for the existance of a keyword
       * 
       * @param key  Name of keyword to check
       * 
       * @return bool True if it exists, false if it doesn't
       */
      bool exists(const std::string &key) const { return (_keys.exists(key)); }

      //  Convenience methods for adding keys
      void add(const std::string &key, const std::string &value = "");
      void replace(const std::string &key, const std::string &value = "");
      void remove(const std::string &key);
      int  count(const std::string &key) const;

      /**
       * Returns the nth key in the profile
       * 
       * This method returns the name of the nth keyword in the profile so one
       * can iterate through all existing keys.  Note that database passwords
       * could be vulnerable to exposure via this method.
       * 
       * Keywords in the profile are sorted in alphabetical order and not in the
       * order in which they are read.
       * 
       * @param nth Specifies the nth key in the profile
       * 
       * @return std::string Name of nth keyword in the profile.
       * 
       * @throws Out-of-range exception if the nth keyword does not exist
       */
      std::string key(int nth) const { return (_keys.key(nth)); }
      std::string value(const std::string &key, int nth = 0) const;
      std::string operator()(const std::string &key, int nth = 0) const;

      protected:
        void loadkeys(PvlContainer &pvl);

        /** 
         * @brief Returns a reference to the key list 
         *  
         * Direct access to the keyword container allows class developers some
         * additional flexibility whilst maintaining integrity through the 
         * public interface. 
         * 
         * @return const KeyList&  Reference to keyword list
         */
        const KeyList &getKeyList() const { return (_keys); }

      private:
        std::string _name;     //!<  Name of this profile
        KeyList _keys;         //!<  List of keys in profile
  };
}


#endif


