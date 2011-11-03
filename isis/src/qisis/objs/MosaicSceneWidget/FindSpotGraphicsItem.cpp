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
    m_centerPoint = new QPointF(center);
    m_mosaicScene = boundingRectSrc;
    setZValue(DBL_MAX);

    setPen(Qt::NoPen);
    setBrush(QBrush(Qt::red, Qt::SolidPattern));
    setRect(calcRect());
  }


  FindSpotGraphicsItem::~FindSpotGraphicsItem() {
    delete m_centerPoint;
    m_centerPoint = NULL;
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

    if(!m_centerPoint->isNull()) {
      static const int size = 8;
      QPoint findSpotScreen =
          m_mosaicScene->getView()->mapFromScene(*m_centerPoint);
      QPoint findSpotTopLeftScreen =
          findSpotScreen - QPoint(size / 2, size / 2);

      QRect findRectScreen(findSpotTopLeftScreen, QSize(size, size));
      findRect =
          m_mosaicScene->getView()->mapToScene(findRectScreen).boundingRect();
    }

    return findRect;
  }
}

