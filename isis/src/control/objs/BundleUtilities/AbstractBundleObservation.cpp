/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "AbstractBundleObservation.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>

#include "BundleImage.h"
#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "Camera.h"
#include "LinearAlgebra.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"

using namespace std;

namespace Isis {

  /**
   * Constructs a AbstractBundleObservation initialized to a default state.
   */
  AbstractBundleObservation::AbstractBundleObservation() {
    m_serialNumbers.clear();
    m_imageNames.clear();
    m_observationNumber = "";
    m_instrumentId = "";
    m_index = 0;
    m_weights.clear();
    m_corrections.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();
  }


  /**
   * Constructs a AbstractBundleObservation from an BundleImage, an instrument id, an observation
   * number to assign to this AbstractBundleObservation, and a target body.
   *
   * @param image QSharedPointer to the primary image in the observation
   * @param observationNumber Observation number of the observation
   * @param instrumentId Id of the instrument for the observation
   * @param bundleTargetBody QSharedPointer to the target body of the observation
   */
  AbstractBundleObservation::AbstractBundleObservation(BundleImageQsp image, QString observationNumber,
                                       QString instrumentId, BundleTargetBodyQsp bundleTargetBody) {
    m_serialNumbers.clear();
    m_imageNames.clear();
    m_observationNumber = "";
    m_instrumentId = "";
    m_index = 0;
    m_weights.clear();
    m_corrections.clear();
    m_aprioriSigmas.clear();
    m_adjustedSigmas.clear();

    m_observationNumber = observationNumber;
    m_instrumentId = instrumentId;

    if (image) {
      append(image);
      m_serialNumbers.append(image->serialNumber());
      m_imageNames.append(image->fileName());
      m_cubeSerialNumberToBundleImageMap.insert(image->serialNumber(), image);
    }
  }


  /**
   * Creates a copy of another AbstractBundleObservation.
   *
   * @param src Reference to the AbstractBundleObservation to copy
   */
  AbstractBundleObservation::AbstractBundleObservation(const AbstractBundleObservation &src) {
    m_serialNumbers = src.m_serialNumbers;
    m_cubeSerialNumberToBundleImageMap = src.m_cubeSerialNumberToBundleImageMap;

    m_observationNumber = src.m_observationNumber;
    m_instrumentId = src.m_instrumentId;

    m_index = src.m_index;
  }


  /**
   * Destructor.
   *
   * Contained BundleImages will remain until all shared pointers are deleted.
   */
  AbstractBundleObservation::~AbstractBundleObservation() {
    clear();
  }


  /**
   * Assignment operator
   *
   * Assigns the state of the source AbstractBundleObservation to this AbstractBundleObservation
   *
   * @param AbstractBundleObservation Reference to the source AbstractBundleObservation to assign from
   *
   * @return @b AbstractBundleObservation& Reference to this AbstractBundleObservation
   */
  AbstractBundleObservation &AbstractBundleObservation::operator=(const AbstractBundleObservation &src) {
    if (&src != this) {
      m_serialNumbers = src.m_serialNumbers;
      m_cubeSerialNumberToBundleImageMap = src.m_cubeSerialNumberToBundleImageMap;

      m_observationNumber = src.m_observationNumber;
      m_instrumentId = src.m_instrumentId;

    }

    return *this;
  }


  /**
   * Appends a BundleImage shared pointer to the AbstractBundleObservation.
   * If the pointer is valid, then the BundleImage and its serial number will be inserted into
   * the serial number to BundleImage map.
   *
   * @param value The BundleImage to be appended.
   *
   * @see QVector::append()
   */
  void AbstractBundleObservation::append(const BundleImageQsp &value) {
    if (value) {
      m_cubeSerialNumberToBundleImageMap.insert(value->serialNumber(), value);
    }
    QVector<BundleImageQsp>::append(value);
  }


  /**
   * Returns the BundleImage shared pointer associated with the given serial number.
   * If no BundleImage with that serial number is contained a NULL pointer is returned.
   *
   * @param cubeSerialNumber The serial number of the cube to be returned.
   *
   * @return @b BundleImageQsp A shared pointer to the BundleImage (NULL if not found).
   */
  BundleImageQsp AbstractBundleObservation::imageByCubeSerialNumber(QString cubeSerialNumber) {
    BundleImageQsp bundleImage;

    if (m_cubeSerialNumberToBundleImageMap.contains(cubeSerialNumber)) {
      bundleImage = m_cubeSerialNumberToBundleImageMap.value(cubeSerialNumber);
    }

    return bundleImage;
  }


  /**
   * Accesses the instrument id
   *
   * @return @b QString Returns the instrument id of the observation
   */
  QString AbstractBundleObservation::instrumentId() {
    return m_instrumentId;
  }


  /**
   * Accesses the solve parameter weights
   *
   * @return @b LinearAlgebra::Vector Returns the parameter weights for solving
   */
  LinearAlgebra::Vector &AbstractBundleObservation::parameterWeights() {
    return m_weights;
  }


  /**
   * Accesses the parameter corrections
   *
   * @return @b LinearAlgebra::Vector Returns the parameter corrections
   */
  LinearAlgebra::Vector &AbstractBundleObservation::parameterCorrections() {
    return m_corrections;
  }


  /**
   * Accesses the a priori sigmas
   *
   * @return @b LinearAlgebra::Vector Returns the a priori sigmas
   */
  LinearAlgebra::Vector &AbstractBundleObservation::aprioriSigmas() {
    return m_aprioriSigmas;
  }


  /**
   * Accesses the adjusted sigmas
   *
   * @return @b LinearAlgebra::Vector Returns the adjusted sigmas
   */
  LinearAlgebra::Vector &AbstractBundleObservation::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * Applies the parameter corrections
   *
   * @param corrections Vector of corrections to apply
   *
   * @throws IException::Unknown "Instrument position is NULL, but position solve option is
   *                              [not NoPositionFactors]"
   * @throws IException::Unknown "Instrument position is NULL, but pointing solve option is
   *                              [not NoPointingFactors]"
   * @throws IException::Unknown "Unable to apply parameter corrections to AbstractBundleObservation."
   *
   * @return @b bool Returns true upon successful application of corrections
   *
   * @internal
   *   @todo always returns true?
   */
// FIXME: can this work at parent level or should be pure virtual?
  bool AbstractBundleObservation::applyParameterCorrections(LinearAlgebra::Vector corrections) {
    return false;
  }


  /**
   * Sets the index for the observation
   *
   * @param n Value to set the index of the observation to
   */
  void AbstractBundleObservation::setIndex(int n) {
    m_index = n;
  }


  /**
   * Accesses the observation's index
   *
   * @return @b int Returns the observation's index
   */
  int AbstractBundleObservation::index() {
    return m_index;
  }


  /**
   * Access to image names for CorrelationMatrix to use.
   *
   * @return @b QStringList Returns a QStringList of the image names
   */
  QStringList AbstractBundleObservation::imageNames() {
    return m_imageNames;
  }
}
