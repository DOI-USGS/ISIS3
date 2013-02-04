/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:07 $
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

#ifndef IsisGuiOutputAttribute_h
#define IsisGuiOutputAttribute_h

#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-07-02 Steven Lambright and Stuart Sides - Modified to
   *                           have similar display values after the CubeAttribute
   *                           refactor that better preserves strings that are
   *                           passsed into CubeAttributeOutput. One of the strings
   *                           was also invalid - didn't match any attributes, but was
   *                           the default so there were no symptoms of a bug. This has
   *                           been fixed. References #961.
   */
  class GuiOutputAttribute : public QDialog {
      Q_OBJECT

    public:
      GuiOutputAttribute(QWidget *parent = 0);

      ~GuiOutputAttribute();

      QString GetAttributes();

      void SetAttributes(const QString &value);

      static int GetAttributes(const QString &defaultAttribute,
                               QString &newAttribute,
                               const QString &title,
                               bool allowProp,
                               QWidget *parent);

      void SetPropagation(bool enabled);

    private:
      QRadioButton *p_propagate;
      QRadioButton *p_unsignedByte;
      QRadioButton *p_signedWord;
      QRadioButton *p_real;
      QLineEdit *p_minEdit;
      QLineEdit *p_maxEdit;
      QRadioButton *p_attached;
      QRadioButton *p_detached;
      QRadioButton *p_tiled;
      QRadioButton *p_bsq;
      QRadioButton *p_lsb;
      QRadioButton *p_msb;
      bool p_propagationEnabled;
  };
};

#endif

