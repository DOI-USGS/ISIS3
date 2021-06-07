/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BundleImage.h"

#include "BundleObservation.h"
#include "Camera.h"

namespace Isis {


  /**
   * Constructs a BundleImage with the given camera pointer, serial number, and filename.
   * Parent observation defaults to NULL.
   *
   * @param camera The camera model for the BundleImage
   * @param serialNumber The serial number for the BundleImage
   * @param fileName The file name for the BundleImage
   */
  BundleImage::BundleImage(Camera *camera, QString serialNumber, QString fileName) {
    m_camera = camera;
    m_serialNumber = serialNumber;
    m_fileName = fileName;
  }


  /**
   * Constructs a BundleImage from another BundleImage object.
   *
   * @param other The BundleImage to copy from.
   */
  BundleImage::BundleImage(const BundleImage &other) {
    m_camera = other.m_camera;
    m_parentObservation = other.m_parentObservation;
    m_serialNumber = other.m_serialNumber;
    m_fileName = other.m_fileName;
  }


  /**
   * Destroys a BundleImage object.
   */
  BundleImage::~BundleImage() {
  }


  /**
   * An assignment operator to set this BundleImage to another BundleImage object.
   *
   * @param other The BundleImage to copy from.
   *
   * @return @b BundleImage& A reference to this BundleImage after being assigned to.
   */
  BundleImage &BundleImage::operator=(const BundleImage &other) {
    if (&other != this) {
      m_camera = other.m_camera;
      m_parentObservation = other.m_parentObservation;
      m_serialNumber = other.m_serialNumber;
      m_fileName = other.m_fileName;
    }
    return *this;
  }


  /**
   * Sets the parent BundleObservation object.
   *
   * @param parentObservation The parent BundleObservation.
   */
  void BundleImage::setParentObservation(QSharedPointer<BundleObservation> parentObservation) {


    // TODO: BundleImage's setParentObservation should take a QSharedPointer. JAM

    m_parentObservation = parentObservation;
  }


  /**
   * Returns the camera model used for the BundleImage.
   *
   * @return @b Camera* A pointer to the camera model.
   */
  Camera *BundleImage::camera() {
    return m_camera;
  }


  /**
   * Returns the parent BundleObservation object.
   *
   * @return @b QSharedPointer<BundleObservation> A pointer to the parent BundleObservation.
   */
  QSharedPointer<BundleObservation> BundleImage::parentObservation() {
    return m_parentObservation;
  }


  /**
   * Returns the serial number for the BundleImage.
   *
   * @return @b QString The image's serial number.
   */
  QString BundleImage::serialNumber() {
    return m_serialNumber;
  }


  /**
   * Returns the file name for the BundleImage.
   *
   * @return @b QString The image's file name.
   */
  QString BundleImage::fileName() {
    return m_fileName;
  }
}
