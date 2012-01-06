#ifndef _WarningWidget_h_
#define _WarningWidget_h_

#include <QToolButton>
#include <QAction>
#include <QDialog>
#include <QWidget>
#include <QStatusBar>
#include <QTextEdit>

#include "CubeViewport.h"
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/06/24 17:17:49 $
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
