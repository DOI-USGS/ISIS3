#include "GraphicsView.h"

#include <QResizeEvent>
#include <QSize>

namespace Isis {
  void GraphicsView::resizeEvent(QResizeEvent *event) {
    if(event->oldSize().isEmpty()) {
      QRectF sceneRect(scene()->itemsBoundingRect());
      fitInView(sceneRect, Qt::KeepAspectRatio);
    }
  }
}

