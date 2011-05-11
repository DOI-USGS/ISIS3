#include "ControlNetGraphicsItem.h"

#include <float.h>
#include <iostream>

#include <QGraphicsScene>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlPointGraphicsItem.h"
#include "Filename.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "ProgressBar.h"
#include "Projection.h"
#include "SerialNumberList.h"
#include "SurfacePoint.h"
#include "UniversalGroundMap.h"

using namespace std;

namespace Isis {
  ControlNetGraphicsItem::ControlNetGraphicsItem(ControlNet *controlNet,
       MosaicSceneWidget *mosaicScene) : QGraphicsObject() {
    p_controlNet = controlNet;
    p_mosaicScene = mosaicScene;
    p_pointToScene = new QMap<ControlPoint *, QPair<QPointF, QPointF> >;
    p_cubeToGroundMap = new QMap<QString, UniversalGroundMap *>;
    p_serialNumbers = NULL;
    mosaicScene->getScene()->addItem(this);

    buildChildren();

    connect(mosaicScene, SIGNAL(projectionChanged(Projection *)),
            this, SLOT(buildChildren()));
    connect(mosaicScene, SIGNAL(cubesChanged()),
            this, SLOT(buildChildren()));

    setZValue(DBL_MAX);
  }


  ControlNetGraphicsItem::~ControlNetGraphicsItem() {
    if(p_pointToScene) {
      delete p_pointToScene;
      p_pointToScene = NULL;
    }

    if(p_cubeToGroundMap) {
      QMapIterator<QString, UniversalGroundMap *> it(*p_cubeToGroundMap);

      while(it.hasNext()) {
        it.next();

        if(it.value())
          delete it.value();
      }

      delete p_cubeToGroundMap;
      p_cubeToGroundMap = NULL;
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
    Projection *proj = p_mosaicScene->getProjection();

    QPointF initial;
    QPointF adjusted;

    QPair<QPointF, QPointF> rememberedLoc = (*p_pointToScene)[cp];

    if(!rememberedLoc.second.isNull()) {
      proj->SetUniversalGround(rememberedLoc.second.y(),
                               rememberedLoc.second.x());
      adjusted = QPointF(proj->XCoord(), -1 * proj->YCoord());

      if(!rememberedLoc.first.isNull()) {
        proj->SetUniversalGround(rememberedLoc.first.y(),
                                 rememberedLoc.first.x());
        initial = QPointF(proj->XCoord(), -1 * proj->YCoord());
      }
    }
    else if(proj) {

      SurfacePoint adjSurfacePoint(cp->GetAdjustedSurfacePoint());
      if(adjSurfacePoint.Valid()) {
        if(proj->SetUniversalGround(adjSurfacePoint.GetLatitude().GetDegrees(),
                                 adjSurfacePoint.GetLongitude().GetDegrees())) {
          adjusted = QPointF(proj->XCoord(), -1 * proj->YCoord());
        }
      }

      SurfacePoint apriSurfacePoint(cp->GetAprioriSurfacePoint());
      if(apriSurfacePoint.Valid()) {
        if(proj->SetUniversalGround(apriSurfacePoint.GetLatitude().GetDegrees(),
            apriSurfacePoint.GetLongitude().GetDegrees())) {
          initial = QPointF(proj->XCoord(), -1 * proj->YCoord());
        }
      }

      // If we have adjusted and not apriori then find camera
      //    OR if we don't have an adjusted and don't have an initial we still
      //       need an initial
      if((!adjusted.isNull() && initial.isNull()) ||
         (adjusted.isNull() && initial.isNull())) {
        try {
          QString sn = cp->GetReferenceSN();
          QString filename = snToFilename(sn);

          if(filename.size() > 0) {
            if((*p_cubeToGroundMap)[filename] == NULL) {
              Pvl label(Filename(filename.toStdString()).Expanded());
              UniversalGroundMap *groundMap = new UniversalGroundMap(label);
              (*p_cubeToGroundMap)[filename] = groundMap;
            }

            if((*p_cubeToGroundMap)[filename]->SetImage(
                  cp->GetRefMeasure()->GetSample(),
                  cp->GetRefMeasure()->GetLine())) {
              double lat = (*p_cubeToGroundMap)[filename]->UniversalLatitude();
              double lon = (*p_cubeToGroundMap)[filename]->UniversalLongitude();

              if(proj->SetUniversalGround(lat, lon))
                initial = QPointF(proj->XCoord(), -1 * proj->YCoord());
            }
          }
        }
        catch(iException &e) {
          e.Clear();
        }
      }
    }

    QPair<QPointF, QPointF> result;
    if(!adjusted.isNull() && adjusted != initial) {
      result.second = adjusted;
      result.first = initial;
    }
    else {
      result.second = initial;
    }

    (*p_pointToScene)[cp] = result;

    return result;
  }


  QString ControlNetGraphicsItem::snToFilename(QString sn) {
    QString result;

    if(p_serialNumbers && p_serialNumbers->Size()) {
      try {
        result = QString::fromStdString(
            p_serialNumbers->Filename(sn.toStdString()));
      }
      catch(iException &e) {
        e.Clear();
      }
    }

    return result;
  }


  void ControlNetGraphicsItem::buildChildren() {
    bool wasVisible = isVisible();
    setVisible(false);

    QList<QGraphicsItem *> children = childItems();
    QGraphicsItem *child;
    foreach(child, children) {
      if(child->scene())
        child->scene()->removeItem(child);

      delete child;
      child = NULL;
    }

    if(p_controlNet) {
      const int numCp = p_controlNet->GetNumPoints();

      if(p_serialNumbers) {
        delete p_serialNumbers;
      }

      p_serialNumbers = new SerialNumberList;

      QStringList cubeFiles(p_mosaicScene->cubeFilenames());

      QString filename;
      foreach(filename, cubeFiles) {
        p_serialNumbers->Add(filename.toStdString());
      }

      ProgressBar *p = (ProgressBar *)p_mosaicScene->getProgress();
      p->setText("Calculating CP Locations");
      p->setRange(0, numCp - 1);
      p->setValue(0);
      p->setVisible(true);

      for(int cpIndex = 0; cpIndex < numCp; cpIndex ++) {
        ControlPoint *cp = p_controlNet->GetPoint(cpIndex);

        // Apriori/Camera, Adjusted
        QPair<QPointF, QPointF> scenePoints = pointToScene(cp);

        new ControlPointGraphicsItem( scenePoints.second, scenePoints.first,
            cp, p_serialNumbers, p_mosaicScene, this);

        p->setValue(cpIndex);
      }

      p->setVisible(false);
    }

    setVisible(wasVisible);
  }
}

