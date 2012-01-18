#ifndef PlotWindowBestFitDialog_h
#define PlotWindowBestFitDialog_h

#include <QDialog>

#include <QPointer>

#include "MultivariateStatistics.h"
#include "PlotWindow.h"

class QwtPlotSpectrogram;

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QWidget;

namespace Isis {
  /**
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

      PlotWindowBestFitDialog(const PlotWindowBestFitDialog &other);
      PlotWindowBestFitDialog &operator=(
          const PlotWindowBestFitDialog &other);

    private:
      QPointer<QComboBox> m_curvesCombo;
      QPointer<QLabel> m_equationLabel;
      QPointer<QLabel> m_correlationLabel;
      QPointer<QLabel> m_determinationLabel;
      QPointer<QPushButton> m_okayButton;

      QPointer<PlotWindow> m_plotWindowWithCurves;

      QScopedPointer<MultivariateStatistics> m_curveMultivariateStats;
  };
}

#endif
