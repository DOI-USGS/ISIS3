#include "LidarData.h"

#include <QList>
#include <QSharedPointer>

#include "FileName.h"
#include "LidarControlPoint.h"

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
   * Gets the list of Lidar data points.
   *
   * @return @b QList<QSharedPointer<LidarControlPoint>> Returns list of Lidar control points.
   */
  QList<QSharedPointer <LidarControlPoint> > LidarData::points() const {
    // Placeholder code follows
    QSharedPointer<LidarControlPoint> placeHolder =
        QSharedPointer<LidarControlPoint>(new LidarControlPoint);
    QList<QSharedPointer< LidarControlPoint> > returnList;
    returnList.append(placeHolder);
    return returnList;
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

  }


}
