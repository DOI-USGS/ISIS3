#ifndef ScatterPlotConfigDialog_h
#define ScatterPlotConfigDialog_h

#include <QDialog>
#include <QPointer>

class QwtInterval;

template <typename A, typename B> class QPair;
class QCheckBox;
class QComboBox;
class QSpinBox;

namespace Isis {
  class Cube;
  class MdiCubeViewport;
  class ScatterPlotTool;
  class Workspace;

  /**
   * This configuration dialog is for users to determine the scatter plot
   *   parameters required to create a scatter plot.
   *
   * @author 2012-01-18 Steven Lambright
   *
   * @internal
   */
  class ScatterPlotConfigDialog : public QDialog {
      Q_OBJECT

    public:
      ScatterPlotConfigDialog(MdiCubeViewport *activeViewport,
                              Workspace *workspace, QWidget *parent = NULL);

      virtual QSize sizeHint() const;

      Cube *xAxisCube() const;
      Cube *yAxisCube() const;
      int xAxisCubeBand() const;
      int yAxisCubeBand() const;
      int xAxisBinCount() const;
      int yAxisBinCount() const;
      QwtInterval sampleRange() const;
      QwtInterval lineRange() const;

      MdiCubeViewport *xAxisCubeViewport() const;
      MdiCubeViewport *yAxisCubeViewport() const;

    private slots:
      void refreshWidgetStates();

    private:
      //! This is used internally to differentiate range accessors
      enum RangeType {
        //! This is used to get the sample range in a generic way
        SampleRange,
        //! This is used to get the line range in a generic way
        LineRange
      };

      QList<Cube *> removeFromList(QList<Cube *> list,
                                   QList<Cube *> itemsToRemove);
      QwtInterval range(RangeType) const;

      //! This is the workspace containing all of the viewports.
      Workspace *m_workspace;

      //! A user-selection for the x axis cube
      QPointer<QComboBox> m_xAxisCubeCombo;
      //! A user-selection for the x axis cube's band to plot
      QPointer<QSpinBox> m_xAxisCubeBandSpinBox;
      //! A user-selection for the x axis cube's resolution
      QPointer<QSpinBox> m_xAxisBinCountSpinBox;
      //! A user-selection for using only the visible range of the viewport
      QPointer<QCheckBox> m_useViewportRangesCheckBox;

      //! A user-selection for the y axis cube
      QPointer<QComboBox> m_yAxisCubeCombo;
      //! A user-selection for the y axis cube's band to plot
      QPointer<QSpinBox> m_yAxisCubeBandSpinBox;
      //! A user-selection for the y axis cube's resolution
      QPointer<QSpinBox> m_yAxisBinCountSpinBox;

      /**
       * This button triggers an accepted() signal. This is only enabled when
       *   the user inputs make sense.
       */
      QPointer<QPushButton> m_createButton;

      //! This is used to detect when a user changes the x-axis cube.
      Cube *m_oldXAxisCube;
  };
}


#endif
