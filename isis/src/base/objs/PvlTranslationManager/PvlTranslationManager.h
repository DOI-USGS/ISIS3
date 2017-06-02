#ifndef PvlTranslationManager_h
#define PvlTranslationManager_h
/**
 * @file
 * $Revision: 1.6 $
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

#include <fstream>
#include <string>

#include "FileName.h"
#include "PvlTokenizer.h"

namespace Isis {
  class Pvl;
  class PvlContainer;
  class PvlKeyword;
  /**
   * @brief Allows applications to translate simple text files
   *
   * This class allows the translation of text files which can be parsed by the
   * Pvl class.
   *
   * @ingroup Parsing
   *
   * @author 2003-05-29 Stuart Sides
   *
   * @internal
   *  @history 2003-09-03 Stuart Sides - Modified to work with new isis label
   *                                     format
   *  @history 2003-09-25 Stuart Sides - Added the Translate member
   *  @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2006-08-09 Brendan George - Modified to support Optional keyword
   *                                       translation
   *  @history 2006-10-01 Stuart Sides - Fixed bug with Optional keyword.
   *                                     Non-optional keywords were being reported
   *                                     instantly.
   *  @history 2006-11-16 Brendan George - Changed instances of "Foreign" to "Input"
   *                                       and "Native" to "Output"
   *  @history 2007-06-22 Stuart Sides - Added ability to have more than one input location
   *                                     keyword for a translation. The first one found
   *                                     which contains the input keyword is used.
   *  @history 2008-05-09 Steven Lambright - Added ability to change input label without
   *                                     re-reading the translation file.
   *  @history 2008-07-10 Noah Hilt - Changed while loops to continue searching
   *           other groups if a group has been found, but the keyword does not
   *           exist in that group.
   *  @history 2008-07-10 Steven Lambright - Changed to use new accessors
   *  @history 2010-01-04 Steven Lambright - Added InputKeyword method and removed
   *                                         InputSize, InputUnits, InputValue.
   *                                         Renamed private Translate method to
   *                                         DoTranslation to remove ambiguity
   *                                         with a parent method, instead of
   *                                         using a dummy parameter.
   *  @history 2017-01-11 Jeannie Backer - Moved several methods to a generic
   *                          parent class, LabelTranslationManager. Fixes #4584.
   *  @todo 2005-02-15 Stuart Sides - add coded example and implementation example
   *                                  to class documentation, and finish
   *                                  documentation
   */
  class PvlTranslationManager : public LabelTranslationManager {
    public:
      PvlTranslationManager(const QString &transFile);

      PvlTranslationManager(std::istream &transStrm);

      PvlTranslationManager(Pvl &inputLabel,
                            const QString &transFile);

      PvlTranslationManager(Pvl &inputLabel,
                            std::istream &transStrm);

      virtual ~PvlTranslationManager();

      // Attempt to translate the requested output name to output value
      // using the input name and value/default value
      virtual QString Translate(QString nName, int findex = 0);

      // Translate all translation table groups which contain "Auto"
      void Auto(Pvl &outputLabel);
      void Auto(Pvl &inputLabel, Pvl &outputLabel);

      // Return the ith input value associated with a output name
      virtual const PvlKeyword &InputKeyword(const QString nName) const;

      // Return true if the input lable contains the translated group and key names
      virtual bool InputHasKeyword(const QString nName);

      void SetLabel(Pvl &inputLabel);

    protected:
      virtual PvlKeyword DoTranslation(const QString nName);
      virtual const PvlContainer *GetContainer(const PvlKeyword &inputGroup) const;
      virtual PvlContainer *CreateContainer(const QString nName, Pvl &pvl);
    private:
      Pvl p_fLabel; //!< A Pvl object for the input label file
  };
};

#endif


