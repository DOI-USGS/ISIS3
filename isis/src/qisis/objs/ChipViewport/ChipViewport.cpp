#include "IsisDebug.h"

#include "ChipViewport.h"

#include <iostream>

#include <QMessageBox>
#include <QPainter>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "CubeViewport.h"
#include "SerialNumber.h"
#include "Stretch.h"

using namespace Isis;

namespace Qisis {

  //!  Construct an empty viewport
  ChipViewport::ChipViewport(int width, int height, QWidget *parent) :
    QWidget(parent) {

    setFixedSize(width, height);
    setBackgroundRole(QPalette::Dark);
//    setFocusPolicy(Qt::StrongFocus);

    p_width = width;
    p_height = height;
    p_zoomFactor = 1.0;
    p_rotation = 0;
    p_geomIt = false;
    p_tempView = NULL;
    p_cross = true;
    p_circle = false;
    p_chip = NULL;
    p_chipCube = NULL;
    p_matchChip = NULL;
    p_matchChipCube = NULL;
    p_image = NULL;
    p_controlNet = NULL;
    p_stretchLocked = false;
    p_stretch = NULL;

    p_stretch = new Stretch;
  }


  /**
   * Destructor
   *
   */
  ChipViewport::~ChipViewport() {
    if (p_stretch) {
      delete p_stretch;
      p_stretch = NULL;
    }
  }


  /**
   * get viewport x and y from cube sample and line
   *
   * @param samp Sample in cube
   * @param line Line in cube
   * @param x calcualated viewport x coordinate
   * @param y calcualated viewport y coordinate
   *
   * @returns True if the point is contained in the viewport, false otherwise
   */
  bool ChipViewport::cubeToViewport(double samp, double line,
                                    int &x, int &y) {

    p_chip->SetCubePosition(samp, line);
    x = ((int) p_chip->ChipSample()) - 1;
    y = ((int) p_chip->ChipLine()) - 1;

    return p_chip->IsInsideChip(samp, line);
  }


  /**
   * Set the chip for this ChipViewport
   *
   * @author Tracie Sucharski
   * @internal
   *   @history 2009-14-2009  Tracie Sucharski - Make sure the p_image is clear
   *                             before allocating new.
   */

  void ChipViewport::setChip(Isis::Chip *chip, Isis::Cube *chipCube) {
    // Is the chip usable?
    if (chip == NULL || chipCube == NULL) {
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      "Can not view NULL chip pointer",
                                      _FILEINFO_);
    }

    p_zoomFactor = 1.0;
    p_rotation = 0;

    p_chip = chip;
    p_chipCube = chipCube;
    if (p_image != NULL)
      delete p_image;
    p_image = new QImage(chip->Samples(), chip->Lines(), QImage::Format_RGB32);

    autoStretch();

    emit tackPointChanged(p_zoomFactor);
  }


  //! Apply automatic stretch using data from entire chip
  void ChipViewport::autoStretch() {
    computeStretch(p_gray.stretch);
    paintImage();
    update();
  }


  void ChipViewport::stretchFromCubeViewport(Isis::Stretch *newStretch,
      Qisis::CubeViewport *cvp) {

    ASSERT(cvp != NULL);

    if (!cvp || !p_chipCube)
      return;

    // only stretch if the CubeViewport is opened to the same cube as we are,
    // otherwise the signal was meant for a different ChipViewport!
    if (cvp->cube()->Filename() == p_chipCube->Filename()) {

      // if user right clicked in the CubeViewport then we get a SIGNAL with a
      // NULL Stretch.  This is used to signify that we need to restretch on our
      // own (go back to global).
      if (!newStretch) {
        computeStretch(p_gray.stretch, true);
        paintImage();
        update();
      }
      else {
        *p_stretch = *newStretch;
        p_gray.stretch = *newStretch;
        paintImage();
        update();
      }
    }
  }


  void ChipViewport::changeStretchLock(int newStatus) {
    if (newStatus == 0)
      p_stretchLocked = false;
    else
      p_stretchLocked = true;
  }


  //! Compute automatic stretch for a portion of the cube
  void ChipViewport::computeStretch(Isis::Stretch &stretch, bool force) {
    if (p_stretchLocked && !force) {
      stretch = *p_stretch;
    }
    else {
      Isis::Statistics stats;
      for (int line = 1; line < p_chip->Lines(); line++) {
        for (int samp = 1; samp < p_chip->Samples(); samp++) {
          double value = p_chip->GetValue(samp, line);
          stats.AddData(&value, 1);
        }
      }

      Isis::Histogram hist(stats.BestMinimum(), stats.BestMaximum());
      for (int line = 1; line <= p_chip->Lines(); line++) {
        for (int samp = 1; samp <= p_chip->Samples(); samp++) {
          double value = p_chip->GetValue(samp, line);
          hist.AddData(&value, 1);
        }
      }

      stretch.ClearPairs();
      if (hist.Percent(0.5) != hist.Percent(99.5)) {
        stretch.AddPair(hist.Percent(0.5), 0.0);
        stretch.AddPair(hist.Percent(99.5), 255.0);
      }
      else {
        stretch.AddPair(-DBL_MAX, 0.0);
        stretch.AddPair(DBL_MAX, 255.0);
      }

      *p_stretch = stretch;
    }
  }


  //! Paint QImage
  void ChipViewport::paintImage() {
    //  TODO: ??? Need something similar to CubeViewport clipping, so that
    //         at edge of image, fill viewport w/ black
    for (int y = 0; y < p_chip->Lines(); y++) {
      QRgb *rgb = (QRgb *) p_image->scanLine(y);
      int r, g, b;
      for (int x = 0; x < p_chip->Samples(); x++) {
        r = g = b = (int) p_gray.stretch.Map(p_chip->GetValue(x + 1, y + 1));
        rgb[x] =  qRgb(r, g, b);
      }
    }
    repaint();

  }


  //! Repaint the viewport
  void ChipViewport::paintEvent(QPaintEvent *e) {
    QPainter painter(this);

    if (p_tempView != NULL) {
      painter.drawImage(0, 0, *(p_tempView->p_image));
    }
    else {
      painter.drawImage(0, 0, *p_image);
    }

    if (p_cross == true) {
      painter.setPen(Qt::red);
      painter.drawLine(0, (p_height - 1) / 2, p_width - 1, (p_height - 1) / 2);
      painter.drawLine((p_width - 1) / 2, 0, (p_width - 1) / 2, p_height - 1);
    }

    if (p_circle == true) {
      painter.setPen(Qt::red);
      painter.drawEllipse((p_height - 1) / 2 - p_circleSize / 2,
                          (p_width - 1) / 2 - p_circleSize / 2,
                          p_circleSize, p_circleSize);
    }

    // draw measure locations if we have a control network
    if (p_controlNet) {
      string serialNumber = Isis::SerialNumber::Compose(*p_chipCube);

      // loop through all points in the control net
      for (int i = 0; i < p_controlNet->GetNumPoints(); i++) {
        const ControlPoint *p = p_controlNet->GetPoint(i);

        // if this point is contained in the image
        if (p->HasSerialNumber(serialNumber)) {
          const ControlMeasure *measure = p->GetMeasure(serialNumber);

          // Find the measurments on the viewport
          double samp = measure->GetSample();
          double line = measure->GetLine();
          int x, y;

          cubeToViewport(samp, line, x, y);

          // Determine pen color
          // if the point or measure is ignored set to yellow
          if (p->IsIgnored() || (!p->IsIgnored() && measure->IsIgnored())) {
            painter.setPen(QColor(255, 255, 0)); // set point marker yellow
          }
          // check for ground measure
          else if (p->GetType() == Isis::ControlPoint::Fixed) {
            painter.setPen(Qt::magenta);// set point marker magenta
          }
          else {
            painter.setPen(Qt::green); // set all other point markers green
          }

          // draw points which are not under cross
          if (x != (p_width - 1) / 2 || y != (p_height - 1) / 2) {
            painter.drawLine(x - 5, y, x + 5, y);
            painter.drawLine(x, y - 5, x, y + 5);
          }
        }
      }
    }

    p_tempView = NULL;
    //painter.end();

  }


  //!  Load with info from given ChipViewport
  void ChipViewport::loadView(ChipViewport &newView) {
    p_tempView = &newView;
    update();
  }


  /**
   * Returns tack sample
   *
   *
   * @return double
   */
  double ChipViewport::tackSample() {
    p_chip->SetChipPosition(p_chip->TackSample(), p_chip->TackLine());
    return p_chip->CubeSample();
  }


  /**
   * returns tack Line
   *
   *
   * @return double
   */
  double ChipViewport::tackLine() {
    p_chip->SetChipPosition(p_chip->TackSample(), p_chip->TackLine());
    return p_chip->CubeLine();
  }


  //!<  Pan up
  void ChipViewport::panUp() {
    int x, y;
    x = p_chip->TackSample();
    y = p_chip->TackLine() - 1;
    //  Reload with new cube position
    p_chip->SetChipPosition((double)x, (double)y);
    reloadChip(p_chip->CubeSample(), p_chip->CubeLine());
  }


  //!<  Pan down
  void ChipViewport::panDown() {
    int x, y;
    x = p_chip->TackSample();
    y = p_chip->TackLine() + 1;
    //  Reload with new cube position
    p_chip->SetChipPosition((double)x, (double)y);
    reloadChip(p_chip->CubeSample(), p_chip->CubeLine());
  }


  //!<  Pan left
  void ChipViewport::panLeft() {
    int x, y;
    x = p_chip->TackSample() - 1;
    y = p_chip->TackLine();
    //  Reload with new cube position
    p_chip->SetChipPosition((double)x, (double)y);
    reloadChip(p_chip->CubeSample(), p_chip->CubeLine());
  }


  //!<  Pan right
  void ChipViewport::panRight() {
    int x, y;
    x = p_chip->TackSample() + 1;
    y = p_chip->TackLine();
    //  Reload with new cube position
    p_chip->SetChipPosition((double)x, (double)y);
    reloadChip(p_chip->CubeSample(), p_chip->CubeLine());
  }


  /**
   * Zoom in
   *
   */
  void ChipViewport::zoomIn() {

    p_zoomFactor /= 2.0;
    reloadChip();
  }


  /**
   * Zoom out
   *
   */
  void ChipViewport::zoomOut() {

    p_zoomFactor *= 2.0;
    reloadChip();
  }


  /**
   * Zoom by a factor of one.
   *
   */
  void ChipViewport::zoom1() {

    p_zoomFactor = 1.0;
    reloadChip();
  }


  //!<  Slot to refresh viewport , point has changed
  void ChipViewport::refreshView(double tackSample, double tackLine) {

    reloadChip(tackSample, tackLine);

  }


  //!<  If mouse enters, make sure key events are processed w/o clicking
  void ChipViewport::enterEvent(QEvent *e) {
    setFocus();
  }


  //!< Process arrow keystrokes on cube
  void ChipViewport::keyPressEvent(QKeyEvent *e) {

    int x, y;
    x = p_chip->TackSample();
    y = p_chip->TackLine();
    if (e->key() == Qt::Key_Up) {
      y--;
    }
    else if (e->key() == Qt::Key_Down) {
      y++;
    }
    else if (e->key() == Qt::Key_Left) {
      x--;
    }
    else if (e->key() == Qt::Key_Right) {
      x++;
    }
    else {
      QWidget::keyPressEvent(e);
      return;
    }

    //  Reload with new cube position
    p_chip->SetChipPosition((double)x, (double)y);
    reloadChip(p_chip->CubeSample(), p_chip->CubeLine());
    emit userMovedTackPoint();
  }



  //!<  Process mouse events
  void ChipViewport::mousePressEvent(QMouseEvent *event) {

    QPoint p = event->pos();
    if (event->button() == Qt::LeftButton) {
      //  Reload with new cube position
      p_chip->SetChipPosition((double)p.x(), (double)p.y());
      reloadChip(p_chip->CubeSample(), p_chip->CubeLine());
      emit userMovedTackPoint();
    }
  }


  //!<  Slot to change state of crosshair
  void ChipViewport::setCross(bool checked) {

    if (checked == p_cross)
      return;

    p_cross = checked;
    repaint();

  }


  void ChipViewport::setCircle(bool checked) {

    if (checked == p_circle)
      return;

    p_circle = checked;
    repaint();
  }


  void ChipViewport::setCircleSize(int size) {

    p_circleSize = size;
    repaint();

  }



  /**
   * Slot to geom chip
   * @internal
   * @history 2010-06-16 Jeannie Walldren - Catch possible iException from Chip's
   *                        Load() method and display in QMessageBox
   */
  void ChipViewport::geomChip(Isis::Chip *matchChip, Isis::Cube *matchChipCube) {

    p_geomIt = true;
    p_matchChip = matchChip;
    p_matchChipCube = matchChipCube;
    try {
      p_chip->Load(*p_chipCube, *matchChip, *matchChipCube);
//    p_chip->ReLoad(*matchChip,p_zoomFactor);
    }
    catch (Isis::iException &e) {
      QString msg = "Cannot geom chip.\n";
      msg += e.Errors().c_str();
      QMessageBox::information((QWidget *)parent(), "Error", msg);
      e.Clear();
      return;
    }

    //  TODO:   ??? Can these be added to paintEvent method ???
    autoStretch();
  }

  /**
   * Slot to geom chip
   * @internal
   * @history 2010-06-16 Jeannie Walldren - Catch possible iException from Chip's
   *                        Load() method and display in QMessageBox
   */
  void ChipViewport::nogeomChip() {

    p_geomIt = false;
    try {
      p_chip->Load(*p_chipCube, p_rotation, p_zoomFactor);
    }
    catch (Isis::iException &e) {
      QString msg = "Cannot load no geom chip.\n";
      msg += e.Errors().c_str();
      QMessageBox::information((QWidget *)parent(), "Error", msg);
      e.Clear();
      return;
    }

    //  TODO:   ??? Can these be added to paintEvent method ???
    autoStretch();
  }


  /**
   * Slot to rotate chip
   *
   *
   *
   * @param rotation
   * @internal
   * @history 2010-06-16 Jeannie Walldren - Catch possible iException from Chip's
   *                        Load() method and display in QMessageBox
   */

  void ChipViewport::rotateChip(int rotation) {

    p_rotation = -rotation;
    try {
      p_chip->Load(*p_chipCube, -rotation, p_zoomFactor);
    }
    catch (Isis::iException &e) {
      QString msg = "Cannot load rotated chip.\n";
      msg += e.Errors().c_str();
      QMessageBox::information((QWidget *)parent(), "Error", msg);
      e.Clear();
      return;
    }

    //  TODO:   ??? Can these be added to paintEvent method ???
    autoStretch();
  }


  /**
   * Reloads the chip.
   *
   *
   * @param tackSample
   * @param tackLine
   * @history 2010-06-16 Jeannie Walldren - Catch possible iException from Chip's
   *                        Load() method and display in QMessageBox
   */
  void ChipViewport::reloadChip(double tackSample, double tackLine) {

    // Is the chip usable?
    if (p_chip == NULL) {
      throw Isis::iException::Message(Isis::iException::Programmer,
                                      "Can not view NULL chip pointer",
                                      _FILEINFO_);
    }

    if (tackSample != 0. && tackLine != 0.)
      p_chip->TackCube(tackSample, tackLine);
    if (p_geomIt) {
      if (p_matchChip == NULL) {
        throw Isis::iException::Message(Isis::iException::User,
                                        "Invalid match chip", _FILEINFO_);
      }
      try {
        p_chip->Load(*p_chipCube, *p_matchChip, *p_matchChipCube);
      }
      catch (Isis::iException &e) {
        QString msg = "Cannot reload chip.\n";
        msg += e.Errors().c_str();
        QMessageBox::information((QWidget *)parent(), "Error", msg);
        e.Clear();
        return;
      }
//      p_chip->ReLoad(*p_matchChip,p_zoomFactor);
    }
    else {
      try {
        p_chip->Load(*p_chipCube, p_rotation, p_zoomFactor);
      }
      catch (Isis::iException &e) {
        QString msg = "Cannot reload chip.\n";
        msg += e.Errors().c_str();
        QMessageBox::information((QWidget *)parent(), "Error", msg);
        e.Clear();
        return;
      }
    }


    //  TODO:   ??? Can these be added to paintEvent method ???
    autoStretch();

    //  emit signal indicating tack point changed so that the TieTool
    //  can update the sample/line label.
    emit tackPointChanged(p_zoomFactor);
  }

}
