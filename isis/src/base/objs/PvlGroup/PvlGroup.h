#ifndef PvlGroup_h
#define PvlGroup_h
/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/06/16 18:15:21 $
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

#include "PvlContainer.h"

namespace Isis {
  /**
   * @brief Contains multiple PvlContainers
   *
   * Contains multiple PvlContainers so that keyword-value pairs can be organized
   * in specific groups. For example, a PvlGroup would be used to group
   * all data from a particular mission together. A PvlGroup object will also
   * organize (indent) objects on output.
   *
   * @ingroup Parsing
   *
   * @author 2002-10-11 Jeff Anderson
   *
   * @internal
   *  @history 2005-04-04 Leah Dahmer wrote class documentation.
   *  @history 2006-04-21 Jacob Danton Added format templates abilities.
   *  @history 2006-09-11 Stuart Sides Added formatting ability
   *  @history 2008-07-02 Steven Lambright Added const functionality and fixed +=
   *  @history 2008-07-02 Steven Lambright Updated to compensate for PvlKeyword no
   *           longer being a parent
   *  @history 2008-09-30 Christopher Austin Replaced all std::endl in the <<
   *           operator with PvlFormat.FormatEOL()
   *  @history 2010-04-13 Eric Hyer - Added copy constructor
   *                                  Added assignment operator
   *  @history 2010-09-27 Sharmila Prasad - Added API to Validate a PVLGroup
   *  @history 2013-03-11 Steven Lambright and Mathew Eis - Brought method names and member variable
   *                          names up to the current Isis 3 coding standards. Fixes #1533.
   *
   *  @todo 2005-04-04 Needs coded example.
   */
  class PvlGroup : public Isis::PvlContainer {
    public:
      PvlGroup();
      PvlGroup(const QString &name);
      PvlGroup(const PvlGroup &other);

      //! Validate a Group comparing with the Template Group
      void validateGroup(PvlGroup & pPvlGrp);

      friend std::istream &operator>>(std::istream &is, PvlGroup &result);
      friend std::ostream &operator<<(std::ostream &os, PvlGroup &group);
      /**
       * Whenever the '==' operator is used on a PvlGroup object, it will call
       * the stringEqual() method. This returns a boolean value.
       * @param group The PvlGroup object to compare.
       * @return True if the other PvlGroup has the same name as this one, false
       * if not.
       */
      bool operator==(const PvlGroup &group) const {
        return PvlKeyword::stringEqual(group.name(), this->name());
      };

      const PvlGroup &operator=(const PvlGroup &other);
  };
}
#endif
