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
  *  @history 2010-05-10 Christopher Austin - added cnet connectivity
  *                          functionality
  *  @history 2011-08-08 Steven Lambright - Refectored for new qmos. Mosaic
  *                          controller is now always visible.
  */
  class MosaicMainWindow : public MainWindow {
      Q_OBJECT
    public:
      MosaicMainWindow(QString title, QWidget *parent = 0);
      ~MosaicMainWindow() { }

      QToolBar *permanentToolBar() {
        return m_permToolbar;
      }

      QToolBar *activeToolBar() {
        return m_activeToolbar;
      }

      ToolPad *toolPad() {
        return m_toolpad;
      }

      QProgressBar *progressBar() {
        return m_progressBar;
      }

      /**
      * Returns the View menu.
      *
      *
      * @return QMenu*
      */
      QMenu *viewMenu() const {
        return m_viewMenu;
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
      
      ToolPad *m_toolpad; //!< Tool pad on this mainwindow

      QToolBar *m_permToolbar; //!< Tool bar attached to mainwindow
      QToolBar *m_activeToolbar; //!< The active toolbar
      QString m_filename;

      QProgressBar *m_progressBar; //!< The mainwindow's progress bar.

      QMenu *m_viewMenu;
      QMenu *m_settingsMenu;
      QMenu *m_fileMenu;
      QMenu *m_exportMenu;
      MosaicController *m_mosaicController;
      QList<QAction *> m_actionsRequiringOpen;
      QList<QAction *> m_actionsRequiringClosed;
      QList<Cube *> m_openCubes;
      QFileInfo m_lastOpenedFile;
      QSettings m_settings;
      QDockWidget *m_fileListDock;
      QDockWidget *m_mosaicPreviewDock;
  };
};

#endif

