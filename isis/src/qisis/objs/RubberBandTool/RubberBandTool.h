#ifndef RubberBandTool_h
#define RubberBandTool_h

#include "Tool.h"

#include <QPointer>

class QRect;
class QPoint;

template <typename A> class QList;

namespace geos {
  namespace geom {
    class Geometry;
  }
}


namespace Isis {
  class Angle;
  class MdiCubeViewport;

  /**
  * @brief Rubber banding tool
  *
  * @ingroup Visualization Tools
  *
  * @author 2007-09-11 Steven Lambright
  *
  * @internal
  *   @history 2007-09-11 Steven Lambright Original version
  *   @history 2008-01-03 Steven Lambright bug fix on the polygons
  *   @history 2008-05-23 Noah Hilt added getRectangle method
  *   @history 2008-08-18 Steven Koechle updated to work with Geos 3.0.0
  *   @history 2008-09-26 Steven Lambright Added Segmented line
  *   @history 2010-05-24 Eric Hyer - Added clear() method
  *   @history 2010-06-03 Eric Hyer - Fixed bug in getInstance method
  *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport
  *   @history 2010-11-08 Eric Hyer - Added mouse snapping
  *   @history 2011-09-20 Steven Lambright - Segmented lines now have updates
  *                           while the user is still drawing them instead of
  *                           just a complete state.
  *   @history 2012-02-08 Tracie Sucharski - Added method to set drawing on the
  *                           active viewport only.
  *   @history 2012-09-18 Steven Lambright - This is no longer a singleton, which prevented it from
  *                           working with multiple workspaces. Brought method naming closer to
  *                           current coding standards.
  */
  class RubberBandTool : public Tool {
      Q_OBJECT

    public:
      RubberBandTool(QWidget *parent = NULL);
      virtual ~RubberBandTool();

      enum RubberBandMode {
        AngleMode,            //<! Measure an angle
        CircleMode,           //<! Draw a perfect circle
        EllipseMode,          //<! Draw an ellipse (oval)
        LineMode,             //<! Draw a simple line
        RectangleMode,     //<! Draw a rectangle without any rotation (perfectly horizonal/verticle)
        RotatedRectangleMode, //<! Draw a rotatable rectangle
        PolygonMode,          //<! Draw any closed shape
        SegmentedLineMode     //<! Draw any open shape
      };

      void enable(RubberBandMode mode, bool showIndicatorColors = false);

      void disable();

      void setDrawActiveViewportOnly(bool activeOnly = false);

      QList<QPoint> vertices();

      RubberBandMode currentMode();
      double area();
      Angle angle();

      geos::geom::Geometry *geometry();
      QRect rectangle();
      Qt::MouseButton mouseButton();

      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

      //! This returns true if we can return complete & valid data.
      bool isValid();
      bool figureComplete();
      bool figureValid();

      bool figureIsPoint();

      void enablePoints(unsigned int pixTolerance = 2);
      void enableAllClicks();

      void clear();

    public slots:
      void escapeKeyPress();

    signals:
      void modeChanged();
      void bandingComplete();
      void measureChange();

    protected:
      void enableRubberBandTool() {}
      void scaleChanged();

    protected slots:
      void mouseMove(QPoint p, Qt::MouseButton);
      void mouseDoubleClick(QPoint p);
      void mouseButtonPress(QPoint p, Qt::MouseButton s);
      void mouseButtonRelease(QPoint p, Qt::MouseButton s);

    private:
      QPoint snapMouse(QPoint);

      void repaint();
      void paintVerticesConnected(QPainter *painter);
      void paintRectangle(QPoint upperLeft, QPoint lowerRight, QPainter *painter);
      void paintRectangle(QPoint upperLeft, QPoint upperRight,
                          QPoint lowerLeft, QPoint lowerRight, QPainter *painter);

      // This is used for rotated rectangle
      void calcRectCorners(QPoint corner1, QPoint corner2, QPoint &corner3, QPoint &corner4);

      void reset();                  //<! This resets the member variables

      bool p_mouseDown;              //<! True if the mouse is pressed
      bool p_doubleClicking;         //<! True if on second click of a double click
      bool p_tracking;               //<! True if painting on mouse move
      bool p_allClicks;              //<! Enables all mouse buttons for banding
      bool p_drawActiveOnly;         //<! True if draw on active viewport only
      RubberBandMode p_bandingMode;  //<! Current type of rubber band
      QList<QPoint> * p_vertices;      //<! Known vertices pertaining to the current rubber band
      QPoint *p_mouseLoc;              //<! Current mouse location, only valid of p_tracking
      Qt::MouseButton p_mouseButton; //<! Last mouse button status
      bool p_indicatorColors;        //<! Color the first side of objects a different color, if it's significant
      unsigned int p_pointTolerance; //<! Tolerance for points (zero for no points)
  };
};

#endif

