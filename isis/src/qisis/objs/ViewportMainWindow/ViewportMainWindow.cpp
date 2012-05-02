#include <QStatusBar>
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QSettings>

#include "iString.h"
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
    installEventFilter(this);
    p_workspace = new Workspace(this);
    setCentralWidget(p_workspace);
    this->setWindowTitle(title);

    p_permToolbar = new QToolBar("Standard Tools", this);
    p_permToolbar->setObjectName("perm");
    p_permToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    p_permToolbar->setIconSize(QSize(22, 22));
    this->addToolBar(p_permToolbar);

    p_activeToolbar = new QToolBar("Active Tool", this);
    p_activeToolbar->setObjectName("Active");
    p_activeToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    p_activeToolbar->setIconSize(QSize(22, 22));
    this->addToolBar(p_activeToolbar);

    QStatusBar *sbar = statusBar();
    sbar->showMessage("Ready");

    mTrackTool = new TrackTool(sbar);
    mTrackTool->addTo(this);

    p_toolpad = new ToolPad("Tool Pad", this);
    p_toolpad->setObjectName("ViewportMainWindow");
    p_toolpad->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea);
    this->addToolBar(Qt::RightToolBarArea, p_toolpad);

    readSettings();
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
   * This overriden method is called from the constructor so that
   * when the viewportmainwindow is created, it know's it's size
   * and location and the tool bar location.
   *
   */
  void ViewportMainWindow::readSettings() {
    /*Call the base class function to read the size and location*/
    this->MainWindow::readSettings();
    /*Now read the settings that are specific to this window.*/
    std::string instanceName = this->windowTitle().toStdString();
    FileName config("$HOME/.Isis/" + instanceName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.expanded()), QSettings::NativeFormat);
    QByteArray state = settings.value("state", QByteArray("0")).toByteArray();
    restoreState(state);
  }


  /**
   * This overriden method is called when the viewportmainwindow
   * is closed or hidden to write the size and location settings
   * (and tool bar location) to a config file in the user's home
   * directory.
   *
   */
  void ViewportMainWindow::writeSettings() {
    /*Call the base class function to write the size and location*/
    this->MainWindow::writeSettings();
    /*Now write the settings that are specific to this window.*/
    std::string instanceName = this->windowTitle().toStdString();
    FileName config("$HOME/.Isis/" + instanceName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.expanded()), QSettings::NativeFormat);
    settings.setValue("state", this->saveState());
  }


  /**
   * This class is called when a close event occurs, it emits a
   * signal and ignores the close event.
   *
   * @param event
   */
  void ViewportMainWindow::closeEvent(QCloseEvent *event) {
    emit closeWindow();
    event->ignore();
  }


  /**
   * This event filter is installed in the constructor.
   * @param o
   * @param e
   *
   * @return bool
   */
  bool ViewportMainWindow::eventFilter(QObject *o, QEvent *e) {
    switch(e->type()) {
      case QEvent::Close: {
          writeSettings();
        }

      default: {
          return false;
        }
    }
  }

}

