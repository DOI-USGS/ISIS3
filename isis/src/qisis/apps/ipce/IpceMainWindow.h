#ifndef IpceMainWindow_H
#define IpceMainWindow_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ViewSubWindow.h"
#include <QEvent>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QMdiSubWindow>

namespace Isis {
  class AbstractProjectItemView;
  class Directory;
  class Project;

  /**
   * The main window for the ipce appication. This handles most of the top-level GUI aspects of the program.
   *
   * @author 2012-??-?? Steven Lambright and Stuart Sides
   *
   * @internal
   *   @history 2012-07-27 Kimberly Oyama and Steven Lambright - Removed progress and warnings
   *                           tab widgets. They are now dock widgets.
   *   @history 2012-08-28 Tracie Sucharski - The Directory no longer takes a container it its
   *                           constructor.
   *   @history 2012-09-17 Steven Lambright - Dock widgets now delete themselves on close. This
   *                           gives the user the correct options when proceeding in the interface,
   *                           but undo/redo are not implemented (it needs to eventually be
   *                           encapsulated in a work order). The undo/redo didn't work correctly
   *                           anyways before setting this flag, so it's an improvement. Example
   *                           change: If I close Footprint View 1, I'm no longer asked if I want
   *                           to view images in footprint view 1.
   *   @history 2015-10-05 Jeffrey Covington - Replaced the ProjectTreeWidget
   *                           with a ProjectItemTreeView. Added the
   *                           eventFilter() method for intercepting some
   *                           events from views.
   *   @history 2016-01-04 Jeffrey Covington - Added a QMdiArea as the central widget
   *                           of the main window. The menus and toolbars are now solely
   *                           handled by the main window. Menus, context menus, and
   *                           toolbars are populated with actions recieved from the Directory
   *                           the active view, and the main window. Both WorkOrders and
   *                           regular QActions can be used in menus and toolbars. Views can
   *                           now be detached from the main window into their own independent
   *                           window with internalized menus and toolbars.
   *   @history 2016-10-20 Tracie Sucharski - Clean up included headers that are commented out,
   *                           updated for Qt5, comment call to saveState for window which caused
   *                           errors.  TODO:  Determine problem with saveState call.
   *   @history 2016-11-09 Tyler Wilson - Move a segment of code in the constructor from the beginning
   *                           to the end.  This code loads a project from the command line instead of the
   *                           GUI, and it wasn't outputting warnings/errors to the warnings/error tab
   *                           when the project was loaded because it was being called before the GUI
   *                           was created.  Fixes #4488.  References #4526, ##4487.
   *   @history 2016-11-09 Ian Humphrey - Modified readSettings() and writeSettings() to take in
   *                           Project pointers to be used to properly read and write settings
   *                           for the IpceMainWindow. Note that when running ipce without
   *                           opening a Project, the config file ipce_Project.config is used.
   *                           Otherwise, when a project is open, the config file
   *                           ipce_ProjectName will be used to restore window geom.
   *                           The m_permToolBar, m_activeToolBar, and m_toolPad now have object
   *                           names set, so the saveState() call within writeSettings() now works.
   *                           Fixes #4358.
   *   @history 2016-12-09 Tracie Sucharski - One of the previous 2 changes caused a problem with
   *                           view toolbars not to be restored.  Added setActiveSubWindow and
   *                           show to the ::addView method.  Fixes #4546.
   *   @history 2017-04-17 Ian Humphrey - Updated createMenus() to set tool tips (hover text)
   *                           visible so the JigsawWorkOrder tool tip can be displayed to user
   *                           (which indicates why it is disabled by default). Fixes #4749.
   *   @history 2017-06-22 Tracie Sucharski - Renamed from CNetSuiteMainWindow when application was
   *                           renamed to ipce from cnetsuite.
   *   @history 2017-07-12 Cole Neubauer - Added removeAllViews function and m_detachedViews member
   *                           variable. Needed to clear out an old projects views from the window
   *                           when opening a new project. Fixes #4969
   *   @history 2017-07-14 Cole Neubauer - Added private slot raiseWarningTab to be able to raise
   *                           the warning tab when a new warning happens.
   *                           Fixes #5041
   *   @history 2017-07-14 Cole Neubauer - Set Object name for Target/Sensor Widgets in addView
   *                           Fixes #5059
   *                           Fixes #5041
   *   @history 2017-07-26 Cole Neubauer - Changed the closevent funtion to check if a project is
   *                           and prompt user accordingly Fixes #4960
   *   @history 2017-08-09 Marjorie Hahn - Hard-coded the size of the icons in the toolbar to
   *                           temporarily fix the shift in size when switching between views
   *                           until docked widgets are implemented. Fixes #5084.
   *   @history 2017-10-06 Cole Neubauer - Made the open from command line use the WorkOrder
   *                           Fixes #5171
   *   @history 2017-11-02 Tyler Wilson - Added the ability to read/write settings for recent
   *                           projects.  Also re-implemented the functionality for loading
   *                           recent projects in IPCE.  Fixes #4492.
   *   @history 2017-11-03 Adam Goins - Modified the detached QMainWindow to be a ViewSubWindow
   *                           so that we can capture the windowClose() signal and remove
   *                           detached views from the m_detachedViews list appropriately.
   *                           This fixes an issue where a detached view would appear to be
   *                           open even after it has been closed. Fixes #5109.
   *   @history 2017-11-12  Tyler Wilson - Removed a resize call in readSettings because it
   *                           was screwing up the display of widgets when a project is loaded.
   *                           Also switched the order in which a project is saved.  A project is
   *                           cleared after it is saved, and not before (which had been the previous
   *                           behavior.  Fixes #5175.
   *   @history 2017-12-08 Tracie Sucharski - Removed project path.  Project root has been
   *                           fixed to correctly show the path. References #5276, #5289.
   *   @history 2018-01-18 Tracie Sucharski - Commented out progressDock until we decide if it's
   *                           needed.  Currently, it is not being used, the progress bar appears in
   *                           the history dock. Fixes #5151.
   *   @history 2018-03-02 Tracie Sucharski - added static keyword to the m_maxRecentProject member
   *                           variable, fixes OSX compile warning.  References #5341.
   *   @history 2018-04-04 Tracie Sucharski - Added removeView slot which removes the view
   *                           containing the given widget. In the closeEvent method check whether
   *                           there is an active control and if it has been modified as additional
   *                           test to determine whether project needs saving.
   *   @history 2018-05-01 Tracie Sucharski - Code accidently left commented from previous checking.
   *                           Fixes #5412.
   *   @history 2018-05-31 Christopher Combs - Added support for JigsawRunWidget to be created as a
   *                           dockable widget in addView(). Fixes #5428.
   *   @history 2018-05-30 Tracie Sucharski - Fix to handle the re-factored docked views.
   *                           Changed from MDI to SDI, changing the centralWidget to a dumy, unused
   *                           widget. Remove all methods having to do with MDI sub-windows,
   *                           detached views.  The dock widgets holding the views are saved off
   *                           for cleanup because there is no way to get the dock from the view.
   *                           Cleanup connections are made for the views and the docks to ensure
   *                           that cleanup happens for both.  Fixes #5433.
   *   @history 2018-06-13 Tracie Sucharski - Fixed cleanup of views and QDockWidgets.
   *   @history 2018-06-13 Kaitlyn Lee - Since views now inherit from QMainWindow, each individual
   *                           view has its own toolbar, so having an active toolbar and tool pad is
   *                           not needed. Removed code adding the save active control net button
   *                           and the toolpad, since control nets can be saved with the project
   *                           save button.
   *   @history 2018-06-14 Christopher Combs - Changed addView method to take in JigsawRunWidget as
   *                           a QDockWidget object instead of wrapping it in one.
   *   @history 2018-06-15 Tracie Sucharski - Fixed break to recent projects.  The readSettings
   *                           must be called before initializeActions to get the recent projects
   *                           from the config file.
   *   @history 2018-06-18 Makayla Shepherd - Set the QApplication name so that BundleAdjust does
   *                           not output text to the command line for ipce. Fixes #4171.
   *   @history 2018-06-19 Kaitlyn Lee - Added tabViews() and the menu option under the View menu to
   *                           tab the views. Currently, this can tab all attached/detached views.
   *                           I left the line setting dock options to allow grouped dragging, but
   *                           tabbing views does not always work with this enabled. With this
   *                           option enabled, the type of a view will randomly change and setting
   *                           its type has no effect. Use windowType() to get the type. Also added
   *                           the toolbar title in the permanent toolbar constructor.
   *   @history 2018-06-22 Tracie Sucharski - Cleanup destruction of dock widgets and the views they
   *                           hold.  Extra destroy slots were causing double deletion of memory.
   *   @history 2018-06-22 Tracie Sucharski - Added a showEvent handler so that the project clean
   *                           state can be reset after the IpceMainWindow::show() causes resize and
   *                           move events which in turn cause the project clean flag to be false
   *                           even though the project has just opened.
   *   @history 2018-07-07 Summer Stapleton - Added check in the closeEvent() for changes to any
   *                           TemplateEditorWidget currently open to create a pop-up warning/
   *                           option to save.
   *   @history 2018-07-09 Kaitlyn Lee - Added tileViews() and the menu option to tile all
   *                           docked/undocked and tabbed/untabbed views. Changed removeView() to
   *                           delete the parent dock widget. If we do not delete the dock widget,
   *                           an empty dock widget will remain where the view used to be.
   *   @history 2018-07-10 Tracie Sucharski - Change initial interface of views to tabbed view.
   *                           Changed the QMainWindow separator to a different color and wider size
   *                           for ease of use.  Create the QMainWindow initial size to prevent the
   *                           Viewports in CubeDnView from being created as a small size.
   *   @history 2018-07-11 Kaitlyn Lee - Added a value in the project settings that stores whether a
   *                           project was in fullscreen or not when saved. If not, we call showNormal()
   *                           to restore the poject's window size. This also fixes the warning/history tabs
   *                           being misplaced when opening a project. Fixes #5175. References #5436.
   *   @history 2018-07-12 Tracie Sucharski - Renamed the signal Directory::viewClosed to
   *                           Directory::closeView since Directory does not close the view but
   *                           indicate that the view needs closing.  This signal is now used by
   *                           more than the cnetEditorView, so updated documentation.  Did a little
   *                           cleanup on the removeView  method by removing some code that
   *                           automatically happens due to connection made on destroyed signal.
   *   @history 2018-07-12 Kaitlyn Lee - Removed code that makes the window fullscreen in memory,
   *                           since this was causing a project's window size to not be restored
   *                           when opening from the command line. Decreased the size and changed
   *                           the color of the splitter. In writeGlobalSettings(), check to see if
   *                           the geometry value does not exist in the config file. This allows the
   *                           geometry to be saved if the config file does not exist and a user
   *                           opens a project. Before, it would not save the geometry because the
   *                           opened project was not temporary. References #5433.
   *   @history 2018-07-17 Kaitlyn Lee - Added signal enableViewActions(bool) to enable/disable
   *                           tab/tile views when views are opened/closed.
   *                           opened project was not temporary. References #5433
   *   @history 2018-07-19 Tracie Sucharski - Keep separate dock lists for the view docks and
   *                           "special" docks such as sensor, target and jigsaw. The
   *                           ControlHealthView is now added under the Project instead of in
   *                           workspace area. Removed unnecessary call to addDock for the History
   *                           widget. It is added with the call to tabifyDockWidget.
   *   @history 2018-07-29 Tracie Sucharski - Set background of centralWidget to a pattern to
   *                           distinguish it from dockable areas.
   */
  class IpceMainWindow : public QMainWindow {
      Q_OBJECT
    public:
      explicit IpceMainWindow(QWidget *parent = 0);
      ~IpceMainWindow();

    signals:
      void enableViewActions(bool value);

    public slots:
      void addView(QWidget *newWidget, Qt::DockWidgetArea area = Qt::LeftDockWidgetArea,
                   Qt::Orientation orientation = Qt::Horizontal);
      void removeView(QWidget *view);
      void removeAllViews();

      void readSettings(Project *);
      void writeSettings(Project *project);
      void writeGlobalSettings(Project *project);

    protected:
      void showEvent(QShowEvent *event);
      void closeEvent(QCloseEvent *event);
      bool eventFilter(QObject *watched, QEvent *event);

    private slots:
      void configureThreadLimit();
      void enterWhatsThisMode();

      void tabViews();
      void tileViews();

      void raiseWarningTab();

      void cleanupViewDockList(QObject *obj);

    private:
      Q_DISABLE_COPY(IpceMainWindow);

      void applyMaxThreadCount();

      void initializeActions();
      void createMenus();
      void createToolBars();

    private:
      /**
       * The directory stores all of the work orders that this program is capable of doing. This
       *   drives most of the functionality.
       */
      QPointer<Directory> m_directory;

      QDockWidget *m_projectDock;
      QDockWidget *m_warningsDock;

      QList<QDockWidget *> m_specialDocks;  //!< Non-view dock widgets such as jigsawRun
      QList<QDockWidget *> m_viewDocks; //!< QDockWidgets holding the views

      /**
       * This is the "goal" or "estimated" maximum number of active threads running in this program
       *   at once. For now, the GUI consumes 1 thread and QtConcurrent
       *   (QThreadPool::globalInstance) consumes the remaining threads. Anything <= 1 means that we
       *   should perform a best-guess for best perfomance.
       */
      int m_maxThreadCount;
      static const int m_maxRecentProjects = 5;

      QToolBar *m_permToolBar; //!< The toolbar for actions that rarely need to be changed.

      QMenu *m_fileMenu; //!< Menu for the file actions
      QMenu *m_projectMenu; //!< Menu for the project actions
      QMenu *m_editMenu; //!< Menu for edit actions
      QMenu *m_viewMenu; //!< Menu for view and window actions
      QMenu *m_settingsMenu; //!< Menu for settings actions
      QMenu *m_helpMenu; //!< Menu for help actions

      QList<QAction *> m_fileMenuActions; //!< Internal list of file actions
      QList<QAction *> m_projectMenuActions;//!< Internal list of project actions
      QList<QAction *> m_editMenuActions;//!< Internal list of edit actions
      QList<QAction *> m_viewMenuActions;//!< Internal list of view actions
      QList<QAction *> m_settingsMenuActions;//!< Internal list of settings actions
      QList<QAction *> m_helpMenuActions;//!< Internal list of help actions

      QList<QAction *> m_permToolBarActions;//!< Internal list of permanent toolbar actions
  };
}

#endif // IpceMainWindow_H
