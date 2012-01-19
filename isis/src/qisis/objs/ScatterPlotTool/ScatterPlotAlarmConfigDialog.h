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
   * This is the configuration dialog for alarming scatter plots between the
   *   plot window and cube viewports.
   *
   *
   * @author 2012-01-18 Steven Lambright and Jai Rideout
   *
   * @internal
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
