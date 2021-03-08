#ifndef StatisticsTool_h
#define StatisticsTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QAction>
#include <QPixmap>

// Only includes for Parents of classes defined in this file are allowed here!
#include "Tool.h"
#include <QWidget>


// FIXME: remove these includes
#include "Stretch.h"
#include <vector>

class QScrollArea;
class QLabel;
class QLineEdit;
class QCheckBox;
class QGroupBox;

namespace Isis {
  /**
   * @brief Tool to visualize statistics in an n * m box.
   *
   * This tool is used to visualize statistics in an n * m box specified
   * by the user. It allows for textual and visual representation of the
   * DN values in the region as well as the standard deviation over the box.
   *
   * @ingroup Visualization Tools
   *
   * @author 2009-10-07 Noah Hilt
   *
   * @internal
   *   @history 2009-10-07 Noah Hilt - Original version.
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport
   *   @history 2012-06-29 Steven Lambright - Improved handling of special pixels so that they
   *                           are reported correctly. Fixes #199.
   */
  class VisualDisplay : public QWidget {
      Q_OBJECT

    public:
      VisualDisplay(QWidget *parent = 0);

      QSize sizeHint() const;

      void setSamples(int samps);
      void setLines(int lines);

      void updateSize();

      void setPixelData(QVector<QVector<double> > data, int samp, int line);

    protected:
      void paintPixmap();
      void paintEvent(QPaintEvent *event);
      void mouseMoveEvent(QMouseEvent *event);
      void leaveEvent(QEvent *event);

    signals:
      /**
       * Signal to set the DN label to st.
       *
       * @param st
       */
      void setDn(const QString st);
      /**
       * Signal to set the Sample label to st.
       *
       * @param st
       */
      void setSample(const QString st);
      /**
       * Signal to set the Line label to st.
       *
       * @param st
       */
      void setLine(const QString st);

    protected slots:
      void setBoxSize(int size);
      void showText(bool b);
      void showPixels(bool b);
      void showDeviation(bool b);

    private:
      int p_boxSamps; //!< Sample size for box
      int p_boxLines; //!< Line size for box
      int p_boxWidth; //!< Box width in pixels
      int p_boxHeight; //!< Box height in pixels
      int p_oldWidth; //!< Previous box width in pixels
      int p_oldHeight; //!< Previous box height in pixels
      int p_ulSamp; //!< Upper left sample of region captured
      int p_ulLine; //!< Upper left line of region captured
      bool p_set; //!< Boolean to see if data is set
      bool p_showText; //!< Display text?
      bool p_showPixels; //!< Display pixels?
      bool p_showDeviation; //!< Display deviation?

      QPixmap p_pixmap; //!< Pixmap used for drawing
      Stretch p_stretch; //!< Stretch used to display pixels
      Statistics p_stats; //!< Stats used for calculating stretch and deviation
      QVector<QVector<double> > p_pixelData; //!< Stored pixel values
  };

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *
   */
  class StatisticsTool : public Tool {
      Q_OBJECT

    public:
      StatisticsTool(QWidget *parent);

    protected:
      QAction *toolPadAction(ToolPad *toolpad);
      QWidget *createToolBarWidget(QStackedWidget *parent);

    protected slots:
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);
      void hideDisplay(bool hide);
      void resizeScrollbars();

    private:
      void getStatistics(QPoint p);

      QAction *p_action; //!< Action associated with this tool
      QDialog *p_dialog; //!< Dialog to show pixel data and statistics
      QScrollArea *p_visualScroll; //!< Scroll area to house visual display
      QGroupBox *p_visualBox; //!< Visual group box to hold visual display
      VisualDisplay *p_visualDisplay; //!< Visual display
      QLabel *p_boxLabel; //!< Box label
      QLabel *p_minLabel; //!< Minimum label
      QLabel *p_maxLabel; //!< Maximum label
      QLabel *p_avgLabel; //!< Average label
      QLabel *p_stdevLabel; //!< Std Dev label
      QLabel *p_sampLabel; //!< Sample label
      QLabel *p_lineLabel; //!< Line label
      QLabel *p_dnLabel; //!< DN label
      QLineEdit *p_sampsEdit; //!< Sample size line edit
      QLineEdit *p_linesEdit; //!< Line size line edit

      int p_boxSamps; //!< Sample size for box
      int p_boxLines; //!< Line size for box

      int p_ulSamp; //!< Upper left sample of region
      int p_ulLine; //!< Upper left line of region

      bool p_set; //!< Boolean to see if data is set

    private slots:
      void changeBoxSamples();
      void changeBoxLines();
  };
};

#endif
