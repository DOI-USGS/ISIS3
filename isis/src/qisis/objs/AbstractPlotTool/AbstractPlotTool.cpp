#include "IsisDebug.h"

#include "AbstractPlotTool.h"

#include "Cube.h"
#include "CubePlotCurve.h"
#include "MdiCubeViewport.h"
#include "PlotWindow.h"

#include <QComboBox>
#include <QPen>

namespace Isis {

  /**
   * This constructs a plot tool. The plot tool graphs either DN values across a
   * line, or statistics across a spectrum (bands).
   *
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


  AbstractPlotTool::~AbstractPlotTool() {
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


  /**
   * This method paints the polygons of the copied curves
   * onto the cubeviewport
   *
   * @param vp
   * @param painter
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
   * Creates the widgets for the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
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


  void AbstractPlotTool::updateTool() {
    foreach (PlotWindow *window, plotWindows()) {
      window->update(cubeViewport());
    }
  }


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
   * This method updates the window where new curves are placed.
   */
  void AbstractPlotTool::selectedWindowChanged() {
    int currentIndex = m_selectWindowCombo->currentIndex();
    if (currentIndex != -1) {
      // Selected an item in the list that isn't a window
      if (m_selectWindowCombo->itemData(currentIndex).isNull()) {
        addWindow();
        ASSERT(m_selectWindowCombo->itemData(
            m_selectWindowCombo->currentIndex()).canConvert<PlotWindow *>());
      }
      else if (selectedWindow(false)) {
        selectedWindow(false)->showWindow();
      }
    }

    detachCurves();
  }


  /**
   * When a user closes a window, we want to remove that window from our
   * QComboBox.
   * 
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


  CubePlotCurve * AbstractPlotTool::createCurve(QString name, QPen pen,
      PlotCurve::Units xUnits, PlotCurve::Units yUnits) {
    CubePlotCurve * newCurve = new CubePlotCurve(xUnits, yUnits);

    newCurve->setTitle(name);
    newCurve->setPen(pen);
    newCurve->setColor(pen.color());

    return newCurve;
  }


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
   * interest associated with the CubePlotCurve's plotwindow.
   * 
   */
  void AbstractPlotTool::repaintViewports(CubePlotCurve *pc) {
    QVector<MdiCubeViewport *> allViewports = *cubeViewportList();

    foreach (MdiCubeViewport * viewport, allViewports) {
      if (viewport->cube()->getFilename() == (iString)pc->sourceCube())
        viewport->repaint();
    }
  }


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
   */
  void AbstractPlotTool::repaintViewports() {
    QVector<MdiCubeViewport *> allViewports = *cubeViewportList();

    foreach (MdiCubeViewport * viewport, allViewports) {
      viewport->viewport()->repaint();
    }
  }
  
}

