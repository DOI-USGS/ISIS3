#ifndef ScatterPlotAlarmConfigDialog_h
#define ScatterPlotAlarmConfigDialog_h

#include <QDialog>

#include <QPointer>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QWidget;

namespace Isis {
  class ScatterPlotWindow;

  /**
   * This should be an inner class for CubePlotCurve, but Qt doesn't support
   *   having a QObject as an inner class.
   */
  class ScatterPlotAlarmConfigDialog : public QDialog {
      Q_OBJECT

    public:
      ScatterPlotAlarmConfigDialog(ScatterPlotWindow *window,
                                   QWidget *parent = NULL);
      virtual ~ScatterPlotAlarmConfigDialog();

    public slots:
      void applySettingsToScatterPlot();
      void readSettingsFromScatterPlot();

    private:
      ScatterPlotAlarmConfigDialog(const ScatterPlotAlarmConfigDialog &other);
      ScatterPlotAlarmConfigDialog &operator=(
          const ScatterPlotAlarmConfigDialog &other);

    private slots:
      void refreshWidgetStates();

    private:
      QPointer<QCheckBox> m_alarmOntoPlot;
      QPointer<QSpinBox> m_alarmOntoPlotSamples;
      QPointer<QSpinBox> m_alarmOntoPlotLines;

      QPointer<QCheckBox> m_alarmOntoViewport;
      QPointer<QComboBox> m_alarmOntoViewportUnits;
      QPointer<QLineEdit> m_alarmOntoViewportXDnSize;
      QPointer<QLineEdit> m_alarmOntoViewportYDnSize;
      QPointer<QSpinBox> m_alarmOntoViewportWidth;
      QPointer<QSpinBox> m_alarmOntoViewportHeight;

      QPointer<QPushButton> m_okayButton;
      QPointer<QPushButton> m_applyButton;

      QPointer<ScatterPlotWindow> m_window;
  };
}

#endif
