#include "UniversalGroundMap.h"

#include <QPointF>

#include "Camera.h"
#include "CameraFactory.h"
#include "ImagePolygon.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Latitude.h"
#include "Longitude.h"
#include "PolygonTools.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "SurfacePoint.h"

namespace Isis {
  /**
   * Constructs a UniversalGroundMap object from a pvl
   *
   * @param pvl The Pvl file to create the UniversalGroundMap from
   * @param priority Try to make a camera or projection first
   */
  UniversalGroundMap::UniversalGroundMap(Pvl &pvl, CameraPriority priority) {
    Init(pvl, priority);
  }

  /**
   * Constructs a UniversalGroundMap object from a cube
   *
   * @param cube The Cube to create the UniversalGroundMap from
   * @param priority Try to make a camera or projection first
   */
  UniversalGroundMap::UniversalGroundMap(Cube &cube, CameraPriority priority) {
    Init(*(cube.getLabel()), priority);
  }

  /**
   * Creates the UniversalGroundMap
   *
   * @param pvl The Pvl file to create the UniversalGroundMap from
   * @param priority Try to make a camera or projection first
   *
   * @throws Isis::iException::Camera - Could not create camera or projection
   */
  void UniversalGroundMap::Init(Pvl &pvl, CameraPriority priority) {
    p_camera = NULL;
    p_projection = NULL;

    try {
      if(priority == CameraFirst)
        p_camera = CameraFactory::Create(pvl);
      else
        p_projection = Isis::ProjectionFactory::CreateFromCube(pvl);
    }
    catch (IException &firstError) {
      p_camera = NULL;
      p_projection = NULL;

      try {
        if(priority == CameraFirst)
          p_projection = Isis::ProjectionFactory::CreateFromCube(pvl);
        else
          p_camera = CameraFactory::Create(pvl);
      }
      catch (IException &secondError) {
        p_projection = NULL;
        std::string msg = "Could not create camera or projection for [" +
                          pvl.FileName() + "]";
        IException realError(IException::Unknown, msg, _FILEINFO_);
        realError.append(firstError);
        realError.append(secondError);
        throw realError;
      }
    }
  }

  /**
   * Set the image band number
   *
   * @param[in] band   (int)  Image band number
   *
   */
  void UniversalGroundMap::SetBand(const int band) {
    if (p_camera != NULL)
      p_camera->SetBand(band);
  }



  //! Destroys the UniversalGroundMap object
  UniversalGroundMap::~UniversalGroundMap() {
    if (p_camera != NULL)
      delete p_camera;
    if (p_projection != NULL)
      delete p_projection;
  }

  /**
   * Returns whether the lat/lon position was set successfully in the camera
   * model or projection
   *
   * @param lat The universal latitude
   * @param lon The universal longitude
   *
   * @return Returns true if the lat/lon position was set successfully, and
   *         false if it was not
   */
  bool UniversalGroundMap::SetUniversalGround(double lat, double lon) {
    if (p_camera != NULL) {
      if (p_camera->SetUniversalGround(lat, lon)) {
        return p_camera->InCube();
      }
      else {
        return false;
      }
    }
    else {
      return p_projection->SetUniversalGround(lat, lon);
    }
  }


  /**
   * Returns whether the lat/lon position was set successfully in the camera
   * model or projection.
   *
   * @param lat The latitude
   * @param lon The longitude
   *
   * @return Returns true if the lat/lon position was set successfully, and
   *         false if it was not
   */
  bool UniversalGroundMap::SetGround(Latitude lat, Longitude lon) {
    if(p_camera != NULL) {
      if(p_camera->SetGround(lat, lon)) {
        return p_camera->InCube();
      }
      else {
        return false;
      }
    }
    else {
      double universalLat = lat.degrees();
      double universalLon = lon.degrees();
      return p_projection->SetUniversalGround(universalLat, universalLon);
    }
  }


  /**
   * Returns whether the SurfacePoint was set successfully in the camera model
   * or projection
   *
   * @param sp The Surface Point to set ground with
   *
   * @return Returns true if the Surface Point was set successfully, and false
   *         otherwise
   */
  bool UniversalGroundMap::SetGround(const SurfacePoint &sp) {
    if (p_camera != NULL) {
      if (p_camera->SetGround(sp)) {
        return p_camera->InCube();
      }
      else {
        return false;
      }
    }
    else {
      return p_projection->SetUniversalGround(sp.GetLatitude().degrees(),
                                              sp.GetLongitude().degrees());
    }
  }

  /**
   * Returns the current line value of the camera model or projection
   *
   * @return Sample value
   */
  double UniversalGroundMap::Sample() const {
    if (p_camera != NULL) {
      return p_camera->Sample();
    }
    else {
      return p_projection->WorldX();
    }
  }

  /**
   * Returns the current line value of the camera model or projection
   *
   * @return Line value
   */
  double UniversalGroundMap::Line() const {
    if (p_camera != NULL) {
      return p_camera->Line();
    }
    else {
      return p_projection->WorldY();
    }
  }

  /**
   * Returns whether the sample/line postion was set successfully in the camera
   * model or projection
   *
   * @param sample The sample position
   * @param line The line position
   *
   * @return Returns true if the sample/line position was set successfully, and
   *         false if it was not
   */
  bool UniversalGroundMap::SetImage(double sample, double line) {
    if (p_camera != NULL) {
      return p_camera->SetImage(sample, line);
    }
    else {
      return p_projection->SetWorld(sample, line);
    }
  }

  /**
   * Returns the universal latitude of the camera model or projection
   *
   * @return Universal Latitude
   */
  double UniversalGroundMap::UniversalLatitude() const {
    if (p_camera != NULL) {
      return p_camera->UniversalLatitude();
    }
    else {
      return p_projection->UniversalLatitude();
    }
  }

  /**
   * Returns the universal longitude of the camera model or projection
   *
   * @return Universal Longitude
   */
  double UniversalGroundMap::UniversalLongitude() const {
    if (p_camera != NULL) {
      return p_camera->UniversalLongitude();
    }
    else {
      return p_projection->UniversalLongitude();
    }
  }

  /**
   * Returns the resolution of the camera model or projection
   *
   * @return Resolution
   */
  double UniversalGroundMap::Resolution() const {
    if (p_camera != NULL) {
      return p_camera->PixelResolution();
    }
    else {
      return p_projection->Resolution();
    }
  }


  /**
   * Find the lat/lon range of the image. This will use the image footprint,
   *   camera, or projection in order to find a good result.
   *
   * @param Cube* This is required for estimation. You can pass in NULL (it will
   *              disable estimation).
   * @param minLat This is an output: minimum latitude
   * @param maxLat This is an output: maximum latitude
   * @param minLon This is an output: minimum longitude
   * @param maxLon This is an output: maximum longitude
   * @param allowEstimation If this is true then extra efforts will be made to
   *     guess the ground range of the input. This can still fail.
   * @return True if a ground range was found, false if no ground range could
   *     be determined. Some lat/lon results may still be populated; their
   *     values are undefined.
   */
  bool UniversalGroundMap::GroundRange(Cube *cube, Latitude &minLat,
      Latitude &maxLat, Longitude &minLon, Longitude &maxLon,
      bool allowEstimation) {
    minLat = Latitude();
    maxLat = Latitude();
    minLon = Longitude();
    maxLon = Longitude();

    // If we have a footprint, use it
    try {
      if (cube) {
        ImagePolygon poly;
        cube->read(poly);
        geos::geom::MultiPolygon *footprint = PolygonTools::MakeMultiPolygon(
            poly.Polys()->clone());

        geos::geom::Geometry *envelope = footprint->getEnvelope();
        geos::geom::CoordinateSequence *coords = envelope->getCoordinates();

        for (unsigned int i = 0; i < coords->getSize(); i++) {
          const geos::geom::Coordinate &coord = coords->getAt(i);

          Latitude coordLat(coord.y, Angle::Degrees);
          Longitude coordLon(coord.x, Angle::Degrees);

          if (!minLat.isValid() || minLat > coordLat)
            minLat = coordLat;
          if (!maxLat.isValid() || maxLat < coordLat)
            maxLat = coordLat;

          if (!minLon.isValid() || minLon > coordLon)
            minLon = coordLon;
          if (!maxLon.isValid() || maxLon < coordLon)
            maxLon = coordLon;
        }

        delete coords;
        coords = NULL;

        delete envelope;
        envelope = NULL;

        delete footprint;
        footprint = NULL;
      }
    }
    catch (IException &) {
    }

    if (!minLat.isValid() || !maxLat.isValid() ||
        !minLon.isValid() || !maxLon.isValid()) {
      if (HasCamera()) {
        // Footprint failed, ask the camera
        PvlGroup mappingGrp("Mapping");
        mappingGrp += PvlKeyword("LatitudeType", "Planetocentric");
        mappingGrp += PvlKeyword("LongitudeDomain", "360");
        mappingGrp += PvlKeyword("LongitudeDirection", "PositiveEast");

        Pvl mappingPvl;
        mappingPvl += mappingGrp;
        double minLatDouble;
        double maxLatDouble;
        double minLonDouble;
        double maxLonDouble;
        p_camera->GroundRange(
            minLatDouble, maxLatDouble,
            minLonDouble, maxLonDouble, mappingPvl);
        minLat = Latitude(minLatDouble, Angle::Degrees);
        maxLat = Latitude(maxLatDouble, Angle::Degrees);
        minLon = Longitude(minLonDouble, Angle::Degrees);
        maxLon = Longitude(maxLonDouble, Angle::Degrees);
      }
      else if (HasProjection()) {
        // Footprint failed, look in the mapping group
        PvlGroup mappingGrp = p_projection->Mapping();
        if (mappingGrp.HasKeyword("MinimumLatitude") &&
            mappingGrp.HasKeyword("MaximumLatitude") &&
            mappingGrp.HasKeyword("MinimumLongitude") &&
            mappingGrp.HasKeyword("MaximumLongitude")) {

          minLat = Latitude(mappingGrp["MinimumLatitude"],
                            mappingGrp, Angle::Degrees);
          maxLat = Latitude(mappingGrp["MaximumLatitude"],
                            mappingGrp, Angle::Degrees);
          minLon = Longitude(mappingGrp["MinimumLongitude"],
                             mappingGrp, Angle::Degrees);
          maxLon = Longitude(mappingGrp["MaximumLongitude"],
                             mappingGrp, Angle::Degrees);

        }
        else if (allowEstimation && cube) {
          // Footprint and mapping failed... no lat/lon range of any kind is
          //   available. Let's test points in the image to try to make our own
          //   extent.
          QList<QPointF> imagePoints;

                    /*
           * This is where we're testing:
           *
           *  |---------------|
           *  |***************|
           *  |**     *     **|
           *  |*  *   *   *  *|
           *  |*    * * *    *|
           *  |***************|
           *  |*    * * *    *|
           *  |*  *   *   *  *|
           *  |**     *     **|
           *  |***************|
           *  |---------------|
           *
           * We'll test at the edges, a plus (+) and an (X) to help DEMs work.
           */

          int sampleCount = cube->getSampleCount();
          int lineCount = cube->getLineCount();

          int stepsPerLength = 20; //number of steps per length
          double aspectRatio = (double)lineCount / (double)sampleCount;
          double xStepSize = sampleCount / stepsPerLength;
          double yStepSize = xStepSize * aspectRatio;

          if (lineCount > sampleCount) {
            aspectRatio = (double)sampleCount / (double)lineCount;
            yStepSize = lineCount / stepsPerLength;
            xStepSize = yStepSize * aspectRatio;
          }

          double yWalked = 0.5;

          //3 vertical lines
          for (int i = 0; i < 3; i++) {
            double xValue = 0.5 + ( i * (sampleCount / 2) );

            while (yWalked <= lineCount) {
              imagePoints.append( QPointF(xValue, yWalked) );
              yWalked += yStepSize;
            }

            yWalked = 0.5;
          }

          double xWalked = 0.5;

          //3 horizontal lines
          for (int i = 0; i < 3; i++) {
            double yValue = 0.5 + ( i * (lineCount / 2) );

            while (xWalked <= sampleCount) {
              imagePoints.append( QPointF(xWalked, yValue) );
              xWalked += xStepSize;
            }

            xWalked = 0.5;
          }

          double xDiagonalWalked = 0.5;
          double yDiagonalWalked = 0.5;
          xStepSize = sampleCount / stepsPerLength;
          yStepSize = lineCount / stepsPerLength;

          //Top-Down Diagonal
          while ( (xDiagonalWalked <= sampleCount) && (yDiagonalWalked <= lineCount) ) {
            imagePoints.append( QPointF(xDiagonalWalked, yDiagonalWalked) );
            xDiagonalWalked += xStepSize;
            yDiagonalWalked += yStepSize;
          }

          xDiagonalWalked = 0.5;

          //Bottom-Up Diagonal
          while ( (xDiagonalWalked <= sampleCount) && (yDiagonalWalked >= 0) ) {
            imagePoints.append( QPointF(xDiagonalWalked, yDiagonalWalked) );
            xDiagonalWalked += xStepSize;
            yDiagonalWalked -= yStepSize;
          }

          foreach (QPointF imagePoint, imagePoints) {
            if (p_projection->SetWorld(imagePoint.x(), imagePoint.y())) {
              Latitude latResult(p_projection->UniversalLatitude(),
                                 Angle::Degrees);
              Longitude lonResult(p_projection->UniversalLongitude(),
                                  Angle::Degrees);
              if (minLat.isValid())
                minLat = qMin(minLat, latResult);
              else
                minLat = latResult;

              if (maxLat.isValid())
                maxLat = qMax(maxLat, latResult);
              else
                maxLat = latResult;

              if (minLon.isValid())
                minLon = qMin(minLon, lonResult);
              else
                minLon = lonResult;

              if (maxLon.isValid())
                maxLon = qMax(maxLon, lonResult);
              else
                maxLon = lonResult;
            }
          }
        }
      }
    }

    return (minLat.isValid() && maxLat.isValid() &&
            minLon.isValid() && maxLon.isValid() &&
            minLat < maxLat && minLon < maxLon);
  }
}
