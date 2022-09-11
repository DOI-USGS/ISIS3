/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractPlotTool.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPen>
#include <QStackedWidget>

#include "Cube.h"
#include "CubePlotCurve.h"
#include "MdiCubeViewport.h"
#include "PlotWindow.h"


namespace Isis {

  /**
   * When you construct a plot tool, this initializes the common functionality
   * between plot tools. For example, the select window combo box that appears
   * when you call createToolBarWidget().
   *
   * @param parent
   */
  AbstractPlotTool::AbstractPlotTool(QWidget *parent) : Tool(parent) {
    m_selectWindowCombo = new QComboBox;
    m_selectWindowCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_selectWindowCombo->setToolTip("Select which plot window to place plot "
                                  "curves.");
    QString selectWindowWhatsThis =
      "<b>Function:</b> This will allow the selection of a window to place new "
      "plot curves into.  Current curves in this window will be replaced by "
      "new plot curves.  You cannot paste plot curves into this window.";
    m_selectWindowCombo->setWhatsThis(selectWindowWhatsThis);
    m_selectWindowCombo->addItem("New Window...");
    m_selectWindowCombo->setCurrentIndex(-1);
    connect(m_selectWindowCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(selectedWindowChanged()));
  }


  /**
   * Clean up the abstract plot tool. This will destroy all of the plot windows.
   */
  AbstractPlotTool::~AbstractPlotTool() {
    if (m_selectWindowCombo) {
      // currentIndexChanged wants to call a pure virtual method, which crashes.
      disconnect(m_selectWindowCombo, SIGNAL(currentIndexChanged(int)),
              this, SLOT(selectedWindowChanged()));

      for (int i = m_selectWindowCombo->count(); i >= 0; i--) {
        QVariant windowVariant = m_selectWindowCombo->itemData(i);

        if (!windowVariant.isNull() && windowVariant.canConvert<PlotWindow *>()) {
          PlotWindow *window = windowVariant.value<PlotWindow *>();
          delete window;
          window = NULL;
        }
      }
    }
  }


  /**
   * This method allows each plot window to paint any information it wants onto
   * the cube viewports. For example, spatial plots can paint the originating
   * selection in the color of the curve.
   *
   * @param vp The viewport to paint onto
   * @param painter The painter to use for painting
   */
  void AbstractPlotTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {
    for (int i = 0; i < m_selectWindowCombo->count(); i++) {
      QVariant windowVariant = m_selectWindowCombo->itemData(i);

      if (!windowVariant.isNull() && windowVariant.canConvert<PlotWindow *>()) {
        PlotWindow *window = windowVariant.value<PlotWindow *>();
        window->paint(vp, painter);
      }
    }
  }


  /**
   * This provides the standard plot tool options, such as selecting an active
   * plot window.
   *
   * @param parent The stacked widget which will contain ours as one of the
   *               widgets it can show.
   *
   * @return QWidget* The widget we want the stacked widget to show when this is
   *               the active tool.
   */
  QWidget *AbstractPlotTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *toolBarWidget = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(new QLabel("Plot Into:"));
    layout->addWidget(m_selectWindowCombo);
    layout->addStretch(1);
    toolBarWidget->setLayout(layout);

    return toolBarWidget;
  }


  /**
   * This forwards all update calls to the plot windows.
   */
  void AbstractPlotTool::updateTool() {
    foreach (PlotWindow *window, plotWindows()) {
      window->update(cubeViewport());
    }
  }


  /**
   * Get a list of linked viewports that should be plotting when a new plot is
   *   requested. This is a utility for child classes. For example, a spatial
   *   plot will make a curve for each viewport.
   *
   * @return A list of viewports containing the active one and any that are
   *         linked to the active viewport.
   */
  QList<MdiCubeViewport *> AbstractPlotTool::viewportsToPlot() {
    QList<MdiCubeViewport *> viewports;
    MdiCubeViewport *activeViewport = cubeViewport();

    for (int i = 0; i < cubeViewportList()->size(); i++) {
      MdiCubeViewport *viewport = (*(cubeViewportList()))[i];

      if (viewport == activeViewport ||
          (activeViewport->isLinked() && viewport->isLinked())) {
        viewports.append(viewport);
      }
    }

    return viewports;
  }


  /**
   * Get a list of all of the instantiated plot windows.
   *
   * @return All of the plot windows associated with this tool
   */
  QList<PlotWindow *> AbstractPlotTool::plotWindows() {
    QList<PlotWindow *> windows;

    for (int i = 0; i < m_selectWindowCombo->count(); i++) {
      QVariant windowVariant = m_selectWindowCombo->itemData(i);

      if (!windowVariant.isNull() && windowVariant.canConvert<PlotWindow *>()) {
        PlotWindow *window = windowVariant.value<PlotWindow *>();
        windows.append(window);
      }
    }

    return windows;
  }


  /**
   * This method is called when the window where new curves are placed is
   *   changed by the user. If the current selection has no window associated
   *   with it, then one is created. Otherwise, the selected window is
   *   explicitly shown and any curves associated with an old window should be
   *   forgotten (but not deleted, see detachCurves()).
   */
  void AbstractPlotTool::selectedWindowChanged() {
    int currentIndex = m_selectWindowCombo->currentIndex();
    if (currentIndex != -1) {
      // Selected an item in the list that isn't a window
      if (m_selectWindowCombo->itemData(currentIndex).isNull()) {
        addWindow();
      }
      else if (selectedWindow(false)) {
        selectedWindow(false)->showWindow();
      }
    }

    detachCurves();
  }


  /**
   * When a user closes a window, we want to remove that window from our
   *   combo box for selecting the active window. We also repaint all of the
   *   cube viewports so that the destroyed window leaves no visible artifacts.
   *
   * @param window The plot window that is to be removed
   */
  void AbstractPlotTool::removeWindow(QObject *window) {
    // See if we need to unselect it before removing it
    int currentWindowIndex = m_selectWindowCombo->currentIndex();
    QVariant windowVariant = QVariant::fromValue((PlotWindow *) window);

    if (currentWindowIndex != -1) {
      if (m_selectWindowCombo->itemData(currentWindowIndex) == windowVariant) {
        m_selectWindowCombo->setCurrentIndex(-1);
      }
    }

    m_selectWindowCombo->removeItem(
        m_selectWindowCombo->findData(windowVariant));

    repaintViewports();
  }


  /**
   * This is a helper method for children. Given a title, a color, and units
   *   a new CubePlotCurve is created.
   *
   * @param name The title of the curve to be created
   * @param pen The color & thickness of the curve
   * @param xUnits The units of the x-axis associated with this curve. This must
   *               match the plot window's x axis.
   * @param yUnits The units of the y-axis associated with this curve. This must
   *               match the plot window's y axis.
   * @return createCurve The requested plot curve
   */
  CubePlotCurve * AbstractPlotTool::createCurve(QString name, QPen pen,
      PlotCurve::Units xUnits, PlotCurve::Units yUnits) {
    CubePlotCurve * newCurve = new CubePlotCurve(xUnits, yUnits);

    newCurve->setTitle(name);
    newCurve->setPen(pen);
    newCurve->setColor(pen.color());

    return newCurve;
  }


  /**
   * Get the 'active' plot window (the window selected by the user to contain
   *   new curves). This may return NULL if and only if createIfNeeded is false.
   *   Windows are created by child classes.
   *
   * @param createIfNeeded If this is true, and no window is selected in the
   *                       active window combo box, then a window will be
   *                       created and selected before this method returns.
   *
   * @return The user-selected active plot window
   */
  PlotWindow *AbstractPlotTool::selectedWindow(bool createIfNeeded) {
    PlotWindow *window = NULL;
    int curIndex = m_selectWindowCombo->currentIndex();

    if (curIndex != -1) {
      QVariant windowVariant = m_selectWindowCombo->itemData(
          m_selectWindowCombo->currentIndex());

      if (!windowVariant.isNull() && windowVariant.canConvert<PlotWindow *>()) {
        window = windowVariant.value<PlotWindow *>();
      }
    }

    if (!window && createIfNeeded) {
      window = addWindow();
    }

    return window;
  }


  /**
   * This method causes the viewports corresponding with the given
   * CubePlotCurve to be repainted with all of the area's of
   * interest associated with the CubePlotCurve's PlotWindow.
   *
   * @param pc The plot curve which needs to repaint
   */
  void AbstractPlotTool::repaintViewports(CubePlotCurve *pc) {
    QVector<MdiCubeViewport *> allViewports = *cubeViewportList();

    foreach (MdiCubeViewport * viewport, allViewports) {
      if (pc->sourceCube().contains(viewport->cube()->fileName())) {
        viewport->repaint();
      }
    }
  }


  /**
   * This creates and initializes everything about a plot window. This updates
   *   the window's title to be unique, adds it to the active plot window combo
   *   box, listens for the window to be removed and selects it in the active
   *   plot window combo box. Call this if you need a new plot window.
   *
   * @return The newly initialized plot window
   */
  PlotWindow *AbstractPlotTool::addWindow() {
    PlotWindow *newPlotWindow = createWindow();

    connect(newPlotWindow, SIGNAL(closed()),
            newPlotWindow, SLOT(deleteLater()));

    QString originalTitle = newPlotWindow->windowTitle();
    QString titleToTry = originalTitle;
    bool titleUsed = false;
    int titleNumber = 0;

    do {
      titleNumber++;
      if (titleNumber > 1) {
        titleToTry = originalTitle + " " + QString::number(titleNumber);
      }

      titleUsed = false;

      for (int i = 0; i < m_selectWindowCombo->count() && !titleUsed; i++) {
        titleUsed = titleUsed ||
                    (m_selectWindowCombo->itemText(i) == titleToTry);
      }
    } while (titleUsed);

    newPlotWindow->setWindowTitle(titleToTry);

    int newItemIndex = m_selectWindowCombo->count() - 1;

    m_selectWindowCombo->setCurrentIndex(-1);
    m_selectWindowCombo->insertItem(newItemIndex,
        newPlotWindow->windowTitle(), QVariant::fromValue(newPlotWindow));
    m_selectWindowCombo->setCurrentIndex(newItemIndex);

    connect(newPlotWindow, SIGNAL(destroyed(QObject *)),
            this, SLOT(removeWindow(QObject *)));
    connect(newPlotWindow, SIGNAL(plotChanged()),
            this, SLOT(repaintViewports()));

    return newPlotWindow;
  }


  /**
   * displays the plot window
   *
   */
  void AbstractPlotTool::showPlotWindow() {
    if (selectedWindow()) {
      selectedWindow()->showWindow();
    }
  }


  /**
   * This method causes all of the viewports to be repainted. This is
   *   useful because it removes visible artifacts from deleted plot
   *   windows/curves.
   */
  void AbstractPlotTool::repaintViewports() {
    QVector<MdiCubeViewport *> allViewports = *cubeViewportList();

    foreach (MdiCubeViewport * viewport, allViewports) {
      viewport->viewport()->repaint();
    }
  }
}
