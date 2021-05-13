/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BundleObservationVector.h"

#include <QDebug>

#include "BundleObservation.h"
#include "IException.h"

namespace Isis {

  /**
   * Constructs an empty BundleObservationVector.
   */
  BundleObservationVector::BundleObservationVector() {
  }


  /**
   * Copy constructor.
   *
   * Constructs a BundleObservationVector as a copy of another BundleObservationVector.
   *
   * @param src A reference to the BundleObservationVector to copy from.
   */
  BundleObservationVector::BundleObservationVector(const BundleObservationVector &src)
      :QVector<BundleObservationQsp>(src) {
    m_observationNumberToObservationMap = src.m_observationNumberToObservationMap;
    m_imageSerialToObservationMap = src.m_imageSerialToObservationMap;
  }


  /**
   * Destructor.
   *
   * Contained BundleObservations will remain until all shared pointers to them are deleted.
   */
  BundleObservationVector::~BundleObservationVector() {
    clear();
  }


  /**
   * Assignment operator.
   *
   * Assigns the state of the source BundleObservationVector to this BundleObservationVector.
   *
   * @param src The BundleObservationVector to assign from.
   *
   * @return @b BundleObservationVector& A reference to this BundleObservationVector.
   */
  BundleObservationVector &BundleObservationVector::operator=(const BundleObservationVector &src) {
    if (&src != this) {
      QVector<BundleObservationQsp>::operator=(src);
      m_observationNumberToObservationMap = src.m_observationNumberToObservationMap;
      m_imageSerialToObservationMap = src.m_imageSerialToObservationMap;
    }
    return *this;
  }


  /**
   * Adds a new BundleObservation to this vector or fetches an existing BundleObservation if this
   * vector already contains the passed observation number.
   *
   * If observation mode is off, adds a new BundleObservation to this vector. Otherwise, if
   * observation mode is on and the BundleObservation has already been added with the same
   * observation number, the passed BundleImage is added to the existing BundleObservation.
   *
   * @param bundleImage QSharedPointer to the BundleImage to create or fetch a BundleObservation
   *                    from
   * @param observationNumber Observation number of the observation to add or fetch
   * @param instrumentId Instrument id of the observation
   * @param bundleSettings Qsp to BundleSettings for the observation
   *
   * @throws IException::Programmer "Unable to allocate new BundleObservation"
   *
   * @return @b BundleObservationQsp Returns a pointer to the BundleObservation that was added
   *
   * @internal
   *   @history 2016-10-13 Ian Humphrey - When appending a new BundleObservation and there are
   *                           multiple BundleObservationSolveSettings, we set the settings
   *                           according to the observation number passed. References #4293.
   */
  BundleObservationQsp BundleObservationVector::addNew(BundleImageQsp bundleImage,
                                                       QString observationNumber,
                                                       QString instrumentId,
                                                       BundleSettingsQsp bundleSettings) {
    BundleObservationQsp bundleObservation;
    bool addToExisting = false;

    if (bundleSettings->solveObservationMode() &&
        m_observationNumberToObservationMap.contains(observationNumber)) {
      bundleObservation = m_observationNumberToObservationMap.value(observationNumber);

      addToExisting = true;
    }

    if (addToExisting) {
      // if we have already added a BundleObservation with this number, we have to add the new
      // BundleImage to this observation
      bundleObservation->append(bundleImage);

      bundleImage->setParentObservation(bundleObservation);

      // update observation number to observation ptr map
      m_observationNumberToObservationMap.insertMulti(observationNumber,bundleObservation);

      // update image serial number to observation ptr map
      m_imageSerialToObservationMap.insertMulti(bundleImage->serialNumber(),bundleObservation);
    }
    else {
      // create new BundleObservation and append to this vector
      bundleObservation = BundleObservationQsp(new BundleObservation(bundleImage,
                                                                     observationNumber,
                                                                     instrumentId,
                                               bundleSettings->bundleTargetBody()));

      if (!bundleObservation) {
        QString message = "Unable to allocate new BundleObservation ";
        message += "for " + bundleImage->fileName();
        throw IException(IException::Programmer, message, _FILEINFO_);
      }

      bundleImage->setParentObservation(bundleObservation);

      // Find the bundle observation solve settings for this new observation
      BundleObservationSolveSettings solveSettings;
      // When there is only one bundle observation solve setting, use it for all observations
      if ( bundleSettings->numberSolveSettings() == 1) {
        solveSettings = bundleSettings->observationSolveSettings(0);
      }
      // Otherwise, we want to grab the bundle observation solve settings that is associated with
      // the observation number for this new observation
      else {
        solveSettings = bundleSettings->observationSolveSettings(observationNumber);
      }

      bundleObservation->setSolveSettings(solveSettings);

      bundleObservation->setIndex(size());

      append(bundleObservation);

      // update observation number to observation ptr map
      m_observationNumberToObservationMap.insertMulti(observationNumber, bundleObservation);

      // update image serial number to observation ptr map
      m_imageSerialToObservationMap.insertMulti(bundleImage->serialNumber(), bundleObservation);
    }

    return bundleObservation;
  }


  /**
   * Accesses the number of position parameters for the contained BundleObservations.
   *
   * @return @b int Returns the total number of position parameters for the BundleObservations
   */
  int BundleObservationVector::numberPositionParameters() {
    int positionParameters = 0;

    for (int i = 0; i < size(); i++) {
      BundleObservationQsp observation = at(i);
      positionParameters += observation->numberPositionParameters();
    }

    return positionParameters;
  }


  /**
   * Accesses the number of pointing parameters for the contained BundleObservations.
   *
   * @return @b int Returns the total number of pointing parameters for the BundleObservations
   */
  int BundleObservationVector::numberPointingParameters() {
    int pointingParameters = 0;

    for (int i = 0; i < size(); i++) {
      BundleObservationQsp observation = at(i);
      pointingParameters += observation->numberPointingParameters();
    }

    return pointingParameters;
  }


  /**
   * Returns the sum of the position parameters and pointing parameters for the contained
   * BundleObservations.
   *
   * @return @b int Returns the total number of parameters for the contained BundleObservations
   */
  int BundleObservationVector::numberParameters() {
    return numberPositionParameters() + numberPointingParameters();
  }


  /**
   * Accesses a BundleObservation associated with the passed serial number.
   *
   * If there is no BundleObservation associated with the serial number, a NULL
   * pointer is returned.
   *
   * @param cubeSerialNumber Serial number of a cube to try to find an associated BundleObservation
   *
   * @return @b BundleObservationQsp Pointer to the associated BundleObservation (NULL if not found)
   */
  BundleObservationQsp BundleObservationVector::
      observationByCubeSerialNumber(QString cubeSerialNumber) {
    BundleObservationQsp bundleObservation;

    if (m_imageSerialToObservationMap.contains(cubeSerialNumber))
      bundleObservation = m_imageSerialToObservationMap.value(cubeSerialNumber);

    return bundleObservation;
  }


  /**
   * Initializes the exterior orientations for the contained BundleObservations.
   *
   * @return @b bool Returns true upon successful initialization
   */
  bool BundleObservationVector::initializeExteriorOrientation() {
    int nObservations = size();
    for (int i = 0; i < nObservations; i++) {
      BundleObservationQsp observation = at(i);
      observation->initializeExteriorOrientation();
    }

    return true;
  }


  /**
   * Initializes the body rotations for the contained BundleObservations.
   *
   * @return @b bool Returns true upon successful initialization
   */
  bool BundleObservationVector::initializeBodyRotation() {
    int nObservations = size();
    for (int i = 0; i < nObservations; i++) {
      BundleObservationQsp observation = at(i);
      observation->initializeBodyRotation();
    }

    return true;
  }
}
