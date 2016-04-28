#ifndef PlotWindowBestFitDialog_h
#define PlotWindowBestFitDialog_h

#include <QDialog>

#include <QPointer>

#include "MultivariateStatistics.h"
#include "PlotWindow.h"

class QwtPlotSpectrogram;

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;

namespace Isis {
  /**
   * @author 2012-01-18 Steven Lambright
   *
   * @internal
   *   @history 2014-07-24 Ian Humphrey - modified readCurvesFromWindow() to add curves/spectrograms
   *                           to the dialog's combobox in sequential order (from first to last) as
   *                           opposed to reverse order. References #2089.
   *                           
   */
  class PlotWindowBestFitDialog : public QDialog {
      Q_OBJECT

    public:
      PlotWindowBestFitDialog(PlotWindow *windowWithCurves, QWidget *parent);
      virtual ~PlotWindowBestFitDialog();

    public slots:
      void createBestFitLine();
      void readCurvesFromWindow();

    private slots:
      void refreshWidgetStates();

    private:
      CubePlotCurve *selectedCurve();
      QwtPlotSpectrogram *selectedSpectrogram();

      /**
       * Copy constructing this class is disabled.
       *
       * @param other Nothing.
       */
      PlotWindowBestFitDialog(const PlotWindowBestFitDialog &other);

      /**
       * Assignments with this class are not allowed.
       *
       * @param other Nothing.
       * @return Nothing.
       */
      PlotWindowBestFitDialog &operator=(
          const PlotWindowBestFitDialog &other);

    private:
      //! A combo box for the user to select a curve/spectrogram to best fit
      QPointer<QComboBox> m_curvesCombo;
      //! A label populated with the resulting equation from a best fit
      QPointer<QLabel> m_equationLabel;
      //! A label populated with the resulting correlation from a best fit
      QPointer<QLabel> m_correlationLabel;
      //! A label populated with the resulting determination from a best fit
      QPointer<QLabel> m_determinationLabel;
      /**
       * The ok button which the user clicks to create the best fit curve. This
       *   gets enabled/disabled based on whether or not a best fit was
       *   successful.
       */
      QPointer<QPushButton> m_okayButton;

      //! The plot window we're creating a best for line for
      QPointer<PlotWindow> m_plotWindowWithCurves;

      //! The MV stats which is doing our regression calculations
      QScopedPointer<MultivariateStatistics> m_curveMultivariateStats;
  };
}

#endif
