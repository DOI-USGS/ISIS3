#ifndef ControlPointGraphicsItem_h
#define ControlPointGraphicsItem_h

#include <QGraphicsRectItem>

class QPointF;

namespace Isis {
  class ControlPoint;
  class MosaicSceneWidget;
  class SerialNumberList;

  /**
   * @brief The visual display of a single control point
   *
   * The control point tries to always draw itself at a constant size and
   *   uses the scene pointer to accomplish this.
   *
   * @author 2011-05-07 Steven Lambright
   *
   * @internal
   *   @history 2011-05-09 Steven Lambright - Fixed known issue with paint() when zoomed in.
   *   @history 2011-05-10 Steven Lambright - Added arrow capabilities, fixed problem with 
   *                           boundingRect() that seemed to cause a crash.
   *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point types
   *                           Ground ------> Fixed
   *                           Tie----------> Free
   *   @history 2013-01-02 Steven Lambright - Updated setArrowVisible() to support new coloring
   *                           options. The design of this configuration is wrong/needs fixed, but
   *                           I'm leaving it alone due to time constraints. Updated paint() method
   *                           to appropriately apply colors. Fixes #479.
   *   @history 2016-05-18 Ian Humphrey - Updated the control point crosses to be cosmetic so that
   *                           they always appear on screen (Qt4 to Qt5).
   *   @history 2016-10-20 Tracie Sucharski - Remove obsolete code that was commented out.
   *                           Fixes #4479.
   *                   
   */
  class ControlPointGraphicsItem : public QGraphicsRectItem {
    public:
      ControlPointGraphicsItem(QPointF center, QPointF apriori,
          ControlPoint *cp, SerialNumberList *snList, MosaicSceneWidget *scene,
          QGraphicsItem *parent);
      virtual ~ControlPointGraphicsItem();

      void paint(QPainter *, const QStyleOptionGraphicsItem *,
                 QWidget * widget = 0);

      ControlPoint *controlPoint();

      void setArrowVisible(bool visible, bool colorByMeasureCount, int measureCount,
                           bool colorByResidualMagnitude, double residualMagnitude);

    protected:
      void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

    private:
      QRectF calcRect() const;
      QRectF calcCrosshairRect() const;
      QPolygonF calcArrowHead() const;
      QString makeToolTip(SerialNumberList *snlist);

      QPointF *m_centerPoint;
      QPointF *m_origPoint;
      MosaicSceneWidget *m_mosaicScene;
      ControlPoint *m_controlPoint;
      bool m_showArrow;
      //! Are we coloring the movement arrow based on CP measure count
      bool m_colorByMeasureCount;
      //! Are we coloring the movement arrow based on max CM residual magnitude
      bool m_colorByResidualMagnitude;
      //! Measure count threshold for colored vs. black 
      int m_measureCount;
      //! Residual magnitude threshold for colored vs. black
      double m_residualMagnitude;
  };
}

#endif

