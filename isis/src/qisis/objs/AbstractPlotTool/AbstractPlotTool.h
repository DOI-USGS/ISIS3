#ifndef AbstractPlotTool_h
#define AbstractPlotTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// this should be the only include in this file!
#include "Tool.h"

// We need PlotCurve::Units
#include "PlotCurve.h"

#include <QPointer>

class QComboBox;
class QPen;

namespace Isis {
  class CubePlotCurve;
  class PlotWindow;
  class RubberBandComboBox;

  /**
   * @brief Parent class for plotting tools which provides common functionality
   *
   * This qview tool is designed to be inherited from by tools which create
   * plots.  This class provides common functionality such as opening new
   * plot windows.
   *
   * @author 2012-01-18 Steven Lambright and Tracie Sucharski
   *
   * @internal
   *   @history 2012-01-20 Steven Lambright - Documentation improved.
   *   @history 2012-03-14 Tracie Sucharski - Update for change to the
   *                          CubePlotCurve::sourceCube(), which now
   *                          returns a QStringList instead of QString.
   */
  class AbstractPlotTool : public Tool {
      Q_OBJECT

    public:
      AbstractPlotTool(QWidget *parent);
      virtual ~AbstractPlotTool();

      virtual void paintViewport(MdiCubeViewport *vp, QPainter *painter);

    public slots:
      void removeWindow(QObject *);
      void repaintViewports(CubePlotCurve *);
      void showPlotWindow();

    protected slots:
      void repaintViewports();

    protected:
      PlotWindow *addWindow();
      static CubePlotCurve *createCurve(QString name, QPen pen,
          PlotCurve::Units xUnits, PlotCurve::Units yUnits);
      QWidget *createToolBarWidget(QStackedWidget *parent);

      /**
       * This needs to be implemented by children to instantiate a plot window
       *   of the appropriate child class of PlotWindow. You should set the
       *   window title, but the rest of the initialization will be handled by
       *   addWindow().
       *
       * @return A newly instantiated, but not fully initialized, plot window.
       */
      virtual PlotWindow *createWindow() = 0;
      virtual void updateTool();
      QList<MdiCubeViewport *> viewportsToPlot();
      QList<PlotWindow *> plotWindows();

      /**
       * This will be called when the selected plot window changes.  The
       * existing curves need to be detached (forgotten, but not deleted).
       * The curves are being detached between the previously selected window
       * and the tool.
       */
      virtual void detachCurves() = 0;
      PlotWindow *selectedWindow(bool createIfNeeded = true);

    private slots:
      void selectedWindowChanged();

    private:
      /**
       * This allows the user to select the active plot window.  New curves
       * will be drawn into this window. The items in the combo box store
       * pointers to the windows themselves so we do not need an explicit list
       * of plot windows.
       */
      QPointer<QComboBox> m_selectWindowCombo;
  };
}

#endif
