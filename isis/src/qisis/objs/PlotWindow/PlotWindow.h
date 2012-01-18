#ifndef PlotWindow_h
#define PlotWindow_h

/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2009/04/07 16:19:40 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

// This is needed for the QVariant macro
#include <QMetaType>

// This is the parent
#include "MainWindow.h"

// This is for an enum inside of plot curve
#include "PlotCurve.h"

class QwtPlot;
class QwtPlotGrid;
class QwtPlotSpectrogram;
class QwtPlotZoomer;

class QDockWidget;
class QListWidgetItem;
class QLineEdit;
class QComboBox;

template <typename A, typename B> class QPair;

namespace Isis {
  class MdiCubeViewport;
  class CubePlotCurve;
  class PvlKeyword;
  class TableMainWindow;

  /**
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2010-11-08 Eric Hyer - Added replot method
   *   @history 2011-03-18 Sharmila Prasad - Connect the viewport's close signal
   *   @history 2011-03-23 Sharmila Prasad - PlotWindow to be ontop even when it
   *                           loses focus. This works for both histogram and
   *                           plot tools.
   *   @history 2011-09-21 Steven Lambright - Removed the fixed size from the
   *                          configure curve dialog. Fixes #203.
   */
  class PlotWindow : public MainWindow {
      Q_OBJECT

    public:
      enum MenuOptions {
        ShowHideMarkersMenuOption  = 1,
        ShowHideCurvesMenuOption   = 2,
        ShowTableMenuOption        = 4,
        SaveMenuOption             = 8,
        PrintMenuOption            = 16,
        TrackMenuOption            = 32,
        BackgroundSwitchMenuOption = 64,
        ShowHideGridMenuOption     = 128,
        RenameLabelsMenuOption     = 256,
        SetDisplayRangeMenuOption  = 512,
        ResetScaleMenuOption       = 1024,
        ClearPlotMenuOption        = 2048,
        DefaultHelpMenuOption      = 4096,
        LineFitMenuOption          = 8192,
        AllMenuOptions = ShowHideMarkersMenuOption | ShowHideCurvesMenuOption |
                         ShowTableMenuOption | SaveMenuOption |
                         PrintMenuOption | TrackMenuOption |
                         BackgroundSwitchMenuOption | ShowHideGridMenuOption |
                         RenameLabelsMenuOption | SetDisplayRangeMenuOption |
                         ResetScaleMenuOption | ClearPlotMenuOption |
                         DefaultHelpMenuOption | LineFitMenuOption
      };

      PlotWindow(QString title, PlotCurve::Units xAxisUnits,
                 PlotCurve::Units yAxisUnits, QWidget *parent,
                 MenuOptions optionsToProvide = AllMenuOptions);
      ~PlotWindow();
      virtual void add(CubePlotCurve *pc);
      virtual void clearPlotCurves();

      bool canAdd(CubePlotCurve *curveToTest) const;
      QColor plotBackgroundColor() const;
      QList<CubePlotCurve *> plotCurves();
      QList<const CubePlotCurve *> plotCurves() const;
      QList<QwtPlotSpectrogram *> plotSpectrograms();
      QList<const QwtPlotSpectrogram *> plotSpectrograms() const;
      QString plotTitle() const;
      bool userCanAddCurves() const;
      PlotCurve::Units xAxisUnits() const;
      PlotCurve::Units yAxisUnits() const;

      virtual void paint(MdiCubeViewport *vp, QPainter *painter);
      void replot();
      void setAxisLabel(int axisId, QString title);
      void setPlotBackground(QColor c);
      void setPlotTitle(QString pt);
      void setUserCanAddCurves(bool);
      void showWindow();
      virtual void update(MdiCubeViewport *activeViewport);

      static QString defaultWindowTitle();


    signals:
      void closed();
      //! Emitted every time there is a change to the plot window.
      void plotChanged();

    public slots:
      void clearPlot();
      void createBestFitLine();
      void printPlot();
      void changePlotLabels();
      void savePlot();
      void setDefaultRange();
      void setLabels();
      void setUserValues();
      void showHideAllMarkers();
      void showHideAllCurves();
      void resetScale();
      void showHideGrid();
      void showHelp();
      void showTable();
      void switchBackground();
      void trackerEnabled();

      void fillTable();

    protected slots:
      void writeSettings();
      void readSettings();

    protected:
      void createWidgets(MenuOptions optionsToProvide);
      void disableAxisAutoScale();
      virtual void dragEnterEvent(QDragEnterEvent *event);
      virtual void dropEvent(QDropEvent *event);
      void enableZoomer(bool enable);
      virtual bool eventFilter(QObject *o, QEvent *e);
      void mousePressEvent(QObject *object, QMouseEvent *e);

      //! Returns the name of the menu.
      QString menuName() const {
        return "&Options";
      };
      QwtPlot *plot();
      void setMenus(QList<QMenu *> menus, QList<QAction *> actions);
      QwtPlotZoomer *zoomer();

    private slots:
      void autoScaleCheckboxToggled();
      void onClipboardChanged();
      void pasteCurve();

    private:
      QPair<double, double> findDataRange(int axisId) const;
      bool userCanAddCurve(const QMimeData *curve);
      void updateVisibility(PlotCurve *curve);
      void setupDefaultMenu(MenuOptions optionsToProvide);

      //! Parent widget
      QWidget       *m_parent;
      //! Plot Zoomer
      QwtPlotZoomer *m_zoomer;
      //! Plot grid lines
      QwtPlotGrid   *m_grid;
      QCheckBox     *m_autoScaleCheckBox;
      QCheckBox     *m_xLogCheckBox;
      QCheckBox     *m_yLogCheckBox;
      QLineEdit     *m_xMinEdit;
      QLineEdit     *m_xMaxEdit;
      QLineEdit     *m_yMinEdit;
      QLineEdit     *m_yMaxEdit;
      QLineEdit     *m_xAxisText;
      QLineEdit     *m_yAxisText;
      QLineEdit     *m_plotTitleText;

      //! Plot window's menu bar.
      QMenuBar      *m_menubar;

      /**
       * This is the paste action in the edit menu to paste a curve into the
       * plot window.  This is enabled or disabled based on what is in the
       * clipboard.
       */
      QAction       *m_pasteAct;

      //! Plot window's action.
      QPointer<QAction> m_action;
      //! Hide all curves action.
      QPointer<QAction> m_showHideAllCurves;
      //! Hide all markers action.
      QPointer<QAction> m_showHideAllMarkers;
      //! Show plot grid lines action.
      QPointer<QAction> m_showHideGrid;

      PlotCurve::Units m_xAxisUnits;
      PlotCurve::Units m_yAxisUnits;

      //! Is the window showing the curve markers?
      bool m_allowUserToAddCurves;
      bool m_autoscaleAxes;

      QwtPlot *m_plot;//!< The plot in this window
      TableMainWindow *m_tableWindow;//!< Table window
      QToolBar *m_toolBar;//!< Tool bar on the plot window
   };
};

Q_DECLARE_METATYPE(Isis::PlotWindow *);

// There isn't a great place to put this currently since it's not a class we
//   can manage the header for.
Q_DECLARE_METATYPE(QwtPlotSpectrogram *);

#endif

