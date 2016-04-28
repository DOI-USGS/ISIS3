#include "ScatterPlotTool.h"

#include <QAction>
#include <QHBoxLayout>
#include <QPushButton>

#include "IString.h"
#include "MdiCubeViewport.h"
#include "ScatterPlotConfigDialog.h"
#include "ScatterPlotWindow.h"
#include "ToolPad.h"

namespace Isis {

  /**
   * ScatterPlotTool constructor.
   *
   * @param parent
   */
  ScatterPlotTool::ScatterPlotTool(QWidget *parent) : Tool(parent),
      m_plotWindows(new QList< QPointer<ScatterPlotWindow> >) {
  }


  /**
   * Gives the programmer more flexibility on when the action
   * button for this tool is checked or not.
   *
   * @param checked
   */
  void ScatterPlotTool::setActionChecked(bool checked) {
    m_action->setChecked(checked);
  }


  /**
   * When a viewport needs repainted this is called. We are going to give the
   *   plot windows a chance to paint onto the viewport.
   *
   * @param vp The viewport to potentially be painted on
   * @param painter The painter to use for painting
   */
  void ScatterPlotTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {
    m_plotWindows->removeAll(NULL);

    foreach (ScatterPlotWindow *window, *m_plotWindows) {
      window->paint(vp, painter);
    }
  }


  /**
   * Configure the QAction for this tool.
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *ScatterPlotTool::toolPadAction(ToolPad *toolpad) {
    m_action = new QAction(toolpad);
    m_action->setIcon(QPixmap(toolIconDir() + "/scatterplot.png"));
    m_action->setToolTip("Scatter Plot");
    //action->setShortcut(Qt::Key_C);
    connect(m_action, SIGNAL(triggered()),
            this, SLOT(showNewScatterPlotConfig()));

    QString text  =
    "<b>Function:</b>  Compare two bands of same image or of a different image. \
      <p><b>Shortcut:</b>nonexsistant right now</p> ";
    m_action->setWhatsThis(text);
    return m_action;
  }


  /**
   * Get the action which activates this tool.
   *
   * @return The action which activates this tool
   */
  QAction *ScatterPlotTool::toolAction() {
    return m_action;
  }


  /**
   * Create the toolbar options widget for this tool's options.
   *
   * @param parent The stacked widget this will be put into.
   * @return A widget containing detailed options for this tool
   */
  QWidget *ScatterPlotTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *wrapper = new QWidget;

    QPushButton *create = new QPushButton("New Scatter Plot");
    connect(create, SIGNAL(clicked()),
            this, SLOT(showNewScatterPlotConfig()));


    QHBoxLayout *layout = new QHBoxLayout(wrapper);
    layout->setMargin(0);

    layout->addWidget(create);

    layout->addStretch(1);
    wrapper->setLayout(layout);
    return wrapper;
  }


  /**
   * The user has asked to create a scatter plot. If we can, create the scatter
   *   plot.
   */
  void ScatterPlotTool::onScatterPlotConfigAccepted() {
    try {
      if (m_configDialog) {
        ScatterPlotWindow *window = new ScatterPlotWindow("Scatter Plot",
          m_configDialog->xAxisCube(), m_configDialog->xAxisCubeBand(),
          m_configDialog->xAxisBinCount(), m_configDialog->yAxisCube(),
          m_configDialog->yAxisCubeBand(), m_configDialog->yAxisBinCount(),
          m_configDialog->sampleRange(), m_configDialog->lineRange(),
          qobject_cast<QWidget *>(parent()));
        connect(window, SIGNAL(closed()), window, SLOT(deleteLater()));
        connect(window, SIGNAL(plotChanged()),
                this, SLOT(repaintViewports()));
        connect(m_configDialog->xAxisCubeViewport(),
                SIGNAL(destroyed(QObject *)),
                window, SLOT(forgetCubes()));
        connect(m_configDialog->yAxisCubeViewport(),
                SIGNAL(destroyed(QObject *)),
                window, SLOT(forgetCubes()));

        delete m_configDialog;
        m_plotWindows->append(window);
        window->show();
      }
    }
    catch (...) {
      delete m_configDialog;
      m_configDialog = NULL;
    }
  }


  /**
   * The user has cancelled creating a scatter plot. Delete the configuration
   *   dialog.
   */
  void ScatterPlotTool::onScatterPlotConfigRejected() {
    delete m_configDialog;
  }


  /**
   * The user has moved their mouse on the cube viewport. We're going to
   *   notify the plot windows about this for alarming viewport->plot.
   *
   * @param p The mouse location
   */
  void ScatterPlotTool::mouseMove(QPoint p, Qt::MouseButton) {
    m_plotWindows->removeAll(NULL);

    foreach (ScatterPlotWindow *window, *m_plotWindows) {
      window->setMousePosition(cubeViewport(), p);
    }
  }


  /**
   * The user moused out of the viewport. Let the plot windows know this for
   *   alarming viewport->plot.
   */
  void ScatterPlotTool::mouseLeave() {
    m_plotWindows->removeAll(NULL);

    foreach (ScatterPlotWindow *window, *m_plotWindows) {
      window->setMousePosition(NULL, QPoint());
    }
  }


  /**
   * This is a helper method for asking every viewport to repaint. Any time
   *   alarming changes this needs to happen.
   */
  void ScatterPlotTool::repaintViewports() {
    QVector<MdiCubeViewport *> allViewports = *cubeViewportList();

    foreach (MdiCubeViewport * viewport, allViewports) {
      viewport->viewport()->repaint();
    }
  }


  /**
   * Ask the user to give us information for a new scatter plot.
   */
  void ScatterPlotTool::showNewScatterPlotConfig() {
    try {
      if (!m_configDialog) {
        m_configDialog = new ScatterPlotConfigDialog(
            cubeViewport(), workspace());
        connect(m_configDialog, SIGNAL(accepted()),
                this, SLOT(onScatterPlotConfigAccepted()));
        connect(m_configDialog, SIGNAL(rejected()),
                this, SLOT(onScatterPlotConfigRejected()));
        m_configDialog->show();
      }
      else {
        m_configDialog->activateWindow();
      }
    }
    catch (...) {
      m_configDialog = NULL;
      throw;
    }
  }
}

