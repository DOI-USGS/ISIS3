#ifndef SpectralPlotTool_h
#define SpectralPlotTool_h

#include "AbstractPlotTool.h"

#include <vector>

#include <QMap>

class QMainWindow;

namespace geos {
  namespace geom {
    class Envelope;
    class Polygon;
  }
}

namespace Isis {
  class CubePlotCurve;
  class PlotWindow;
  class RubberBandComboBox;
  class Statistics;

  /**
   * @brief Plot cube DN statistics against the cube band numbers
   *
   * This will plot DN statistics against the cube band numbers. 
   * The statistical values plotted are the minimum, maximum, mean, 
   * mean + standard deviation and mean - standard deviation.
   *
   * @author ????-??-?? Stacy Alley
   *  
   * @internal 
   *   @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
   *   @history 2008-09-05 Stacy Alley allowed spectral plotting of a single
   *                           point.
   *   @history 2009-01-29 Steven Lambright - Added RotatedRectangle to the
   *                           spatial plot
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *                           CubeViewport.  Fixed some include issues (many
   *                           still remain!).
   *   @history 2010-11-08 Eric Hyer - Spacial plot now handles linked images. 
   *   @history 2011-03-18 Sharmila Prasad - Connect the viewport's close signal  
   *   @history 2011-09-20 Steven Lambright - Now handles NULL statistical values
   *                         when graphing by not displaying them. Fixes #234.
   *   @history 2012-02-16 Steven Lambright and Jai Rideout - Added standard
   *                         error of the mean curves. Fixes #713.
   */
  class SpectralPlotTool : public AbstractPlotTool {
      Q_OBJECT

    public:
      SpectralPlotTool(QWidget *parent);

    protected:
      void addTo(QMenu *menu);
      QWidget *createToolBarWidget(QStackedWidget *parent);
      virtual PlotWindow *createWindow();
      virtual void detachCurves();
      void enableRubberBandTool();
      QComboBox *spectralDisplayCombo() const;
      QAction *toolPadAction(ToolPad *pad);
      void updateTool();

    protected slots:
      virtual void rubberBandComplete();
      void viewportSelected();

    public slots:
      void refreshPlot();

    private slots:
      void selectCurvesToPlot();

    private:
      void getSpectralStatistics(QVector<double> &labels,
                                 QVector<Statistics> &data,
                                 MdiCubeViewport *viewport);
      void validatePlotCurves();

      //! wavelength vs band #
      QPointer<QComboBox> m_displayCombo;

      //! Combo box with all rubber banding types
      QPointer<RubberBandComboBox> m_rubberBandCombo;

      //! Plot tool's action
      QPointer<QAction> m_toolPadAction;

      //! This QAction actives/deactivates plotting the average values
      QPointer<QAction> m_plotAvgAction;
      //! This QAction actives/deactivates plotting the min values
      QPointer<QAction> m_plotMinAction;
      //! This QAction actives/deactivates plotting the max values
      QPointer<QAction> m_plotMaxAction;
      //! This QAction actives/deactivates plotting the avg+std dev values
      QPointer<QAction> m_plotStdDev1Action;
      //! This QAction actives/deactivates plotting the avg-std dev values
      QPointer<QAction> m_plotStdDev2Action;
      //! This QAction actives/deactivates plotting the avg+std dev values
      QPointer<QAction> m_plotStdErr1Action;
      //! This QAction actives/deactivates plotting the avg-std dev values
      QPointer<QAction> m_plotStdErr2Action;

      //! Plot curves for max values
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_maxCurves;

      //! Plot curves for min values
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_minCurves;

      //! Plot curves for average values
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_avgCurves;

      //! Plot curves for avg. + std. dev
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_stdDev1Curves;

      //! Plot curves for avg. - std. dev
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_stdDev2Curves;

      //! Plot curves for avg. + std. err
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_stdErr1Curves;

      //! Plot curves for avg. - std. err
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_stdErr2Curves;


      //! Hide/show lines action
      QPointer<QAction> m_showHideBandMarkers;
  };
};

#endif
