#include "ShapeModel.h"

#include <algorithm>
#include <cfloat>
#include <vector>

#include <cmath>

#include <naif/SpiceUsr.h>
#include <naif/SpiceZfc.h>
#include <naif/SpiceZmc.h>

#include "Distance.h"
#include "SurfacePoint.h"
#include "IException.h"
#include "IString.h"
#include "NaifStatus.h"
#include "Spice.h"
#include "Target.h"

using namespace std;

namespace Isis {
  /**
   * default constructor
   */
  ShapeModel::ShapeModel() {
    Initialize();
    m_target = NULL;
  }


  /**
   * Construct and load a shape model from a pvl
   *
   * @param pvl Valid Isis3 cube label.
   */
  ShapeModel::ShapeModel(Target *target, Pvl &pvl) {
    Initialize();
    m_target = target;
  }


  /**
   * Construct and load a shape model from a target only
   *
   * @param target Valid Isis3 target.
   */
  ShapeModel::ShapeModel(Target *target) {
    Initialize();
    m_target = target;
  }


  /**
   * Initialize the ShapeModel private variables.
   */
  void ShapeModel::Initialize() {
    m_name = new IString();
    m_surfacePoint = new SurfacePoint();
    m_hasIntersection = false;
    m_hasNormal = false;
    m_normal.resize(3,0.);
  }

  //! Destroys the ShapeModel
  ShapeModel::~ShapeModel() {

    delete m_name;
    m_name = NULL;

    delete m_surfacePoint;
    m_surfacePoint = NULL;
  }


  /** Calculate ellipsoidal surface normal
   *
   */
  void ShapeModel::calculateEllipsoidalSurfaceNormal()  {
    // The below code is not truly normal unless the ellipsoid is a sphere.  TODO Should this be
    // fixed? Send an email asking Jeff and Stuart.  See Naif routine surfnm.c to get the true 
    // for an ellipsoid.  For testing purposes to match old results do as Isis3 currently does until
    // Jeff and Stuart respond.

    if (!surfaceIntersection()->Valid() || !m_hasIntersection) {
     IString msg = "A valid intersection must be defined before computing the surface normal";
      throw IException(IException::Programmer, msg, _FILEINFO_);
   }

   // Get the coordinates of the current surface point
    SpiceDouble pB[3];
    pB[0] = surfaceIntersection()->GetX().kilometers();
    pB[1] = surfaceIntersection()->GetY().kilometers();
    pB[2] = surfaceIntersection()->GetZ().kilometers();

    // Unitize the vector
    SpiceDouble upB[3];
    SpiceDouble dist;
    unorm_c(pB, upB, &dist);
    memcpy(&m_normal[0], upB, sizeof(double) * 3);

    m_hasNormal = true;
  }


  /**
   * Computes and returns emission angle in degrees given the observer position.
   *
   * Emission Angle: The angle between the surface normal vector at the
   * intersection point and a vector from the intersection point to the
   * spacecraft. The emission angle varies from 0 degrees when the spacecraft is
   * viewing the sub-spacecraft point (nadir viewing) to 90 degrees when the
   * intercept is tangent to the surface of the target body. Thus, higher values
   * of emission angle indicate more oblique viewing of the target.
   *
   * @param sB: Spacecraft position in body-fixed coordinates
   *
   * @return Emmision angle in decimal degrees
   *
   */
  double ShapeModel::emissionAngle(const std::vector<double> & sB) {

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
    return acos(angle) * RAD2DEG;
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
    return acos(angle) * RAD2DEG;
  }


  /** Find the intersection point on the ellipsoid model
   * 
   */
  bool ShapeModel::intersectEllipsoid(const std::vector<double> observerBodyFixedPosition,
                              const std::vector<double> &observerLookVectorToTarget) {

    // Clear out previous surface point and normal
    clearSurfacePoint();

    SpiceDouble lookB[3];

    // This memcpy does:
    // lookB[0] = observerLookVectorToTarget[0];
    // lookB[1] = observerLookVectorToTarget[1];
    // lookB[2] = observerLookVectorToTarget[2];
    memcpy(lookB,&observerLookVectorToTarget[0], 3*sizeof(double));

    // get target radii
    std::vector<Distance> radii = targetRadii();
    SpiceDouble a = radii[0].kilometers();
    SpiceDouble b = radii[1].kilometers();
    SpiceDouble c = radii[2].kilometers();

    // check if observer look vector intersects the target
    SpiceDouble intersectionPoint[3];
    SpiceBoolean intersected = false;
    surfpt_c((SpiceDouble *) &observerBodyFixedPosition[0], lookB, a, b, c,
             intersectionPoint, &intersected);

    NaifStatus::CheckErrors();
    
    if (intersected) {
      m_surfacePoint->FromNaifArray(intersectionPoint);
      m_hasIntersection = true;
    }
    else {
      m_hasIntersection = false;
    }
   
    return m_hasIntersection;
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

    // How can these lines be tested???
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * RAD2DEG;
  }


  /** 
   * Return the surface intersection
   */
  SurfacePoint *ShapeModel::surfaceIntersection() const {
    return m_surfacePoint;
  }


  /** 
   * Return intersection status
   */
  bool ShapeModel::hasIntersection() {
    return m_hasIntersection;
  }


  /** 
   * Return surface point normal status
   */
  bool ShapeModel::hasNormal() const {
    return m_hasNormal;
  }


  /** 
   * Clear or reset the current surface point
   */
  void ShapeModel::clearSurfacePoint() {
    setHasIntersection(false);
  }


  /** 
   * Return the local normal of the current intersection point.
   *
   * @param returns normal vector if it exists
   */
  std::vector<double> ShapeModel::normal() {
    if (m_hasNormal ) {
      return m_normal;
    }
    else {
      IString message = "The local normal has not been computed.";
      throw IException(IException::Unknown, message, _FILEINFO_);
    }

  }


  /**
   * Returns the radii of the body in km. The radii are obtained from the
   * target.
   */
  std::vector<Distance> ShapeModel::targetRadii() const {
    return m_target->radii();
  }


  /** Set the normal for the currect intersection point
   *
   */
  void ShapeModel::setNormal(const std::vector<double> normal) {
    if (m_hasIntersection) {
      m_normal = normal;
    }
    else {
      IString message = "No intersection point in known.  A normal can not be set.";
      throw IException(IException::Unknown, message, _FILEINFO_);
    }
  }


  /** Set the shape name
   *
   */
  void ShapeModel::setName(const IString name) {
     *m_name = name;
  }


  /** Get the shape name
   *
   */
  IString ShapeModel::name() const{
    return *m_name;
  }


  /** Set m_hasIntersection;
   *
   */
  void ShapeModel::setHasIntersection(bool b) {
    m_hasIntersection  = b;
    m_hasNormal = false;
  }


  /** Set surface intersection point
   *
   */
  void ShapeModel::setSurfacePoint(const SurfacePoint &surfacePoint) {
    *m_surfacePoint  = surfacePoint;

    // Update status of intersection and normal
    m_hasIntersection  = true;
    // Set normal as not calculated
    setHasNormal(false);
  }


  /** Set the hasNormal flag
   *
   */
  void ShapeModel::setHasNormal(bool status) {
    m_hasNormal = status;
  }


  /** Convenience method to get pixel resolution (m/pix) at current intersection point
   *
   */
  double ShapeModel::resolution() {
    if (m_hasIntersection) {
//??? cout << "No scope lower case = " << (m_target->spice())->resolution() << endl;
//??? cout << "No scope lower case no parens = " << m_target->spice()->resolution() << endl;
//??? cout << "Spice with parens = " << (m_target->spice())->Spice::resolution() << endl;
//??? cout << "Camera = " << (m_target->spice())->Camera::Resolution() << endl;
//??? cout << "Sensor = " << (m_target->spice())->Sensor::Resolution() << endl;
      return m_target->spice()->resolution();
    }
    else {
      IString message = "No valid intersection point for computing resolution.";
      throw IException(IException::Programmer, message, _FILEINFO_);
    }
  }

}
