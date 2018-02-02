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
    // Set up the input file
    QFile loadFile(lidarFile.expanded());

    if (!loadFile.open(QIODevice::ReadOnly)) {
      QString msg("Could not open " + loadFile.fileName());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Load file
    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    // Unserialize LidarData
    QJsonObject lidarDataObject = loadDoc.object();
    if (lidarDataObject.contains("points") && lidarDataObject["points"].isArray()) {
      // Unserialize LidarControlPoints
      QJsonArray pointArray = lidarDataObject["points"].toArray();
      for (int pointIndex = 0; pointIndex < pointArray.size(); pointIndex++) {
        // Unserialize LidarControlPoint
        QJsonObject pointObject = pointArray[pointIndex].toObject();

        QString id;
        if (pointObject.contains("id") && pointObject["id"].isString()) {
          id = pointObject["id"].toString();
        }

        double range = 0.0;
        if (pointObject.contains("range") && pointObject["range"].isDouble()) {
          range = pointObject["range"].toDouble();
        }

        double sigmaRange = 0.0;
        if (pointObject.contains("sigmaRange") && pointObject["sigmaRange"].isDouble()) {
          sigmaRange = pointObject["sigmaRange"].toDouble();
        }

        double time = 0.0;
        if (pointObject.contains("time") && pointObject["time"].isDouble()) {
          time = pointObject["time"].toDouble();
        }

        double latitude = 0.0;
        if (pointObject.contains("latitude") && pointObject["latitude"].isDouble()) {
          latitude = pointObject["latitude"].toDouble();
        }

        double longitude = 0.0;
        if (pointObject.contains("longitude") && pointObject["longitude"].isDouble()) {
          longitude = pointObject["longitude"].toDouble();
        }

        double radius = 0.0;
        if (pointObject.contains("radius") && pointObject["radius"].isDouble()) {
          radius = pointObject["radius"].toDouble();
        }

        QSharedPointer<LidarControlPoint> lcp =
            QSharedPointer<LidarControlPoint>(new LidarControlPoint(iTime(time), range, sigmaRange));
        lcp->SetId(id);
        lcp->SetAprioriSurfacePoint(SurfacePoint(Latitude(latitude, Angle::Units::Degrees),
                                                 Longitude(longitude, Angle::Units::Degrees),
                                                 Distance(radius, Distance::Units::Kilometers)));

        // Unserialize ControlMeasures
        if (pointObject.contains("measures") && pointObject["measures"].isArray()) {
          QJsonArray measureArray = pointObject["measures"].toArray();
          for (int measureIndex = 0; measureIndex < measureArray.size(); measureIndex++) {
            // Unserialize ControlMeasure
            QJsonObject measureObject = measureArray[measureIndex].toObject();

            double line = 0.0;
            if (measureObject.contains("line") && measureObject["line"].toDouble()) {
              line = measureObject["line"].toDouble();
            }

            double sample = 0.0;
            if (measureObject.contains("sample") && measureObject["sample"].toDouble()) {
              sample = measureObject["sample"].toDouble();
            }

            QString serialNumber;
            if (measureObject.contains("serialNumber") && measureObject["serialNumber"].isString()) {
              serialNumber = measureObject["serialNumber"].toString();
            }

            ControlMeasure *measure = new ControlMeasure();
            measure->SetCoordinate(sample, line);
            measure->SetCubeSerialNumber(serialNumber);
            lcp->Add(measure);
          }
        }

        insert(lcp);
      }
    }
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
