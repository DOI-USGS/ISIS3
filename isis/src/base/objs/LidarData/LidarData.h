#ifndef LidarData_h
#define LidarData_h

#include <QHash>
#include <QList>
#include <QPointer>
#include <QString>

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
   */
  class LidarData {

    LidarData();
    LidarData(FileName);

    QList< QSharedPointer<LidarControlPoint> > points() const;

    void read(FileName);
    void write(FileName);

    /** */
    QHash<QString, QSharedPointer <LidarControlPoint> > m_points;

  };

};
#endif
