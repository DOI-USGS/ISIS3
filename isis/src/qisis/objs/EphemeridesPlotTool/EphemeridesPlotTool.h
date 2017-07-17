#ifndef EphemeridesPlotTool_h
#define EphemeridesPlotTool_h

#include "AbstractPlotTool.h"

class QAction;
class QWidget;


namespace Isis {
  class Brick;
  class Cube;
  class CubePlotCurve;
  class EphemeridesItem;
  class EphemeridesPlotWindow;
  class MdiCubeViewport;

  /**
  * @brief Tool for histograms
  *
  * @ingroup Visualization Tools
  *
  * @author ????-??-?? Noah Hilt
  *
  * @internal
  *  @history 2008-08-18 Christopher Austin - Upgraded to geos3.0.0
  *  @history 2012-01-18 Steven Lambright and Jai Rideout - Fixed issue where
  *                          histograms were not created correctly for any bands
  *                          but band 1. Added check for RGB mode. Fixes #668.
  *  @history 2012-01-20 Steven Lambright - Completed documentation.
  *  @history 2013-12-11 Janet Barrett - Fixed refreshPlot method so that it 
  *                          checks the start sample and end sample for the
  *                          plot and starts the plot at the minimum of the 
  *                          2 samples. Fixes #1760.
  *  @history 2016-04-28 Tracie Sucharski - Removed qwt refernces to merge with
  *                          Qt5 library changes.
  *  @history 2016-07-06 Adam Paquette - Fixed the histogram tool to analyze the
  *                          appropriate pixels selected when using box banding
  *                          
  */
  class EphemeridesPlotTool : public AbstractPlotTool {
      Q_OBJECT

    public:
      EphemeridesPlotTool(QWidget *parent);

    protected:
      void detachCurves();
      PlotWindow *createWindow();
      void enableRubberBandTool();
      QAction *toolPadAction(ToolPad *pad);

    public slots:
      void refreshPlot();

    protected slots:
      void rubberBandComplete();

    private:
      void validatePlotCurves();
      void collectEphemerides(Cube *cube,
                              std::vector<double> &coordinateTimes,
                              std::vector<double> &xCoordinates,
                              std::vector<double> &yCoordinates,
                              std::vector<double> &zCoordinates,
                              std::vector<double> &angleTimes,
                              std::vector<double> &raAngles,
                              std::vector<double> &decAngles,
                              std::vector<double> &twiAngles);

      QPointer<CubePlotCurve> m_xCurve; /**! Plot curve for x data. */
      QPointer<CubePlotCurve> m_yCurve; /**! Plot curve for y data. */
      QPointer<CubePlotCurve> m_zCurve; /**! Plot curve for z data. */
      QPointer<CubePlotCurve> m_raCurve; /**! Plot curve for right ascension data. */
      QPointer<CubePlotCurve> m_decCurve; /**! Plot curve for declination data. */
      QPointer<CubePlotCurve> m_twiCurve; /**! Plot curve for twist data. */
      //! This is the action that activates this tool
      QAction *m_action;
  };
};

#endif
