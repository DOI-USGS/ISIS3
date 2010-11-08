#ifndef RubberBandTool_h
#define RubberBandTool_h

#include "Tool.h"

class QRect;
class QPoint;

template <typename A> class QList;

namespace geos {
  namespace geom {
    class Geometry;
  }
}


namespace Qisis {
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
  */
  class RubberBandTool : public Tool {
      Q_OBJECT

    public:
      static RubberBandTool *getInstance(QWidget *parent = NULL);
      virtual ~RubberBandTool();

      enum RubberBandMode {
        Angle,            //<! Measure an angle
        Circle,           //<! Draw a perfect circle
        Ellipse,          //<! Draw an ellipse (oval)
        Line,             //<! Draw a simple line
        Rectangle,        //<! Draw a rectangle without any rotation (perfectly horizonal/verticle)
        RotatedRectangle, //<! Draw a rotatable rectangle
        Polygon,          //<! Draw any closed shape
        SegmentedLine     //<! Draw any open shape
      };

      static void enable(RubberBandMode mode, bool showIndicatorColors = false);
      void enableBanding(RubberBandMode mode, bool showIndicatorColors = false);

      static void disable();
      void disableBanding();

      static QList<QPoint> getVertices();
      QList<QPoint> getFoundVertices();

      static RubberBandMode getMode();
      RubberBandMode getCurrentMode();
      static double getArea();
      double getAreaMeasure();
      static double getAngle();
      double getAngleMeasure(); //<! Returns the angle measurement (in radians)

      static geos::geom::Geometry *geometry();
      geos::geom::Geometry *getGeometry();
      static QRect rectangle();
      QRect getRectangle();
      static Qt::MouseButton mouseButton();
      Qt::MouseButton getMouseButton();
      void paintViewport(MdiCubeViewport *vp, QPainter *painter);

      //<! This returns true if we can return complete & valid data.
      static bool isValid();
      bool figureComplete();
      bool figureValid();

      static bool isPoint();
      bool figureIsPoint();

      static void allowPoints(unsigned int pixTolerance = 2);
      void enablePoints(unsigned int pixTolerance = 2);
      static void allowAllClicks();
      void enableAllClicks();

      static void clear();

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
      RubberBandTool(QWidget *parent);

      void repaint();
      void paintVerticesConnected(QPainter *painter);
      void paintRectangle(QPoint upperLeft, QPoint lowerRight, QPainter *painter);
      void paintRectangle(QPoint upperLeft, QPoint upperRight,
                          QPoint lowerLeft, QPoint lowerRight, QPainter *painter);

      // This is used for rotated rectangle
      void calcRectCorners(QPoint corner1, QPoint corner2, QPoint &corner3, QPoint &corner4);

      static RubberBandTool *p_instance;

      void reset();                  //<! This resets the member variables

      bool p_mouseDown;              //<! True if the mouse is pressed
      bool p_doubleClicking;         //<! True if on second click of a double click
      bool p_tracking;               //<! True if painting on mouse move
      bool p_allClicks;              //<! Enables all mouse buttons for banding
      RubberBandMode p_bandingMode;  //<! Current type of rubber band
      QList<QPoint> * p_vertices;      //<! Known vertices pertaining to the current rubber band
      QPoint *p_mouseLoc;              //<! Current mouse location, only valid of p_tracking
      Qt::MouseButton p_mouseButton; //<! Last mouse button status
      bool p_indicatorColors;        //<! Color the first side of objects a different color, if it's significant
      unsigned int p_pointTolerance; //<! Tolerance for points (zero for no points)
  };
};

#endif

