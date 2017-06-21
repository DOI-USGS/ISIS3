#ifndef LabelTranslationManager_h
#define LabelTranslationManager_h
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

#include <string>
#include <fstream>

#include "FileName.h"
#include "PvlTokenizer.h"
#include "PvlTranslationTable.h"

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
   * @author 2017-01-11 Jeannie Backer
   *
   * @internal
   *  @history 2017-01-11 Jeannie Backer - Original Version. Code moved out of
   *                          PvlToPvlTranslationManager to make a generic parent
   *                          class. Fixes #4584.
   *  @history 2017-01-20 Jesse Mapel - Updated documentation and unit test. Fixes #4584.
   */
  class LabelTranslationManager : public PvlTranslationTable {
    public:
      LabelTranslationManager();

      LabelTranslationManager(const QString &transFile);

      LabelTranslationManager(std::istream &transStrm);

      virtual ~LabelTranslationManager();

      // Attempt to translate the requested output name to output value
      // using the input name and value/default value
      virtual QString Translate(QString nName, int findex = 0) = 0;

      // Translate all translation table groups which contain "Auto"
      virtual void Auto(Pvl &outputLabel);

    protected:

      virtual PvlKeyword DoTranslation(const QString nName);
      virtual PvlContainer *CreateContainer(const QString nName, Pvl &pvl);
  };
};

#endif


