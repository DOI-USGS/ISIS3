#ifndef ChipViewport_h
#define ChipViewport_h

/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/06/17 00:08:45 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>

#include "Chip.h"
#include "Stretch.h"
#include "iException.h"
#include "Histogram.h"

class QImage;

namespace Qisis {
  /**
    * @brief Viewport for Isis Chips
    *
    * @ingroup Visualization Tools
    *
    * @author 2007-05-01 Tracie Sucharski
    *
    * @internal
    * @history 2008-09-09 Tracie Sucharski - Added setCircle and setCircleSize
    *                          methods.
    * @history 2010-06-16 Jeannie Walldren - Modified geomChip(), nogeomChip(),
    *                        rotateChip() and reloadChip() to catch possible
    *                        iExceptions from Chip's Load() method and display
    *                        Error in QMessageBox
    *
    */

  class ChipViewport : public QWidget {
      Q_OBJECT


    public:
      ChipViewport(int width, int height, QWidget *parent = 0);
      ~ChipViewport();

      //!  Set chip
      void setChip(Isis::Chip *chip, Isis::Cube *chipCube);

      //!  Load with another ChipViewport
      void loadView(ChipViewport &newView);

      //!  Return chip
      Isis::Chip *chip() const {
        return p_chip;
      };

      //! Return the number of samples in the chip
      int chipSamples() const {
        return p_chip->Samples();
      };

      //! Return the number of lines in the chip
      int chipLines() const {
        return p_chip->Lines();
      };

      //! Return the gray band currently viewed
      int grayBand() const {
        return p_gray.band;
      };

      //!  Return the position of cube under cross hair
      double tackSample();
      double tackLine();

      //!  Draw X on point
      //void markPoint (double sample, double line);


      //! Return the gray band stretch
      Isis::Stretch grayStretch() const {
        return p_gray.stretch;
      };

    signals:
      //!< Signal sent when tack point changes
      void tackPointChanged(double);

    public slots:

      void autoStretch();
      void setCross(bool checked);
      void rotateChip(int rotation);
      void setCircle(bool checked);
      void setCircleSize(int size);

      void geomChip(Isis::Chip *matchChip, Isis::Cube *matchChipCube);
      void nogeomChip();

      void panUp();
      void panDown();
      void panLeft();
      void panRight();

      void zoomIn();
      void zoomOut();
      void zoom1();

      void refreshView(double tackSample, double tackLine);

    protected:
      void paintEvent(QPaintEvent *e);
      void enterEvent(QEvent *e);
      void keyPressEvent(QKeyEvent *e);
      void mousePressEvent(QMouseEvent *event);

    protected slots:

    private:
      void reloadChip(double tackSample = 0., double tackLine = 0.);

      void computeStretch(Isis::Stretch &stretch);
      void paintImage();

      class BandInfo {
        public:
          int band;
          Isis::Stretch stretch;
          BandInfo() {
            band = 1;
            stretch.SetNull(0.0);
            stretch.SetLis(0.0);
            stretch.SetLrs(0.0);
            stretch.SetHis(255.0);
            stretch.SetHrs(255.0);
          };
      };

      BandInfo p_gray;//!< info for the gray bands.
      Isis::Chip *p_chip;//!< The chip
      Isis::Cube *p_chipCube;//!< The chip's cube

      int p_width;//!< Chip width
      int p_height;//!< Chip height

      bool p_geomIt;//!< geomIt?
      Isis::Chip *p_matchChip;//!< The matching chip.
      Isis::Cube *p_matchChipCube;//!< The matching chip's cube

      double p_zoomFactor;//!< Zoom Factor
      int p_rotation;//!< Rotation

      QImage *p_image;//!< The image
      bool p_paintImage;//!< Paint Image?
      bool p_cross;//!< Draw crosshair
      bool p_circle;//!< Draw circle
      int p_circleSize;//!<Circle size

      ChipViewport *p_tempView;//!< Temporary viewport

  };
};

#endif
