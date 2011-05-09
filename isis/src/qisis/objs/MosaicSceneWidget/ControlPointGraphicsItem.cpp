#include "ControlPointGraphicsItem.h"

#include <float.h>
#include <iostream>

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

#include "ControlPoint.h"
#include "Filename.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "SerialNumberList.h"

using namespace std;

namespace Isis {
  /**
   * Create a CP graphics item. This will colorize and set the appropriate
   *   toolTip for this control point. 
   */
  ControlPointGraphicsItem::ControlPointGraphicsItem(QPointF center,
      ControlPoint *cp, SerialNumberList *snList,
      MosaicSceneWidget *boundingRectSrc, QGraphicsItem *parent) :
      QGraphicsRectItem(parent) {
    p_centerPoint = new QPointF(center);
    p_mosaicScene = boundingRectSrc;
    p_controlPoint = cp;
    setZValue(DBL_MAX);

    if(cp->IsIgnored())
      setPen(QPen(Qt::red));
    else if(cp->GetType() == ControlPoint::Ground)
      setPen(QPen(Qt::green));
    else if(cp->GetType() == ControlPoint::Constrained)
      setPen(QPen(Qt::darkGreen));
    else // Tie
      setPen(QPen(Qt::blue));

    setBrush(Qt::NoBrush);

    setToolTip(makeToolTip(snList));
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

      QPointF center = pointRect.center();

      QPointF centerLeft(pointRect.left(), center.y());
      QPointF centerRight(pointRect.right(), center.y());
      QPointF centerTop(center.x(), pointRect.top());
      QPointF centerBottom(center.x(), pointRect.bottom());

      painter->drawLine(centerLeft, centerRight);
      painter->drawLine(centerTop, centerBottom);
    }
  }


  void ControlPointGraphicsItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent * event) {
    QMenu menu;

    QAction *title = menu.addAction(
        QString::fromStdString(p_controlPoint->GetId()));
    title->setEnabled(false);
    menu.addSeparator();

    QAction *infoAction = menu.addAction("Show Point Info");

    QAction *selected = menu.exec(event->screenPos());

    if(selected == infoAction) {
      QMessageBox::information(p_mosaicScene, "Control Point Information",
          toolTip());
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


  QString ControlPointGraphicsItem::makeToolTip(SerialNumberList *snList) {
    QString toolTip = "Point ID: " +
        QString::fromStdString(p_controlPoint->GetId());
    toolTip += "\nPoint Type: " +
        QString::fromStdString(p_controlPoint->GetPointTypeString());
    toolTip += "\nNumber of Measures: ";
    toolTip += QString::number(p_controlPoint->GetNumMeasures());
    toolTip += "\n";

    if(snList == NULL) {
      toolTip += QStringList(p_controlPoint->GetCubeSerialNumbers()).join("\n");
    }
    else {
      QStringList serialNums(p_controlPoint->GetCubeSerialNumbers());

      for(int snIndex = 0; snIndex < serialNums.size(); snIndex ++) {
        QString serialNum = serialNums[snIndex];

        if(snIndex > 0)
          toolTip += "\n";

        if(snList->HasSerialNumber(serialNum)) {
          toolTip +=
              Filename(snList->Filename(serialNum.toStdString())).fileName();
          toolTip += " (" + serialNum + ")";
        }
        else {
          toolTip += serialNum;
        }
      }
    }

    return toolTip;
  }
}

