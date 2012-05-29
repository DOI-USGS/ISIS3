#ifndef ViewportMainWindow_h
#define ViewportMainWindow_h

#include <map>
#include <QToolBar>
#include <QMenu>
#include "FileName.h"
#include "MainWindow.h"

namespace Isis {
  class Preference;
  class ToolPad;
  class TrackTool;
  class Workspace;

  /**
   * @brief This was called the Qisis MainWindow.  Now this is
   *        being subclassed from the mainwindow class which keeps
   *        track of the size and location of the qisis windows.
   *        qview and qnet are two applications that use
   *        WiewportMainWindow.
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *
   *  @history 2008-06-19 Noah Hilt - Added a method for sending a signal to qview
   *           when this window recieves a close event. This signal calls the file
   *           tool's exit method and ignores this class's close method.
   *  @history 2010-18-03 Sharmila Prasad - Added ability to handle exception and display warning status
   *           when exception occured
   *  @history 2012-05-29 Steven Lambright - Updated closeEvent() to ask the user to save any
   *                          unsaved modifications to the opened cube. References #854.
   */
  class ViewportMainWindow : public MainWindow {
      Q_OBJECT

    signals:
      void closeWindow(); //!< Signal called when the window receives a close event

    public slots:
      void displayWarning(std::string &pStr, const std::string &pExStr);
      void resetWarning(void);

    public:
      ViewportMainWindow(QString title, QWidget *parent = 0);
      virtual ~ViewportMainWindow();

      //! Returns the current workspace
      Workspace *workspace() {
        return p_workspace;
      };

      //! Returns the permanent toolbar
      QToolBar *permanentToolBar() {
        return p_permToolbar;
      };

      //! Returns the active toolbar
      QToolBar *activeToolBar() {
        return p_activeToolbar;
      };

      //! Returns the toolpad
      ToolPad *toolPad() {
        return p_toolpad;
      };

      QMenu *getMenu(const QString &name);
      TrackTool *getTrackTool() {
        return mTrackTool;
      };

    protected:
      virtual void closeEvent(QCloseEvent *event);

    private:
      Workspace *p_workspace;       //!< The current workspace
      QToolBar *p_permToolbar;             //!< The permanent toolbar
      QToolBar *p_activeToolbar;           //!< The active toolbar
      ToolPad *p_toolpad;           //!< The toolpad
      std::map<QString, QMenu *> p_menus;  //!< Map of qstrings to menus
      std::string p_appName;               //!< The app name
      TrackTool *mTrackTool;        //!< Pointer to application's Status bar
  };
};

#endif
