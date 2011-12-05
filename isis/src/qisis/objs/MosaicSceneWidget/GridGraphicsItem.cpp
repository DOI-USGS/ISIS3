#include "GridGraphicsItem.h"

#include <cmath>
#include <float.h>
#include <iostream>

#include <QGraphicsScene>

#include "Angle.h"
#include "GroundGrid.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"
#include "UniversalGroundMap.h"

using namespace std;

namespace Isis {
  GridGraphicsItem::GridGraphicsItem(Latitude baseLat, Longitude baseLon,
      Angle latInc, Angle lonInc, MosaicSceneWidget *projectionSrc,
      int density) {
    setZValue(DBL_MAX);

    // Walk the grid, creating a QGraphicsLineItem for each line segment.
    Projection *proj = projectionSrc->getProjection();

    if (proj) {
      PvlGroup mappingGroupForUnits(proj->Mapping());


      Latitude minLat(proj->MinimumLatitude(), mappingGroupForUnits,
                        Angle::Degrees);
      Latitude maxLat(proj->MaximumLatitude(), mappingGroupForUnits,
                        Angle::Degrees);

      Longitude minLon(proj->MinimumLongitude(), mappingGroupForUnits,
                        Angle::Degrees);
      Longitude maxLon(proj->MaximumLongitude(), mappingGroupForUnits,
                        Angle::Degrees);

      Latitude startLat = Latitude(
          baseLat - Angle(floor((baseLat - minLat) / latInc) * latInc),
          mappingGroupForUnits);

      Longitude startLon = Longitude(
          baseLon - Angle(floor((baseLon - minLon) / lonInc) * lonInc));

      Latitude endLat = Latitude(
          (long)((maxLat - startLat) / latInc) * latInc + startLat,
          mappingGroupForUnits);

      Longitude endLon =
          (long)((maxLon - startLon) / lonInc) * lonInc + startLon;

      int numCurvedLines = (int)ceil(((endLat - startLat) / latInc) + 1);
      numCurvedLines += (int)ceil(((endLon - startLon) / lonInc) + 1);

      int curvedLineDensity = density / numCurvedLines + 1;
      Angle latRes((endLon - startLon) / (double)curvedLineDensity);
      Angle lonRes((endLat - startLat) / (double)curvedLineDensity);

      if (latRes <= Angle(0, Angle::Degrees))
        latRes = Angle(1E-10, Angle::Degrees);

      if (lonRes <= Angle(0, Angle::Degrees))
        lonRes = Angle(1E-10, Angle::Degrees);

      // We're looping like this to guarantee we hit the correct end position in
      //   the loop despite double math.
      for(Latitude lat = startLat; lat != endLat + latInc; lat += latInc) {
        if (lat > endLat)
          lat = endLat;

        double previousX = 0;
        double previousY = 0;
        bool havePrevious = false;

        for(Longitude lon = startLon; lon != endLon + latRes; lon += latRes) {
          if (lon > endLon)
            lon = endLon;

          double x = 0;
          double y = 0;
          bool valid = proj->SetUniversalGround(lat.GetDegrees(),
                                                lon.GetDegrees());
          if (valid) {
            x = proj->XCoord();
            y = -1 * proj->YCoord();

            if(havePrevious) {
              if(previousX != x || previousY != y) {
                new QGraphicsLineItem(QLineF(previousX, previousY, x, y), this);
              }
            }
          }


          havePrevious = valid;
          previousX = x;
          previousY = y;
        }
      }

      for(Longitude lon = startLon; lon != endLon + lonInc; lon += lonInc) {
        if (lon > endLon)
          lon = endLon;

        double previousX = 0;
        double previousY = 0;
        bool havePrevious = false;

        for(Latitude lat = startLat; lat != endLat + lonRes; lat += lonRes) {
          if (lat > endLat)
            lat = endLat;

          double x = 0;
          double y = 0;

          bool valid = proj->SetUniversalGround(lat.GetDegrees(),
                                                lon.GetDegrees());

          if (valid) {
            x = proj->XCoord();
            y = -1 * proj->YCoord();

            if(havePrevious) {
              x = proj->XCoord();
              y = -1 * proj->YCoord();

              if(previousX == x && previousY == y) {
                continue;
              }

              new QGraphicsLineItem(QLineF(previousX, previousY, x, y), this);
            }
          }

          havePrevious = valid;
          previousX = x;
          previousY = y;
        }
      }
    }

    setRect(calcRect());
  }


  GridGraphicsItem::~GridGraphicsItem() {
  }


  void GridGraphicsItem::paint(QPainter *painter,
      const QStyleOptionGraphicsItem *style,  QWidget * widget) {
  }


  QRectF GridGraphicsItem::boundingRect() const {
    return m_boundingRect;
  }


  QRectF GridGraphicsItem::rect() const {
    return m_boundingRect;
  }


  QRectF GridGraphicsItem::calcRect() const {
    QRectF sceneRect;

    foreach (QGraphicsItem *child, children()) {
      sceneRect = sceneRect.united(child->boundingRect());
    }

    return sceneRect;
  }


  void GridGraphicsItem::setRect(QRectF newBoundingRect) {
    if (m_boundingRect != newBoundingRect) {
      prepareGeometryChange();
      m_boundingRect = newBoundingRect;
    }
  }
}

