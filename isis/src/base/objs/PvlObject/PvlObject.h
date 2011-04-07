#ifndef PvlObject_h
#define PvlObject_h
/**
 * @file
 * $Revision: 1.16 $
 * $Date: 2010/04/14 01:11:17 $
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
#include "PvlGroup.h"


template<typename T> class QList;

namespace Isis {
  /**
   * @brief Contains Pvl Groups and Pvl Objects.
   *
   * Contains Pvl groups and objects. Organizes text on output to be indented
   * correctly.
   *
   * @ingroup Parsing
   *
   * @author 2002-10-11 Jeff Anderson
   *
   * @internal
   *  @history 2005-04-04 Leah Dahmer wrote class documentation.
   *  @history 2006-04-21 Jacob Danton Added format templates abilities.
   *  @history 2006-05-18 Jacob Danton Added DeleteGroup by index @history
   *  @history 2006-09-11 Stuart Sides Added formatting ability
   *  @history 2007-08-11 Stuart Sides Added recursive FindKeyword and
   *                      HasKeyword
   *  @history 2007-08-14 Steven Koechle Fixed missing pointers in FindKeyword
   *                      and HasKeyword
   *  @history 2008-06-18 Steven Lambright Fixed documentation
   *  @history 2008-07-03 Steven Lambright Added const functionality and fixed +=
   *  @history 2008-07-10 Steven Lambright Compensated for PvlKeyword no longer
   *                      being a parent
   *  @history 2008-09-30 Christopher Austin Replaced all std::endl in the <<
   *                      operator with PvlFormat.FormatEOL()
   *  @history 2009-10-28 Travis Addair Fixed bug causing a format
   *                      template's object comments to not appear in the output
   *  @history 2009-12-17 Steven Lambright Added reading methods
   *  @history 2009-12-28 Steven Lambright Implemented const FindObject
   *  @history 2010-04-13 Eric Hyer - Added copy constructor
   *                                  Added assignment operator
   *  @history 2010-09-27 Sharmila Prasad - Validate an Object comparing with the
   *                                        corresponding Template Object
   */
  class PvlObject : public Isis::PvlContainer {
    public:
      PvlObject();
      PvlObject(const std::string &name);
      PvlObject(const PvlObject &other);

      friend std::ostream &operator<<(std::ostream &os, Isis::PvlObject &object);
      friend std::istream &operator>>(std::istream &is, PvlObject &result);

      /**
       * Returns the number of groups contained.
       * @return The number of groups.
       */
      int Groups() const {
        return p_groups.size();
      };

      PvlGroup &Group(const int index);
      const PvlGroup &Group(const int index) const;

      //! The counter for groups.
      typedef QList<Isis::PvlGroup>::iterator PvlGroupIterator;
      typedef QList<Isis::PvlGroup>::const_iterator ConstPvlGroupIterator;


      /**
       * Returns the beginning group index.
       * @return The iterator of the beginning group..
       */
      PvlGroupIterator BeginGroup() {
        return p_groups.begin();
      };


      /**
       * Returns the beginning group index.
       * @return The iterator of the beginning group..
       */
      ConstPvlGroupIterator BeginGroup() const {
        return p_groups.begin();
      };


      /**
       * Returns the ending group index.
       * @return The iterator of the ending group.
       */
      PvlGroupIterator EndGroup() {
        return p_groups.end();
      };


      /**
       * Returns the const ending group index.
       * @return The iterator of the ending group.
       */
      ConstPvlGroupIterator EndGroup() const {
        return p_groups.end();
      };


      /**
       * Find a group with the specified name, within these indexes.
       * @param name The name of the group to look for.
       * @param beg The lower index
       * @param end The higher index
       */
      PvlGroupIterator FindGroup(const std::string &name,
                                 PvlGroupIterator beg,
                                 PvlGroupIterator end) {
        Isis::PvlGroup temp(name);
        return std::find(beg, end, temp);
      }


      /**
       * Find a group with the specified name, within these indexes.
       * @param name The name of the group to look for.
       * @param beg The lower index
       * @param end The higher index
       */
      ConstPvlGroupIterator FindGroup(const std::string &name,
                                      ConstPvlGroupIterator beg,
                                      ConstPvlGroupIterator end) const {
        Isis::PvlGroup temp(name);
        return std::find(beg, end, temp);
      }


      /**
       * A collection of options to use when finding.
       */
      enum FindOptions {
        //! Search only the current level
        None,
        //! Search child objects
        Traverse
      };

      // The using statements below are used to make the PvlContainer's version
      // of FindKeyword and HasKeyword vissible to other code that otherwise would not be
      // able to see those versions.
      using PvlContainer::FindKeyword;


      PvlKeyword &FindKeyword(const std::string &kname,
                              FindOptions opts);


      using PvlContainer::HasKeyword;


      bool HasKeyword(const std::string &kname,
                      FindOptions opts) const;

      Isis::PvlGroup &FindGroup(const std::string &name,
                                FindOptions opts = None);

      const Isis::PvlGroup &FindGroup(const std::string &name,
                                      FindOptions opts = None) const;
      /**
       * Add a group to the object.
       * @param group The PvlGroup object to add.
       */
      void AddGroup(const Isis::PvlGroup &group) {
        p_groups.push_back(group);
        //p_groups[p_groups.size()-1].SetFilename(Filename());
      };

      using PvlContainer::operator+=;
      void operator+= (const Isis::PvlGroup &group) {
        AddGroup(group);
      }
      void operator+= (const Isis::PvlObject &obj) {
        AddObject(obj);
      }

      void DeleteGroup(const std::string &name);

      void DeleteGroup(const int index);


      /**
       * Returns a boolean value based on whether the object has the specified
       * group or not.
       * @param name The name of the group to look for.
       * @return True if the object has the group, false if not.
       */
      bool HasGroup(const std::string &name) const {
        if(FindGroup(name, BeginGroup(), EndGroup()) == EndGroup()) return false;
        return true;
      }

      /**
       * Returns the number of objects.
       * @return The number of objects.
       */
      int Objects() const {
        return p_objects.size();
      };

      PvlObject &Object(const int index);
      const PvlObject &Object(const int index) const;

      //! The counter for objects.
      typedef QList<PvlObject>::iterator PvlObjectIterator;
      typedef QList<PvlObject>::const_iterator ConstPvlObjectIterator;


      /**
       * Returns the index of the beginning object.
       * @return The beginning object's index.
       */
      PvlObjectIterator BeginObject() {
        return p_objects.begin();
      };


      /**
       * Returns the const index of the beginning object.
       * @return The beginning object's index.
       */
      ConstPvlObjectIterator BeginObject() const {
        return p_objects.begin();
      };


      /**
       * Returns the index of the ending object.
       * @return The ending object's index.
       */
      PvlObjectIterator EndObject() {
        return p_objects.end();
      };


      /**
       * Returns the const index of the ending object.
       * @return The ending object's index.
       */
      ConstPvlObjectIterator EndObject() const {
        return p_objects.end();
      };


      /**
       * Find the index of object with a specified name, between two indexes.
       * @param name The name of the object to find.
       * @param beg The lower index.
       * @param end The higher index.
       * @return The index of the object.
       */
      PvlObjectIterator FindObject(const std::string &name,
                                   PvlObjectIterator beg,
                                   PvlObjectIterator end) {
        PvlObject temp(name);
        return std::find(beg, end, temp);
      }


      /**
       * Find the index of object with a specified name, between two indexes.
       * @param name The name of the object to find.
       * @param beg The lower index.
       * @param end The higher index.
       * @return The index of the object.
       */
      ConstPvlObjectIterator FindObject(const std::string &name,
                                        ConstPvlObjectIterator beg,
                                        ConstPvlObjectIterator end) const {
        PvlObject temp(name);
        return std::find(beg, end, temp);
      }


      PvlObject &FindObject(const std::string &name,
                            FindOptions opts = None);

      const PvlObject &FindObject(const std::string &name,
                                  FindOptions opts = None) const;

      /**
       * Add a PvlObject.
       * @param object The PvlObject to add.
       */
      void AddObject(const PvlObject &object) {
        p_objects.push_back(object);
        p_objects[p_objects.size()-1].SetFilename(Filename());
      }

      void DeleteObject(const std::string &name);
      void DeleteObject(const int index);


      /**
       * Returns a boolean value based on whether the object exists in the current
       * PvlObject or not.
       * @param name The name of the object to search for.
       * @return True if the current PvlObject has the specified object, false
       * if it does not.
       */
      bool HasObject(const std::string &name) const {
        if(FindObject(name, BeginObject(), EndObject()) == EndObject()) return false;
        return true;
      }


      /**
       * Compares two PvlObjects. Returns a boolean value based on the
       * StringEqual() method.
       * @param object The PvlObject to compare.
       * @return True if they are equal, false if not.
       */
      bool operator==(const PvlObject &object) const {
        return PvlKeyword::StringEqual(object.Name(), this->Name());
      }


      //! Remove everything from the current PvlObject.
      void Clear() {
        Isis::PvlContainer::Clear();
        p_objects.clear();
        p_groups.clear();
      }

      const PvlObject &operator=(const PvlObject &other);
      
      //! Validate Object
      void ValidateObject(PvlObject & pPvlObj);
      
    private:
      QList<PvlObject> p_objects;    /**<A vector of PvlObjects contained
                                                in the current PvlObject. */
      QList<PvlGroup> p_groups;/**<A vector of PvlGroups contained
                                                in the current PvlObject. */
  };
}

#endif
