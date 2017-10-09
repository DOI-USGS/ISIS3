#ifndef XmlToPvlTranslationManager_h
#define XmlToPvlTranslationManager_h
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

#include <QDomDocument>

#include "FileName.h"
#include "PvlTokenizer.h"

namespace Isis {
  class Pvl;
  class PvlContainer;
  class PvlKeyword;
  /**
   * @brief Allows applications to translate Xml label files
   *
   * <p>
   * This class allows for translating Xml label files into Pvl objects.
   * The translation process is driven by the output keyword names. Given an
   * output keyword name, the Translate method uses the translation table to
   * find the input value and then translates it into the output value.
   * </p>
   * <p>
   * The translation table is a Pvl object containing a Pvl group for each
   * output keyword. The translation group for an output keyword is named the
   * output keyword's name. The required PvlKeywords in a translation group
   * are:
   * </p>
   * <ul>
   *   <li><b>InputPosition:</b> The series of element tag names leading from
   *                             the second highest level element to the parent
   *                             element of the input element.</li>
   *   <li><b>InputKey:</b> The tag name of the input element. The text value
   *                        of this element is the input value for the
   *                        translation.</li>
   *   <li><b>OutputName:</b> The output keyword's name.</li>
   *   <li><b>OutputPosition:</b> The location of the output keyword
   *                              in the output label.</li>
   *   <li><b>Translation:</b> a pair defining how to convert the input
   *                           value into the output value. The pair
   *                           is consists of an output value and then
   *                           the specific input value that will be
   *                           converted into that output value. A star
   *                           [*] for the output value indicates that
   *                           the input value will not be changes and a
   *                           star [*] for the input value matches any
   *                           input value.</li>
   * </ul>
   * <p>
   * There are also optional keywords to modify the translation:
   * </p>
   * <ul>
   *   <li><b>InputDefault:</b> A default value that will be used for the input
   *                            value if no input value can be found in the
   *                            label.</li>
   *   <li><b>Auto:</b> The Auto keyword is not associated with a value. It
   *                    indicates that the output keyword should be translated
   *                    when the Auto() method is called.</li>
   *   <li><b>Optional:</b> The Optional keyword is not associated with a
   *                        value. It indicates that the output keyword is not
   *                        necessary. If an input value or default value is
   *                        not found for an optional keyword, it will be
   *                        skipped and translation will continue.</li>
   *   <li><b>Debug:</b> The Debug keyword is not associated with a value. It
   *                     indicates that debug information is to be output when
   *                     the translation is performed.</li>
   *   <li><b>InputKeyAttribute:</b> The name of the attribute of the input
   *                                 element that the input value will be read
   *                                 from. This will override reading the input
   *                                 value from the input element's text.</li>
   *   <li><b>InputKeyDependencies:</b> A list of dependencies that uniquely
   *                                    identify the input element. Each entry
   *                                    consists of a string specifying either
   *                                    the text value of a sibling element of
   *                                    the input element or the value of one
   *                                    of the input element's attributes. The
   *                                    specification string is formatted as
   *                                    <code>TYPE\@NAME:VALUE</code>. The
   *                                    type is either <code>tag</code> or
   *                                    <code>att</code> indicating to check
   *                                    the value of an element of attribute
   *                                    respectively.</li>
   * </ul>
   * <p>
   * An example Xml translation table can be found at
   * $base/translations/XmlLabel.trn.
   * </p>
   *
   * @ingroup Parsing
   *
   * @author 2017-01-12 Jeannie Backer
   *
   * @internal
   *  @history 2017-01-12 Jeannie Backer - Original version. Adapted from
   *                          PvlTranslationManager class. Fixes #4584.
   *  @history 2017-01-13 Jeannie Backer and Jesse Mapel - Initial Translate
   *                          and Auto design. Added dependencies for uniquely
   *                          identifying input elements. Fixes #4584.
   *  @history 2017-01-18 Jesse Mapel - Updated documentation and error messages. Fixes #4584.
   *  @history 2017-01-25 Jesse Mapel - Created unit test. Fixes #4584.
   *  @history 2017-05-26 Makayla Shepherd - Renamed XmlToPvlTranslationManager.
   */
  class XmlToPvlTranslationManager : public LabelTranslationManager {
    public:
      XmlToPvlTranslationManager(const QString &transFile);

      XmlToPvlTranslationManager(std::istream &transStrm);

      XmlToPvlTranslationManager(FileName &inputLabel,
                            const QString &transFile);

      XmlToPvlTranslationManager(FileName &inputLabel,
                            std::istream &transStrm);

      virtual ~XmlToPvlTranslationManager();

      // Attempt to translate the requested output name to output value
      // using the input name and value/default value
      virtual QString Translate(QString nName, int findex = 0);

      // Translate all translation table groups which contain "Auto"
      using LabelTranslationManager::Auto;
      void Auto(FileName &inputLabel, Pvl &outputLabel);

      void SetLabel(FileName &inputLabel);

    protected:
      virtual std::vector< std::pair<QString, int> > validKeywords() const;
      bool checkDependencies(QDomElement element, PvlKeyword dependencies, bool isDebug) const;
      void parseFile(const FileName &xmlFileName);

    private:
      QDomDocument m_xmlLabel; //!< The contents of the xml label.

  };
};

#endif
