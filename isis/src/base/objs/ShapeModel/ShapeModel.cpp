#include <algorithm>
#include <cfloat>

#include <cmath>
#include <iomanip>

#include "ShapeModel.h"
#include "SurfacePoint.h"
#include "IException.h"
#include "NaifStatus.h"

namespace Isis {
  /**
   * default constructor
   */
  ShapeModel::ShapeModel() {
    Initialize();
  }


  /**
   * Initialize the ShapeModel private variables.
   *
   * @param pvl Valid Isis3 cube label.
   */
  ShapeModel::ShapeModel(Pvl &pvl) {
    Initialize();
  }


  /**
   * Initialize the ShapeModel private variables.
   *
   * @param pvl Valid Isis3 cube label.
   */
  ShapeModel::ShapeModel(Distance radii[3]) {
    Initialize();
    setRadii(radii);
  }


  /**
   * Initialize the ShapeModel private variables.
   */
  void ShapeModel::Initialize() {
    m_name = new std::string();
    m_name = NULL;
    m_surfacePoint = NULL;
    m_tolerance = new double();
    *m_tolerance = 0.0;
    m_radii[0] = m_radii[1] = m_radii[2] = Distance();
    m_hasIntersection = false;
    m_hasNormal = false;
    m_surfaceNormal.resize(3,0.);
  }

  //! Destroys the ShapeModel
  ShapeModel::~ShapeModel() {

    if (m_surfacePoint) {
      delete m_surfacePoint;
      m_surfacePoint = NULL;
    }

    if (m_tolerance) {
      delete m_tolerance;
      m_tolerance = NULL;
    }
  }


  /** Find the intersection point on the ellipsoid model
   * 
   */
  bool ShapeModel::intersectEllipsoid(const std::vector<double> observerBodyFixedPosition,
      const std::vector<double> &observerLookVectorToTarget) {
    SpiceDouble lookB[3];

    // This memcpy does:
    // lookB[0] = observerLookVectorToTarget[0];
    // lookB[1] = observerLookVectorToTarget[1];
    // lookB[2] = observerLookVectorToTarget[2];
    memcpy(lookB,&observerLookVectorToTarget[0], 3*sizeof(double));

    // get target radii
    SpiceDouble a = m_radii[0].kilometers();
    SpiceDouble b = m_radii[1].kilometers();
    SpiceDouble c = m_radii[2].kilometers();

    // check if observer look vector intersects the target
    SpiceDouble intersectionPoint[3];
    SpiceBoolean bIntersected = false;
    surfpt_c((SpiceDouble *) &observerBodyFixedPosition[0], lookB, a, b, c,
             intersectionPoint, &bIntersected);

    NaifStatus::CheckErrors();
    
    if (bIntersected) {
      m_surfacePoint = new SurfacePoint(); 
      m_surfacePoint->FromNaifArray(intersectionPoint);
    }

    return true;
  }


  /** Return the surface intersection
   *
   */
  SurfacePoint *ShapeModel::surfaceIntersection() {
    return m_surfacePoint;
  }


  /** Return intersection status
   *
   */
  bool ShapeModel::hasIntersection() {
    return m_hasIntersection;
  }


  /** Return surface point normal status
   *
   */
  bool ShapeModel::hasNormal() {
    return m_hasNormal;
  }


  /** Return the surface normal of the current intersection point.
   *
   */
  std::vector<double> ShapeModel::surfaceNormal() {
    if ( m_hasNormal )
      return surfaceNormal();
    std::string message = "The surface normal has not been computed.";
    throw IException(IException::Unknown, message, _FILEINFO_);
  }


  /** Calculate the emission angle at the current intersection point.
   *
   * This method returns the emission angle at the current intersection point.
   *
   */
  //virtual double ShapeModel::EmissionAngle() {
  //  if ( !m_hasNormal ) SurfaceNormal();
  //  double emissionAngle = 0.0;
  //  return emissionAngle;
  //}


  /** Calculate the incidence angle at the current intersection point.
   *
   * This method returns the incidence angle at the current intersection point.
   *
   */
  //virtual double ShapeModel::IncidenceAngle() {
  //  if ( !m_hasNormal ) SurfaceNormal();
  //  double incidenceAngle = 0.0;
  //  return incidenceAngle;
  //}
  
  
  /** Calculate the phase angle at the current intersection point.
   *
   * This method returns the phase angle at the current intersection point.
   *
   */
  //virtual double ShapeModel::PhaseAngle() {
  //  if ( !m_hasNormal ) SurfaceNormal();
  //  double phaseAngle=0.0;
  //  return phaseAngle;
  //}
  
  
  /** Calculate the phase angle at the current intersection point.
   *
   * This method returns the phase angle at the current intersection point.
   *
   */


  /** Set the shape name
   *
   */
  void ShapeModel::setName(const std::string name) {
    *m_name = name;
  }


  /** Get the shape name
   *
   */
  std::string ShapeModel::name() const{
    return *m_name;
  }


  /** Set m_hasIntersection;
   *
   */
  void ShapeModel::setHasSurfaceIntersection(bool b) {
    m_hasIntersection  = b;
  }


  /** Set surface intersection point
   *
   */
  void ShapeModel::setSurfaceIntersectionPoint(const SurfacePoint &surfacePoint) {
    *m_surfacePoint  = surfacePoint;
  }


  /** Set  tolerance for acceptance in iterative loops
   *
   */
  void ShapeModel::setTolerance(const double tol) {
    *m_tolerance = tol;
  }


  /** Return triaxial target radii from shape model
   *
   */
  Distance *ShapeModel::targetRadii() {
     return  m_radii;
  }


  /** Return the tolerance for acceptance in iterative loops
   *
   */
  double ShapeModel::tolerance() {
    return *m_tolerance;
  }


  /** Set the radii
   *
   */
  void ShapeModel::setRadii(Distance radii[3]) {
    m_radii[0] = radii[0];
    m_radii[1] = radii[1];
    m_radii[2] = radii[2];
  }


}
