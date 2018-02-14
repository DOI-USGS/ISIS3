#ifndef BundleObservationVector_h
#define BundleObservationVector_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/22 01:35:17 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include "BundleImage.h"
#include "BundleObservation.h"
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
   *   @history 2017-07-14 Ken Edmundson Added support for piecewise polynomials...
   *                           -methods...
   *                               int numberContinuityConstraintEquations()
   *   @history 2017-11-01 Ken Edmundson Additional support for piecewise polynomials...
   *                           -methods...
   *                               int numberPositionParameters()
   *                               int numberPointingParameters()
   *                               int numberParameters()
   *                               int numberPolynomialPositionSegments()
   *                               int numberPolynomialPointingSegments()
   *                               int numberPolynomialSegments()
   *   @history 2018-02-12 Ken Edmundson Renamed initializeBodyRotation method to setBodyRotation.
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

      int numberPositionParameters();
      int numberPointingParameters();
      int numberParameters();

      int numberPolynomialPositionSegments();
      int numberPolynomialPointingSegments();
      int numberPolynomialSegments();

      int numberContinuityConstraintEquations() const;

      BundleObservationQsp observationByCubeSerialNumber(QString cubeSerialNumber);

      bool initializeExteriorOrientation();
      bool setBodyRotation();

  private:
      //! Map between observation number and pointer to observation.
      QMap<QString, BundleObservationQsp> m_observationNumberToObservationMap;
      //! Map between image serial number and pointer to observation.
      QMap<QString, BundleObservationQsp> m_imageSerialToObservationMap;
  };
}

#endif // BundleObservationVector_h
