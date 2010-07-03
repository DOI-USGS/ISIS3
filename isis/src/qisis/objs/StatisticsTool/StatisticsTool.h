#ifndef StatisticsTool_h
#define StatisticsTool_h

/**
 * @file
 * $Date: 2010/06/28 09:21:33 $ $Revision: 1.3 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

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

namespace Qisis {
  /**
   * @brief Tool to visualize statistics in an n * m box.
   *
   * This tool is used to visualize statistics in an n * m box specified 
   * by the user. It allows for textual and visual representation of the 
   * DN values in the region as well as the standard deviation over the box. 
   * 
   * @ingroup Visualization Tools
   * 
   * @author  Noah Hilt
   * 
   * @internal

   *  @history 2009-10-07 Noah Hilt - Original version.
   *  
   *  @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport
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
      Isis::Stretch p_stretch; //!< Stretch used to display pixels
      Isis::Statistics p_stats; //!< Stats used for calculating stretch and deviation
      QVector<QVector<double> > p_pixelData; //!< Stored pixel values
  };

  class StatisticsTool : public Tool {
    Q_OBJECT

    public:
      StatisticsTool (QWidget *parent);

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
