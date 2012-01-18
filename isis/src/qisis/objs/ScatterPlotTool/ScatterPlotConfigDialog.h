#ifndef ScatterPlotConfigDialog_h
#define ScatterPlotConfigDialog_h

#include <QDialog>
#include <QPointer>

class QwtDoubleRange;

template <typename A, typename B> class QPair;
class QCheckBox;
class QComboBox;
class QSpinBox;

namespace Isis {
  class Cube;
  class MdiCubeViewport;
  class ScatterPlotTool;
  class Workspace;

  class ScatterPlotConfigDialog : public QDialog {
      Q_OBJECT

    public:
      ScatterPlotConfigDialog(MdiCubeViewport *activeViewport,
                              Workspace *ws, ScatterPlotTool *tool,
                              QWidget *parent = NULL);

      virtual QSize sizeHint() const;

      Cube *xAxisCube() const;
      Cube *yAxisCube() const;
      int xAxisCubeBand() const;
      int yAxisCubeBand() const;
      int xAxisBinCount() const;
      int yAxisBinCount() const;
      QwtDoubleRange sampleRange() const;
      QwtDoubleRange lineRange() const;

      MdiCubeViewport *xAxisCubeViewport() const;
      MdiCubeViewport *yAxisCubeViewport() const;

    private slots:
      void refreshWidgetStates();

    private:
      QList<Cube *> removeFromList(QList<Cube *> list,
                                   QList<Cube *> itemsToRemove);
      QPair<QwtDoubleRange, QwtDoubleRange> sampleLineRanges() const;

      ScatterPlotTool *m_tool;
      Workspace *m_workspace;

      QPointer<QComboBox> m_xAxisCubeCombo;
      QPointer<QSpinBox> m_xAxisCubeBandSpinBox;
      QPointer<QSpinBox> m_xAxisBinCountSpinBox;
      QPointer<QCheckBox> m_useViewportRangesCheckBox;

      QPointer<QComboBox> m_yAxisCubeCombo;
      QPointer<QSpinBox> m_yAxisCubeBandSpinBox;
      QPointer<QSpinBox> m_yAxisBinCountSpinBox;

      QPointer<QPushButton> m_createButton;

      Cube *m_oldXAxisCube;
  };
}


#endif

