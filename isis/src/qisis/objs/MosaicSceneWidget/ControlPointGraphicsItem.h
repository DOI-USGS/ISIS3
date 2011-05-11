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
   *   @history 2011-05-09 Steven Lambright - Fixed known issue with paint()
   *                       when zoomed in.
   *   @history 2011-05-10 Steven Lambright - Added arrow capabilities, fixed
   *                       problem with boundingRect() that seemed to cause a
   *                       crash.
   */
  class ControlPointGraphicsItem : public QGraphicsRectItem {
    public:
      ControlPointGraphicsItem(QPointF center, QPointF apriori,
          ControlPoint *cp, SerialNumberList *snList, MosaicSceneWidget *scene,
          QGraphicsItem *parent);
      virtual ~ControlPointGraphicsItem();

      void paint(QPainter *, const QStyleOptionGraphicsItem *,
                 QWidget * widget = 0);

      void setArrowVisible(bool visible) {
        m_showArrow = visible;
        setRect(calcRect());
      }

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
  };
}

#endif

