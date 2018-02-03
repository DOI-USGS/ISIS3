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
   *   @history 2018-01-31 Ian Humphrey - Added insert method to insert a LidarControlPoint into
   *                           the LidarData. Added documentation for m_points.
   */
  class LidarData {

    public:
      enum Format {
        Binary,
        Json
      };

      LidarData();
      LidarData(FileName);

      void readCsv(FileName);

      void insert(QSharedPointer<LidarControlPoint> point);
      QList< QSharedPointer<LidarControlPoint> > points() const;

      void read(FileName);
      void write(FileName, Format);

    private:
      /** Hash of the LidarControlPoints this class contains. */
      QHash<QString, QSharedPointer <LidarControlPoint> > m_points;

  };

};
#endif
