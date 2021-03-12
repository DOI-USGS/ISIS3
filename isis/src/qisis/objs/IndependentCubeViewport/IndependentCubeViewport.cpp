/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "IndependentCubeViewport.h"

#include <QEvent>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QScrollBar>
#include <QRect>

#include <iostream>

#include "Camera.h"
#include "Projection.h"
#include "RingPlaneProjection.h"
#include "TProjection.h"
#include "CubeStretch.h"
#include "Stretch.h"
#include "StretchTool.h"
#include "ViewportBuffer.h"

using namespace std;


namespace Isis
{
  IndependentCubeViewport::IndependentCubeViewport(Cube * cube,
      CubeDataThread * cdt, QWidget * parent) :
      CubeViewport(cube, cdt, parent)
  {
    panningPrevPoint = NULL;
    bandingPoint1 = NULL;
    bandingPoint2 = NULL;

    panningPrevPoint = new QPoint;
    bandingPoint1 = new QPoint;
    bandingPoint2 = new QPoint;

    leftClick = false;
    banding = false;
    zooming = false;
    panning = false;
    viewport()->setMouseTracking(true);
  }


  IndependentCubeViewport::~IndependentCubeViewport()
  {
    if (panningPrevPoint)
    {
      delete panningPrevPoint;
      panningPrevPoint = NULL;
    }

    if (bandingPoint1)
    {
      delete bandingPoint1;
      bandingPoint1 = NULL;
    }

    if (bandingPoint2)
    {
      delete bandingPoint2;
      bandingPoint2 = NULL;
    }
  }


  bool IndependentCubeViewport::eventFilter(QObject * o, QEvent * e)
  {
    // Handle standard mouse tracking on the viewport
    if (o == viewport())
    {
      QMouseEvent *m = (QMouseEvent *) e;
      QPoint currentPosition(m->pos());
      Qt::MouseButton b = (Qt::MouseButton)(m->button() + m->modifiers());

      switch (e->type())
      {
        case QEvent::Enter:
        {
//          viewport()->setMouseTracking(true);
          emit mouseEnter();
          break;
        }
        case QEvent::MouseMove:
        {
          if (panning)
            emit synchronize(this);

          handleMouseMove(currentPosition);
          break;
        }
        case QEvent::Leave:
        {
//          viewport()->setMouseTracking(false);
          emit mouseLeave();
          break;
        }
        case QEvent::MouseButtonPress:
        {
          handleMousePress(currentPosition, b);
          break;
        }
        case QEvent::MouseButtonRelease:
        {
          handleMouseRelease(currentPosition);
          emit synchronize(this);
          break;
        }
        default:
        {
          return false;
        }
      }
    }
    else
    {
      return QAbstractScrollArea::eventFilter(o, e);
    }
    return true;
  }


  void IndependentCubeViewport::paintEvent(QPaintEvent * e)
  {
    CubeViewport::paintEvent(e);

    QPainter painter(viewport());
    painter.drawPixmap(0, 0, p_pixmap);

    if (banding)
    {
      QPen pen(QColor(255, 0, 0));
      pen.setStyle(Qt::SolidLine);
      painter.setPen(pen);
      painter.drawRect(bandingRect());
    }

    painter.end();
  }


  void IndependentCubeViewport::restretch(ViewportBuffer * buffer)
  {
    CubeStretch globalStretch = grayStretch();
    globalStretch.CopyPairs(StretchTool::stretchBand(this, StretchTool::Gray));
    stretchGray(globalStretch);
  }


  void IndependentCubeViewport::showEvent(QShowEvent * event)
  {
    QAbstractScrollArea::show();

    CubeViewport::showEvent(event);
    restretch(grayBuffer());
  }


  void IndependentCubeViewport::resetKnownGlobal()
  {

    p_globalStretches->clear();
    for (int i = 0; i < cubeBands(); i++)
      p_globalStretches->append(NULL);

    if (isVisible())
      grayBuffer()->addStretchAction();
  }


  void IndependentCubeViewport::cubeDataChanged(int cubeId,
      const Isis::Brick * data)
  {
    if (data->SampleDimension() == cubeSamples() &&
        data->LineDimension() == cubeLines() &&
        data->Sample() == 1 &&
        data->Line() == 1 &&
        data->BandDimension() == 1 &&
        data->Band() == grayBand() &&
        cubeId == cubeID())
    {
      // reset the global stretch
      Stretch *& globalStretch = (*p_globalStretches)[data->Band() - 1];

      if (globalStretch)
      {
        delete globalStretch;
        globalStretch = NULL;
      }


      Stretch newGlobal = grayStretch();
      newGlobal.ClearPairs();
      Statistics stats;
      stats.AddData(data->DoubleBuffer(), data->size());

      if (stats.ValidPixels() > 1 &&
          fabs(stats.Minimum() - stats.Maximum()) > DBL_EPSILON)
      {
        Histogram hist(stats.BestMinimum(), stats.BestMaximum(), 65536);
        hist.AddData(data->DoubleBuffer(), data->size());

        if (fabs(hist.Percent(0.5) - hist.Percent(99.5)) > DBL_EPSILON)
        {
          newGlobal.AddPair(hist.Percent(0.5), 0.0);
          newGlobal.AddPair(hist.Percent(99.5), 255.0);
        }
      }

      if (newGlobal.Pairs() == 0)
      {
        newGlobal.AddPair(-DBL_MAX, 0.0);
        newGlobal.AddPair(DBL_MAX, 255.0);
      }

      globalStretch = new CubeStretch(newGlobal);

      if (isVisible())
        stretchGray(newGlobal);
    }

    CubeViewport::cubeDataChanged(cubeId, data);
  }


  void IndependentCubeViewport::handleMouseMove(QPoint p)
  {
    if (panning)
    {
      QPoint diff = *panningPrevPoint - p;
      *panningPrevPoint = p;
      scrollBy(diff.x(), diff.y());
    }
    else
    {
      if (banding)
      {
        *bandingPoint2 = p;
        viewport()->repaint();
      }

      track(p);
    }
  }


  void IndependentCubeViewport::handleMousePress(QPoint p, Qt::MouseButton b)
  {
    bool ctrlPressed = ((b & Qt::ControlModifier) == Qt::ControlModifier);
    bool shiftPressed = ((b & Qt::ShiftModifier) == Qt::ShiftModifier);
    bool ctrlShiftPressed = ctrlPressed && shiftPressed;
    leftClick = ((b & Qt::LeftButton) == Qt::LeftButton);

    *bandingPoint1 = p;
    *bandingPoint2 = p;
    if (ctrlPressed && !shiftPressed)
    {
      stretching = true;
      banding = true;
    }
    else
    {
      if (ctrlShiftPressed)
      {
        panning = true;
        *panningPrevPoint = p;
      }
      else
      {
        zooming = true;
        banding = true;
      }
    }
  }


  void IndependentCubeViewport::handleMouseRelease(QPoint p)
  {
    banding = false;
    *bandingPoint2 = p;
    if (zooming)
    {
      zooming = false;
      zoom();
    }
    else
    {
      if (panning)
      {
        panning = false;
      }
      else
      {
        if (stretching)
        {
          stretching = false;
          if (leftClick)
            stretch();
          else
            stretchKnownGlobal();
        }
      }
    }
  }


  void IndependentCubeViewport::handleSynchronization(
      IndependentCubeViewport * other)
  {
    if (!isVisible())
      return;

    int deltaWidth = other->viewport()->width() - viewport()->width();
    int deltaHeight = other->viewport()->height() - viewport()->height();

    double offsetXToCenter = deltaWidth / 2.0 + viewport()->width() / 2.0;
    double offsetYToCenter = deltaHeight / 2.0 + viewport()->height() / 2.0;

    double offsetSampsToCenter = offsetXToCenter / other->scale();
    double offsetLinesToCenter = offsetYToCenter / other->scale();

    double otherSS, otherSL; // SS = start sample, SL = start line :)
    other->viewportToCube(0, 0, otherSS, otherSL);

    double thisCenterSamp = otherSS + offsetSampsToCenter;
    double thisCenterLine = otherSL + offsetLinesToCenter;

    if (scale() == other->scale())
      center(thisCenterSamp, thisCenterLine);
    else
      setScale(other->scale(), thisCenterSamp, thisCenterLine);
  }


  QRect IndependentCubeViewport::bandingRect()
  {
    QRect rect;
    rect.setTop(qMin(bandingPoint1->y(), bandingPoint2->y()));
    rect.setLeft(qMin(bandingPoint1->x(), bandingPoint2->x()));
    rect.setBottom(qMax(bandingPoint1->y(), bandingPoint2->y()));
    rect.setRight(qMax(bandingPoint1->x(), bandingPoint2->x()));

    return rect;
  }


  void IndependentCubeViewport::stretch()
  {
    QRect rect(bandingRect());
    if (rect.width() == 0 || rect.height() == 0)
      return;

    Stretch newStretch;

    newStretch = grayStretch();
    newStretch.CopyPairs(StretchTool::stretchBuffer(grayBuffer(), rect));

    stretchGray(newStretch);
  }

  void IndependentCubeViewport::track(const QPoint & p)
  {
    if (grayBuffer()->working())
    {
      emit cantTrack("busy", this);
      return;
    }

    double sample, line;
    viewportToCube(p.x(), p.y(), sample, line);

    /*    cerr << "cubeSamples() + 0.5: " << cubeSamples() + 0.5 << "\n"
     << "cubeLines() + 0.5: " << cubeLines() + 0.5 << "\n"
     << "sample: " << sample << "\n"
     << "line: " << line << "\n";
     */
    // if sample and line in range then do tracking

    double dn;
    if (sample >= 0.5 && sample <= cubeSamples() + 0.5 &&
        line >= 0.5 && line <= cubeLines() + 0.5 &&
        trackBuffer(grayBuffer(), p, dn))
    {
      bool camSucceeds = camera() && camera()->SetImage(sample, line);
      bool projSucceeds = !camSucceeds && projection() &&
          projection()->SetWorld(sample, line);

      // Determine the projection type if we have a projection
      Projection::ProjectionType projType = Projection::Triaxial;
      if (projSucceeds) projType = projection()->projectionType();

      if (camSucceeds || projSucceeds)
      {
        double lat = 0.0;
        double lon = 0.0;

        if (camSucceeds)
        {
          lat = camera()->UniversalLatitude();
          lon = camera()->UniversalLongitude();
        }
        else
        {
          // For now just put radius/az into lat/lon  TODO do it better
          if (projType == Projection::Triaxial) {
            TProjection *tproj = (TProjection *) projection();
            lat = tproj->Latitude();
            lon = tproj->Longitude();
          }
          else { // Must be RingPlane
            RingPlaneProjection *rproj = (RingPlaneProjection *) projection();
            lat = rproj->RingRadius();
            lon = rproj->RingLongitude();
          }
        }

        emit trackingChanged(sample, line, lat, lon, dn, this);
      }
      else
      {
        emit trackingChanged(sample, line, dn, this);
      }
    }
    else
    {
      emit cantTrack("n/a", this);
    }
  }


  bool IndependentCubeViewport::trackBuffer(ViewportBuffer * buffer,
      const QPoint & p, double & dn)
  {
    const QRect rect(buffer->bufferXYRect());
    bool success = rect.contains(p, true);
    if (success)
    {
      const int bufX = p.x() - rect.left();
      const int bufY = p.y() - rect.top();
      dn = buffer->getLine(bufY)[bufX];
    }

    return success;
  }


  void IndependentCubeViewport::zoom()
  {
    //    cerr << "zooming!\n";
    // To do the zooming we call setScale(double scale, int x, int y) which
    // will scale the viewport after centering to x, y.  Before we can call
    // setScale however we must first calculate the new scale and the new
    // center point.

    QRect rect = bandingRect();
    int x = rect.x();
    int y = rect.y();
    double scale;

    //    cerr << "topleft:     " << rect.left() << ", " << rect.top() << "\n"
    //         << "bottomright: " << rect.right() << ", " << rect.bottom() << "\n\n";

    if (rect.topLeft() == rect.bottomRight())
    {
      // initialize zoom factor and invert if zooming out
      double factor = 2.0;
      if (!leftClick)
        factor = 1.0 / factor;

      scale = this->scale() * factor;
    }
    else
    {
      if ((rect.width() < 5) || (rect.height() < 5))
      {
        viewport()->repaint();
        return;
      }

      x += rect.width() / 2;
      y += rect.height() / 2;
      double xscale = (double) viewport()->width() / (double) rect.width();
      double yscale = (double) viewport()->height() / (double) rect.height();
      scale = xscale < yscale ? xscale : yscale;

      if (!leftClick)
        scale = 1.0 / scale;

      scale *= this->scale();
    }

    // execute zoom
    setScale(scale, x, y);
  }

}
