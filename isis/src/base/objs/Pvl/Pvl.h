#ifndef Pvl_h
#define Pvl_h

/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2010/06/25 20:40:06 $
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
#include "PvlObject.h"

namespace Isis {
  /**
   * @brief Container for cube-like labels
   *
   * This class is used for creating, reading, and writing grouped labels
   * generally from a file. An example of a label is:
   *   @code
   *     Group = Cube
   *       Samples = 512
   *       Lines = 512
   *       Bands = 1
   *     EndGroup
   *   @endcode
   * There are three different groupings, "root", "object", and "group". The
   * information is stored in "keywords". The root grouping can contain objects,
   * groups, and keywords. Object groupings can contain other objects, groups
   * and keywords. Group groupings can only contain keywords. Contents within the
   * group are called keywords which can have integer, double, string values or
   * no value. A keyword with no value is treated as a boolean.
   *
   * If you would like to see Pvl being used in implementation,
   * see class Cube or Preference
   *
   * @ingroup Parsing
   *
   * @author 2002-10-11 Jeff Anderson
   *
   * @internal
   *  @history 2003-01-31 Jeff Anderson - Added Keywords, Keyword, Groups, Group,
   *                                      and CopyGroup methods
   *  @history 2003-03-27 Jeff Anderson - Fixed problem caused by new compiler
   *                                      when attempting to left justify streams.
   *                                      Depricated the ReadInternal and
   *                                      WriteInternali methods. They were
   *                                      replaced by Read and Write methods.
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2003-08-18 Stuart Sides - Modified so labels with repeated objects,
   *                                     groups and keywords could be read in.
   *  @history 2003-09-25 Stuart Sides - Modified "keyword does not exist message"
   *                                     so it has two forms. One which says the
   *                                     keyword was not found at all and the
   *                                     other says the requested occurance was
   *                                     not found.
   *  @history 2003-09-26 Stuart Sides - Modified constructors to take a bool
   *                                     parameter which tells many member
   *                                     functions if they should allow
   *                                     duplacates or not. Also took the
   *                                     allowDuplicates parameter off all of the
   *                                     members who used it.
   *  @history 2003-10-15 Jeff Anderson - Modified write method to place quotes
   *                                      around null parameters (e.g. "")
   *  @history 2003-10-22 Stuart Sides - Added new method Insert.
   *  @history 2003-11-06 Jeff Anderson - Modify UpdateKeyword methods so that
   *                                      they do not throw errors if the keyword
   *                                      does not exist. That is, they simply
   *                                      add a new keyword.
   *  @history 2003-11-06 Jeff Anderson - Modify WriteContainer method to put a
   *                                      space between the value and unit of
   *                                      keywords.
   *  @history 2003-11-06 Jeff Anderson - Added Merge method
   *  @history 2003-12-01 Stuart Sides - Added new occurrence parameter to
   *                                     UseGroup. And added new member
   *                                     GroupOccurrences.
   *  @history 2004-01-22 Jeff Anderson - Fixed iterator out of bounds when
   *                                      parsing for units if the PVL terminated
   *                                      with and EOF instead of the usual END
   *                                      statement
   *  @history 2004-02-10 Jeff Anderson - Added new suite of AddKeyword methods
   *                                      for vectors which take a single unit
   *                                      instead of a vector of units
   *  @history 2004-02-10 Jeff Anderson - Modified parse and write methods to
   *                                      properly handle a single unit on
   *                                      vectors
   *  @history 2004-02-11 Jeff Anderson - Implemented GetUnits method and added
   *                                      AddKeyword method with a NULL value
   *  @history 2004-02-20 Jeff Anderson - Fixed a bug in the Merge method that
   *                                      was unwinding the container pointer too
   *                                      far.
   *  @history 2005-02-14 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2005-02-23 Elizabeth Ribelin - Fixed unitTest
   *  @history 2005-03-07 Leah Dahmer - Added missing documentation to class.
   *  @history 2006-04-21 Jacob Danton Added format templates abilities.
   *  @history 2006-08-30 Stuart Sides & Brendan George - Added ability to output
   *                                                      in PDS format.
   *  @history 2007-04-13 Stuart Sides - Added new test for long
   *           strings
   *  @history 2008-02-27 Stuart Sides - Fixed memory leak in
   *           output operator
   *  @history 2008-07-10 Steven Lambright - Changed StringEqual to use
   *           PvlKeyword::StringEqual
   *  @history 2008-10-2 Christopher Austin - Replaced all std::endl in the <<
   *           operator, Write() and Append() with PvlFormat.FormatEOL()
   *  @history 2009-12-17 Steven Lambright - Rewrote read (istream operator)
   *  @history 2010-04-13 Eric Hyer - Added copy constructor
   *                                - Added assignment operator
   *  @history 2010-06-25 Steven Lambright - Quicker to give up
   *            counting line numbers on error
   *  @history 2010-07-12 Steven Lambright - It's a bad idea to copy 
   *            p_internalTemplate in the copy constructor since it describes
   *            whether or not to delete an internal pointer.
   *  @history 2010-079-27 Sharmila Prasad - Validate a Pvl with the Template Pvl
   *  @todo 2005-02-14 add coded example to class documentation.
   */
  class Pvl : public Isis::PvlObject {
    public:
      Pvl();
      Pvl(const std::string &file);
      Pvl(const Pvl &other);

      friend std::istream &operator>>(std::istream &is, Pvl &pvl);
      friend std::ostream &operator<<(std::ostream &os, Isis::Pvl &pvl);

      ~Pvl() {
        if(p_internalTemplate) delete p_formatTemplate;
      };

      void Read(const std::string &file);

      void Write(const std::string &file);
      void Append(const std::string &file);

      /**
       * Sets the terminator used to signify the end of the PVL
       * informationDefaults to "END"
       *
       * @param term The user-defined terminator
       */
      void SetTerminator(const std::string &term) {
        p_terminator = term;
      };
      /**
       * Returns the terminator used to signify the end of the PVL
       * informationDefaults to "END".
       *
       * @return The terminator used by the Pvl object.
       */
      std::string Terminator() const {
        return p_terminator;
      };

      void SetFormatTemplate(Isis::Pvl &temp);
      void SetFormatTemplate(const std::string &filename);

      const Pvl &operator=(const Pvl &other);
      
      //! Validate a Pvl with the Template Pvl
      void ValidatePvl(const Pvl & pPvl, Pvl & pPvlResults);

    private:
      void Init();
      bool p_internalTemplate;
      std::string p_terminator; /**<Terminator used to signify the end of the
                                    PVL informationDefaults to "END"*/
  };
};
#endif
