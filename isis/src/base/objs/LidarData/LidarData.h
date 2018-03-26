#ifndef LidarData_h
#define LidarData_h

#include <QHash>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QString>
#include <QVector>

#include "boost/numeric/ublas/symmetric.hpp"
#include "boost/numeric/ublas/io.hpp"

class QJsonObject;

namespace Isis {
  class Camera;
  class FileName;
  class LidarControlPoint;
  class Progress;
  class SerialNumberList;

  /**
   * LidarData class.
   *
   * @author 2018-01-29 Ian Humphrey
   *
   * @internal
   *   @history 2018-01-29 Ian Humphrey - original version.
   *   @history 2018-01-31 Tyler Wilson - Implemented Lidar::read(Filename &).
   *   @history 2018-01-31 Ian Humphrey - Added insert method to insert a
   *                           LidarControlPoint into the LidarData. Added
   *                           documentation for m_points.
   *   @history 2018-02-03 Ian Humphrey - Renamed read to readCsv. read() and write()
   *                           methods support JSON or binary serialization. Added
   *                           documentation to new Format enumeration.
   *   @history 2018-03-19 Debbie A. Cook - Added simultaneousImages, 
   *                           apriori variance/covariance matrix, adjusted point coordinates,
   *                           and adjusted variance/covariance matrix to the read and
   *                           write methods. Ref #5343.
   *  
   */
  class LidarData {

    public:
      /** Enumerates the file formats for serializing the LidarData class. */
      enum Format {
        Binary, /**< Serializes to a binary (QByteArray) .dat file. */
        Json    /**< Serializes to a JSON .json file. */
      };

      LidarData();

      void insert(QSharedPointer<LidarControlPoint> point);
      QList< QSharedPointer<LidarControlPoint> > points() const;

      void SetImages(SerialNumberList &list, Progress *progress = 0);

      // Serialization methods or LidarData
      void read(FileName);
      void write(FileName, Format);

    private:
      /** Hash of the LidarControlPoints this class contains. */
      QHash<QString, QSharedPointer <LidarControlPoint> > m_points;

      QMap<QString, Isis::Camera *> p_cameraMap; //!< A map from serialnumber to camera
      QMap<QString, int> p_cameraValidMeasuresMap; //!< A map from serialnumber to #measures
      QMap<QString, int> p_cameraRejectedMeasuresMap; //!< A map from serialnumber to
      //!  #rejected measures
      QVector<Isis::Camera *> p_cameraList; //!< Vector of image number to camera
  };

};
#endif
