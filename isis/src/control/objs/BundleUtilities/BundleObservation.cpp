/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BundleObservation.h"

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
   * Constructs a BundleObservation initialized to a default state.
   */
  BundleObservation::BundleObservation() {
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
   * Constructs a BundleObservation from an BundleImage, an instrument id, an observation
   * number to assign to this BundleObservation, and a target body.
   *
   * @param image QSharedPointer to the primary image in the observation
   * @param observationNumber Observation number of the observation
   * @param instrumentId Id of the instrument for the observation
   * @param bundleTargetBody QSharedPointer to the target body of the observation
   */
  BundleObservation::BundleObservation(BundleImageQsp image, QString observationNumber,
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
   * Creates a copy of another BundleObservation.
   *
   * @param src Reference to the BundleObservation to copy
   */
  BundleObservation::BundleObservation(const BundleObservation &src) {
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
  BundleObservation::~BundleObservation() {
    clear();
  }


  /**
   * Assignment operator
   *
   * Assigns the state of the source BundleObservation to this BundleObservation
   *
   * @param BundleObservation Reference to the source BundleObservation to assign from
   *
   * @return @b BundleObservation& Reference to this BundleObservation
   */
  BundleObservation &BundleObservation::operator=(const BundleObservation &src) {
    if (&src != this) {
      m_serialNumbers = src.m_serialNumbers;
      m_cubeSerialNumberToBundleImageMap = src.m_cubeSerialNumberToBundleImageMap;

      m_observationNumber = src.m_observationNumber;
      m_instrumentId = src.m_instrumentId;

    }

    return *this;
  }


  /**
   * Appends a BundleImage shared pointer to the BundleObservation.
   * If the pointer is valid, then the BundleImage and its serial number will be inserted into
   * the serial number to BundleImage map.
   *
   * @param value The BundleImage to be appended.
   *
   * @see QVector::append()
   */
  void BundleObservation::append(const BundleImageQsp &value) {
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
  BundleImageQsp BundleObservation::imageByCubeSerialNumber(QString cubeSerialNumber) {
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
  QString BundleObservation::instrumentId() {
    return m_instrumentId;
  }


  /**
   * Accesses the solve parameter weights
   *
   * @return @b LinearAlgebra::Vector Returns the parameter weights for solving
   */
  LinearAlgebra::Vector &BundleObservation::parameterWeights() {
    return m_weights;
  }


  /**
   * Accesses the parameter corrections
   *
   * @return @b LinearAlgebra::Vector Returns the parameter corrections
   */
  LinearAlgebra::Vector &BundleObservation::parameterCorrections() {
    return m_corrections;
  }


  /**
   * Accesses the a priori sigmas
   *
   * @return @b LinearAlgebra::Vector Returns the a priori sigmas
   */
  LinearAlgebra::Vector &BundleObservation::aprioriSigmas() {
    return m_aprioriSigmas;
  }


  /**
   * Accesses the adjusted sigmas
   *
   * @return @b LinearAlgebra::Vector Returns the adjusted sigmas
   */
  LinearAlgebra::Vector &BundleObservation::adjustedSigmas() {
    return m_adjustedSigmas;
  }


  /**
   * Sets the index for the observation
   *
   * @param n Value to set the index of the observation to
   */
  void BundleObservation::setIndex(int n) {
    m_index = n;
  }


  /**
   * Accesses the observation's index
   *
   * @return @b int Returns the observation's index
   */
  int BundleObservation::index() {
    return m_index;
  }


  /**
   * Access to image names for CorrelationMatrix to use.
   *
   * @return @b QStringList Returns a QStringList of the image names
   */
  QStringList BundleObservation::imageNames() {
    return m_imageNames;
  }


  /**
   * Compute vtpv, the weighted sum of squares of constrained image parameter residuals.
   *
   * @return double Weighted sum of squares of constrained image parameter residuals.
   */
  double BundleObservation::vtpv() {
    double vtpv = 0.0;

    for (int i = 0; i < (int)m_corrections.size(); i++) {
      if (m_weights[i] > 0.0) {
        vtpv += m_corrections[i]* m_corrections[i] * m_weights[i];
      }

    }

    return vtpv;
  }
}
