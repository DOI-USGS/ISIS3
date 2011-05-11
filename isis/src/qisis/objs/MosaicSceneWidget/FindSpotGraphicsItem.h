#ifndef FindSpotGraphicsItem_h
#define FindSpotGraphicsItem_h

#include <QGraphicsEllipseItem>

class QPointF;

namespace Isis {
  class MosaicSceneWidget;

  /**
   * @brief The visual display of the find point
   *
   * @author 2011-05-07 Steven Lambright
   *
   * @internal
   *   @history 2011-05-10 Steven Lambright - Fixed problem with boundingRect()
   */
  class FindSpotGraphicsItem : public QGraphicsEllipseItem {
    public:
      FindSpotGraphicsItem(QPointF center,
                           MosaicSceneWidget *boundingRectSrc);
      virtual ~FindSpotGraphicsItem();

      void paint(QPainter *, const QStyleOptionGraphicsItem *,
                 QWidget * widget = 0);
    private:
      QRectF calcRect() const;
      QPointF *m_centerPoint;
      MosaicSceneWidget *m_mosaicScene;
  };
}

#endif

