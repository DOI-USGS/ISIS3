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
  * Tool for visualizing cube ephemerides, instrument position and orientation.
  * This tool plots the ephemeris data associated with the cube. The
  * ephemerides may come from SPICE, cached values, bundle adjusted values, or
  * other sources.
  *
  * @ingroup Visualization Tools
  *
  * @author 2017-07-17 Jesse Mapel
  *
  * @internal
  *  @history 2017-07-17 Jesse Mapel - Original Version
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
      QAction *m_action; /**! The action that activates this tool. */
  };
};

#endif
