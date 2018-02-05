#ifndef LidarData_h
#define LidarData_h

#include <QHash>
#include <QList>
#include <QPointer>
#include <QString>

class QJsonObject;

namespace Isis {

  class FileName;
  class LidarControlPoint;

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
   */
  class LidarData {

    public:
      /** Enumerates the file formats for serializing the LidarData class. */
      enum Format {
        Binary, /**< Serializes to a binary (QByteArray) .dat file. */
        Json    /**< Serializes to a JSON .json file. */
      };

      LidarData();
      LidarData(FileName);

      void readCsv(FileName);

      void insert(QSharedPointer<LidarControlPoint> point);
      QList< QSharedPointer<LidarControlPoint> > points() const;

      // Serialization methods or LidarData
      void read(FileName);
      void write(FileName, Format);

    private:
      /** Hash of the LidarControlPoints this class contains. */
      QHash<QString, QSharedPointer <LidarControlPoint> > m_points;

  };

};
#endif
