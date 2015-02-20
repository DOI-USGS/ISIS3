#include "BundleImage.h"

#include "BundleObservation.h"
#include "Camera.h"

namespace Isis {


  /**
   * constructor
   */
  BundleImage::BundleImage(Camera *camera, QString serialNumber, QString fileName) {
    m_camera = camera;
    m_parentObservation = NULL;
    m_serialNumber = serialNumber;
    m_fileName = fileName;    
  }



  /**
   * copy constructor
   */
  BundleImage::BundleImage(const BundleImage &src) {
    m_camera = src.m_camera;
    m_parentObservation = src.m_parentObservation;
    m_serialNumber = src.m_serialNumber;
    m_fileName = src.m_fileName;
  }



  /**
   * destructor
   */
  BundleImage::~BundleImage() {
  }



  BundleImage &BundleImage::operator=(const BundleImage &src) {
    if (&src != this) {
      m_camera = src.m_camera;
      m_parentObservation = src.m_parentObservation;
      m_serialNumber = src.m_serialNumber;
      m_fileName = src.m_fileName;
    }
    return *this;
  }



  void BundleImage::setParentObservation(BundleObservation *parentObservation) {
    m_parentObservation = parentObservation;
  }



  Camera *BundleImage::camera() {
    return m_camera;
  }



  BundleObservation *BundleImage::parentObservation() {
    return m_parentObservation;
  }



  QString BundleImage::serialNumber() {
    return m_serialNumber;
  }



  QString BundleImage::fileName() {
    return m_fileName;
  }
}
