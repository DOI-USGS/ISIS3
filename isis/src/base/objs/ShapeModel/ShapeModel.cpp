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
    m_target = NULL;
  }


  /**
   * Initialize the ShapeModel private variables.
   *
   * @param pvl Valid Isis3 cube label.
   */
  ShapeModel::ShapeModel(Target *target, Pvl &pvl) {
    Initialize();
    m_target = target;
  }


  /**
   * Initialize the ShapeModel private variables.
   *
   * @param target Valid Isis3 target.
   */
  // ShapeModel::ShapeModel(Distance radii[3]) {
  ShapeModel::ShapeModel(Target *target) {
    Initialize();
    // setRadii(radii);
    m_target = target;
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
    // m_radii new Distance[3];
    m_hasIntersection = false;
    m_hasNormal = false;
    m_normal.resize(3,0.);
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


  /** Calculate ellipsoidal surface normal
   *
   */
  void ShapeModel::calculateEllipsoidalSurfaceNormal()  {
    // The below code is not truly normal unless the ellipsoid is a sphere.  TODO Should this be
    // fixed? Send an email asking Jeff and Stuart.  See Naif routine surfnm.c to get the true 
    // for an ellipsoid.  For testing purposes to match old results do as Isis3 currently does until
    // Jeff and Stuart respond.

   if ( !m_hasIntersection ) {
     Isis::iString msg = "A valid intersection must be defined before computing the surface normal";
      throw IException(IException::Programmer, msg, _FILEINFO_);
   }

   // Get the coordinates of the current surface point
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // SpiceDouble normal[3];
    // surfnm_c(kilometers(m_radii[0]), kilometers(m_radii[1]), kilometers(m_radii[2], pB, normal);

    // Unitize the vector
    SpiceDouble upB[3];
    SpiceDouble dist;
    unorm_c(pB, upB, &dist);
    memcpy(&m_normal[0], upB, sizeof(double) * 3);
    m_hasNormal = true;
  }


  /**
   * Returns the emission angle in degrees. 
   *
   * @return double
   */
  double ShapeModel::emissionAngle(const std::vector<double> & sB)  {

    // Calculate the surface normal if we haven't yet
    if (!hasNormal()) calculateDefaultNormal();

    // Get vector from center of body to surface point
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Get vector from surface point to observer and normalize it
    SpiceDouble psB[3], upsB[3], dist;
    vsub_c((ConstSpiceDouble *) &sB[0], pB, psB);
    unorm_c(psB, upsB, &dist);

    double angle = vdot_c((SpiceDouble *) &m_normal[0], upsB);
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * 180.0 / PI;
  }


  /**
   * Returns the incidence angle in degrees. This does not use the surface model.
   *
   * @return @b double Incidence angle, in degrees.
   */
  double ShapeModel::incidenceAngle(const std::vector<double> &uB) {

    // Calculate the surface normal if we haven't yet.
    if (!m_hasNormal) calculateDefaultNormal();

    // Get vector from center of body to surface point
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Get vector from surface point to sun and normalize it
    SpiceDouble puB[3], upuB[3], dist;
    vsub_c((SpiceDouble *) &uB[0], pB, puB);
    unorm_c(puB, upuB, &dist);

    double angle = vdot_c((SpiceDouble *) &m_normal[0], upuB);
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * 180.0 / PI;
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
    Distance *radii = m_target->radii();
    SpiceDouble a = radii[0].kilometers();
    SpiceDouble b = radii[1].kilometers();
    SpiceDouble c = radii[2].kilometers();

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


  /**
   * Returns the phase angle in degrees.
   *
   * @return @b double Phase angle, in degrees.
   */
  double ShapeModel::phaseAngle(const std::vector<double> & sB, const std::vector<double> &uB) {

    // Get vector from center of body to surface point
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Get vector from surface point to observer and normalize it
    SpiceDouble psB[3], upsB[3], dist;
    vsub_c((SpiceDouble *) &sB[0], pB, psB);
    unorm_c(psB, upsB, &dist);

    // Get vector from surface point to sun and normalize it
    SpiceDouble puB[3], upuB[3];
    vsub_c((SpiceDouble *) &uB[0], pB, puB);
    unorm_c(puB, upuB, &dist);

    double angle = vdot_c(upsB, upuB);
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * 180.0 / PI;
  }


  /** Return the surface intersection
   *
   */
  SurfacePoint *ShapeModel::surfaceIntersection() const {
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
  bool ShapeModel::hasNormal() const {
    return m_hasNormal;
  }


  /** Return the local normal of the current intersection point.
   *
   */
  void ShapeModel::normal(std::vector<double> normal) {
    if (m_hasNormal ) {
      std::vector<double> normal(3);
      normal = m_normal;
    }

    std::string message = "The local normal has not been computed.";
    throw IException(IException::Unknown, message, _FILEINFO_);
  }

  /**
   * Returns the radii of the body in km. The radii are obtained from the
   * target.
   */
  Distance *ShapeModel::targetRadii() const {
    return m_target->radii();
  }


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


  /** Set surface normal
   *
   */
  // void ShapeModel::setSurfaceNormal(const std::vector<double> normalB) const{
  //   m_surfaceNormal  = normalB;
  // }


  /** Set  tolerance for acceptance in iterative loops
   *
   */
  void ShapeModel::setTolerance(const double tol) {
    *m_tolerance = tol;
  }


  /** Return triaxial target radii from shape model
   *
   */
  // Distance *ShapeModel::targetRadii() {
  //    return  m_radii;
  // }


  /** Return the tolerance for acceptance in iterative loops
   *
   */
  double ShapeModel::tolerance() {
    return *m_tolerance;
  }


  /** Set the radii
   *
   */
  // void ShapeModel::setRadii(Distance radii[3]) {
  //   m_radii[0] = radii[0];
  //   m_radii[1] = radii[1];
  //   m_radii[2] = radii[2];
  // }


}
