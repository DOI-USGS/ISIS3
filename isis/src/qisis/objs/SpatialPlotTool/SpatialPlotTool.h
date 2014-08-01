#ifndef SpatialPlotTool_h
#define SpatialPlotTool_h

#include "AbstractPlotTool.h"

#include <QMap>

class QAction;
class QComboBox;
class QMainWindow;

template <typename T> class QVector;

namespace Isis {
  class CubePlotCurve;
  class PlotWindow;
  class RubberBandComboBox;
  class SurfacePoint;
  class UniversalGroundMap;

  /**
   * @brief Spatial Plots
   *
   * This tool provides spatial plots for the user.
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
   *   @history 2011-09-20 Steven Lambright - Now handles NULL statistical
   *                           values when graphing by not displaying them.
   *                           Fixes #234.
   *   @history 2011-07-03 Steven Lambright - Added options for plotting meters/kilometers
   *                           on the x-axis instead of just pixel number. Fixes #853.
   *   @history 2012-11-30 Debbie A. Cook - Changed to use TProjection and RingPlaneProjection 
   *                           instead of Projection.  References #775.
   *   @history 2013-01-24 Steven Lambright - Fixed positioning of portal/interpolator used
   *                           when reading DN data to create a plot. Fixes #997.
   *   @history 2014-04-15 Tracie Sucharski - Reset defaults for plots to the following:
   *                         SolidLine, Width=1, NoSymbols.  This is a temporary fix until
   *                         the defaults can be saved on a user basis.  Fixes #2062.
   *   @history 2014-06-20 Janet Barrett - Fixed SpatialPlotTool::getSpatialStatistics to
   *                         check for length of line used to draw profile to make sure it
   *                         is greater than zero to avoid divide by zero error. Fixes #1921
   *                         and #1950.
   *   @history 2014-07-31 Ian Humphrey - Added What's This help for SpatialPlotTool.
   *                           References #2089.
   */
  class SpatialPlotTool : public AbstractPlotTool {
      Q_OBJECT

    public:
      SpatialPlotTool(QWidget *parent);

    public slots:
      void refreshPlot();

    protected:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      virtual PlotWindow *createWindow();
      virtual void detachCurves();
      void enableRubberBandTool();
      QAction *toolPadAction(ToolPad *pad);
      void updateTool();

    protected slots:
      virtual void rubberBandComplete();
      void viewportSelected();

    private:
      QVector<QPointF> getSpatialStatistics(MdiCubeViewport *);
      static SurfacePoint resultToSurfacePoint(UniversalGroundMap *);
      void validatePlotCurves();

    private:
      QPointer<QComboBox> m_xUnitsCombo;

      //! Plot tool's action
      QPointer<QAction> m_toolPadAction;

      //! Allows the user to choose the interpolation type
      QPointer<QComboBox> m_interpolationCombo;

      //! Spatial curve
      QScopedPointer<
        QMap< MdiCubeViewport *, QPointer<CubePlotCurve> > > m_spatialCurves;

      //! Spatial plot rubber band combo box
      QPointer<RubberBandComboBox> m_rubberBandCombo;
  };
};

#endif
