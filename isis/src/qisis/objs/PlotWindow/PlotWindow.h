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
      /**
       * There is a menu option for everything in the plot window's menu. This
       *   is provided so that you can enable/disable any given option.
       */
      enum MenuOptions {
        //! This option is titled 'Hide All Symbols' which hides all markers
        ShowHideMarkersMenuOption  = 1,
        /**
         * This option is titled 'Hide All Curves' which makes all curves
         *   invisible
         */
        ShowHideCurvesMenuOption   = 2,
        /**
         * This option brings up the table. No table will be available
         *   without this option.
         */
        ShowTableMenuOption        = 4,
        /**
         * This option exports the plot into a standard image format.
         *   No conversion to a standard image format file will be available
         *   without this option, but printing will still be available.
         */
        SaveMenuOption             = 8,
        /**
         * This option sends the plot to a printer.
         */
        PrintMenuOption            = 16,
        /**
         * This option enables mouse tracking on the plot area (displays next
         *   to the mouse which x/y point you are hovering over). This option
         *   is by default disabled.
         */
        TrackMenuOption            = 32,
        /**
         * This option enables the ability for a user to change the background
         *   color of the plot from black to white.
         */
        BackgroundSwitchMenuOption = 64,
        /**
         * This option enables the ability for a user to enable a grid over the
         *   plot area.
         */
        ShowHideGridMenuOption     = 128,
        /**
         * This option enables the ability for a user to change the x/y axis
         *   labels and plot title.
         */
        RenameLabelsMenuOption     = 256,
        /**
         * This option allows the user to set the x/y axis display value ranges.
         */
        SetDisplayRangeMenuOption  = 512,
        /**
         * This option provides the user with an alternative zoom out button.
         */
        ResetScaleMenuOption       = 1024,
        /**
         * This option allows the user to delete all of the data inside the
         *   plot. If you don't want the user deleting a custom plot item, you
         *   should disable this option.
         */
        ClearPlotMenuOption        = 2048,
        /**
         * Not implemented.
         */
        DefaultHelpMenuOption      = 4096,
        /**
         * This option allows the user to create a best fit line for any of the
         *   scatter plot data or cube plot curve data in the window. This
         *   allows the user to create new plot curves.
         */
        LineFitMenuOption          = 8192,
        /**
         * This is all of the available menu options.
         */
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
      /**
       * Emitted when there is a close event on this window that will be
       *   accepted.
       */
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
      virtual bool eventFilter(QObject *o, QEvent *e);
      void mousePressEvent(QObject *object, QMouseEvent *e);

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
      //! Set Scale Dialog's checkbox for enabling automatic scaling on x & y
      QCheckBox     *m_autoScaleCheckBox;
      //! Set Scale Dialog's checkbox for using logarithmic scale for the x axis
      QCheckBox     *m_xLogCheckBox;
      //! Set Scale Dialog's checkbox for using logarithmic scale for the y axis
      QCheckBox     *m_yLogCheckBox;
      //! Set Scale Dialog's edit for the min X-axis value
      QLineEdit     *m_xMinEdit;
      //! Set Scale Dialog's edit for the max X-axis value
      QLineEdit     *m_xMaxEdit;
      //! Set Scale Dialog's edit for the min Y-axis value
      QLineEdit     *m_yMinEdit;
      //! Set Scale Dialog's edit for the max Y-axis value
      QLineEdit     *m_yMaxEdit;
      //! Set Labels Dialog's edit for the x-axis label
      QLineEdit     *m_xAxisText;
      //! Set Labels Dialog's edit for the y-axis label
      QLineEdit     *m_yAxisText;
      //! Set Labels Dialog's edit for the plot title
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

      //! The units of the data on the x-bottom axis
      PlotCurve::Units m_xAxisUnits;
      //! The units of the data on the y-left axis
      PlotCurve::Units m_yAxisUnits;

      //! Is the window showing the curve markers?
      bool m_allowUserToAddCurves;
      //! True if we are autoscaling the x-bottom and y-left axes
      bool m_autoscaleAxes;

      QwtPlot *m_plot;//!< The plot in this window
      TableMainWindow *m_tableWindow;//!< Table window
      QToolBar *m_toolBar;//!< Tool bar on the plot window
   };
};

//! We have plot windows as QVariant data types, so here it's enabled.
Q_DECLARE_METATYPE(Isis::PlotWindow *);

// There isn't a great place to put this currently since it's not a class we
//   can manage the header for.
//! We have scatter plot types as QVariant data types, so here it's enabled.
Q_DECLARE_METATYPE(QwtPlotSpectrogram *);

#endif

