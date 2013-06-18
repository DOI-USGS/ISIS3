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

#ifndef IsisGuiInputAttribute_h
#define IsisGuiInputAttribute_h

#include <QDialog>
#include <QLineEdit>
#include <QButtonGroup>

namespace Isis {
  /**
   * @brief GUI interface for input cube file attributes. 
   *  
   * Graphical user interface for the input cube attributes 
   * dialog. 
   *  
   * @ingroup ApplicationInterface
   *
   * @author 2003-01-01 Stuart Sides
   *
   * @internal
   *  @history 2013-06-04 Stuart Sides Fixed a bug where the input
   *           cube attributes did not propagate to the text field
   *           after being modified in the input cube attribute
   *           dialog.
   */



/**
   * @author Stuart Sides
   *
   * @internal
   */
  class GuiInputAttribute : public QDialog {
      Q_OBJECT

    public:
      GuiInputAttribute(QWidget *parent = 0);

      ~GuiInputAttribute();

      QString GetAttributes();

      void SetAttributes(const QString &value);

      static int GetAttributes(const QString &defaultAttribute,
                               QString &newAttribute,
                               const QString &title,
                               QWidget *parent);
    private:
      QLineEdit *p_lineEdit;
      QButtonGroup *p_buttonGroup;
  };
};

#endif

