#include "MosaicGraphicsView.h"

#include <iostream>

#include <QResizeEvent>
#include <QScrollBar>

namespace Isis {
  MosaicGraphicsView::MosaicGraphicsView(QGraphicsScene *scene,
      QWidget *parent) : QGraphicsView(scene, parent) {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    p_resizeZooming = true;

//    setRenderHint(QPainter::SmoothPixmapTransform,false);
//    setRenderHint(QPainter::HighQualityAntialiasing,false);
//    setRenderHint(QPainter::NonCosmeticDefaultPen,true);
//    setOptimizationFlag(QGraphicsView::DontSavePainterState);
//    setRenderHint(QPainter::Qt4CompatiblePainting, true);
    setCacheMode(QGraphicsView::CacheBackground);
  }

  MosaicGraphicsView::~MosaicGraphicsView() {
  }


  void MosaicGraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    QGraphicsView::contextMenuEvent(event);
  }


  void MosaicGraphicsView::resizeEvent(QResizeEvent *event) {
    if(m_oldSize.isEmpty() || p_resizeZooming) {
      QRectF sceneRect(scene()->itemsBoundingRect());
      fitInView(sceneRect, Qt::KeepAspectRatio);
      m_oldSize = event->size();
    }

    QGraphicsView::resizeEvent(event);
  }
}

