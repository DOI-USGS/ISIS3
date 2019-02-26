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

#include "LidarControlPoint.h"

class QJsonObject;

namespace Isis {
  class Camera;
  class FileName;
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
   *   @history 2018-06-14 Ken Edmundson - Added typedef for QSharedPointer to LidarData object.
   *   @history 2019-02-23 Debbie A. Cook - Added sorting option to points() method with default 
   *                           being to not sort. References #5343.
   *
   */
  class LidarData {

    public:
      /** Enumerates the file formats for serializing the LidarData class. */
      enum Format {
        Binary, /**< Serializes to a binary (QByteArray) .dat file. */
        Json,    /**< Serializes to a JSON .json file. */
        Test    /**< Serializes to an ordered JSON .json file for comparing to truth data. */
      };

      LidarData();

      void insert(QSharedPointer<LidarControlPoint> point);

      QList< QSharedPointer<LidarControlPoint> > points(bool sort = false) const;

      void SetImages(SerialNumberList &list, Progress *progress = 0);

      // Serialization methods for LidarData
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

  // typedefs
  //! Definition for a shared pointer to a LidarData object.
  typedef QSharedPointer<LidarData> LidarDataQsp;
}
#endif
