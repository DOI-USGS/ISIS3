#include "LidarData.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QRegExp>
#include <QSharedPointer>
#include <QTextStream>
#include <QStringList>

#include "IException.h"
#include "iTime.h"
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
    return m_points.values();
  }


  /**
   * @brief Reads in a Lidar CSV file.
   *
   * @param FileName lidarFile Name of the Lidar CSV file to read.
   * @throws If the header for the file does not contain the correct info,
   *         an IException will be thrown.
   */
  void LidarData::read(FileName lidarFile) {

    QFile data(lidarFile.expanded());

    if (data.open(QIODevice::ReadOnly|QIODevice::Text)) {

      QTextStream datain(&data);
      QString header = datain.readLine();
      QStringList headerCells = header.split(",",QString::KeepEmptyParts);

      QRegExp utc("*Universal*",Qt::CaseInsensitive,QRegExp::Wildcard);
      QRegExp longitude("*Longitude",Qt::CaseInsensitive,QRegExp::Wildcard);
      QRegExp latitude("*Latitude",Qt::CaseInsensitive,QRegExp::Wildcard);
      QRegExp range("*Range",Qt::CaseInsensitive,QRegExp::Wildcard);
      QRegExp shot("S",Qt::CaseInsensitive,QRegExp::Wildcard);
      QRegExp frame("*Frm*",Qt::CaseInsensitive,QRegExp::Wildcard);

      int longIx = headerCells.indexOf(longitude);
      int latIx = headerCells.indexOf(latitude);
      int utcIx = headerCells.indexOf(utc);
      int rangeIx = headerCells.indexOf(range);
      int shotIx = headerCells.indexOf(shot);
      int frameIx = headerCells.indexOf(frame);
      if ( longIx < 0 || latIx < 0 || utcIx < 0 || rangeIx < 0 ||shotIx < 0 || frameIx < 0) {
        QString msg = "Header line in "+lidarFile.expanded() + " is missing expected information.";
          throw IException(IException::Io,msg, _FILEINFO_);
        }

      while (!datain.atEnd() ) {
        QString key;
        QString row = datain.readLine();
        QStringList cells = row.split(",",QString::KeepEmptyParts);
        iTime time(cells[utcIx]);
        QString shot(cells[shotIx]);
        QString frame(cells[frameIx]);
        key = "lrolola"+time.EtString()+"_"+frame+"_"+shot;        
        QSharedPointer<LidarControlPoint> pt = QSharedPointer<LidarControlPoint>
            (new LidarControlPoint(time,cells[rangeIx].toDouble(),0.0));

        m_points.insert(key,pt);
      }
    }
    return;
  }


  /**
   * Writes out the Lidar data to a CSV file.
   *
   * @param FileName outputFile Name of the file to write to.
   */
  void LidarData::write(FileName outputFile) {

  }


}
