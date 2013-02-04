#include "ControlNetGraphicsItem.h"

#include <float.h>
#include <iostream>

#include <QGraphicsScene>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlPointGraphicsItem.h"
#include "FileName.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "ProgressBar.h"
#include "Projection.h"
#include "Pvl.h"
#include "SerialNumberList.h"
#include "SurfacePoint.h"
#include "UniversalGroundMap.h"

using namespace std;

namespace Isis {
  ControlNetGraphicsItem::ControlNetGraphicsItem(ControlNet *controlNet,
       MosaicSceneWidget *mosaicScene) : QGraphicsObject() {
    m_controlNet = controlNet;
    m_mosaicScene = mosaicScene;
    m_pointToScene = new QMap<ControlPoint *, QPair<QPointF, QPointF> >;
    m_cubeToGroundMap = new QMap<QString, UniversalGroundMap *>;
    m_serialNumbers = NULL;
    mosaicScene->getScene()->addItem(this);

    buildChildren();

    connect(mosaicScene, SIGNAL(projectionChanged(Projection *)),
            this, SLOT(buildChildren()));
    connect(mosaicScene, SIGNAL(cubesChanged()),
            this, SLOT(buildChildren()));

    setZValue(DBL_MAX);
  }


  ControlNetGraphicsItem::~ControlNetGraphicsItem() {
    if(m_pointToScene) {
      delete m_pointToScene;
      m_pointToScene = NULL;
    }

    if(m_cubeToGroundMap) {
      QMapIterator<QString, UniversalGroundMap *> it(*m_cubeToGroundMap);

      while(it.hasNext()) {
        it.next();

        if(it.value())
          delete it.value();
      }

      delete m_cubeToGroundMap;
      m_cubeToGroundMap = NULL;
    }
  }


  QRectF ControlNetGraphicsItem::boundingRect() const {
    return QRectF();
  }


  void ControlNetGraphicsItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *style,  QWidget * widget) {
  }


  QPair<QPointF, QPointF> ControlNetGraphicsItem::pointToScene(ControlPoint *cp)
      {
    Projection *proj = m_mosaicScene->getProjection();

    QPointF initial;
    QPointF adjusted;

    QPointF initialLatLon;
    QPointF adjustedLatLon;

    QPair<QPointF, QPointF> rememberedLoc = (*m_pointToScene)[cp];

    if(!rememberedLoc.second.isNull()) {
      proj->SetUniversalGround(rememberedLoc.second.y(),
                               rememberedLoc.second.x());
      adjusted = QPointF(proj->XCoord(), -1 * proj->YCoord());
      adjustedLatLon = rememberedLoc.second;

      if(!rememberedLoc.first.isNull()) {
        proj->SetUniversalGround(rememberedLoc.first.y(),
                                 rememberedLoc.first.x());
        initial = QPointF(proj->XCoord(), -1 * proj->YCoord());
        initialLatLon = rememberedLoc.first;
      }
    }
    else if(proj) {

      SurfacePoint adjSurfacePoint(cp->GetAdjustedSurfacePoint());
      if(adjSurfacePoint.Valid()) {
        if(proj->SetUniversalGround(adjSurfacePoint.GetLatitude().degrees(),
                                 adjSurfacePoint.GetLongitude().degrees())) {
          adjusted = QPointF(proj->XCoord(), -1 * proj->YCoord());
          adjustedLatLon = QPointF(adjSurfacePoint.GetLongitude().degrees(),
              adjSurfacePoint.GetLatitude().degrees());
        }
      }

      SurfacePoint apriSurfacePoint(cp->GetAprioriSurfacePoint());
      if(apriSurfacePoint.Valid()) {
        if(proj->SetUniversalGround(apriSurfacePoint.GetLatitude().degrees(),
            apriSurfacePoint.GetLongitude().degrees())) {
          initial = QPointF(proj->XCoord(), -1 * proj->YCoord());
          initialLatLon = QPointF(apriSurfacePoint.GetLongitude().degrees(),
              apriSurfacePoint.GetLatitude().degrees());
        }
      }

      // If we have adjusted and not apriori then find camera
      //    OR if we don't have an adjusted and don't have an initial we still
      //       need an initial
      if((!adjusted.isNull() && initial.isNull()) ||
         (adjusted.isNull() && initial.isNull())) {
        try {
          QString sn = cp->GetReferenceSN();
          QString filename = snToFileName(sn);

          if(filename.size() > 0) {
            if((*m_cubeToGroundMap)[filename] == NULL) {
              Pvl label(FileName(filename).expanded());
              UniversalGroundMap *groundMap = new UniversalGroundMap(label);
              (*m_cubeToGroundMap)[filename] = groundMap;
            }

            if((*m_cubeToGroundMap)[filename]->SetImage(
                  cp->GetRefMeasure()->GetSample(),
                  cp->GetRefMeasure()->GetLine())) {
              double lat = (*m_cubeToGroundMap)[filename]->UniversalLatitude();
              double lon = (*m_cubeToGroundMap)[filename]->UniversalLongitude();

              if(proj->SetUniversalGround(lat, lon)) {
                initial = QPointF(proj->XCoord(), -1 * proj->YCoord());
                initialLatLon = QPointF(lon, lat);
              }
            }
          }
        }
        catch(IException &) {
        }
      }
    }

    QPair<QPointF, QPointF> result;
    QPair<QPointF, QPointF> latLonResult;
    if(!adjusted.isNull() && adjusted != initial) {
      result.second = adjusted;
      result.first = initial;
      latLonResult.second = adjustedLatLon;
      latLonResult.first = initialLatLon;
    }
    else {
      result.second = initial;
      latLonResult.second = initialLatLon;
    }

    (*m_pointToScene)[cp] = latLonResult;

    return result;
  }


  QString ControlNetGraphicsItem::snToFileName(QString sn) {
    QString result;

    if(m_serialNumbers && m_serialNumbers->Size()) {
      try {
        result = m_serialNumbers->FileName(sn);
      }
      catch(IException &) {
      }
    }

    return result;
  }


  /**
   * Enable/disable and configure movement arrows for all CP displays in the network.
   *
   * @see ControlPointGraphicsItem::setArrowsVisible()
   * @see MosaicControlNetTool::setMovementArrowColorSource()
   */
  void ControlNetGraphicsItem::setArrowsVisible(bool visible,
      bool colorByMeasureCount, int maxMeasureCount,
      bool colorByJigsawError, double maxResidualMagnitude) {

    foreach(QGraphicsItem *child, childItems()) {
      ((ControlPointGraphicsItem *)child)->setArrowVisible(
          visible, colorByMeasureCount, maxMeasureCount, colorByJigsawError, maxResidualMagnitude);
    }
  }


  /**
   * Call this to re-calculate where control points ought to lie.
   *
   * This creates a new cube list and re-projects everything
   */
  void ControlNetGraphicsItem::buildChildren() {
    QList<QGraphicsItem *> children = childItems();
    QGraphicsItem *child;
    foreach(child, children) {
      if(child->scene())
        child->scene()->removeItem(child);

      delete child;
      child = NULL;
    }

    if(m_controlNet) {
      const int numCp = m_controlNet->GetNumPoints();

      if(m_serialNumbers) {
        delete m_serialNumbers;
      }

      m_serialNumbers = new SerialNumberList;

      QStringList cubeFiles(m_mosaicScene->cubeFileNames());

      QString filename;
      foreach(filename, cubeFiles) {
        try {
          m_serialNumbers->Add(filename);
        }
        catch(IException &) {
        }
      }

      ProgressBar *p = (ProgressBar *)m_mosaicScene->getProgress();
      p->setText("Calculating CP Locations");
      p->setRange(0, numCp - 1);
      p->setValue(0);
      p->setVisible(true);

      for(int cpIndex = 0; cpIndex < numCp; cpIndex ++) {
        ControlPoint *cp = m_controlNet->GetPoint(cpIndex);

        // Initial, Final
        QPair<QPointF, QPointF> scenePoints = pointToScene(cp);

        new ControlPointGraphicsItem( scenePoints.second, scenePoints.first,
            cp, m_serialNumbers, m_mosaicScene, this);

        p->setValue(cpIndex);
      }

      p->setVisible(false);
    }
  }
}
