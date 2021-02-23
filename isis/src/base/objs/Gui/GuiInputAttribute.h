/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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

