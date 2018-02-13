#include "LidarData.h"

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QRegExp>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include "Angle.h"
#include "CameraFactory.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "Progress.h"
#include "SerialNumberList.h"
#include "SurfacePoint.h"


namespace Isis {


  /**
   * Default constructor.
   */
  LidarData::LidarData() {

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
   * Creates the ControlNet's image camera's based on the list of Serial Numbers
   *
   * @param list The list of Serial Numbers
   * @param progress A pointer to the progress of creating the cameras
   * @throws Isis::iException::System - "Unable to create camera
   *        for cube file"
   * @throws Isis::iException::User - "Control point measure does
   *        not have a cube with a matching serial number"
   * @internal
   *   @history 2009-01-06 Jeannie Walldren - Fixed typo in
   *            exception output.
   *   @history 2016-10-13 Ian Humphrey - Added initial check to see if cameras have already been
   *                           set, and immediately return if yes. References #4293.
   */
  void LidarData::SetImages(SerialNumberList &list, Progress *progress) {
    // First check if cameras have already been setup via another SetImages call
    if (p_cameraList.size() > 0) {
      return;
    }
    // Prep for reporting progress
    if (progress != NULL) {
      progress->SetText("Setting input images...");
      progress->SetMaximumSteps(list.size());
      progress->CheckStatus();
    }
    // Open the camera for all the images in the serial number list
    for (int i = 0; i < list.size(); i++) {
      QString serialNumber = list.serialNumber(i);
      QString filename = list.fileName(i);
      Cube cube(filename, "r");

      try {
        Isis::Camera *cam = CameraFactory::Create(cube);
        p_cameraMap[serialNumber] = cam;
        p_cameraValidMeasuresMap[serialNumber] = 0;
        p_cameraRejectedMeasuresMap[serialNumber] = 0;
        p_cameraList.push_back(cam);
      }
      catch (IException &e) {
        QString msg = "Unable to create camera for cube file ";
        msg += filename;
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }

      if (progress != NULL)
        progress->CheckStatus();
    }

    // Loop through all measures and set the camera
    QHashIterator< QString, QSharedPointer<LidarControlPoint> > p(m_points);
    while (p.hasNext()) {
      p.next();
      LidarControlPointQsp curPoint = p.value();

      QList< QString > serialNums = curPoint->getCubeSerialNumbers();
      for (int m = 0; m < serialNums.size(); m++) {
        ControlMeasure *curMeasure = (*curPoint)[serialNums[m]];

        QString serialNumber = curMeasure->GetCubeSerialNumber();
        if (list.hasSerialNumber(serialNumber)) {
          curMeasure->SetCamera(p_cameraMap[serialNumber]);

          // increment number of measures for this image (camera)
          if (!curMeasure->IsIgnored()) p_cameraValidMeasuresMap[serialNumber]++;
        }
        else {
          IString msg = "Control point [" + curPoint->GetId() +
              "], measure [" + curMeasure->GetCubeSerialNumber() +
              "] does not have a cube with a matching serial number";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }
    }
  }


  /**
   * @brief Unserialize LidarData.
   *
   * This method unserializes LidarData from a JSON or binary (QByteArray) file. It will
   * automatically determine if it is JSON or binary formatted date.
   *
   * @param FileName lidarFile Name of the serialized LidarData file to read.
   * @throws IException::User Throws User exception if it cannot open the file passed.
   */
  void LidarData::read(FileName lidarDataFile) {
    // Set up the input file
    QFile loadFile(lidarDataFile.expanded());
    // Make sure we can open the file successfully
    if (!loadFile.open(QIODevice::ReadOnly)) {
      QString msg("Could not open " + loadFile.fileName());
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Load file
    QByteArray saveData = loadFile.readAll();
    // Try to load from JSON (ASCII); if it can not, load as binary.
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    if (loadDoc.isNull()) {
      loadDoc = QJsonDocument::fromBinaryData(saveData);
    }

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
            QSharedPointer<LidarControlPoint>(new LidarControlPoint());
        lcp->SetId(id);
        lcp->setTime(iTime(time));
        lcp->setRange(range);
        lcp->setSigmaRange(sigmaRange);
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
   * @brief Serializes LidarData.
   *
   * This method serializes the LidarData to either a JSON or binary (QByteArray) file. If JSON,
   * the file extension will be .json; otherwise (if binary), the file extension will be .dat.
   *
   * @param FileName outputFile Name of the file to serialize to.
   * @param LidarData::Format format Format of the serialized file (Json or Binary).
   *
   * @throws IException::User Throws User exception if it cannot open the file for writing.
   */
  void LidarData::write(FileName outputFile, LidarData::Format format) {
    // Set up the output file
    if (format == Json) {
      outputFile = outputFile.setExtension("json");
    }
    else {
      outputFile = outputFile.setExtension("dat");
    }
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
    if (format == Json) {
      saveFile.write(lidarDataDoc.toJson());
    }
    else {
      saveFile.write(lidarDataDoc.toBinaryData());
    }
  }
}
