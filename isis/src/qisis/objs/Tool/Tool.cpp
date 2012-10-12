#include "Tool.h"

#include <QLayout>
#include <QMenuBar>
#include <QVector>

#include "CubeViewport.h"
#include "MdiCubeViewport.h"
#include "FileName.h"
#include "IString.h"
#include "RubberBandTool.h"
#include "ToolPad.h"
#include "Workspace.h"
#include "ViewportMainWindow.h"


namespace Isis {
  QStackedWidget *Tool::p_activeToolBarStack = NULL;

  /**
   * Tool constructor
   *
   * @param parent
   */
  Tool::Tool(QWidget *parent) : QObject(parent) {
    p_cvp = NULL;
    p_workspace = NULL;
    p_active = false;
    p_toolPadAction = NULL;
    p_toolBarWidget = NULL;
    std::string tempFileName = FileName("$base/icons").expanded();
    p_toolIconDir = tempFileName.c_str();
  }


  /**
   * Adds the given workspace to the cubeviewport list.
   *
   * @param ws
   */
  void Tool::addTo(Workspace *ws) {
    p_workspace = ws;

    connect(ws, SIGNAL(cubeViewportAdded(MdiCubeViewport *)),
            this, SLOT(setCubeViewport(MdiCubeViewport *)));
    connect(ws, SIGNAL(cubeViewportActivated(MdiCubeViewport *)),
            this, SLOT(setCubeViewport(MdiCubeViewport *)));
    connect(ws, SIGNAL(cubeViewportAdded(MdiCubeViewport *)),
            this, SLOT(registerTool(MdiCubeViewport *)));
  }

  /**
   * Adds the tool to the application
   *
   * @param pViewPortMnWin
   */
  void Tool::addTo(ViewportMainWindow *pViewPortMnWin) {
    addTo(pViewPortMnWin->workspace());
    addToPermanent(pViewPortMnWin->permanentToolBar());
    addToActive(pViewPortMnWin->activeToolBar());
    addTo(pViewPortMnWin->toolPad());
    if(!menuName().isEmpty()) {
      QMenu *menu = pViewPortMnWin->getMenu(menuName());
//      if (menu->actions().size() > 0) menu->addSeparator();
      addTo(menu);
    }

    //connect(this, SIGNAL(clearWarningSignal ()), pViewPortMnWin, SLOT(resetWarning ()));
  }


  /**
   * Adds the tool to the toolpad
   *
   * @param toolpad
   */
  void Tool::addTo(ToolPad *toolpad) {
    p_toolPadAction = toolPadAction(toolpad);
    if(p_toolPadAction != NULL) {
      toolpad->addAction(p_toolPadAction);
      connect(p_toolPadAction, SIGNAL(toggled(bool)), this, SLOT(activate(bool)));
    }
  }


  /**
   *
   *
   * @param toolbar
   */
  void Tool::addToActive(QToolBar *toolbar) {
    if(p_activeToolBarStack == NULL) {
      p_activeToolBarStack = new QStackedWidget(toolbar);
      toolbar->addWidget(p_activeToolBarStack);
    }

    p_toolBarWidget = createToolBarWidget(p_activeToolBarStack);
    if(p_toolBarWidget != NULL) {
      p_activeToolBarStack->addWidget(p_toolBarWidget);
    }
    disableToolBar();
  }


  /**
   * Activates the tool.
   *
   * @param on
   */
  void Tool::activate(bool on) {
    if(p_active) {
      emit clearWarningSignal();
      if(on)
        return;
      removeViewportConnections();
      disableToolBar();
      if(p_toolPadAction != NULL)
        p_toolPadAction->setChecked(false);
      p_active = false;
    }
    else {
      if(!on)
        return;
      if(p_toolPadAction != NULL)
        p_toolPadAction->setChecked(true);
      addViewportConnections();
      enableToolBar();
      emit toolActivated();
      p_active = true;
    }
  }


  /**
   * Sets the current viewport to the given cvp
   *
   * @param cvp
   */
  void Tool::setCubeViewport(MdiCubeViewport *cvp) {
    if(cvp == p_cvp) {
      updateTool();
      return;
    }

    if(p_active)
      removeViewportConnections();

    p_cvp = cvp;

    if(p_active) {
      addViewportConnections();
      enableToolBar();
    }
    else {
      updateTool();
    }

    emit viewportChanged();
  }


  /**
   * Makes all the connections for the tool.
   *
   */
  void Tool::addViewportConnections() {
    if(p_cvp == NULL)
      return;

    connect(p_cvp, SIGNAL(scaleChanged()),
            this, SLOT(scaleChanged()));

    connect(RubberBandTool::getInstance(), SIGNAL(measureChange()),
            this, SLOT(updateMeasure()));

    connect(RubberBandTool::getInstance(), SIGNAL(bandingComplete()),
            this, SLOT(rubberBandComplete()));

    connect(p_cvp, SIGNAL(mouseEnter()),
            this, SLOT(mouseEnter()));

    connect(p_cvp, SIGNAL(screenPixelsChanged()),
            this, SLOT(screenPixelsChanged()));

    connect(p_cvp, SIGNAL(mouseMove(QPoint)),
            this, SLOT(mouseMove(QPoint)), Qt::DirectConnection);

    connect(p_cvp, SIGNAL(mouseMove(QPoint, Qt::MouseButton)),
            this, SLOT(mouseMove(QPoint, Qt::MouseButton)), Qt::DirectConnection);

    connect(p_cvp, SIGNAL(mouseLeave()),
            this, SLOT(mouseLeave()));

    connect(p_cvp, SIGNAL(mouseDoubleClick(QPoint)),
            this, SLOT(mouseDoubleClick(QPoint)));

    connect(p_cvp, SIGNAL(mouseButtonPress(QPoint, Qt::MouseButton)),
            this, SLOT(mouseButtonPress(QPoint, Qt::MouseButton)));

    connect(p_cvp, SIGNAL(mouseButtonRelease(QPoint, Qt::MouseButton)),
            this, SLOT(mouseButtonRelease(QPoint, Qt::MouseButton)));

    addConnections(p_cvp);

    if(p_toolPadAction != NULL) {
      enableRubberBandTool();
    }
  }


  /**
   * Removes all the connections from the tool.
   *
   */
  void Tool::removeViewportConnections() {
    if(p_cvp == NULL)
      return;

    disconnect(p_cvp, SIGNAL(scaleChanged()),
               this, SLOT(scaleChanged()));

    disconnect(RubberBandTool::getInstance(), SIGNAL(measureChange()),
               this, SLOT(updateMeasure()));

    disconnect(RubberBandTool::getInstance(), SIGNAL(bandingComplete()),
               this, SLOT(rubberBandComplete()));

    disconnect(p_cvp, SIGNAL(mouseEnter()),
               this, SLOT(mouseEnter()));

    disconnect(p_cvp, SIGNAL(screenPixelsChanged()),
               this, SLOT(screenPixelsChanged()));

    disconnect(p_cvp, SIGNAL(mouseMove(QPoint)),
               this, SLOT(mouseMove(QPoint)));

    disconnect(p_cvp, SIGNAL(mouseMove(QPoint, Qt::MouseButton)),
               this, SLOT(mouseMove(QPoint, Qt::MouseButton)));

    disconnect(p_cvp, SIGNAL(mouseLeave()),
               this, SLOT(mouseLeave()));

    disconnect(p_cvp, SIGNAL(mouseDoubleClick(QPoint)),
               this, SLOT(mouseDoubleClick(QPoint)));

    disconnect(p_cvp, SIGNAL(mouseButtonPress(QPoint, Qt::MouseButton)),
               this, SLOT(mouseButtonPress(QPoint, Qt::MouseButton)));

    disconnect(p_cvp, SIGNAL(mouseButtonRelease(QPoint, Qt::MouseButton)),
               this, SLOT(mouseButtonRelease(QPoint, Qt::MouseButton)));

    removeConnections(p_cvp);
  }


  /**
   * Disables entire tool bar.
   *
   */
  void Tool::disableToolBar() {
    if(p_toolBarWidget == NULL)
      return;
//    if (p_toolBarWidget->isVisible()) p_toolBarWidget->hide();
    p_toolBarWidget->setEnabled(false);
  }


  /**
   * Enables entire tool bar.
   *
   */
  void Tool::enableToolBar() {
    updateTool();
    if(p_toolBarWidget == NULL)
      return;
    if(cubeViewport() == NULL) {
      p_toolBarWidget->setEnabled(false);
    }
    else {
      p_toolBarWidget->setEnabled(true);
    }
    p_activeToolBarStack->setCurrentWidget(p_toolBarWidget);
  }


  /**
   * Updates the tool.
   *
   */
  void Tool::updateTool() {
  }


  /**
   * Registers the tool to the viewport.
   *
   * @param viewport
   */
  void Tool::registerTool(MdiCubeViewport *viewport) {
    viewport->registerTool(this);

    connect(p_cvp, SIGNAL(requestRestretch(MdiCubeViewport *, int)),
            this, SLOT(stretchRequested(MdiCubeViewport *, int)));
  }


  /**
   * Enable the use of the rubberband tool.
   *
   */
  void Tool::enableRubberBandTool() {
    RubberBandTool::disable();
  }


  /**
   * @param p
   */
  void Tool::mouseMove(QPoint p) {};


  /**
   * @param p
   */
  void Tool::mouseDoubleClick(QPoint p) {
    emit clearWarningSignal();
  }


  /**
   * @param p
   * @param s
   */
  void Tool::mouseButtonPress(QPoint p, Qt::MouseButton s) {
    emit clearWarningSignal();
  }


  /**
   * Resets the Warning to Nowarning when a different activity
   * occurs on the application. This is called by all the
   * mouseButtonRelease events in all the tools.
   *
   * @param p
   * @param s
   */
  void Tool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    emit clearWarningSignal();
  }


  /**
   * Return the list of cubeviewports.
   *
   * @return CubeViewportList*
   */
  Tool::CubeViewportList *Tool::cubeViewportList() const {
    return p_workspace->cubeViewportList();
  }
}



