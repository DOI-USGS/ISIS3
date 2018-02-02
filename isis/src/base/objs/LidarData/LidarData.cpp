#include "LidarData.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QSharedPointer>

#include "Angle.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "SurfacePoint.h"

namespace Isis {


  /**
   * Default constructor.
   */
  LidarData::LidarData() {

  }


  /**
   * Constructor that takes a Lidar CSV file.
   *
   * @param FileName lidarFile Name of the Lidar CSV file to use.
   */
  LidarData::LidarData(FileName lidarFile) {

  }


  /**
   * Adds a LidarControlPoint to the LidarData.
   *
   * @param QSharedPointer<LidarControlPoint> point LidarControlPoint to add.
   */
  void LidarData::insert(QSharedPointer<LidarControlPoint> point) {
    m_points.insert(point->GetId(), point);
  }


  /**
   * Gets the list of Lidar data points.
   *
   * @return @b QList<QSharedPointer<LidarControlPoint>> Returns list of Lidar control points.
   */
  QList< QSharedPointer<LidarControlPoint> > LidarData::points() const {
    return m_points.values();
  }


  /**
   * Reads in a Lidar CSV file.
   *
   * @param FileName lidarFile Name of the Lidar CSV file to read.
   */
  void LidarData::read(FileName lidarFile) {

  }


  /**
   * Writes out the Lidar data to a CSV file.
   *
   * @param FileName outputFile Name of the file to write to.
   */
  void LidarData::write(FileName outputFile) {
    // Set up the output file
    QFile saveFile(outputFile.expanded());

    if (!saveFile.open(QIODevice::WriteOnly)) {
      QString msg("Could not open " + saveFile.fileName());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Serialize LidarData
    QJsonObject lidarDataObject;
    QJsonArray pointArray;
    // Serialize the LidarControlPoints it contains
    foreach (QSharedPointer<LidarControlPoint> lcp, points()) {
      // Serialize LidarControlPoint
      QJsonObject pointObject;
      pointObject["id"] = lcp->GetId();
      pointObject["range"] = lcp->range();
      pointObject["sigmaRange"] = lcp->sigmaRange();
      pointObject["time"] = lcp->time().Et();
      // Serialize the lat/lon/radius (AprioriSurfacePoint)
      SurfacePoint aprioriSurfacePoint = lcp->GetAprioriSurfacePoint();
      pointObject["latitude"] = aprioriSurfacePoint.GetLatitude().planetocentric(Angle::Units::Degrees);
      pointObject["longitude"] = aprioriSurfacePoint.GetLongitude().positiveEast(Angle::Units::Degrees);
      pointObject["radius"] = aprioriSurfacePoint.GetLocalRadius().kilometers();

      QJsonArray measureArray;
      // Serialize the ControlMeasures it contains
      foreach (ControlMeasure *measure, lcp->getMeasures()) {
        // Serialize ControlMeasure
        QJsonObject measureObject;
        measureObject["line"] = measure->GetLine();
        measureObject["sample"] = measure->GetSample();
        measureObject["serialNumber"] = measure->GetCubeSerialNumber();
        measureArray.append(measureObject);

      }
      // Add the ControlMeasures to the LidarControlPoint
      pointObject["measures"] = measureArray;

      pointArray.append(pointObject);

    }
    // Add the LidarControlPoints to the LidarData
    lidarDataObject["points"] = pointArray;

    // Write the JSON to the file
    QJsonDocument lidarDataDoc(lidarDataObject);
    saveFile.write(lidarDataDoc.toJson());
  }
}
