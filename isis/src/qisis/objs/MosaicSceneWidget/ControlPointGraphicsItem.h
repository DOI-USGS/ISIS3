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

    protected:
      void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);

    private:
      QRectF calcRect() const;
      QRectF calcCrosshairRect() const;
      QString makeToolTip(SerialNumberList *snlist);

      QPointF *p_centerPoint;
      QPointF *p_origPoint;
      MosaicSceneWidget *p_mosaicScene;
      ControlPoint *p_controlPoint;
  };
}

#endif

