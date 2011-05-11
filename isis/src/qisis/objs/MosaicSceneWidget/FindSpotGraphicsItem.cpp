#include "FindSpotGraphicsItem.h"

#include <float.h>
#include <iostream>

#include <QGraphicsScene> 

#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"

using namespace std;

namespace Isis {
  FindSpotGraphicsItem::FindSpotGraphicsItem(QPointF center,
                           MosaicSceneWidget *boundingRectSrc) {
    p_centerPoint = new QPointF(center);
    p_mosaicScene = boundingRectSrc;
    setZValue(DBL_MAX);

    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::red, Qt::SolidPattern));
    setRect(calcRect());
  }


  FindSpotGraphicsItem::~FindSpotGraphicsItem() {
    if(p_centerPoint) {
      delete p_centerPoint;
      p_centerPoint = NULL;
    }
  }


  void FindSpotGraphicsItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *style,  QWidget * widget) {
    QRectF findRect = calcRect();

    if(rect() != findRect)
      setRect(findRect);
    else
      QGraphicsEllipseItem::paint(painter, style, widget);
  }


  QRectF FindSpotGraphicsItem::calcRect() const {
    QRectF findRect;

    if(!p_centerPoint->isNull()) {
      static const int size = 8;
      QPoint findSpotScreen =
          p_mosaicScene->getView()->mapFromScene(*p_centerPoint);
      QPoint findSpotTopLeftScreen =
          findSpotScreen - QPoint(size / 2, size / 2);

      QRect findRectScreen(findSpotTopLeftScreen, QSize(size, size));
      findRect =
          p_mosaicScene->getView()->mapToScene(findRectScreen).boundingRect();
    }

    return findRect;
  }
}

