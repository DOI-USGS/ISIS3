#ifndef _WarningWidget_h_
#define _WarningWidget_h_

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QToolButton>
#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QStatusBar>
#include <QTextEdit>

#include "CubeViewport.h"

namespace Isis {
  /**
   * @brief Displays the NoWarning icon as default and the Warning
   *        icon in case of exception and also pops up a a menu
   *        when an exception occurs. Clicking on the Warning icon
   *        will display a dialog with the Exception details. With
   *        the next mouse press event, the Warning icon
   *        disappears and the default NoWarning icon appears.
   *
   * @ingroup Part of active tool bar of all the Tools
   *
   * @author 2009-12-11 Sharmila Prasad
   *
   * @internal
   * @history 2009-12-11 Sharmila Prasad - Initial Version
   * @history 2010-03-22 Sharmila Prasad - Point the warning icons to right path
   */
  class WarningWidget : public QObject {
      Q_OBJECT
    public:
      WarningWidget(QStatusBar *pParent);
      ~WarningWidget();
      void viewWarningWidgetIcon(std::string &pStr,  const std::string   &pExStr);
      void setWarningText(std::string pStr);
      QString setValuesToRed(std::string psMessage);

    public slots:
      void resetWarning(void);
      void checkMessage(void);

    private:
      QDialog  *mDialog;// Dialog that pops up when Warning icon is clicked
      QWidget  *mWindow;                  // Dialog Window
      QPushButton *mNoWarning, *mWarning; // Tool buttons displaying Warning and Nowarning icons
      bool mbWarningFlag;// Flag to indicate Warning or Nowarning status
      QStatusBar *mSBar;// Pointer to application's Status Bar
      QString mMsgStr;// Message displayed on the status bar
      QTextEdit *mTextEdit;// Textedit which contains detailed error message in the Dialog window
  };
};
#endif
