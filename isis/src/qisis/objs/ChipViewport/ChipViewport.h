#ifndef ChipViewport_h
#define ChipViewport_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>

#include "Chip.h"
#include "Histogram.h"
#include "Stretch.h"

class QImage;

namespace Isis {
  class ControlNet;
  class CubeViewport;

  /**
   * @brief Viewport for Isis Chips
   *
   * @ingroup Visualization Tools
   *
   * @author 2007-05-01 Tracie Sucharski
   *
   * @internal
   *   @history 2008-09-09 Tracie Sucharski - Added setCircle and setCircleSize
   *                           methods.
   *   @history 2010-06-16 Jeannie Walldren - Modified geomChip(), nogeomChip(),
   *                           rotateChip() and reloadChip() to catch possible
   *                           iExceptions from Chip's Load() method and display
   *                           Error in QMessageBox
   *   @history 2010-11-17 Eric Hyer - Added cubeToViewport method and
   *                           setControlNet slot.  paintEvent can now use the
   *                           control net to paint measures in the viewport.
   *   @history 2010-11-22 Eric Hyer - ChipViewports can now be stretched by
   *                           stretching CubeViewports opened to the same cube
   *   @history 2010-11-24 Eric Hyer - Fixed bug where crosses were painted one
   *                           screen pixel off on each direction.  Also no
   *                           longer paint cross under the large main red
   *                           crosses.
   *   @history 2010-12-01 Eric Hyer - Added stretch locking
   *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Changed point
   *                           type "Ground" to "Fixed".
   *   @history 2011-06-14 Tracie Sucharski - Added mouseClick signal.  Qnet
   *                           needed to know if user moved the tackpoint vs.
   *                           simply loading a new chip, or geoming the chip.
   *   @history 2011-06-15 Tracie Sucharski - Changed signal mouseClick to
   *                           userMovedTackPoint.
   *                           //TODO  Could not use tackPointChanged signal
   *                           because that signal is emitted whenever the
   *                           measure is loaded not just when the user
   *                           initiates the move.  This should be cleaned up.
   *   @history 2011-09-14 Tracie Sucharski - Added user option to determine
   *                           whether control points are drawn.
   *   @history 2012-07-26 Tracie Sucharski - Added method to return zoom factor and
   *                           slot to zoom to a specific zoom factor.
   *   @history 2014-09-05 Kim Oyama - Added default initialization of Image to ChipViewport
   *                           construtor.
   *   @history 2015-12-08 Jeannie Backer - Fixed whitespace per ISIS coding standards.
   *   @history 2016-03-31 Tracie Sucharski - Added method to return the chip cube.
   *   @history 2016-06-07 Ian Humphrey - Updated documentation and coding standards. Fixes #3958.
   */
  class ChipViewport : public QWidget {
      Q_OBJECT


    public:
      ChipViewport(int width, int height, QWidget *parent = 0);
      virtual ~ChipViewport();

      bool cubeToViewport(double samp, double line, int &x, int &y);

      //!  Set chip
      void setChip(Chip *chip, Cube *chipCube);

      //!  Load with another ChipViewport, used for blinking
      void loadView(ChipViewport &newView);

      //!  Return chip
      Chip *chip() const {
        return m_chip;
      };

      Cube *chipCube() const {
        return m_chipCube;
      };

      //! Return the number of samples in the chip
      int chipSamples() const {
        return m_chip->Samples();
      };

      //! Return the number of lines in the chip
      int chipLines() const {
        return m_chip->Lines();
      };

      //! Return the gray band currently viewed
      int grayBand() const {
        return m_gray.band;
      };

      //!  Return the position of cube under cross hair
      double tackSample();
      double tackLine();

      //!  Return the zoom factor
      double zoomFactor();

      //!  Draw X on point
      //void markPoint (double sample, double line);

      //! Return the gray band stretch
      Stretch grayStretch() const {
        return m_gray.stretch;
      };


    signals:
      //!< Signal sent when tack point changes
      void tackPointChanged(double);

      //   TODO: This needs better name, tackPointChanged signal is emitted
      //   even if user does not initiate, maybe change tackPointChanged signal
      //   to only emit if user moves the tack point???
      void userMovedTackPoint();


    public slots:
      void autoStretch();
      void stretchFromCubeViewport(Stretch *, CubeViewport *);
      void changeStretchLock(int);
      void setPoints(bool checked);
      void setCross(bool checked);
      void rotateChip(int rotation);
      void setCircle(bool checked);
      void setCircleSize(int size);

      void geomChip(Chip *matchChip, Cube *matchChipCube);
      void nogeomChip();

      void panUp();
      void panDown();
      void panLeft();
      void panRight();

      void zoomIn();
      void zoomOut();
      void zoom1();
      void zoom(double zoomFactor);

      void refreshView(double tackSample, double tackLine);

      /**
       * sets the ControlNet to be used for drawing measure locations
       *
       * @param newControlNet The new ControlNet to be used
       */
      void setControlNet(ControlNet *newControlNet) {
        m_controlNet = newControlNet;
      }


    protected:
      void paintEvent(QPaintEvent *e);
      void enterEvent(QEvent *e);
      void keyPressEvent(QKeyEvent *e);
      void mousePressEvent(QMouseEvent *event);


    private:
      void reloadChip(double tackSample = 0., double tackLine = 0.);

      void computeStretch(Stretch &stretch, bool force = false);
      void paintImage();

      /**
       * Sets the mapping for gray band stretch
       *
       * @author ????-??-?? Unknown
       *
       * @internal
       */
      class BandInfo {
        public:
          int band;         //!< The gray band
          Stretch stretch;  //!< Stretch for the band
          //! BandInfo constructor
          BandInfo() {
            band = 1;
            stretch.SetNull(0.0);
            stretch.SetLis(0.0);
            stretch.SetLrs(0.0);
            stretch.SetHis(255.0);
            stretch.SetHrs(255.0);
          };
      };

      BandInfo m_gray;   //!< Info for the gray bands.
      Chip *m_chip;      //!< The chip
      Cube *m_chipCube;  //!< The chip's cube

      int m_width;  //!< Chip width
      int m_height; //!< Chip height

      bool m_geomIt;         //!< geomIt?
      Chip *m_matchChip;     //!< The matching chip.
      Cube *m_matchChipCube; //!< The matching chip's cube

      double m_zoomFactor; //!< Zoom Factor
      int m_rotation;      //!< Rotation

      QImage *m_image;   //!< The image
      bool m_paintImage; //!< Paint Image?
      bool m_showPoints; //!< Draw control points
      bool m_cross;      //!< Draw crosshair
      bool m_circle;     //!< Draw circle
      int m_circleSize;  //!< Circle size

      ChipViewport *m_tempView;  //!< Temporary viewport

      // The ControlNet pointed to by this pointer is not owned by this class!
      // It is ok for m_controlNet to be NULL any time.  If it is not NULL then
      // it is used to paint measures in the viewport.
      //
      // After construction, it is the responsibility of the user of this class
      // to maintain this pointer with the setControlNet method (to make sure
      // that either NULL or a valid ControlNet is being pointed to).
      ControlNet *m_controlNet;

      bool m_stretchLocked; //!< Whether or not to lock the stretch when transforming
      Stretch *m_stretch;   //!< Current stretch on the chip viewport
  };
};

#endif
