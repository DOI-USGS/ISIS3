#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"

#include "ControlMeasure.h"

namespace Isis {

  /**
   * constructor
   */
  BundleMeasure::BundleMeasure(ControlMeasure *controlMeasure,
                               BundleControlPoint *bundleControlPoint) {
    m_controlMeasure = controlMeasure;
    m_parentControlPoint = bundleControlPoint;

    m_parentBundleImage = NULL;
    m_parentObservation = NULL;
  }


  /**
   * destructor
   */
  BundleMeasure::~BundleMeasure() {
  }


  /**
   * copy constructor
   */
  BundleMeasure::BundleMeasure(const BundleMeasure &src) {
    m_controlMeasure = src.m_controlMeasure;
    m_parentControlPoint = src.m_parentControlPoint;
    m_parentBundleImage = src.m_parentBundleImage;
    m_parentObservation = src.m_parentObservation;

  }


  /**
   * TODO
   */
  void BundleMeasure::setParentObservation(BundleObservation *observation) {
    m_parentObservation = observation;
  }


  /**
   * TODO
   */
  bool BundleMeasure::isRejected() {
    return m_controlMeasure->IsRejected();
  }


  /**
   * TODO
   */
  Camera *BundleMeasure::camera() {
    return m_controlMeasure->Camera();
  }


  /**
   * TODO
   */
  BundleControlPoint *BundleMeasure::parentControlPoint() {
    return m_parentControlPoint;
  }


  /**
   * TODO
   */
  BundleImage *BundleMeasure::parentBundleImage() {
    return m_parentBundleImage;
  }


  /**
   * TODO
   */
  BundleObservation *BundleMeasure::parentBundleObservation() {
    return m_parentObservation;
  }


  /**
   * TODO
   */
  const BundleObservationSolveSettings *BundleMeasure::observationSolveSettings() {
    return m_parentObservation->solveSettings();
  }


  /**
   * TODO
   */
  double BundleMeasure::sample() const {
    return m_controlMeasure->GetSample();
  }


  /**
   * TODO
   */
  double BundleMeasure::line() const {
    return m_controlMeasure->GetLine();
  }


  /**
   * TODO
   */
  QString BundleMeasure::cubeSerialNumber() const {
    return m_controlMeasure->GetCubeSerialNumber();
  }


  /**
   * TODO
   */
  double BundleMeasure::focalPlaneMeasuredX() const {
    return m_controlMeasure->GetFocalPlaneMeasuredX();
  }


  /**
   * TODO
   */
  double BundleMeasure::focalPlaneMeasuredY() const {
    return m_controlMeasure->GetFocalPlaneMeasuredY();
  }

  /**
   * TODO
   */
  int BundleMeasure::observationIndex() const {
    return m_parentObservation->index();
  }



}
