#ifndef PvlToXmlTranslationManager_h
#define PvlToXmlTranslationManager_h
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

#include <string>

#include <QDomDocument>

#include "FileName.h"
#include "Pvl.h"
#include "PvlTokenizer.h"

class QDomElement;
class QString;

namespace Isis {
  class Pvl;
  class PvlContainer;
  class PvlKeyword;
  /**
   * @brief Allows applications to translate simple text files
   *
   * This class allows the translation of text files which can be parsed by the
   * Pvl class into Xml files.
   *
   * @ingroup Parsing
   *
   * @author 2017-05-24 Jeannie Backer and Ian Humphrey
   *
   * @internal
   *  @history 2017-05-24 Jeannie Backer and Ian Humphrey - Original version.
   *  @history 2017-05-26 Ian Humphrey & Makayla Shepherd - Added createParentElements.
   *  @history 2017-05-26 Makayla Shepherd & Adam Paquette - Added addSiblings and setElementValue.
   *  @history 2017-05-30 Makayla Shepherd - Renamed p_fLabel to m_inputLabel, and updated
   *                          documentation.
   *  @history 2017-05-31 Adam Paquette - Added addAttributes method.
   *  @history 2017-05-31 Ian Humphrey & Makayla Shepherd - Fixed duplicate root xml element in
   *                          output xml file.
   *  @history 2017-06-02 Makayla Shepherd - Made setElementValue public.
   *  @history 2017-10-18 Jeannie Backer & Makayla Shepherd - Added convenience methods, addElement
   *                          and resetElementValue, and made setElementValue static. See #5202.
   */
  class PvlToXmlTranslationManager : public LabelTranslationManager {
    public:
      PvlToXmlTranslationManager(const QString &transFile);

      PvlToXmlTranslationManager(Pvl &inputLabel,
                            const QString &transFile);

      virtual ~PvlToXmlTranslationManager();

      // Attempt to translate the requested output name to output value
      // using the input name and value/default value
      virtual QString Translate(QString nName, int inputIndex = 0);

      // Translate all translation table groups which contain "Auto"
      void Auto(QDomDocument &outputLabel);
      void Auto(Pvl &inputLabel, QDomDocument &outputLabel);

      // Return the ith input value associated with a output name
      virtual const PvlKeyword &InputKeyword(const QString nName) const;

      // Return true if the input lable contains the translated group and key names
      virtual bool InputHasKeyword(const QString nName);

      void SetLabel(Pvl &inputLabel);
      
      static void addElement(QDomElement &parent, QString name, QString value, QString units = "");
      static void setElementValue(QDomElement &element, QString value, QString units = "");
      static void resetElementValue(QDomElement &element, QString value, QString units = "");


    protected:
      void doTranslation(PvlGroup transGroup, QDomElement &parent);
      virtual const PvlContainer *GetContainer(const PvlKeyword &inputGroup) const;

      virtual std::vector< std::pair<QString, int> > validKeywords() const;
      bool checkDependencies(QDomElement element, PvlKeyword dependencies, bool isDebug) const;
      QDomElement *createParentElements(const QString translationGroupName, QDomElement &xml);
      void addSiblings(PvlKeyword outputSiblings, QDomElement &parent);
      void addAttributes(PvlKeyword something, QDomElement &parent);

    private:
      Pvl m_inputLabel; //!< A Pvl object for the input label file
  };
};

#endif
