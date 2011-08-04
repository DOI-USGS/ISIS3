#ifndef MosaicMainWindow_h
#define MosaicMainWindow_h

#include <iostream>

#include "MainWindow.h"

namespace Isis {
  class ToolPad;
}

namespace Isis {
  class Cube;
  class iString;
  class MosaicController;

  /**
  * @brief
  *
  * @ingroup Visualization Tools
  *
  * @author Stacy Alley
  *
  * @internal
  *
  *  @history 2010-05-10 Christopher Austin - added cnet connectivity
  *           functionality
  */
  class MosaicMainWindow : public MainWindow {
      Q_OBJECT
    public:
      MosaicMainWindow(QString title, QWidget *parent = 0);
      ~MosaicMainWindow() { }

      QToolBar *permanentToolBar() {
        return p_permToolbar;
      }

      QToolBar *activeToolBar() {
        return p_activeToolbar;
      }

      ToolPad *toolPad() {
        return p_toolpad;
      }

      QProgressBar *progressBar() {
        return p_progressBar;
      }

      /**
      * Returns the View menu.
      *
      *
      * @return QMenu*
      */
      QMenu *viewMenu() const {
        return p_viewMenu;
      };

      void saveSettings();
      void loadProject(QString filename);

    public slots:
      void open();
      void openList();
      void saveProject();
      void saveProjectAs();
      void loadProject();
      void closeMosaic();
      void updateMenuVisibility();

    protected:
      bool eventFilter(QObject *o, QEvent *e);

    private:
      void setupMenus();
      void setupPvlToolBar();
      void readSettings();
      void saveSettings2();
      void openFiles(QStringList cubeNames);
      bool updateMenuVisibility(QMenu *menu);
      void createController();
      void displayController();

      bool m_controllerVisible;
      
      ToolPad *p_toolpad; //!< Tool pad on this mainwindow

      QToolBar *p_permToolbar; //!< Tool bar attached to mainwindow
      QToolBar *p_activeToolbar; //!< The active toolbar
      QString p_filename;

      QProgressBar *p_progressBar; //!< The mainwindow's progress bar.

      QMenu *p_viewMenu;
      QMenu *p_settingsMenu;
      QMenu *p_fileMenu;
      QMenu *p_exportMenu;
      MosaicController *p_mosaicController;
      QList<QAction *> p_actionsRequiringOpen;
      QList<QAction *> p_actionsRequiringClosed;
      QList<Cube *> p_openCubes;
      QFileInfo p_lastOpenedFile;
      QSettings p_settings;
      QDockWidget *p_fileListDock;
      QDockWidget *p_mosaicPreviewDock;
  };
};

#endif

