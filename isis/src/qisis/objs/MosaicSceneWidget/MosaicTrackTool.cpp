#include "MosaicTrackTool.h"

#include <QLabel>
#include <QStatusBar>

#include "MosaicSceneWidget.h"
#include "Projection.h"
#include "TProjection.h"

namespace Isis {
  /**
   * MosaicTrackTool constructor
   *
   *
   * @param parent
   */
  MosaicTrackTool::MosaicTrackTool(MosaicSceneWidget *scene,
                                   QStatusBar *status) : MosaicTool(scene) {
    p_sbar = status;

    p_latLabel = new QLabel(getWidget());
    p_latLabel->setText("");
    p_latLabel->setMinimumSize(p_latLabel->sizeHint());
    p_latLabel->setToolTip("Latitude");
    p_sbar->addPermanentWidget(p_latLabel);
    connect(p_latLabel, SIGNAL(destroyed(QObject *)),
            this, SLOT(labelDestroyed(QObject *)));

    p_lonLabel = new QLabel(getWidget());
    p_lonLabel->setText("");
    p_lonLabel->setMinimumSize(p_lonLabel->sizeHint());
    p_lonLabel->setToolTip("Longitude");
    p_sbar->addPermanentWidget(p_lonLabel);
    connect(p_lonLabel, SIGNAL(destroyed(QObject *)),
            this, SLOT(labelDestroyed(QObject *)));

    p_xLabel = new QLabel(getWidget());
    p_xLabel->setText("X");
    p_xLabel->setMinimumSize(p_lonLabel->sizeHint());
    p_xLabel->setToolTip("Longitude");
    p_sbar->addPermanentWidget(p_xLabel);
    connect(p_xLabel, SIGNAL(destroyed(QObject *)),
            this, SLOT(labelDestroyed(QObject *)));

    p_yLabel = new QLabel(getWidget());
    p_yLabel->setText("Y");
    p_yLabel->setMinimumSize(p_lonLabel->sizeHint());
    p_yLabel->setToolTip("Longitude");
    p_sbar->addPermanentWidget(p_yLabel);
    connect(p_yLabel, SIGNAL(destroyed(QObject *)),
            this, SLOT(labelDestroyed(QObject *)));

    clearLabels();
  }


  MosaicTrackTool::~MosaicTrackTool() {
    if(p_latLabel) {
      p_latLabel->deleteLater();
    }

    if(p_lonLabel) {
      p_lonLabel->deleteLater();
    }

    if(p_xLabel) {
      p_xLabel->deleteLater();
    }

    if(p_yLabel) {
      p_yLabel->deleteLater();
    }
  }


  void MosaicTrackTool::mouseMove(QPointF p) {
    updateLabels(p);
  }


  QAction * MosaicTrackTool::getPrimaryAction() {
    return NULL;
  }


  /**
   * Clears the labels if the mouse leaves the application.
   *
   */
  void MosaicTrackTool::mouseLeave() {
    clearLabels();
  }


  void MosaicTrackTool::labelDestroyed(QObject *obj) {
    if(p_latLabel == obj) {
      p_latLabel = NULL;
    }

    if(p_lonLabel == obj) {
      p_lonLabel = NULL;
    }

    if(p_xLabel == obj) {
      p_xLabel = NULL;
    }

    if(p_yLabel == obj) {
      p_yLabel = NULL;
    }
  }


  /**
   * Updates the tracking labels.
   * Displays the Planetocentric Latitude and 360 Positive East
   * Longitude in the lower right corner of the window.
   *
   *
   * @param p
   */
  void MosaicTrackTool::updateLabels(QPointF p) {
    //----------------------------------------------------------------------
    // we need to find out if the point given is over an item, if not, call
    // clearLables() if so, then we need to get the item and figure out the
    // lat/lon that corresponds with the given point.
    //----------------------------------------------------------------------
    Projection *proj = getWidget()->getProjection();
    TProjection *tproj = (TProjection *) proj;

    if (!proj || proj->projectionType() != Projection::Triaxial) {
      clearLabels();
      return;
    }

    proj->SetCoordinate(p.x(), -1 * p.y());

    if(p_lonLabel) {
      p_lonLabel->setVisible(true);
      p_lonLabel->setText("LON " + QString::number(tproj->Longitude()));
    }

    if(p_latLabel) {
      p_latLabel->setVisible(true);
      p_latLabel->setText("LAT " + QString::number(tproj->Latitude()));
    }

    if(p_xLabel) {
      p_xLabel->setVisible(true);
      p_xLabel->setText("X " + QString::number(p.x()));
    }

    if(p_yLabel) {
      p_yLabel->setVisible(true);
      p_yLabel->setText("Y " + QString::number(-1 * p.y()));
    }
  }


  /**
   * Clears the labels.
   *
   */
  void MosaicTrackTool::clearLabels() {
    if(p_lonLabel)
      p_lonLabel->setVisible(false);

    if(p_latLabel)
      p_latLabel->setVisible(false);

    if(p_xLabel)
      p_xLabel->setVisible(false);

    if(p_yLabel)
      p_yLabel->setVisible(false);
  }

}

