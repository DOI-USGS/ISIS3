#include "ViewportMdiSubWindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>

#include "CubeViewport.h"
#include "MdiCubeViewport.h"

namespace Isis {
  ViewportMdiSubWindow::ViewportMdiSubWindow(Cube *cubeToView, QWidget *parent)
      : QMdiSubWindow(parent) {
    m_viewport = NULL;

    setWidget(new QWidget());

    widget()->setLayout(new QVBoxLayout());

    m_viewport = new MdiCubeViewport(cubeToView, NULL, widget());
    widget()->layout()->addWidget(m_viewport);

    QProgressBar *progressBar = new QProgressBar;
    progressBar->setRange(0, 100);
    progressBar->setValue(0);

    connect(m_viewport, SIGNAL(progressChanged(int)),
            progressBar, SLOT(setValue(int)));
    connect(m_viewport, SIGNAL(progressComplete()), progressBar, SLOT(hide()));
    connect(this, SIGNAL(closeViewport(CubeViewport *)),
            m_viewport, SIGNAL(viewportClosed(CubeViewport *)));
    widget()->layout()->addWidget(progressBar);

    setOption(QMdiSubWindow::RubberBandResize, true);
    setOption(QMdiSubWindow::RubberBandMove, true);
  }


  ViewportMdiSubWindow::~ViewportMdiSubWindow() {
    m_viewport = NULL;
  }


  MdiCubeViewport *ViewportMdiSubWindow::viewport() {
    return m_viewport;
  }


  void ViewportMdiSubWindow::closeEvent(QCloseEvent *e) {
    if (!m_viewport->confirmClose()) {
      e->ignore();
    }
    else {
      e->accept();
      QMdiSubWindow::closeEvent(e);
      emit closeViewport(m_viewport);
    }
  }
}
