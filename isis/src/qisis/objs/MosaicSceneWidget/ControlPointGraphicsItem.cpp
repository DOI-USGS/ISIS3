#include "ControlPointGraphicsItem.h"

#include <float.h>
#include <iostream>

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

#include "Constants.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "FileName.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "SerialNumberList.h"
#include "Statistics.h"

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
    m_centerPoint = new QPointF(center);
    m_mosaicScene = boundingRectSrc;
    m_controlPoint = cp;
    m_showArrow = false;
    m_colorByMeasureCount = false;
    m_colorByResidualMagnitude = false;

    m_measureCount = -1;
    m_residualMagnitude = Null;

    setZValue(DBL_MAX);

    m_origPoint = new QPointF(apriori);

    if(cp->IsIgnored())
      setPen(QPen(Qt::red));
    else if(cp->IsEditLocked())
      setPen(QPen(Qt::magenta));
    else if(cp->GetType() == ControlPoint::Fixed)
      setPen(QPen(Qt::green));
    else if(cp->GetType() == ControlPoint::Constrained)
      setPen(QPen(Qt::darkGreen));
    else // Free
      setPen(QPen(Qt::blue));

    setBrush(Qt::NoBrush);

    setToolTip(makeToolTip(snList));

    setRect(calcRect());
  }


  ControlPointGraphicsItem::~ControlPointGraphicsItem() {
    if(m_centerPoint) {
      delete m_centerPoint;
      m_centerPoint = NULL;
    }

    if(m_origPoint) {
      delete m_origPoint;
      m_origPoint = NULL;
    }

    m_mosaicScene = NULL;
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

      if(!m_origPoint->isNull() && *m_origPoint != *m_centerPoint
         && m_showArrow) {

        if (!m_colorByMeasureCount && !m_colorByResidualMagnitude) {
          painter->setPen(Qt::black);
          painter->setBrush(Qt::black);
        }
        else {
          QColor zeroColor(Qt::black);
          QColor fullColor(Qt::green);

          bool isColored = false;

          if (m_colorByMeasureCount) {
            int fullColorMeasureCount = qMax(1, m_measureCount);
            int measureCount = m_controlPoint->getMeasures(true).count();
            isColored = (measureCount >= fullColorMeasureCount);
          }
          else {
            fullColor = QColor(Qt::red);

            double fullColorErrorMag = 0.0;

            if (!IsSpecial(m_residualMagnitude)) {
              fullColorErrorMag = m_residualMagnitude;
            }

            Statistics residualStats;
            foreach (ControlMeasure *cm, m_controlPoint->getMeasures(true)) {
              residualStats.AddData(cm->GetResidualMagnitude());
            }

            if (residualStats.Average() != Null) {
              double errorMag = residualStats.Maximum();
              if (errorMag >= fullColorErrorMag) {
                isColored = true;
              }
            }
          }

          QColor finalColor = isColored? fullColor : zeroColor;

          painter->setPen(finalColor);
          painter->setBrush(finalColor);
        }

        painter->drawLine(*m_origPoint, *m_centerPoint);

        QPolygonF arrowHead = calcArrowHead();
        painter->drawPolygon(arrowHead);
      }
    }
  }


  void ControlPointGraphicsItem::contextMenuEvent(
      QGraphicsSceneContextMenuEvent * event) {
    QMenu menu;

    QAction *title = menu.addAction(
        m_controlPoint->GetId());
    title->setEnabled(false);
    menu.addSeparator();

    QAction *infoAction = menu.addAction("Show Point Info");

    QAction *selected = menu.exec(event->screenPos());

    if(selected == infoAction) {
      QMessageBox::information(m_mosaicScene, "Control Point Information",
          toolTip());
    }
  }


  QRectF ControlPointGraphicsItem::calcRect() const {
    QRectF pointRect(calcCrosshairRect());

    if(!m_origPoint->isNull() && pointRect.isValid()) {
      // Make the rect contain the orig point
      if(pointRect.left() > m_origPoint->x()) {
        pointRect.setLeft(m_origPoint->x());
      }
      else if(pointRect.right() < m_origPoint->x()) {
        pointRect.setRight(m_origPoint->x());
      }

      if(pointRect.top() > m_origPoint->y()) {
        pointRect.setTop(m_origPoint->y());
      }
      else if(pointRect.bottom() < m_origPoint->y()) {
        pointRect.setBottom(m_origPoint->y());
      }
    }

    QPolygonF arrowHead = calcArrowHead();

    if(arrowHead.size() > 2) {
      pointRect = pointRect.united(arrowHead.boundingRect());
    }

    return pointRect;
  }


  QRectF ControlPointGraphicsItem::calcCrosshairRect() const {
    QRectF pointRect;

    if(m_centerPoint && !m_centerPoint->isNull() && m_mosaicScene) {
      static const int size = 12;
      QPoint findSpotScreen =
          m_mosaicScene->getView()->mapFromScene(*m_centerPoint);
      QPoint findSpotTopLeftScreen =
          findSpotScreen - QPoint(size / 2, size / 2);

      QRect pointRectScreen(findSpotTopLeftScreen, QSize(size, size));

      pointRect =
          m_mosaicScene->getView()->mapToScene(pointRectScreen).boundingRect();
    }

    return pointRect;
  }


  QPolygonF ControlPointGraphicsItem::calcArrowHead() const {
    QPolygonF arrowHead;

    if(m_showArrow && !m_origPoint->isNull() &&
       *m_origPoint != *m_centerPoint) {
      QRectF crosshairRect = calcCrosshairRect();
      double headSize = crosshairRect.width() * 4.0 / 5.0;

      double crosshairSize = headSize;

      // Draw arrow
      QPointF lineVector = *m_centerPoint - *m_origPoint;
      double lineVectorMag = sqrt(lineVector.x() * lineVector.x() +
                                  lineVector.y() * lineVector.y());
      double thetaPointOnLine = crosshairSize / (2 * (tanf(PI / 6) / 2) *
                            lineVectorMag);
      QPointF pointOnLine = *m_centerPoint - thetaPointOnLine * lineVector;

      QPointF normalVector = QPointF(-lineVector.y(), lineVector.x());
      double thetaNormal = crosshairSize / (2 * lineVectorMag);

      QPointF leftPoint = pointOnLine + thetaNormal * normalVector;
      QPointF rightPoint = pointOnLine - thetaNormal * normalVector;

      arrowHead << leftPoint << crosshairRect.center() << rightPoint;
    }

    return arrowHead;
  }


  QString ControlPointGraphicsItem::makeToolTip(SerialNumberList *snList) {
    QString toolTip = "<div>Point ID: " +
        m_controlPoint->GetId();
    toolTip += "<br />Point Type: " +
        m_controlPoint->GetPointTypeString();
    toolTip += "<br />Number of Measures: ";
    toolTip += toString(m_controlPoint->GetNumMeasures());
    toolTip += "<br />Ignored: ";
    toolTip += m_controlPoint->IsIgnored() ? "Yes" : "No";
    toolTip += "<br />Edit Locked: ";
    toolTip += m_controlPoint->IsEditLocked() ? "Yes" : "No";

    toolTip += "<br />";

    if(snList == NULL) {
      toolTip += QStringList(m_controlPoint->getCubeSerialNumbers()).join("\n");
    }
    else {
      QStringList serialNums(m_controlPoint->getCubeSerialNumbers());

      for(int snIndex = 0; snIndex < serialNums.size(); snIndex ++) {
        QString serialNum = serialNums[snIndex];

        if(snIndex > 0)
          toolTip += "<br />";

        if(snList->HasSerialNumber(serialNum)) {
          toolTip +=
              FileName(snList->FileName(serialNum)).name();
          toolTip += " (" + serialNum + ")";
        }
        else {
          toolTip += serialNum;
        }

        double residMag = m_controlPoint->GetMeasure(serialNum)->GetResidualMagnitude();
        if (residMag != Null) {
          toolTip += " [residual: <font color='red'>" + toString(residMag) + "</font>]";
        }
      }
    }

    toolTip += "</div>";

    return toolTip;
  }
}

