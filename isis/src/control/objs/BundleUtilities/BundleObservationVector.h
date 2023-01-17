#ifndef BundleObservationVector_h
#define BundleObservationVector_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include "BundleImage.h"
#include "BundleObservation.h"
#include "IsisBundleObservation.h"
#include "CsmBundleObservation.h"
#include "BundleSettings.h"

namespace Isis {


  /**
   * This class is a container class for BundleObservations. It can be used for managing multiple
   * BundleObservations.
   *
   * Contained BundleObservations are stored as shared pointers, so they will be automatically
   * deleted when all shared pointers are deleted.
   *
   * @author 2014-05-22 Ken Edmundson
   *
   * @internal
   *   @history 2014-05-22 Ken Edmundson - Original version.
   *   @history 2015-02-20 Jeannie Backer - Brought closer to ISIS coding standards.
   *   @history 2016-06-30 Jeannie Backer - Changed method name from
   *                           "getObservationByCubeSerialNumber" to
   *                           "observationByCubeSerialNumber" to comply with
   *                           ISIS coding standards. References #4078.
   *   @history 2016-08-03 Jesse Mapel - Changed contained member type to a vector of
   *                           QSharedPointers.  Removed commented out block of paths.
   *                           Fixes #4150, #4155.
   *   @history 2016-08-03 Ian Humphrey - Updated documentation and coding standards. Replaced
   *                           getObservationByCubeSerialNumber with observationByCubeSerialNumber.
   *   @history 2016-08-11 Jesse Mapel - Defined assignment operator and copy constructor.
   *                           Fixes #4159.
   *   @history 2016-10-13 Ian Humphrey - Modified addnew so that we set solve settings based
   *                           on the BundleObsevation's observation number. Renamed addnew to
   *                           addNew(). References #4293.
   *   @history 2018-02-12 Ken Edmundson Renamed initializeBodyRotation method to setBodyRotation.
   *   @history 2018-11-29 Ken Edmundson Modified addNew method. Removed setBodyRotation method.
   */
  class BundleObservationVector : public QVector<BundleObservationQsp> {

    public:
      BundleObservationVector();
      BundleObservationVector(const BundleObservationVector &src);
      ~BundleObservationVector();

      BundleObservationVector &operator=(const BundleObservationVector &src);
      BundleObservationQsp addNew(BundleImageQsp image,
                                  QString observationNumber,
                                  QString instrumentId,
                                  BundleSettingsQsp bundleSettings);

      int numberParameters();

      BundleObservationQsp observationByCubeSerialNumber(QString cubeSerialNumber);

      double vtpvContribution();

      QList<QString> instrumentIds() const;
      QList<BundleObservationQsp> observationsByInstId(QString instrumentId) const;

  private:
      //! Map between observation number and pointer to observation.
      QMap<QString, BundleObservationQsp> m_observationNumberToObservationMap;
      //! Map between image serial number and pointer to observation.
      QMap<QString, BundleObservationQsp> m_imageSerialToObservationMap;
      //! Map between instrument ID and pointer to observation.
      QMultiMap<QString, BundleObservationQsp> m_instIdToObservationMap;
  };
}

#endif // BundleObservationVector_h
