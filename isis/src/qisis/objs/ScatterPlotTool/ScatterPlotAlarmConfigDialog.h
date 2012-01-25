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
      /**
       * Copying this class is disallowed.
       * @param other Nothing.
       */
      ScatterPlotAlarmConfigDialog(const ScatterPlotAlarmConfigDialog &other);
      /**
       * Assignment of this class is disallowed.
       * @param other Nothing.
       * @return Nothing.
       */
      ScatterPlotAlarmConfigDialog &operator=(
          const ScatterPlotAlarmConfigDialog &other);

    private slots:
      void refreshWidgetStates();

    private:
      //! This is the user option for enabling alarming viewport->plot
      QPointer<QCheckBox> m_alarmOntoPlot;
      //! This is the sample box size for alarming viewport->plot
      QPointer<QSpinBox> m_alarmOntoPlotSamples;
      //! This is the line box size for alarming viewport->plot
      QPointer<QSpinBox> m_alarmOntoPlotLines;

      //! This is the user option for enabling alarming plot->viewport
      QPointer<QCheckBox> m_alarmOntoViewport;
      /**
       * This is whether alarming plot->viewport should be screen pixels
       *   or a set box size regardless of zoom level/axis units.
       */
      QPointer<QComboBox> m_alarmOntoViewportUnits;
      //! The X-Pixel Cube DN Box Size for alarming plot->viewport
      QPointer<QLineEdit> m_alarmOntoViewportXDnSize;
      //! The Y-Pixel Cube DN Box Size for alarming plot->viewport
      QPointer<QLineEdit> m_alarmOntoViewportYDnSize;
      //! The X-Pixel Screen Box Size for alarming plot->viewport
      QPointer<QSpinBox> m_alarmOntoViewportWidth;
      //! The Y-Pixel Screen Box Size for alarming plot->viewport
      QPointer<QSpinBox> m_alarmOntoViewportHeight;

      /**
       * Button for accepting the current settings and closing the window. This
       *   may be disabled if the user inputs don't make sense.
       */
      QPointer<QPushButton> m_okayButton;

      /**
       * Button for accepting the current settings. This may be disabled if the
       *   user inputs don't make sense.
       */
      QPointer<QPushButton> m_applyButton;

      //! The scatter plot window that we're configuring alarming on
      QPointer<ScatterPlotWindow> m_window;
  };
}

#endif
