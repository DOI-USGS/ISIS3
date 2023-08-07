#include "RubberBandTool.h"

#include <cmath>
#include <float.h>

#include <QDebug>
#include <QList>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QRect>

#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/Coordinate.h"
#include "geos/geom/LineString.h"
#include "geos/geom/MultiLineString.h"
#include "geos/geom/Polygon.h"

#include "Angle.h"
#include "Constants.h"
#include "MdiCubeViewport.h"
#include "PolygonTools.h"
#include "SerialNumberList.h"

using namespace std;

namespace Isis {
  /**
   * This is the constructor. It's private because this class is a singleton.
   *
   * @param parent
   */
  RubberBandTool::RubberBandTool(QWidget *parent) : Tool(parent) {
    p_mouseLoc = NULL;
    p_vertices = NULL;

    p_mouseLoc = new QPoint;
    p_vertices = new QList< QPoint >;
    p_bandingMode = LineMode;

    activate(false);
    repaint();
  }


  RubberBandTool::~RubberBandTool() {
    if(p_mouseLoc) {
      delete p_mouseLoc;
      p_mouseLoc = NULL;
    }

    if(p_vertices) {
      delete p_vertices;
      p_vertices = NULL;
    }
  }

  /**
   * This is the main paint method for the rubber bands.
   *
   * For angles and lines, simply connect the known vertices.vertices[0].x()
   * For polygons, paint the vertices & close if completed the shape.
   * For circles, figure out the circle's square and draw the circle inside of it.
   * For EllipseModes, figure out the EllipseMode's rectangle and draw the circle inside of it.
   * For rectangles, paint the rectangle either to the mouse or back to the start depending on if the shape is complete.
   * For rotated rectangles, if we can interpolate extra sides draw them and draw all known sides.
   *
   * @param vp
   * @param painter
   */
  void RubberBandTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {
    QPen pen(QColor(255, 0, 0));
    QPen greenPen(QColor(0, 255, 0));
    pen.setStyle(Qt::SolidLine);
    greenPen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    if ( (vp != cubeViewport() && p_drawActiveOnly) ||
        !(vp == cubeViewport() || (cubeViewport()->isLinked() &&
          vp->isLinked()))) {
       return;
     }

    switch(p_bandingMode) {
      case AngleMode:
        paintVerticesConnected(painter);
        break;

      case LineMode:
        // if point needed, draw an X
        if(figureIsPoint() && !p_tracking) {
          painter->drawLine((*p_vertices)[0].x() - 10, (*p_vertices)[0].y() - 10,
                            (*p_vertices)[0].x() + 10, (*p_vertices)[0].y() + 10);
          painter->drawLine((*p_vertices)[0].x() - 10, (*p_vertices)[0].y() + 10,
                            (*p_vertices)[0].x() + 10, (*p_vertices)[0].y() - 10);
        }
        else {
          paintVerticesConnected(painter);
        }

        break;

      case PolygonMode:
        paintVerticesConnected(painter);

        if(!p_tracking && p_vertices->size() > 0) {
          painter->drawLine((*p_vertices)[0], (*p_vertices)[ p_vertices->size() - 1 ]);
        }
        break;

      case CircleMode:
      case EllipseMode: {
          if(p_vertices->size() != 0) {
            QList<QPoint> verticesList = vertices();
            int width = 2 * (verticesList[1].x() - verticesList[0].x());
            int height = 2 * (verticesList[1].y() - verticesList[0].y());

            // upper left x,y,width,height
            painter->drawEllipse(verticesList[0].x() - width / 2, verticesList[0].y() - height / 2,
                                 width,
                                 height
                                );
          }
        }
        break;

      case RectangleMode: {
          if(figureIsPoint() && !p_tracking) {
            painter->drawLine((*p_vertices)[0].x() - 10, (*p_vertices)[0].y() - 10,
                              (*p_vertices)[0].x() + 10, (*p_vertices)[0].y() + 10);
            painter->drawLine((*p_vertices)[0].x() - 10, (*p_vertices)[0].y() + 10,
                              (*p_vertices)[0].x() + 10, (*p_vertices)[0].y() - 10);
          }
          else {
            if(p_tracking && p_vertices->size() > 0) {
              paintRectangle((*p_vertices)[0], *p_mouseLoc, painter);
            }
            else if(p_vertices->size() > 0) {
              paintVerticesConnected(painter);
              painter->drawLine((*p_vertices)[0], (*p_vertices)[ p_vertices->size() - 1 ]);
            }
          }
        }
        break;

      case RotatedRectangleMode: {
          if(p_vertices->size() == 2) {
            QPoint missingVertex;
            calcRectCorners((*p_vertices)[0], (*p_vertices)[1], *p_mouseLoc, missingVertex);
            painter->drawLine(*p_mouseLoc, missingVertex);
            painter->drawLine(missingVertex, (*p_vertices)[0]);
          }
          else if(p_vertices->size() == 4) {
            painter->drawLine((*p_vertices)[0], (*p_vertices)[ 3 ]);
          }

          paintVerticesConnected(painter);

          // Draw indicator on top of original lines if applicable
          if(p_indicatorColors) {
            painter->setPen(greenPen);
            if(p_vertices->size() > 1) {
              painter->drawLine((*p_vertices)[0], (*p_vertices)[1]);
            }
            else if(p_vertices->size() == 1) {
              painter->drawLine((*p_vertices)[0], *p_mouseLoc);
            }

            painter->setPen(pen);
          }
        }
        break;

      case SegmentedLineMode:
        paintVerticesConnected(painter);
        break;
    }
  }

  /**
   * Given two set corners, and the mouse location, this method will interpolate the last two corners.
   *
   * @param corner1 Known point
   * @param corner2 Known point
   * @param corner3 Guessed corner (point to interpolate to).
   * @param corner4 Unknown corner.
   */
  void RubberBandTool::calcRectCorners(QPoint corner1, QPoint corner2, QPoint &corner3, QPoint &corner4) {
    double slope = ((double)corner2.y() - (double)corner1.y()) / ((double)corner2.x() - (double)corner1.x());

    if((fabs(slope) > DBL_EPSILON) && (slope < DBL_MAX) && (slope > -DBL_MAX)) {
      // corner1,corner2 make up y=m(x-x1)+y1
      // corner3,corner4 must make up || line crossing corner3.
      // b (y-intercept) is what differs from the original line and our parallel line.
      // Go ahead and figure out our new b by using b = -mx1+y1 from the point-slope formula.
      double parallelB = -1 * slope * corner3.x() + corner3.y();

      // Now we have our equation for a parallel line, which our new points lie on. Let's find the perpendicular lines
      // which cross corner1,corner2 in order to figure out where they cross it. Use -1/slope = perpendicular slope and
      // now we have y=m(x-x1)+y1. What we care about is b in y=mx+b, so figure it out using b = m*(-x1)+y1
      double perpSlope = -1.0 / slope;
      double perpLineMode1b = perpSlope * (-1 * corner1.x()) + corner1.y();
      double perpLineMode2b = perpSlope * (-1 * corner2.x()) + corner2.y();

      // Now let's find the perpendicular lines' intercepts on the parallel line.
      // y = mx+b = y = mx+b => mx+b(perpendicular) = mx+b(parallel) for the perp. lines and the parallel line.
      // Combine the b's on the left to make y= m(perp)x+k = m(par)x.
      double perpLineMode1k = perpLineMode1b - parallelB;
      double perpLineMode2k = perpLineMode2b - parallelB;

      // Now we have mx + k = mx (parallel). Divive the parallel slope out to get
      //    (m(perp)x+k)/m(parallel) = x. Move the x over from the left side of the equation by subtracting...
      //    k/m(parallel) = x - m(perp)x/m(parallel). Factor out the x's... k/m(par) = x(1-m(per)/m(par)) and divive
      //    both sides by "(1-m(per)/m(par))". So we end up with: (k/m(par)) / (1 - m(per) / m(par) ) =
      //    k/m(par) / ( (m(par)-m(per)) / m(par) ) = k / m(par) * m(par) / (m(par) - m(per)) = k / (m(par) - m(per))
      double perpLineMode1IntersectX = perpLineMode1k / (slope - perpSlope);
      double perpLineMode2IntersectX = perpLineMode2k / (slope - perpSlope);

      // The intersecting X values are now known, and the equation of the parallel line, so let's roll with it and
      // get our two corners set. perpLineMode1 => corner1 => corner4, perpLineMode2 => corner2 => corner3
      corner3.setX((int)perpLineMode2IntersectX);
      corner3.setY((int)(perpLineMode2IntersectX * slope + parallelB)); //mx+b
      corner4.setX((int)perpLineMode1IntersectX);
      corner4.setY((int)(perpLineMode1IntersectX * slope + parallelB)); //mx+b
    }
    else if(fabs(slope) < DBL_EPSILON) {
      corner3.setX(corner2.x());
      corner3.setY(corner3.y());
      corner4.setX(corner1.x());
      corner4.setY(corner3.y());
    }
    else {
      corner3.setX(corner3.x());
      corner3.setY(corner2.y());
      corner4.setX(corner3.x());
      corner4.setY(corner1.y());
    }
  }

  /**
   * This paints connecting lines to p_vertices. If tracking, a line is also drawn to
   *   the mouse location.
   *
   * @param painter
   */
  void RubberBandTool::paintVerticesConnected(QPainter *painter) {
    for(int vertex = 1; vertex < p_vertices->size(); vertex++) {
      QPoint start = (*p_vertices)[vertex - 1];
      QPoint end = (*p_vertices)[vertex];

      painter->drawLine(start, end);
    }

    if(p_tracking && (p_vertices->size() > 0)) {
      QPoint start = (*p_vertices)[p_vertices->size() - 1];
      QPoint end = *p_mouseLoc;

      painter->drawLine(start, end);
    }
  }

  /**
   * Given opposite corners, the other two are interpolated and the rectangle is drawn.
   *
   * @param upperLeft Corner opposite of lowerRight
   * @param lowerRight Corner opposite of upperLeft
   * @param painter
   */
  void RubberBandTool::paintRectangle(QPoint upperLeft, QPoint lowerRight, QPainter *painter) {
    QPoint upperRight = QPoint(lowerRight.x(), upperLeft.y());
    QPoint lowerLeft = QPoint(upperLeft.x(), lowerRight.y());

    paintRectangle(upperLeft, upperRight, lowerLeft, lowerRight, painter);
  }

  /**
   * This draws a box around the 4 points using the painter.
   *
   * @param upperLeft Initial corner
   * @param upperRight Corner connected to upperLeft, lowerRight
   * @param lowerLeft Corner connected to lowerRight, upperLeft
   * @param lowerRight Corner connected to lowerLeft, upperRight
   * @param painter
   */
  void RubberBandTool::paintRectangle(QPoint upperLeft, QPoint upperRight,
                                      QPoint lowerLeft, QPoint lowerRight, QPainter *painter) {
    painter->drawLine(upperLeft, upperRight);
    painter->drawLine(upperRight, lowerRight);
    painter->drawLine(lowerRight, lowerLeft);
    painter->drawLine(lowerLeft, upperLeft);
  }

  /**
   * This is called when changing modes or turning on. So, set the mode, reset, and activate
   *   our event handlers.
   *
   * @param mode
   * @param showIndicatorColors Color the first side of figures differently
   */
  void RubberBandTool::enable(RubberBandMode mode, bool showIndicatorColors) {
    RubberBandMode oldMode = p_bandingMode;
    p_bandingMode = mode;
    p_indicatorColors = showIndicatorColors;
    //Took this out because it was reseting and not letting us plot single points.
    //p_pointTolerance = 0;
    p_allClicks = false;
    p_drawActiveOnly = false;
    reset();
    activate(true);

    if(oldMode != mode) {
      emit modeChanged();
    }
  }

  /**
   * This is called when something is not using me, so
   *   turn off events, reset & repaint to clear the clear.
   */
  void RubberBandTool::disable() {

    activate(false);
    reset();
    repaint();
  }

  /**
   * This called to set whether rubber band is drawn on active viewport only
   * rather than all linked viewports.
   */
  void RubberBandTool::setDrawActiveViewportOnly(bool activeOnly) {
    p_drawActiveOnly = activeOnly;
    repaint();
  }

  /**
   * This triggers on a second mouse press. Only polygons care about this, and it signifies an end of
   *   shape. So, if we're in polygon mode, stop tracking the mouse and emit a complete.
   * @param p
   */
  void RubberBandTool::mouseDoubleClick(QPoint p) {
    p_doubleClicking = true;
    *p_mouseLoc = p;

    switch(p_bandingMode) {
      case AngleMode:
      case CircleMode:
      case EllipseMode:
      case LineMode:
      case RectangleMode:
      case RotatedRectangleMode:
        break;

      case SegmentedLineMode:
      case PolygonMode:
        p_tracking = false;
        repaint();
        emit bandingComplete();
        break;
    }
  }

  /**
   * If the click is not the left mouse button, this does nothing.
   *
   * This will set mouseDown as true. When the calculations are complete,
   *   p_mouseDown is set to true.
   *
   * For drag-only,
   *   A press means starting a new rubber band so reset & record the point. This applies to
   *     CircleModes, Eliipsoids, LineModes and RectangleModes.
   *
   * For Rotated RectangleModes,
   *   A mount press means we're starting over, setting the first point, or completing.
   *     For the first two, simply reset and record the point. For the latter, figure out the
   *     corners and store those points.
   *
   * For polygons,
   *   A press means record the current point. Reset first if we're not currently drawing.
   *
   * @param p
   * @param s
   */
  void RubberBandTool::mouseButtonPress(QPoint p, Qt::MouseButton s) {
    *p_mouseLoc = p;
    p_mouseButton = s;

    if((s & Qt::LeftButton) != Qt::LeftButton && !p_allClicks) {
      return;
    }

    switch(p_bandingMode) {
      case AngleMode:
        break;

      case CircleMode:
      case EllipseMode:
      case LineMode:
      case RectangleMode:
        reset();
        p_tracking = true;
        p_vertices->push_back(p);
        break;

      case RotatedRectangleMode:
        if(p_vertices->size() == 4) {
          reset();
        }

        if(p_vertices->size() == 0) {
          p_vertices->push_back(p);
          p_tracking = true;
        }
        break;

      case SegmentedLineMode:
      case PolygonMode:
        if(!p_tracking) {
          reset();
          p_tracking = true;
        }

        if(p_vertices->size() == 0 || (*p_vertices)[ p_vertices->size() - 1 ] != p) {
          p_vertices->push_back(p);
        }

        break;
    }

    p_mouseDown = true;
  }

  /**
   * If this is not the left mouse button, this does nothing.
   *
   * This will set mouseDown as false. When the calculations are complete,
   * p_doubleClicking is
   *   set to false. The double click event occurs with
   * `the press event so it's safe to set that flag here.
   *
   * The implementation differs, based on the mode, as follows:
   *
   * For angles,
   *   This signifies a click. We're always setting one of the
   *  three vertexes, but if there is an already
   *     complete vertex go ahead and reset first to start a new angle.
   *
   * For circles,
   *   Since this is a drag-only rubber band, a release signifies a complete. Figure out the corner, based
   *     on the mouse location, and push it onto the back of the vertex list and emit a complete.
   *
   * For EllipseModes,
   *   Since this is a drag-only rubber band, a release signifies a complete. We know the corner, it's the mouse loc,
   *     push it onto the back of the vertex list and emit a complete.
   *
   * For lines,
   *   Since this is a drag-only rubber band, a release signifies a complete. We know the end point,
   *     push it onto the back of the vertex list and emit a complete.
   *
   * For rectangles,
   *   Since this is a drag-only rubber band, a release signifies a complete. We know the opposite corner,
   *     figure out the others and push them onto the back of the vertex list and emit a complete.
   *
   * For rotated rectangles,
   *   If we're finishing dragging the first side, store the end point.
   *
   * For polygons,
   *   Do nothing, this is taken care of on press.
   *
   * @param p Current mouse Location
   * @param s Which mouse button was released
   */
  void RubberBandTool::mouseButtonRelease(QPoint p, Qt::MouseButton s) {
    if ((s & Qt::ControlModifier) == Qt::ControlModifier)
      *p_mouseLoc = snapMouse(p);
    else
      *p_mouseLoc = p;

    p_mouseButton = s;

    if((s & Qt::LeftButton) == Qt::LeftButton || p_allClicks) {
      p_mouseDown = false;
    }
    else {
      return;
    }

    switch(p_bandingMode) {
      case AngleMode: {
          if(p_vertices->size() == 3) {
            reset();
          }

          p_vertices->push_back(*p_mouseLoc);
          p_tracking = true;

          if(p_vertices->size() == 3) {
            p_tracking = false;
            emit bandingComplete();
          }
        }
        break;

      case LineMode:
      case CircleMode:
      case EllipseMode:
      case RectangleMode: {
          *p_vertices = vertices();
          p_tracking = false;
          emit bandingComplete();
        }
        break;

      case RotatedRectangleMode: {
          if(p_vertices->size() == 1) {
            p_vertices->push_back(*p_mouseLoc);
          }
          else if(p_vertices->size() == 2) {
            *p_vertices = vertices();
            p_tracking = false;
            emit bandingComplete();
          }
        }
        break;

      case SegmentedLineMode:
      case PolygonMode:
        break;
    }

    p_doubleClicking = false; // If we were in a double click, it's over now.


    MdiCubeViewport * activeViewport = cubeViewport();
    for (int i = 0; i < (int) cubeViewportList()->size(); i++) {
      MdiCubeViewport * curViewport = cubeViewportList()->at(i);
      if (curViewport == activeViewport ||
          (activeViewport->isLinked() && curViewport->isLinked())) {
        curViewport->viewport()->repaint();
      }
    }
  }


  /**
   * moves the mouse's location p to the nearest axis
   *
   * @param p The mouse's current location
   *
   * @returns The snapped point
   */
  QPoint RubberBandTool::snapMouse(QPoint p) {
    if (p_vertices->size()) {
      QPoint lastVertex = p_vertices->at(p_vertices->size() - 1);

      int deltaX = abs(p.x() - lastVertex.x());
      int deltaY = abs(p.y() - lastVertex.y());

      if (deltaX > deltaY)
        p.setY(lastVertex.y());
      else
        p.setX(lastVertex.x());
    }

    return p;
  }


  /**
   * If tracking is not enabled, this does nothing.
   *
   * This will first update the mouse location for painting purposes.
   *
   * Most of the implementation is a matter of emitting measureChanges:
   * For angles, if the first two vertices are specified a measureChange will be emitted.
   * For circles, if the center of the circle is known a measureChange will be emitted.
   * For EllipseModes, if the center of the EllipseMode is known a measureChange will be emitted.
   * For lines, if the first point of the line is known a measureChange will be emitted.
   * For rectangles, if the starting point is known a measureChange will be emitted.
   * For rotated rectangles, if the first side is specified a measureChange will be emitted.
   *
   * However, there is one exception:
   * For polygons, if the mouse button is pressed the mouse location is recorded as a valid vertex.
   *
   * @param p Current mouse Location
   */
  void RubberBandTool::mouseMove(QPoint p, Qt::MouseButton mouseButton) {
    if(!p_tracking) {
      return;
    }

    p_mouseButton = mouseButton;

    if ((p_mouseButton & Qt::ControlModifier) == Qt::ControlModifier)
      *p_mouseLoc = snapMouse(p);
    else
      *p_mouseLoc = p;

    switch(p_bandingMode) {
      case AngleMode:
      case RotatedRectangleMode:
        if(p_vertices->size() == 2) {
          emit measureChange();
        }
        break;

      case CircleMode:
      case EllipseMode:
      case RectangleMode:
        if(p_vertices->size() == 1) {
          emit measureChange();
        }
        break;

      case LineMode:
        emit measureChange();
        break;

      case SegmentedLineMode:
      case PolygonMode: {
          if(p_mouseDown && p != (*p_vertices)[ p_vertices->size() - 1 ]) {
            p_vertices->push_back(p);
          }

          if (p_bandingMode == SegmentedLineMode)
            emit measureChange();
        }
        break;
    }

    MdiCubeViewport * activeViewport = cubeViewport();
    for (int i = 0; i < (int) cubeViewportList()->size(); i++) {
      MdiCubeViewport * curViewport = cubeViewportList()->at(i);
      if (curViewport == activeViewport ||
          (activeViewport->isLinked() && curViewport->isLinked())) {
        curViewport->viewport()->repaint();
      }
    }
  }

  /**
   * This method returns the vertices. The return value is mode-specific, and the return will be
   *   consistent whether in a measureChange or bandingComplete slot.
   *
   * The return values are always in pixels.
   *
   * The return values are as follows:
   * For angles, the return will always be of size 3. The elements at 0 and 2 are the edges of the angle,
   *   while the element at 1 is the vertex of the angle.
   *
   * For circles, the return will always be of size 2. The element at 0 is the center of the circle, and the
   *   element at 1 is offset by the radius in both directions.
   *
   * For EllipseModes, the return will always be of size 2. The element at 0 is the center of the circle, and the
   *   element at 1 is offset by the radius in both directions.
   *
   * For lines, the return will always be of size 2. The elements are the start and end points.
   *
   * For rectangles, the return will always be of size 4. The elements will be the corners,
   *   in either a clockwise or counter-clockwise direction.
   *
   * For rotated rectangles, the same applies.
   *
   * For polygons, the return will be a list of vertices in the order that the user drew them.
   *
   * **It is NOT valid to call this unless you're in a measureChange or bandingComplete slot.
   *
   * @return QList<QPoint>
   */
  QList<QPoint> RubberBandTool::vertices() {
    QList<QPoint> vertices = *p_vertices;

    if(!figureComplete())
      return vertices;

    if(p_tracking) {
      switch(p_bandingMode) {
        case AngleMode:
        case LineMode:
        case SegmentedLineMode:
          vertices.push_back(*p_mouseLoc);
          break;

        case RectangleMode: {
            QPoint corner1 = QPoint(p_mouseLoc->x(), vertices[0].y());
            QPoint corner2 = QPoint(p_mouseLoc->x(), p_mouseLoc->y());
            QPoint corner3 = QPoint(vertices[0].x(), p_mouseLoc->y());
            vertices.push_back(corner1);
            vertices.push_back(corner2);
            vertices.push_back(corner3);
          }
          break;

        case RotatedRectangleMode: {
            QPoint missingVertex;
            calcRectCorners((*p_vertices)[0], (*p_vertices)[1], *p_mouseLoc, missingVertex);
            vertices.push_back(*p_mouseLoc);
            vertices.push_back(missingVertex);
          }
          break;


        case CircleMode: {
            int xradius = abs(p_mouseLoc->x() - vertices[0].x()) / 2;
            int yradius = xradius;

            if(p_mouseLoc->x() - vertices[0].x() < 0) {
              xradius *= -1;
            }

            if(p_mouseLoc->y() - vertices[0].y() < 0) {
              yradius *= -1;
            }

            // Adjust p_vertices[0] from upper left to center
            vertices[0].setX(vertices[0].x() + xradius);
            vertices[0].setY(vertices[0].y() + yradius);

            vertices.push_back(*p_mouseLoc);

            vertices[1].setX(vertices[0].x() + xradius);
            vertices[1].setY(vertices[0].y() + yradius);
          }
          break;

        case EllipseMode: {
            // Adjust p_vertices[0] from upper left to center
            double xradius = (p_mouseLoc->x() - vertices[0].x()) / 2.0;
            double yradius = (p_mouseLoc->y() - vertices[0].y()) / 2.0;
            vertices[0].setX((int)(vertices[0].x() + xradius));
            vertices[0].setY((int)(vertices[0].y() + yradius));

            vertices.push_back(*p_mouseLoc);
          }
          break;

        case PolygonMode:
          break;
      }
    }

    return vertices;
  }

  /**
   * This initializes the class except for the current mode, which is
   *   set on enable.
   *
   */
  void RubberBandTool::reset() {
    p_vertices->clear();
    p_tracking = false;
    p_mouseDown = false;
    p_doubleClicking = false;
    repaint();
  }

  Angle RubberBandTool::angle() {
    Angle result;

    if(currentMode() == AngleMode) {
      // We cancluate the angle by considering each line an angle itself, with respect to the
      //   X-Axis, and then differencing them.
      //
      //  theta1 = arctan((point0Y - point1Y) / (point0X - point1X))
      //  theta2 = arctan((point2Y - point1Y) / (point2X - point1X))
      //                   |
      //                   |        / <-- point0
      //                   |      / |
      //                   |    /   |
      //           theta1  |  /     |
      //            -->    |/       | <-- 90 degrees
      //  point1 --> ------|---------------------------
      //(vertex)     -->   |\     | <-- 90 degrees
      //           theta2  | \    |
      //                   |  \   |
      //                   |   \  |
      //                   |    \ |
      //                   |     \|
      //                   |      | <-- point 2
      //
      // angle = theta1 - theta2; **
      QList<QPoint> verticesList = vertices();
      double theta1 = atan2((double)(verticesList[0].y() - verticesList[1].y()), (double)(verticesList[0].x() - verticesList[1].x()));
      double theta2 = atan2((double)(verticesList[2].y() - verticesList[1].y()), (double)(verticesList[2].x() - verticesList[1].x()));
      double angle = (theta1 - theta2);

      // Force the angle into [0,2PI]
      while(angle < 0.0) {
        angle += PI * 2;
      }
      while(angle > PI * 2) {
        angle -= PI * 2;
      }

      // With our [0,2PI] angle, let's make it [0,PI] to get the interior angle.
      if(angle > PI) {
        angle = (PI * 2.0) - angle;
      }

      result = Angle(angle, Angle::Radians);
    }

    return result;
  }

  /**
   * This method will call the viewport's repaint if there is a current cube viewport.
   */
  void RubberBandTool::repaint() {
    if(cubeViewport() != NULL) {
      cubeViewport()->viewport()->repaint();
    }
  }

  /**
   *
   *
   * @return geos::Geometry*
   */
  geos::geom::Geometry *RubberBandTool::geometry() {
    geos::geom::Geometry *geometry = NULL;
    QList<QPoint> verticesList = vertices();

    switch(p_bandingMode) {
      case AngleMode: {
          if(verticesList.size() != 3)
            break;

          geos::geom::CoordinateArraySequence *points1 = new geos::geom::CoordinateArraySequence();
          geos::geom::CoordinateArraySequence *points2 = new geos::geom::CoordinateArraySequence();

          points1->add(geos::geom::Coordinate(verticesList[0].x(), verticesList[0].y()));
          points1->add(geos::geom::Coordinate(verticesList[1].x(), verticesList[1].y()));
          points2->add(geos::geom::Coordinate(verticesList[1].x(), verticesList[1].y()));
          points2->add(geos::geom::Coordinate(verticesList[2].x(), verticesList[2].y()));

          geos::geom::LineString *line1 = globalFactory->createLineString(points1);
          geos::geom::LineString *line2 = globalFactory->createLineString(points2);
          std::vector<geos::geom::Geometry *> *lines = new std::vector<geos::geom::Geometry *>;
          lines->push_back(line1);
          lines->push_back(line2);

          geos::geom::MultiLineString *angle = globalFactory->createMultiLineString(lines);
          geometry = angle;
        }
        break;

      case CircleMode:
      case EllipseMode: {
          if(verticesList.size() != 2)
            break;
          // A circle is an ellipse, so it gets no special case
          // Equation of an ellipse: (x-h)^2/a^2 + (y-k)^2/b^2 = 1 where
          // h is the X-location of the center, k is the Y-location of the center
          // a is the x-radius and b is the y-radius.
          // Solving for y, we get
          //   y = +-sqrt(b^2(1-(x-h)^2/a^2)) + k
          //   and our domain is [h-a,h+a]
          // We need the equation of this ellipse!
          double h = verticesList[0].x();
          double k = verticesList[0].y();
          double a = abs(verticesList[0].x() - verticesList[1].x());
          double b = abs(verticesList[0].y() - verticesList[1].y());
          if(a == 0)
            break; // Return null, we can't make an ellipse from this.
          if(b == 0)
            break; // Return null, we can't make an ellipse from this.

          // We're ready to try to solve
          double originalX = 0.0, originalY = 0.0;
          geos::geom::CoordinateArraySequence *points = new geos::geom::CoordinateArraySequence();

          // Now iterate through our domain, solving for y positive, using 1/5th of a pixel increments
          for(double x = h - a; x <= h + a; x += 0.2) {
            double y = sqrt(pow(b, 2) * (1.0 - pow((x - h), 2) / pow(a, 2))) + k;
            points->add(geos::geom::Coordinate(x, y));

            if(x == h - a) {
              originalX = x;
              originalY = y;
            }
          }

          // Iterate through our domain backwards, solving for y negative, using 1/5th of a pixel decrements
          for(double x = h + a; x >= h - a; x -= 0.2) {
            double y = -1.0 * sqrt(pow(b, 2) * (1.0 - pow((x - h), 2) / pow(a, 2))) + k;
            points->add(geos::geom::Coordinate(x, y));
          }

          points->add(geos::geom::Coordinate(originalX, originalY));

          geometry = globalFactory->createPolygon(
                       globalFactory->createLinearRing(points), NULL
                     );
        }
        break;

      case RectangleMode:
      case RotatedRectangleMode:
      case PolygonMode: {
          if(verticesList.size() < 3)
            break;

          geos::geom::CoordinateArraySequence *points = new geos::geom::CoordinateArraySequence();

          for(int vertex = 0; vertex < verticesList.size(); vertex++) {
            points->add(geos::geom::Coordinate(verticesList[vertex].x(), verticesList[vertex].y()));
          }

          points->add(geos::geom::Coordinate(verticesList[0].x(), verticesList[0].y()));

          geometry = globalFactory->createPolygon(globalFactory->createLinearRing(points), NULL);

        }
        break;

      case LineMode: {
          if(verticesList.size() != 2)
            break;

          geos::geom::CoordinateArraySequence *points = new geos::geom::CoordinateArraySequence();
          points->add(geos::geom::Coordinate(verticesList[0].x(), verticesList[0].y()));
          points->add(geos::geom::Coordinate(verticesList[1].x(), verticesList[1].y()));
          geos::geom::LineString *line = globalFactory->createLineString(points);
          geometry = line;
        }
        break;

      case SegmentedLineMode: {
          if(verticesList.size() < 2)
            break;
          geos::geom::CoordinateArraySequence *points = new geos::geom::CoordinateArraySequence();

          for(int vertex = 0; vertex < verticesList.size(); vertex++) {
            points->add(geos::geom::Coordinate(verticesList[vertex].x(), verticesList[vertex].y()));
          }

          geos::geom::LineString *line = globalFactory->createLineString(points);
          geometry = line;
        }
        break;
    }

    if(geometry != NULL && !geometry->isValid()) {
      geometry = NULL;
    }

    return geometry;
  }

  /**
   * This method returns a rectangle from the vertices set by the
   * RubberBandTool. It calculates the upper left and bottom right
   * points for the rectangle and properly initializes a QRect
   * object with these vertices. If the RubberBandTool is in the
   * incorrect mode, or the rectangle is not valid it will return
   * an error message.
   *
   *
   * @return QRect The QRect the user selected on the viewport in
   *         pixels
   */
  QRect RubberBandTool::rectangle() {
    QRect rect;

    if(currentMode() == RectangleMode && figureValid()) {
      QList<QPoint> verticesList = vertices();

      //Check the corners for upper left and lower right.
      int x1, x2, y1, y2;
      //Point 1 is in the left, make point1.x upperleft.x
      if(verticesList[0].x() < verticesList[2].x()) {
        x1 = verticesList[0].x();
        x2 = verticesList[2].x();
      }
      //Otherwise Point1 is in the right, make point1.x lowerright.x
      else {
        x1 = verticesList[2].x();
        x2 = verticesList[0].x();
      }

      //Point 1 is in the top, make point1.y upperleft.y
      if(verticesList[0].y() < verticesList[2].y()) {
        y1 = verticesList[0].y();
        y2 = verticesList[2].y();
      }
      //Otherwise Point1 is in the bottom, make point1.y lowerright.y
      else {
        y1 = verticesList[2].y();
        y2 = verticesList[0].y();
      }

      rect = QRect(x1, y1, x2 - x1, y2 - y1);
    }
    //RectangleMode is not valid, or RubberBandTool is in the wrong mode, return error
    else {
      QMessageBox::information((QWidget *)parent(),
                               "Error", "**PROGRAMMER ERROR** Invalid RectangleMode");
    }

    return rect;
  }

  /**
   * This method returns the mouse button  modifier
   *
   * @return MouseButton Mouse button modifier on last release
   */
  Qt::MouseButton RubberBandTool::mouseButton() {
    return p_mouseButton;
  }

  bool RubberBandTool::isValid() {
    return figureComplete() && figureValid();
  }


  bool RubberBandTool::figureComplete() {
    bool complete = false;

    switch(p_bandingMode) {
      case AngleMode:
        complete = (p_vertices->size() == 2 && p_tracking) || (p_vertices->size() == 3);
        break;
      case LineMode:
        complete = (p_vertices->size() == 1 && p_tracking) || (p_vertices->size() == 2);
        break;

      case RectangleMode:
        complete = (p_vertices->size() == 1 && p_tracking) || (p_vertices->size() == 4);
        break;

      case RotatedRectangleMode:
        complete = (p_vertices->size() == 2 && p_tracking) || (p_vertices->size() == 4);
        break;

      case CircleMode:
      case EllipseMode:
        complete = (p_vertices->size() == 1 && p_tracking) || (p_vertices->size() == 2);
        break;

      case SegmentedLineMode:
        complete = (p_vertices->size() > 1 && !p_tracking) || (p_vertices->size() > 0);
        break;

      case PolygonMode:
        complete = (p_vertices->size() > 2 && !p_tracking);
        break;
    }
    return complete;
  }

  bool RubberBandTool::figureValid() {
    if(!figureComplete())
      return false;
    bool valid = true;
    QList<QPoint> verticesList = vertices();

    switch(p_bandingMode) {
      case AngleMode: {
          // Just make sure the vertex and an angle side don't lie on the same point
          // No point tolerance allowed
          valid = verticesList[0].x() != verticesList[1].x() || verticesList[0].y() != verticesList[1].y();
          valid &= verticesList[2].x() != verticesList[1].x() || verticesList[2].y() != verticesList[1].y();
        }
        break;
      case LineMode: {
          // Just make sure the line doesnt start/end at same point if point not allowed
          if(p_pointTolerance == 0) {
            valid = verticesList[0].x() != verticesList[1].x() || verticesList[0].y() != verticesList[1].y();
          }
        }
        break;

      case RectangleMode: {
          // Make sure there's a height and width if point not allowed
          if(p_pointTolerance == 0) {
            valid = verticesList[0].x() != verticesList[2].x() && verticesList[0].y() != verticesList[2].y();
          }
        }
        break;

      case RotatedRectangleMode: {
          // Make sure there's a height and width once again, point tolerance not allowed
          valid = verticesList[0].x() != verticesList[1].x() || verticesList[0].y() != verticesList[1].y();
          valid &= verticesList[1].x() != verticesList[2].x() || verticesList[1].y() != verticesList[2].y();
        }
        break;

      case CircleMode:
      case EllipseMode: {
          // Make sure there's a height and width, point tolerance not allowed
          valid = verticesList[0].x() != verticesList[1].x() && verticesList[0].y() != verticesList[1].y();
        }
        break;

      case SegmentedLineMode: {
          valid = verticesList.size() > 1;
        }
        break;

      case PolygonMode: {
          // For polygons, we must revert back to using geos
          geos::geom::Geometry *poly = geometry();
          valid = poly && poly->isValid();
          delete poly;
        }
        break;
    }

    return valid;
  }

  void RubberBandTool::enablePoints(unsigned int pixTolerance) {
    p_pointTolerance = pixTolerance;
  }

  bool RubberBandTool::figureIsPoint() {
    if(!figureValid())
      return false;
    bool isPoint = true;
    QList<QPoint> verticesList = vertices();

    switch(p_bandingMode) {
      case AngleMode:
      case RotatedRectangleMode:
      case CircleMode:
      case EllipseMode:
      case PolygonMode:
      case SegmentedLineMode:
        isPoint = false;
        break;

      case LineMode: {
          isPoint = (abs(verticesList[0].x() - verticesList[1].x()) +
                     abs(verticesList[0].y() - verticesList[1].y()) < (int)p_pointTolerance);
        }
        break;

      case RectangleMode: {
          isPoint = (abs(verticesList[0].x() - verticesList[2].x()) +
                     abs(verticesList[0].y() - verticesList[2].y()) < (int)p_pointTolerance);
        }
        break;
    }

    return isPoint;
  }


  //! clears the rubber band!
  void RubberBandTool::clear() {
    reset();
    repaint();
  }

  
  RubberBandTool::RubberBandMode RubberBandTool::currentMode() {
    return p_bandingMode;
  };


  double RubberBandTool::area() {
    return 0.0; //<! Returns the area of the figure
  }


  void RubberBandTool::enableAllClicks() {
    p_allClicks = true;
  }


  void RubberBandTool::escapeKeyPress() {
    reset();
  }


  void RubberBandTool::scaleChanged() {
    reset();
  }


}
