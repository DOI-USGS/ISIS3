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
    p_pointToLatLon = new QMap<ControlPoint *, QPointF>;
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
    if(p_pointToLatLon) {
      delete p_pointToLatLon;
      p_pointToLatLon = NULL;
    }

    if(p_cubeToGroundMap) {
      QMapIterator<QString, UniversalGroundMap *> it(*p_cubeToGroundMap);

      while(it.hasNext()) {
        it.next();
        delete it.value();
      }

      delete p_cubeToGroundMap;
      p_cubeToGroundMap = NULL;
    }
  }


  QRectF ControlNetGraphicsItem::boundingRect() const {
    QRectF tmp;
    return tmp;
    QRectF networkRect = calcRect();

    if(networkRect.isNull())
      return p_mosaicScene->cubesBoundingRect();
    else
      return p_mosaicScene->cubesBoundingRect().united(networkRect);
  }


  void ControlNetGraphicsItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *style,  QWidget * widget) {
  }


  QRectF ControlNetGraphicsItem::calcRect() const {
    return childrenBoundingRect();
  }


  QRectF ControlNetGraphicsItem::calcRect(QPointF pt) {
    QRectF networkRect;

    if(!pt.isNull()) {
      static const int size = 8;
      QPoint findSpotScreen =
          p_mosaicScene->getView()->mapFromScene(pt);
      QPoint findSpotTopLeftScreen =
          findSpotScreen - QPoint(size / 2, size / 2);

      QRect findRectScreen(findSpotTopLeftScreen, QSize(size, size));
      networkRect =
          p_mosaicScene->getView()->mapToScene(findRectScreen).boundingRect();
    }

    return networkRect;
  }


  QPointF ControlNetGraphicsItem::pointToScene(ControlPoint *cp) {
    Projection *proj = p_mosaicScene->getProjection();

    QPointF sceneLoc;

    QPointF rememberedLoc = (*p_pointToLatLon)[cp];

    if(!rememberedLoc.isNull()) {
      proj->SetUniversalGround(rememberedLoc.y(),
                               rememberedLoc.x());
      sceneLoc = QPointF(proj->XCoord(), -1 * proj->YCoord());
    }
    else if(proj) {
      const SurfacePoint &sp = cp->GetBestSurfacePoint();

      if(sp.Valid()) {
        proj->SetUniversalGround(sp.GetLatitude().GetDegrees(),
                                sp.GetLongitude().GetDegrees());
        sceneLoc = QPointF(proj->XCoord(), -1 * proj->YCoord());
      }
      else {
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

              (*p_pointToLatLon)[cp] = QPointF(lon, lat);

              if(proj->SetUniversalGround(lat, lon))
                sceneLoc = QPointF(proj->XCoord(), -1 * proj->YCoord());
            }
          }
        }
        catch(iException &e) {
          e.Clear();
        }
      }
    }

    return sceneLoc;
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
    QList<QGraphicsItem *> children = childItems();
    QGraphicsItem *child;
    foreach(child, children) {
      if(child->scene())
        child->scene()->removeItem(child);

      delete child;
      child = NULL;
    }

    if(p_controlNet) {
      QList<ControlPointGraphicsItem *> newChildren;
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
      p->setText("Calculating Control Point Locations");
      p->setRange(0, numCp - 1);
      p->setValue(0);
      p->setVisible(true);

      for(int cp = 0; cp < numCp; cp ++) {
        newChildren.append(new ControlPointGraphicsItem(
            pointToScene(p_controlNet->GetPoint(cp)),
            p_controlNet->GetPoint(cp), p_serialNumbers, p_mosaicScene, this));
        p->setValue(cp);
      }

      p->setVisible(false);
    }
  }
}

