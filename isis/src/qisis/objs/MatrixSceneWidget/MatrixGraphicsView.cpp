#include "MatrixGraphicsView.h"

#include <iostream>

#include <QResizeEvent>
#include <QScrollBar>

namespace Isis {
  MatrixGraphicsView::MatrixGraphicsView(QGraphicsScene *scene,
      QWidget *parent) : QGraphicsView(scene, parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    p_resizeZooming = true;

//     setRenderHint(QPainter::SmoothPixmapTransform,false);
//     setRenderHint(QPainter::HighQualityAntialiasing,false);
//     setRenderHint(QPainter::NonCosmeticDefaultPen,true);
//     setOptimizationFlag(QGraphicsView::DontSavePainterState);
    setCacheMode(QGraphicsView::CacheBackground);
  }

  MatrixGraphicsView::~MatrixGraphicsView() {
  }


  void MatrixGraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    QGraphicsView::contextMenuEvent(event);
  }


  void MatrixGraphicsView::resizeEvent(QResizeEvent *event) {
    if(event->oldSize().isEmpty() || p_resizeZooming) {
      QRectF sceneRect(scene()->itemsBoundingRect());
      fitInView(sceneRect, Qt::KeepAspectRatio);
    }

    QGraphicsView::resizeEvent(event);
  }
}

