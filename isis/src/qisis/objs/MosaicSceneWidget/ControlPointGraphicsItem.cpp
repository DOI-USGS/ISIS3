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
      QPointF apriori, ControlPoint *cp, SerialNumberList *snList,
      MosaicSceneWidget *boundingRectSrc, QGraphicsItem *parent) :
      QGraphicsRectItem(parent) {
    p_centerPoint = new QPointF(center);
    p_mosaicScene = boundingRectSrc;
    p_controlPoint = cp;
    setZValue(DBL_MAX);

    p_origPoint = new QPointF(apriori);

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

    setRect(calcRect());
  }


  ControlPointGraphicsItem::~ControlPointGraphicsItem() {
    if(p_centerPoint) {
      delete p_centerPoint;
      p_centerPoint = NULL;
    }

    if(p_origPoint) {
      delete p_origPoint;
      p_origPoint = NULL;
    }

    p_mosaicScene = NULL;
  }


  void ControlPointGraphicsItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *style,  QWidget * widget) {
    QRectF fullRect = calcRect();
    QRectF crosshairRect = calcCrosshairRect();

    if(crosshairRect.isNull())
      return;

    if(rect() != fullRect) {
      setRect(fullRect);
    }
    else {
      painter->setPen(pen());
      painter->setBrush(brush());

      QPointF center = crosshairRect.center();

      QPointF centerLeft(crosshairRect.left(), center.y());
      QPointF centerRight(crosshairRect.right(), center.y());
      QPointF centerTop(center.x(), crosshairRect.top());
      QPointF centerBottom(center.x(), crosshairRect.bottom());

      painter->drawLine(centerLeft, centerRight);
      painter->drawLine(centerTop, centerBottom);

      if(!p_origPoint->isNull() && *p_origPoint != *p_centerPoint) {
        painter->setPen(Qt::black);
        painter->setBrush(Qt::black);
        painter->drawLine(*p_origPoint, *p_centerPoint);

        double crosshairSize = crosshairRect.width() * 4.0 / 5.0;

        // Draw arrow
        QPointF lineVector = *p_centerPoint - *p_origPoint;
        double lineVectorMag = sqrt(lineVector.x() * lineVector.x() +
                                    lineVector.y() * lineVector.y());
        double thetaPointOnLine = crosshairSize / (2 * (tanf(PI / 6) / 2) *
                              lineVectorMag);
        QPointF pointOnLine = *p_centerPoint - thetaPointOnLine * lineVector;

        QPointF normalVector = QPointF(-lineVector.y(), lineVector.x());
        double thetaNormal = crosshairSize / (2 * lineVectorMag);

        QPointF leftPoint = pointOnLine + thetaNormal * normalVector;
        QPointF rightPoint = pointOnLine - thetaNormal * normalVector;

        QPolygonF arrowHead;
        arrowHead << leftPoint << center << rightPoint;
        painter->drawPolygon(arrowHead);
      }
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
    QRectF pointRect(calcCrosshairRect());

    if(!p_origPoint->isNull() && pointRect.isValid()) {
      // Make the rect contain the orig point
      if(pointRect.left() > p_origPoint->x()) {
        pointRect.setLeft(p_origPoint->x());
      }
      else if(pointRect.right() < p_origPoint->x()) {
        pointRect.setRight(p_origPoint->x());
      }

      if(pointRect.top() > p_origPoint->y()) {
        pointRect.setTop(p_origPoint->y());
      }
      else if(pointRect.bottom() < p_origPoint->y()) {
        pointRect.setBottom(p_origPoint->y());
      }
    }

    return pointRect;
  }


  QRectF ControlPointGraphicsItem::calcCrosshairRect() const {
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

