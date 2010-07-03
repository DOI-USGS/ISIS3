#ifndef ViewportMainWindow_h
#define ViewportMainWindow_h

#include <map>
#include <QToolBar>
#include <QMenu>
#include "Filename.h"
#include "MainWindow.h"

namespace Isis {
  class Preference;
}

namespace Qisis {
  /**
  * @brief This was called the Qisis MainWindow.  Now this is
  *        being subclassed from the mainwindow class which keeps
  *        track of the size and location of the qisis windows.
  *        qview and qnet are two applications that use
  *        WiewportMainWindow.
  *
  * @ingroup Visualization Tools
  *
  * @author Stacy Alley
  *
  * @internal
  *
  *  @history 2008-06-19 Noah Hilt - Added a method for sending a signal to qview
  *           when this window recieves a close event. This signal calls the file
  *  		  tool's exit method and ignores this class's close method.
  *  @history 2010-18-03 Sharmila Prasad - Added ability to handle exception and display warning status
  *  		  when exception occured
  */
  class Workspace;
  class ToolPad;
  class TrackTool;

  class ViewportMainWindow : public Qisis::MainWindow {
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
      Qisis::Workspace *workspace() {
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
      Qisis::ToolPad *toolPad() {
        return p_toolpad;
      };

      QMenu *getMenu(const QString &name);
      Qisis::TrackTool *getTrackTool() {
        return mTrackTool;
      };

    protected:
      bool eventFilter(QObject *o, QEvent *e);
      void readSettings();
      void writeSettings();
      virtual void closeEvent(QCloseEvent *event);

    private:
      Qisis::Workspace *p_workspace;       //!< The current workspace
      QToolBar *p_permToolbar;             //!< The permanent toolbar
      QToolBar *p_activeToolbar;           //!< The active toolbar
      Qisis::ToolPad *p_toolpad;           //!< The toolpad
      std::map<QString, QMenu *> p_menus;  //!< Map of qstrings to menus
      std::string p_appName;               //!< The app name
      Qisis::TrackTool *mTrackTool;        //!< Pointer to application's Status bar
  };
};

#endif
