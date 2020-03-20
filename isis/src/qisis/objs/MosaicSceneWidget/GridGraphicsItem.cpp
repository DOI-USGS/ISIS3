#include "GridGraphicsItem.h"

#include <cmath>
#include <float.h>
#include <iostream>

#include <QDebug>
#include <QGraphicsScene>
#include <QPen>

#include "Angle.h"
#include "Distance.h"
#include "GroundGrid.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "Projection.h"
#include "TProjection.h"
#include "UniversalGroundMap.h"

using namespace std;

namespace Isis {
  GridGraphicsItem::GridGraphicsItem(Latitude baseLat, Longitude baseLon,
      Angle latInc, Angle lonInc, MosaicSceneWidget *projectionSrc,
      int density, Latitude latMin, Latitude latMax,
      Longitude lonMin, Longitude lonMax) {
    setZValue(DBL_MAX);


    if (latInc > Angle(0.0, Angle::Degrees) && lonInc > Angle(0.0, Angle::Degrees)) {
      // Walk the grid, creating a QGraphicsLineItem for each line segment.
      Projection *proj = projectionSrc->getProjection();
      Projection::ProjectionType pType = proj->projectionType();

      if (proj && pType == Projection::Triaxial && lonMin < lonMax && latMin < latMax) {
        TProjection *tproj = (TProjection *) proj;
        PvlGroup mappingGroup(tproj->Mapping());

        Latitude minLat;
        Latitude maxLat;
        Latitude startLat;
        Latitude endLat;
      

      if (tproj->IsPositiveWest()) {
        // GridGraphicsItem is written assuming positive east 
        // for all angles. On positive West, lons come in swapped so we 
        // need to account for this.
        Longitude temp = lonMin;
        lonMin = lonMax;
        lonMax = temp;
      }
      if (mappingGroup["LatitudeType"][0] == "Planetographic") {

          Distance equaRad(tproj->EquatorialRadius(), Distance::Meters);
          Distance polRad(tproj->PolarRadius(), Distance::Meters);

          minLat = Latitude(latMin.planetographic(Angle::Degrees), mappingGroup,
                            Angle::Degrees);
          maxLat = Latitude(latMax.planetographic(Angle::Degrees), mappingGroup,
                            Angle::Degrees);
          baseLat = Latitude(baseLat.degrees(), equaRad, polRad,
                            Latitude::Planetocentric, Angle::Degrees);

          // Make sure our lat increment is non-zero
          if (!qFuzzyCompare(latInc.radians(), 0.0)) {
            startLat = baseLat;

            // We need startLat to start above min, and be as close to min as possible
            try {
              while (startLat < minLat) {
                startLat = startLat.add(latInc, mappingGroup);
              }
            }
            catch (IException &) {
            }

            try {
              while (startLat.add(latInc * -1, mappingGroup) >= minLat) {
                startLat = startLat.add(latInc * -1, mappingGroup);
              }
            }
            catch (IException &) {
              // Do nothing if we hit up against a pole
            }
          }

          endLat = baseLat;

          // We need endLat to start below max, and be as close to max as possible
          try {
            while (endLat > maxLat) {
              endLat = endLat.add(latInc * -1, mappingGroup);
            }
          }
          catch (IException &) {
          }


          try {
            while (endLat.add(latInc, mappingGroup) <= maxLat) {
              endLat = endLat.add(latInc, mappingGroup);
            }
          }
          catch (IException &) {
            // Do nothing if we hit up against a pole
          }
        }
        else {
          minLat = Latitude(latMin.degrees(), mappingGroup,
                          Angle::Degrees);
          maxLat = Latitude(latMax.degrees(), mappingGroup,
                          Angle::Degrees);
          

          // Make sure our lat increment is non-zero
          if (!qFuzzyCompare(latInc.radians(), 0.0)) {
            startLat = Latitude(
              baseLat - Angle(floor((baseLat - minLat) / latInc) * latInc), mappingGroup);

          if (qFuzzyCompare(startLat.degrees(), -90.0))
            startLat = Latitude(-90.0, mappingGroup, Angle::Degrees);
          }

          endLat = Latitude(
            (long)((maxLat - startLat) / latInc) * latInc + startLat,
            mappingGroup);
          if (qFuzzyCompare(endLat.degrees(), 90.0))
            endLat = Latitude(90.0, mappingGroup, Angle::Degrees);
        }
        
        Longitude minLon(lonMin.degrees(), mappingGroup,
                        Angle::Degrees);
        Longitude maxLon(lonMax.degrees(), mappingGroup,
                        Angle::Degrees);

        Longitude startLon;
        // Make sure our lon increment is non-zero
        if (!qFuzzyCompare(lonInc.radians(), 0.0)) {
          startLon = Longitude(
            baseLon - Angle(floor((baseLon - minLon) / lonInc) * lonInc));
        }
        
        Longitude endLon =
            (long)((maxLon - startLon) / lonInc) * lonInc + startLon;

        if (qFuzzyCompare( (endLon + lonInc).radians(), maxLon.radians() )) {
          endLon = maxLon;
        }

        // Make sure our increments will move our lat/lon values... prevent infinite loops
        if (!qFuzzyCompare( (startLat + latInc).radians(), startLat.radians() ) &&
            !qFuzzyCompare( (startLon + lonInc).radians(), startLon.radians() )) {

          int numCurvedLines = (int)ceil(((maxLat - minLat) / latInc) + 1);
          numCurvedLines += (int)ceil(((maxLon - minLon) / lonInc) + 1);

          int curvedLineDensity = density / numCurvedLines + 1;
          Angle latRes((maxLon - minLon) / (double)curvedLineDensity);
          Angle lonRes((maxLat - minLat) / (double)curvedLineDensity);

          if (mappingGroup["LatitudeType"][0] == "Planetographic") {
            lonRes = Angle(
                (maxLat.planetographic() - minLat.planetographic()) / (double)curvedLineDensity,
                Angle::Radians);
          }

          if (latRes <= Angle(0, Angle::Degrees))
            latRes = Angle(1E-10, Angle::Degrees);

          if (lonRes <= Angle(0, Angle::Degrees))
            lonRes = Angle(1E-10, Angle::Degrees);

          bool firstIteration = true;
          bool atMaxLat = false;
          bool atMaxLon = false;

          // We're looping like this to guarantee we hit the correct end position in
          //   the loop despite double math.


          Latitude lat = minLat;
          while(!atMaxLat) {
            double previousX = 0;
            double previousY = 0;
            bool havePrevious = false;

            for(Longitude lon = minLon; lon != maxLon + latRes; lon += latRes) {

              if (lon > maxLon && !atMaxLon) {
                lon = maxLon;
                atMaxLon = true;
              }

              double x = 0;
              double y = 0;
              
              bool valid;
              
              // Set ground according to lon direction to get correct X,Y values 
              // when drawing lines. 
              if (tproj->IsPositiveWest()) {
                valid = tproj->SetGround(lat.degrees(), lon.positiveWest(Angle::Degrees));
              }
              else {
                valid = tproj->SetGround(lat.degrees(), lon.positiveEast(Angle::Degrees));
              }

              if (valid) {
                x = tproj->XCoord();
                y = -1 * tproj->YCoord();

                if(havePrevious) {
                  if(previousX != x || previousY != y) {
                    QGraphicsLineItem* latLine = 
                        new QGraphicsLineItem(QLineF(previousX, previousY, x, y), this);
                    // Ensure the line is cosmetic
                    // (i.e. the line width is always 1 pixel wide on screen)
                    QPen pen;
                    pen.setCosmetic(true);
                    latLine->setPen(pen);
                  }
                }
              }

              havePrevious = valid;
              previousX = x;
              previousY = y;
            }

  //           if (firstIteration) {
  //             if (startLat.planetographic(Angle::Degrees) - latInc.degrees() < -90.0)
  //               lat = Latitude(-90.0, mappingGroup, Angle::Degrees);
  //             else {
  //               lat = Latitude(startLat.planetographic() - latInc.radians(), mappingGroup,
  //                              Angle::Radians);
  //             }
  //           }

            firstIteration = false;
            atMaxLon = false;

            Latitude nextLat;

            try {
              nextLat = lat.add(latInc, mappingGroup);
            }
            catch (IException &) {
              nextLat = maxLat;
            }

            if (lat == minLat && minLat != startLat) {
              // If our increment doesn't intersect the lat range, set ourselves to max.
              if (startLat < minLat || startLat > maxLat) {
                nextLat = maxLat;
              }
              else {
                // Our increment lands inside the range, go to start and begin incrementing towards
                // end.
                nextLat = startLat;
              }
            }
            else if (lat >= maxLat) {
              atMaxLat = true;
            }
            else if (nextLat > endLat) {
              nextLat = maxLat;
            }

            lat = nextLat;
          }

          firstIteration = true;
          atMaxLat = false;
          atMaxLon = false;
          
          // Create the longitude grid lines
          for (Longitude lon = minLon; lon != maxLon + lonInc; lon += lonInc) {
            if (lon > endLon && lon < maxLon) {
              lon = endLon;
            }

            if (lon > maxLon && !atMaxLon) {
              lon = maxLon;
              atMaxLon = true;
            }

            double previousX = 0;
            double previousY = 0;
            bool havePrevious = false;

            Latitude lat =  minLat;
            while (!atMaxLat) {
              double x = 0;
              double y = 0;
              
              // Set ground according to lon direction to get correct X,Y values 
              // when drawing lines.  
              bool valid;
              
              if (tproj->IsPositiveWest()) {
                  double glon = tproj->Has180Domain() ? -1*lon.positiveEast(Angle::Degrees): lon.positiveWest(Angle::Degrees);
                  valid = tproj->SetGround(lat.degrees(), glon);
              }
              else {
                valid = tproj->SetGround(lat.degrees(), lon.positiveEast(Angle::Degrees));
              }

              if (valid) {
                x = tproj->XCoord();
                y = -1 * tproj->YCoord();

                if(havePrevious) {
                  if(previousX != x || previousY != y) {
                    QGraphicsLineItem* lonLine = 
                        new QGraphicsLineItem(QLineF(previousX, previousY, x, y), this);
                    // Ensure the line is cosmetic 
                    // (i.e. the line width is always 1 pixel wide on screen)
                    QPen pen;
                    pen.setCosmetic(true);
                    lonLine->setPen(pen);
                  }
                }
              }

              havePrevious = valid;
              previousX = x;
              previousY = y;

              if (lat >= maxLat) {
                atMaxLat = true;
              }
              else {
                lat = lat.add(lonRes, mappingGroup);
              }
            }

            if (firstIteration)
              lon = startLon - lonInc;

            firstIteration = false;
            atMaxLat = false;
          }
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

    foreach (QGraphicsItem *child, childItems()) {
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

