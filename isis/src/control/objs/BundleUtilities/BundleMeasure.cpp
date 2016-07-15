#include "BundleMeasure.h"
#include "BundleObservation.h"
#include "BundleObservationSolveSettings.h"

#include "ControlMeasure.h"

namespace Isis {

  /**
   * Constructor
   *
   * Constructs a BundleMeasure from a ControlMeasure with the passed BundleControlPoint as its
   * parent control point 
   *
   * @param controlMeasure Pointer to the ControlMeasure to store
   * @param bundleControlPoint Pointer to the BundleControlPoint that contains this
   *                           BundleMeasure
   */
  BundleMeasure::BundleMeasure(ControlMeasure *controlMeasure,
                               BundleControlPoint *bundleControlPoint) {
    m_controlMeasure = controlMeasure;
    m_parentControlPoint = bundleControlPoint;

    m_parentBundleImage = NULL;
    m_parentObservation = NULL;
  }


  /**
   * Destructor
   */
  BundleMeasure::~BundleMeasure() {
  }


  /**
   * Copy constructor
   *
   * Constructs a BundleMeasure from another BundleMeasure
   *
   * @param src The source BundleMeasure to copy
   */
  BundleMeasure::BundleMeasure(const BundleMeasure &src) {
    m_controlMeasure = src.m_controlMeasure;
    m_parentControlPoint = src.m_parentControlPoint;
    m_parentBundleImage = src.m_parentBundleImage;
    m_parentObservation = src.m_parentObservation;
  }


  /**
   * Assignment operator
   *
   * Assigns the state of this BundleMeasure from another BundleMeasure
   *
   * @param src The source BundleMeasure to assign state from 
   *
   * @return @b BundleMeasure& Returns a reference to this BundleMeasure
   */
  BundleMeasure &BundleMeasure::operator=(const BundleMeasure &src) {
    // Prevent self assignment
    if (this != &src) {
      m_controlMeasure = src.m_controlMeasure;
      m_parentControlPoint = src.m_parentControlPoint;
      m_parentBundleImage = src.m_parentBundleImage;
      m_parentObservation = src.m_parentObservation;
    }

    return *this;
  }


  /**
   * Sets the parent bundle observation
   *
   * @param observation Pointer to the parent BundleObservation  
   */
  void BundleMeasure::setParentObservation(BundleObservation *observation) {
    m_parentObservation = observation;
  }


  /**
   * Determines whether or not this BundleMeasure is rejected
   *
   * @return @b bool Returns a boolean indicating whether this BundleMeasure is rejected
   */
  bool BundleMeasure::isRejected() {
    return m_controlMeasure->IsRejected();
  }


  /**
   * Accesses the associated camera for this bundle measure
   *
   * @see ControlMeasure::camera()
   *
   * @return @b Camera* Returns a pointer to the camera associated with this bundle measure
   */
  Camera *BundleMeasure::camera() {
    return m_controlMeasure->Camera();
  }


  /**
   * Accesses the parent BundleControlPoint for this bundle measure
   *
   * @return @b BundleControlPoint* Returns a pointer to the parent BundleControlPoint
   */
  BundleControlPoint *BundleMeasure::parentControlPoint() {
    return m_parentControlPoint;
  }


  /**
   * Access the parent BundleImage for this bundle measure
   *
   * @return @b BundleImage* Returns a pointer to the parent BundleImage
   */
  BundleImage *BundleMeasure::parentBundleImage() {
    return m_parentBundleImage;
  }


  /**
   * Accesses the parent BundleObservation for this bundle measure
   *
   * @return @b BundleObservation* Returns a pointer to the parent BundleObservation
   */
  BundleObservation *BundleMeasure::parentBundleObservation() {
    return m_parentObservation;
  }


  /**
   * Accesses the parent observation's solve settings
   *
   * @see BundleObservation::solveSettings()
   *
   * @return @b const BundleObservationSolveSettings* Returns a const pointer to the 
   *     BundleObservationSolveSettings for the parent BundleObservation
   */
  const BundleObservationSolveSettings *BundleMeasure::observationSolveSettings() {
    return m_parentObservation->solveSettings();
  }


  /**
   * Accesses the current sample measurement for this control measure
   *
   * @see ControlMeasure::GetSample()
   *
   * @return @b double Returns the sample measurement for this control measure 
   */
  double BundleMeasure::sample() const {
    return m_controlMeasure->GetSample();
  }


  /**
   * Accesses the current line measurement for this control measure
   *
   * @see ControlMeasure::GetLine()
   *
   * @return @b double Returns the line measurement for this control measure 
   */
  double BundleMeasure::line() const {
    return m_controlMeasure->GetLine();
  }


  /**
   * Accesses the serial number of the cube containing this control measure 
   *
   * @see ControlMeasure::GetCubeSerialNumber()
   * 
   * @return @b QString Returns the serial number of the cube that contains this control measure
   */
  QString BundleMeasure::cubeSerialNumber() const {
    return m_controlMeasure->GetCubeSerialNumber();
  }


  /**
   * Accesses the measured focal plane x value for this control measure //TODO verify?
   *
   * @see ControlMeasure::GetFocalPlaneMeasuredX()
   *
   * @return @b double Returns the measured focal plane x value
   */
  double BundleMeasure::focalPlaneMeasuredX() const {
    return m_controlMeasure->GetFocalPlaneMeasuredX();
  }


  /**
   * Accesses the measured focal plane y value for this control measure  //TODO verify?
   *
   * @see ControlMeasure::GetFocalPlaneMeasuredY()
   *
   * @return @b double Returns the measured focal plane y value
   */
  double BundleMeasure::focalPlaneMeasuredY() const {
    return m_controlMeasure->GetFocalPlaneMeasuredY();
  }

  /**
   * Accesses the observation index for the parent observation
   *
   * @see BundleObservation::index()
   *
   * @return @b int Returns the observation index of the parent observation
   */
  int BundleMeasure::observationIndex() const {
    return m_parentObservation->index();
  }



}
