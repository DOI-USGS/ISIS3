#ifndef PlotTool_h
#define PlotTool_h

// this should be the only include in this file!
#include "Tool.h"


// FIXME: remove these includes
#include <vector>
#include <QMap>
#include "geos/geom/Polygon.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"
#include <QColor>
#include "PlotCurve.h"


class QComboBox;
class QMainWindow;

// FIXME: uncomment this
//template< class A, class B > class QMap;


namespace Isis {
  class Statistics;
}

namespace Qisis {
  class PlotToolCurve;
  class PlotToolWindow;
  class PlotWindow;
  class RubberBandComboBox;

  /**
   * @brief plot tool
   *
   * This class is a plot tool.
   *
   * @author Stacy Alley
   *
   * @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
   * @history 2008-09-05 Stacy Alley allowed spectral plotting of
   *          a single point.
   * @history 2009-01-29 Steven Lambright - Added RotatedRectangle to the spatial
   *          plot
   * @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *          CubeViewport.  Fixed some include issues (many still remain!).
   */
  class PlotTool : public Tool {
      Q_OBJECT

    public:
      PlotTool(QWidget *parent);
      virtual void paintViewport(MdiCubeViewport *vp, QPainter *painter);

    protected:
      //!< Returns the menu name.
      QString menuName() const {
        return "&Options";
      };
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);
      QAction *p_showHideLines;//!< Hide/show lines action
      QAction *p_autoScale;//!< Auto scale the plot
      QComboBox *plotType;//!< Plot type combobox
      void addTo(QMenu *menu);
      void enableRubberBandTool();
      void updateTool();

    protected slots:
      void createWindow();
      virtual void rubberBandComplete();
      void updateViewPort();
      void viewportSelected();

    public slots:
      void changePlot();
      void showPlotWindow();
      void updateViewPort(Qisis::PlotToolCurve *);
      void copyCurve(Qisis::PlotCurve *);
      void pasteCurve(Qisis::PlotWindow *);
      void pasteCurveSpecial(Qisis::PlotWindow *);
      void removeWindow(QObject *);

    private slots:
      void bilinearInterpolationChanged();
      void changePlotType(int newType);
      void cubicInterpolationChanged();
      void nearestInterpolationChanged();
      void newPlotWindow();
      void showHideLines();
      void setPlotType();

    private:
      void getSpectralStatistics(std::vector< double > & labels,
                                 std::vector< Isis::Statistics > & data);
      void getSpatialStatistics(std::vector< double >  & labels,
                                std::vector< double > & data, double &xmax);
      void setupPlotCurves();

      /**
       * Enum for the different plot types
       */
      enum PlotType {
        SpectralPlot, //!< Spectral plot
        SpatialPlot //!< Spatial plot
      };

      PlotType p_currentPlotType; //!< current plot type
      QWidget *p_parent; //!< parent widget
      PlotToolWindow *p_plotToolWindow;//!< Plot Tool Window Widget
      QMainWindow *p_tableWin; //!< Window for the table
      QComboBox *p_plotTypeCombo;//!< Plot type combobox

      QMap<QString, QString> p_headerToMenu;//!< Head to menu map

      QAction *p_cubicInterp; //!< Cubic interpolation action
      QAction *p_bilinearInterp;//!< Bilinear interpolation action
      QAction *p_nearestNeighborInterp;//!< Nearest neighbor interpolation action
      QAction *p_action;//!< Plot tool's action

      bool p_changingInterp;//!< Do user change interpolation type?
      bool p_scaled;//!< Has the plot been scaled?

      PlotToolCurve *p_maxCurve;//!< Plot curve for max values
      PlotToolCurve *p_minCurve;//!< Plot curve for min values
      PlotToolCurve *p_avgCurve;//!< Plot curve for average values
      PlotToolCurve *p_stdDev1Curve;//!< Plot curve for avg. + std. dev
      PlotToolCurve *p_stdDev2Curve;//!< Plot curve for avg. - std. dev
      PlotToolCurve *p_copyCurve;//!< Plot curve for copying curves
      PlotToolCurve *p_paintCurve;//!< Plot curve for painting
      PlotToolCurve *p_dnCurve;//!< Plot curve for DN values

      QList <QColor> p_colors;//!< List of colors
      QList<PlotWindow *> p_plotWindows;//!< List of all plot windows
      RubberBandComboBox *p_spectralRubberBand;//!< Spectral plot rubber band combo box
      RubberBandComboBox *p_spacialRubberBand;//!< Spacial plot rubber band combo box

      geos::geom::Polygon *p_poly;//!< For plotting/drawing polygons
      const geos::geom::Envelope *p_envelope;//!< Bounding box of polygon
      int p_color;//!< Keeps track of which color we are at
  };
};

#endif

