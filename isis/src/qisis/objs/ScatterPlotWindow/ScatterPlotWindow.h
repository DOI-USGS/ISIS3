#ifndef ScatterPlotWindow_h
#define ScatterPlotWindow_h

// Only Parents of classes defined in this file should be included in this file!
#include "MainWindow.h"
#include <qwt_plot_zoomer.h>


// FIXME: remove these includes
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>


namespace Isis {
  class ScatterPlotTool;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class MyZoomer: public QwtPlotZoomer {
    public:
      MyZoomer(QwtPlotCanvas *canvas): QwtPlotZoomer(canvas) {
        setRubberBandPen(QPen(Qt::red));
      }

      virtual QwtText trackerText(const QwtDoublePoint &pos) const {
        QColor bg(Qt::white);
        bg.setAlpha(160);

        QwtText text = QwtPlotZoomer::trackerText(pos);
        text.setBackgroundBrush(QBrush(bg));
        return text;
      }
  };


  /**
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   */
  class ScatterPlotWindow : public MainWindow {
      Q_OBJECT

    public:
      ScatterPlotWindow(QString title, ScatterPlotTool *tool, QWidget *parent);

    protected slots:
      void cancel();
      void colorPlot();
      void fillBands();
      void printPlot();
      void reLabel();
      void resetScale();
      void savePlot();
      void setDisplayRange();
      void setLabels();
      void setUserValues();
      void showConfig();
      void showScatterPlot();
      void trackerEnabled();
      void showContour();

    private:
      void createDialogs();
      void setupMenus();
      void setScale(int axisId, double minimum, double maximum, double stepSize = 0);

      ScatterPlotTool *p_tool; //!< Pointer to the scatter plot tool
      MainWindow *p_scatterPlotWindow;//!< Plot Tool Window Widget

      QwtPlotSpectrogram *p_spectrogram;//!< Our spectrogram
      QwtPlot *p_plot; //!< Our plot
      QwtScaleWidget *p_rightAxis; //!< The color code axis

      QAction *p_colorize;//!< The action for switching the scatter plot from B/W to color.
      QWidget *p_parent; //!< parent widget

      QDialog *p_configDialog; //!<The dialog box for setting up the scatter plot.
      QDialog *p_selectCube1Dialog; //!< The dialog box for selecting the Cube1 cube.
      QDialog *p_selectCube2Dialog; //!< The dialog box for selecting the Cube2 cube.
      QDialog *p_minMaxDialog; //!< The dialog box for changing the x/y axis range.

      QLabel *p_cube1Label;//!< A label on p_selectCube1Dialog
      QLabel *p_cube2Label;//!< A label on p_selectCube2Dialog

      //QPushButton *p_clearButton;//!< A button on p_configDialog

      QComboBox *p_cube1BandComboBox;//!< A combo box on p_configDialog
      QComboBox *p_cube2BandComboBox;//!< A combo box on p_configDialog
      //QComboBox *p_cube1SelectionComboBox; //!< A combo box on p_selectCube1Dialog
      //QComboBox *p_cube2SelectionComboBox; //!< A combo box on p_selectCube2Dialog

      QComboBox *p_cube1ComboBox;//!< Cube1 combo box in p_configDialog
      QComboBox *p_cube2ComboBox;//!< Cube2 combo box in p_configDialog
      //
      QLineEdit *p_plotTitleText;//!< Plot title line edit
      QLineEdit *p_xAxisText;//!< X-axis line edit
      QLineEdit *p_yAxisText;//!< Y-axis line edit
      QLineEdit *p_xMinEdit;//!< X min. line edit
      QLineEdit *p_xMaxEdit;//!< X max. line edit
      QLineEdit *p_yMinEdit;//!< Y min. line edit
      QLineEdit *p_yMaxEdit;//!< Y max. line edit
      QLineEdit *p_numBinsOne;
      QLineEdit *p_numBinsTwo;

      QMenuBar  *p_menubar; //!< The scatterplotwindow menu bar.
      QToolBar *p_toolBar; //!< The scatterplotwindow tool bar.

      double p_MinOne;//!< X minimum
      double p_MaxOne;//!< X maximum
      double p_MinTwo;//!< Y minimum
      double p_MaxTwo;//!< Y maximum

      int p_band1;
      int p_band2;

      bool p_scaled;//!< Has this window been rescaled?

      MyZoomer *p_zoomer;  //!< The QPlot zoomer.
      //QwtPlotZoomer *p_zoomer;

  };
};


#endif

