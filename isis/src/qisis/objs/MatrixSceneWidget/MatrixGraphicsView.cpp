#include "MatrixGraphicsView.h"

#include <iostream>

#include <QContextMenuEvent>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QScrollBar>

namespace Isis {
  /**
   * Constructor.
   *
   * Constructs the MatrixGraphicsView.
   *
   * @param scene Pointer to the graphics scene to visualize.
   * @param widget Pointer to the parent widget.
   */
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


  /**
   * Destructor.
   */
  MatrixGraphicsView::~MatrixGraphicsView() {
  }


  /**
   * Handles context menu events on the matrix graphics view.
   *
   * @param event Pointer to the context menu event being captured
   */
  void MatrixGraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    QGraphicsView::contextMenuEvent(event);
  }


  /**
   * Handles resize events on the matrix graphics view.
   *
   * @param event Pointer to the resize event being captured.
   */
  void MatrixGraphicsView::resizeEvent(QResizeEvent *event) {
    if (event->oldSize().isEmpty() || p_resizeZooming) {
      QRectF sceneRect(scene()->itemsBoundingRect());
      fitInView(sceneRect, Qt::KeepAspectRatio);
    }

    QGraphicsView::resizeEvent(event);
  }
}

