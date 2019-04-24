#include <QStatusBar>
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QSettings>
#include "ViewportMainWindow.h"
#include "Workspace.h"
#include "TrackTool.h"
#include "ToolPad.h"

namespace Isis {
  /**
   * Constructs a ViewportMainWindow object with windowTitle = title.
   *
   * @param title
   * @param parent
   */
  ViewportMainWindow::ViewportMainWindow(QString title, QWidget *parent) : MainWindow(title, parent) {
    p_workspace = new Workspace(false, this);
    setCentralWidget(p_workspace);
    setWindowTitle(title);
    setObjectName("MainWindow");

    p_permToolbar = new QToolBar("Standard Tools", this);
    p_permToolbar->setObjectName("perm");
    p_permToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    p_permToolbar->setIconSize(QSize(22, 22));
    addToolBar(p_permToolbar);

    p_activeToolbar = new QToolBar("Active Tool", this);
    p_activeToolbar->setObjectName("Active");
    p_activeToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    p_activeToolbar->setIconSize(QSize(22, 22));
    addToolBar(p_activeToolbar);

    QStatusBar *sbar = statusBar();
    sbar->showMessage("Ready");

    mTrackTool = new TrackTool(sbar);
    mTrackTool->addTo(this);

    p_toolpad = new ToolPad("Tool Pad", this);
    p_toolpad->setObjectName("ViewportMainWindow");
    p_toolpad->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
    addToolBar(Qt::RightToolBarArea, p_toolpad);

    readSettings(QSize(800, 700));
  }


  /**
   *
   *
   */
  ViewportMainWindow::~ViewportMainWindow() {
  }

  /**
   * Slot which receives the warning signal. Calls the Track Tool
   * to display the warning status
   *
   * @param pStr   - Warning message
   * @param pExStr - Propagated exception message
   */
  void ViewportMainWindow::displayWarning(std::string &pStr, const std::string   &pExStr) {
    if(mTrackTool != NULL) {
      mTrackTool->displayWarning(pStr, pExStr);
    }
  }

  /**
   *  Slot which receives the message to reset warning status
   */
  void ViewportMainWindow::resetWarning(void) {
    if(mTrackTool != NULL) {
      mTrackTool->resetStatusWarning();
    }
  }

  /**
   * Returns the menu with menu name = name.
   *
   * @param name
   *
   * @return QMenu*
   */
  QMenu *ViewportMainWindow::getMenu(const QString &name) {
    std::map<QString, QMenu *>::iterator pos;
    pos = p_menus.find(name);
    if(pos != p_menus.end()) {
      return pos->second;
    }
    else {
      QMenu *menu = menuBar()->addMenu(name);
      p_menus[name] = menu;
      return menu;
    }
  }


  /**
   * This class is called when a close event occurs, it emits a
   * signal and ignores the close event.
   *
   * @param event
   *
   *  @internal
   *  @history 2018-04-24 Adam Goins - Added optional parameter QCloseEvent to
   *                          the closeWindow() signal so that the close event can be caught
   *                          and set to rejected by listening applications (such as qnet).
   *                          Fixes an issue where closing qnet and clicking 'cancel' from the
   *                          proceeding popup dialogue would still close the window but leave
   *                          the application running. Fixes #4146.
   */
  void ViewportMainWindow::closeEvent(QCloseEvent *event) {
    if (p_workspace->confirmClose()) {
      emit closeWindow(event);
      if (event->isAccepted()) {
        MainWindow::closeEvent(event);
      }
      else {
        event->ignore();
      }
    }
    else {
      event->ignore();
    }
  }
}
