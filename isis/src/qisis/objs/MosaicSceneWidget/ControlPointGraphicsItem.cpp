#include "ControlPointGraphicsItem.h"

#include <float.h>
#include <iostream>

#include <QGraphicsScene>

#include "ControlPoint.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"

using namespace std;

namespace Isis {
  ControlPointGraphicsItem::ControlPointGraphicsItem(QPointF center,
      ControlPoint *cp, MosaicSceneWidget *boundingRectSrc,
      QGraphicsItem *parent) : QGraphicsRectItem(parent) {
    p_centerPoint = new QPointF(center);
    p_mosaicScene = boundingRectSrc;
    p_controlPoint = cp;
    setZValue(DBL_MAX);

    if(cp->IsIgnored())
      setPen(QPen(Qt::red));
    else if(cp->GetType() == ControlPoint::Ground)
      setPen(QPen(Qt::green));
//     else if(cp->GetType() == ControlPoint::Constrained)
//       setPen(QPen(Qt::darkGreen));
    else // Tie
      setPen(QPen(Qt::blue));

    setBrush(Qt::NoBrush);

    QString toolTip = "Point ID: " +
        QString::fromStdString(p_controlPoint->GetId());
    toolTip += "\nPoint Type: " +
        QString::fromStdString(p_controlPoint->GetPointTypeString());
    toolTip += "\nNumber of Measures: ";
    toolTip += QString::number(p_controlPoint->GetNumMeasures());
    toolTip += "\n";
    toolTip += QStringList(p_controlPoint->GetCubeSerialNumbers()).join("\n");

    setToolTip(toolTip);
  }


  ControlPointGraphicsItem::~ControlPointGraphicsItem() {
    if(p_centerPoint) {
      delete p_centerPoint;
      p_centerPoint = NULL;
    }

    p_mosaicScene = NULL;
  }


  QRectF ControlPointGraphicsItem::boundingRect() const {
    return calcRect();
  }


  void ControlPointGraphicsItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *style,  QWidget * widget) {
    QRectF pointRect = calcRect();

    if(pointRect.isNull())
      return;

    if(rect() != pointRect) {
      setRect(pointRect);
    }
    else {
      painter->setPen(pen());
      painter->setBrush(brush());

      QPointF center = rect().center();

      // This is the source of the CP zoom in draw bug I'm fairly sure...
      painter->drawLine((int)rect().left(), (int)center.y(),
                        (int)rect().right(), (int)center.y());
      painter->drawLine((int)center.x(), (int)rect().top(),
                        (int)center.x(), (int)rect().bottom());
    }
  }


  QRectF ControlPointGraphicsItem::calcRect() const {
    QRectF pointRect;

    if(p_centerPoint && !p_centerPoint->isNull() && p_mosaicScene) {
      static const int size = 12;
      QPoint findSpotScreen =
          p_mosaicScene->getView()->mapFromScene(*p_centerPoint);
      QPoint findSpotTopLeftScreen =
          findSpotScreen - QPoint(size / 2, size / 2);

      QRect pointRectScreen(findSpotTopLeftScreen, QSize(size, size));
      pointRect =
          p_mosaicScene->getView()->mapToScene(pointRectScreen).boundingRect();
    }

    return pointRect;
  }
}

