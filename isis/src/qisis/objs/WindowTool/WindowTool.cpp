#include "WindowTool.h"

#include <iostream>

#include <QAction>
#include <QList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPoint>
#include <QRect>
#include <QToolBar>

#include "MdiCubeViewport.h"
#include "MainWindow.h"
#include "Workspace.h"

namespace Isis {
  /**
   * WindowTool constructor.
   *
   *
   * @param parent
   */
  WindowTool::WindowTool(QWidget *parent) : Tool(parent) {
    p_cascadeWindows = new QAction(parent);
    p_cascadeWindows->setText("&Cascade");
    p_cascadeWindows->setEnabled(false);

    p_tileWindows = new QAction(parent);
    p_tileWindows->setText("&Tile");
    p_tileWindows->setEnabled(false);

    p_resizeWindows = new QAction(parent);
    p_resizeWindows->setText("Resize");
    p_resizeWindows->setEnabled(true);
    connect(p_resizeWindows, SIGNAL(triggered()), this, SLOT(resizeWindows()));
    QString text = "<b>Function: </b> Resize all linked viewports to the same \
                   size as the active viewport.";
    p_resizeWindows->setWhatsThis(text);

    p_closeWindow = new QAction(parent);
    p_closeWindow->setText("Close");
    p_closeWindow->setShortcut(Qt::Key_F3);
    p_closeWindow->setEnabled(false);

    p_closeAllWindows = new QAction(parent);
    p_closeAllWindows->setText("Close All");
    p_closeAllWindows->setShortcut(Qt::CTRL + Qt::Key_F3);
    p_closeAllWindows->setEnabled(false);

    p_nextWindow = new QAction(parent);
    p_nextWindow->setText("&Next");
    p_nextWindow->setShortcut(Qt::Key_F5);
    p_nextWindow->setEnabled(false);

    p_prevWindow = new QAction(parent);
    p_prevWindow->setText("&Prev");
    p_prevWindow->setShortcut(Qt::Key_F6);
    p_prevWindow->setEnabled(false);

    QIcon icon;
    icon.addPixmap(toolIconDir() + "/linked.png", QIcon::Normal, QIcon::On);
    icon.addPixmap(toolIconDir() + "/unlinked.png", QIcon::Normal, QIcon::Off);
    p_linkWindow = new QAction(parent);
    p_linkWindow->setIcon(icon);
    p_linkWindow->setText("&Link");
    p_linkWindow->setToolTip("Link viewports");
    text =
      "<b>Function:</b> Used to link viewports.  Some tools apply their \
      functions to all linked viewports.  For example, when the zoom tool \
      is used on a linked viewport then all other linked viewports will zoom \
      as well. \
      <p><b>Shortcut:</b>  Ctrl+L</p> \
      <p><b>Hint:</b> The icons <img src=\"" +
      toolIconDir() + "/linked.png\" width=22 height=22> and <img src=\"" +
      toolIconDir() + "/unlinked.png\" width=22 height=22> at the left edge \
      of each viewport titlebar indicate the current link state</p> \
      <p><b>Tools using Link:</b> Zoom, Pan, Blink, and Advanced Tracking </p>";
    p_linkWindow->setWhatsThis(text);
    p_linkWindow->setShortcut(Qt::CTRL + Qt::Key_L);
    p_linkWindow->setCheckable(true);
    p_linkWindow->setEnabled(false);

    p_linkAllWindows = new QAction(parent);
    p_linkAllWindows->setText("&Link All");
    p_linkAllWindows->setToolTip("Link all viewports");
    p_linkAllWindows->setWhatsThis("<b>Function: </b> Links all open viewports \
                                   together. <p><b>Shortcut: </b> Ctrl+Shift+L");
    p_linkAllWindows->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_L);
    p_linkAllWindows->setEnabled(false);
    connect(p_linkAllWindows, SIGNAL(triggered()), this, SLOT(linkWindows()));

    p_unlinkAllWindows = new QAction(parent);
    p_unlinkAllWindows->setText("&Unlink All");
    p_unlinkAllWindows->setToolTip("Unlink all viewports");
    p_unlinkAllWindows->setWhatsThis("<b>Function: </b> Unlinks all open viewports. \
                                     <p><b>Shortcut: </b> Ctrl+Shift+U");
    p_unlinkAllWindows->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_U);
    p_unlinkAllWindows->setEnabled(false);
    connect(p_unlinkAllWindows, SIGNAL(triggered()), this, SLOT(unlinkWindows()));

    p_changeCursor = new QAction(parent);
    p_changeCursor->setText("Change cursor to arrow.");
    p_changeCursor->setWhatsThis("<b>Function: </b> Toggles the cursor shape between \
                                 and arrow and crosshair cursor when cursor is over the \
                                 viewport window.");
    p_changeCursor->setEnabled(false);
    connect(p_changeCursor, SIGNAL(triggered()), this, SLOT(changeCursor()));

    activate(true);
  }


  /**
   * Adds the window to the workspace.
   *
   *
   * @param ws
   */
  void WindowTool::addTo(Workspace *ws) {
    p_mdiArea = ws->mdiArea();
    Tool::addTo(ws);
    connect(p_cascadeWindows, SIGNAL(triggered()), ws->mdiArea(), SLOT(cascadeSubWindows()));
    connect(p_tileWindows, SIGNAL(triggered()), this, SLOT(tileViewports()));
    connect(p_prevWindow, SIGNAL(triggered()), ws->mdiArea(), SLOT(activatePreviousSubWindow()));
    connect(p_nextWindow, SIGNAL(triggered()), ws->mdiArea(), SLOT(activateNextSubWindow()));
    connect(p_closeWindow, SIGNAL(triggered()), ws->mdiArea(), SLOT(closeActiveSubWindow()));
    connect(p_closeAllWindows, SIGNAL(triggered()), ws->mdiArea(), SLOT(closeAllSubWindows()));
    connect(ws, SIGNAL(cubeViewportAdded(MdiCubeViewport *)),
            this, SLOT(updateViewportCursor(MdiCubeViewport *)));
  }


  /**
   * Helper function for determining the size of the viewports.
   *
   * @return Returns the size that the viewports should be.
   * @internal
   *  @history 2017-07-19 Tracie Sucharski - Original version.
   */
  int WindowTool::viewportSize() {
    int numViewports = p_mdiArea->subWindowList().size();

    double mdiWidth = p_mdiArea->width();
    double mdiHeight = p_mdiArea->height();

    double px = ceil(sqrt(numViewports*mdiWidth/mdiHeight));
    double sx, sy;
    if (floor(px*mdiHeight/mdiWidth)*px < numViewports) {
      sx = mdiHeight/ceil(px*mdiHeight/mdiWidth);
    }
    else {
      sx = mdiWidth/px;
    }

    double py = ceil(sqrt(numViewports * mdiHeight / mdiWidth));
    if (floor(py*mdiWidth/mdiHeight)*py < numViewports) {
      sy = mdiWidth/ceil(mdiWidth*py/mdiHeight);
    }
    else {
      sy = mdiHeight/py;
    }

    return std::max(sx,sy);
  }

 /**
   * Tiles the cube viewports over the Cube DN View.
   *
   * @internal
   *  @history 2017-07-19 Marjorie Hahn and Tracie Sucharski - Original version.
   */
  void WindowTool::tileViewports() {
    int vpSize = viewportSize();

    QPoint position(0, 0);

    QList<QMdiSubWindow *> windowList = p_mdiArea->subWindowList();

    for (int i = windowList.size() - 1; i >= 0; i--) {
      QMdiSubWindow *window = windowList[i];
      window->showNormal();
      QRect rect(0, 0, vpSize, vpSize);
      window->setGeometry(rect);
      window->move(position);

      position.setX(position.x() + window->width());
      if (position.x() + window->width() > p_mdiArea->width()) {
        position.setX(0);
        position.setY(position.y() + window->height());
      }
    }
  }


  /**
   * Adds the link window action to the tool bar.
   *
   *
   * @param perm
   */
  void WindowTool::addToPermanent(QToolBar *perm) {
    perm->addAction(p_linkWindow);
  }


  /**
   * Adds the cascade windows, tile windows, resize windows, next
   * window, previous window, close window, and close all windows
   * actions to the menu.
   *
   *
   * @param menu
   */
  void WindowTool::addTo(QMenu *menu) {
    menu->addAction(p_cascadeWindows);
    menu->addAction(p_tileWindows);
    menu->addAction(p_resizeWindows);
    menu->addSeparator();

    menu->addAction(p_changeCursor);
    menu->addSeparator();

    menu->addAction(p_nextWindow);
    menu->addAction(p_prevWindow);
    menu->addAction(p_closeWindow);
    menu->addAction(p_closeAllWindows);
    menu->addSeparator();

    menu->addAction(p_linkWindow);
    menu->addAction(p_linkAllWindows);
    menu->addAction(p_unlinkAllWindows);
  }


  /**
   * Adds the connections to the cube viewport.
   *
   *
   * @param cvp
   */
  void WindowTool::addConnections(MdiCubeViewport *cvp) {
    connect(p_linkWindow, SIGNAL(toggled(bool)),
            cubeViewport(), SLOT(setLinked(bool)));
    connect(cvp, SIGNAL(linkChanging(bool)), p_linkWindow, SLOT(setChecked(bool)));
  }


  /**
   * Removes the connections from the cube viewport.
   *
   *
   * @param cvp
   */
  void WindowTool::removeConnections(MdiCubeViewport *cvp) {
    disconnect(p_linkWindow, SIGNAL(toggled(bool)),
               cubeViewport(), SLOT(setLinked(bool)));
    disconnect(cvp, SIGNAL(linkChanging(bool)), p_linkWindow, SLOT(setChecked(bool)));
  }


  /**
   * Links all viewport windows in the workspace.
   *
   */
  void WindowTool::linkWindows() {
    MdiCubeViewport *d;
    for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
      d = (*(cubeViewportList()))[i];
      d->setLinked(true);
    }
  }


  /**
   * Unlinks all the viewport windows in the workspace.
   *
   */
  void WindowTool::unlinkWindows() {
    MdiCubeViewport *d;
    for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
      d = (*(cubeViewportList()))[i];
      d->setLinked(false);
    }
  }

  /**
   * Toggles the cursor from an arrow to a crosshair.
   *
   */
  void WindowTool::changeCursor() {
    if (p_changeCursor->text() == "Change cursor to arrow.") {
      p_changeCursor->setText("Change cursor to crosshair.");
    }
    else {
      p_changeCursor->setText("Change cursor to arrow.");
    }

    for (int i = 0; i < (int)cubeViewportList()->size(); i++) {
      updateViewportCursor(cubeViewportList()->at(i));
    }
  }

  /**
   * Updates the cursor over the viewport.
   *
   */
  void WindowTool::updateViewportCursor(MdiCubeViewport *cvp) {
    if (p_changeCursor->text() == "Change cursor to crosshair." &&
        cvp->viewport()->cursor().shape() != Qt::ArrowCursor) {
      cvp->viewport()->setCursor(Qt::ArrowCursor);
    }
    else if (p_changeCursor->text() == "Change cursor to arrow." &&
        cvp->viewport()->cursor().shape() != Qt::CrossCursor) {
      cvp->viewport()->setCursor(Qt::CrossCursor);
    }
  }


  /**
   * Resizes all the viewport windows to the active viewport
   * window size.
   *
   */
  void WindowTool::resizeWindows() {
    MdiCubeViewport *d;
    QSize size = cubeViewport()->parentWidget()->size();

    for(int i = 0; i < (int)cubeViewportList()->size(); i++) {
      d = (*(cubeViewportList()))[i];
      if(d->isLinked()) d->parentWidget()->parentWidget()->resize(size);
    }
  }


  /**
   * Updates the WindowTool.
   *
   */
  void WindowTool::updateTool() {
    if(cubeViewport() == NULL) {
      p_linkWindow->setChecked(false);
      p_linkWindow->setEnabled(false);
      p_linkAllWindows->setEnabled(false);
      p_unlinkAllWindows->setEnabled(false);
      p_cascadeWindows->setEnabled(false);
      p_tileWindows->setEnabled(false);
      p_resizeWindows->setEnabled(false);
      p_closeWindow->setEnabled(false);
      p_closeAllWindows->setEnabled(false);
      p_nextWindow->setEnabled(false);
      p_prevWindow->setEnabled(false);
      p_changeCursor->setEnabled(false);
    }
    else {
      p_cascadeWindows->setEnabled(true);
      p_tileWindows->setEnabled(true);
      p_resizeWindows->setEnabled(true);
      p_closeWindow->setEnabled(true);
      p_closeAllWindows->setEnabled(true);
      p_changeCursor->setEnabled(true);

      if(cubeViewportList()->size() > 1) {
        p_linkWindow->setEnabled(true);
        p_linkAllWindows->setEnabled(true);
        p_unlinkAllWindows->setEnabled(true);
        p_nextWindow->setEnabled(true);
        p_prevWindow->setEnabled(true);
        p_linkWindow->setChecked(cubeViewport()->isLinked());
      }
      else {
        p_linkWindow->setEnabled(false);
        p_linkAllWindows->setEnabled(false);
        p_unlinkAllWindows->setEnabled(false);
        p_nextWindow->setEnabled(false);
        p_prevWindow->setEnabled(false);
      }
    }
  }
}
