#include <string>
#include <algorithm>
#include <vector>
#include <cfloat>

#include <cmath>
#include <iomanip>

//#include "SimpShape.h"
#include "Cube.h"
#include "CubeManager.h"
#include "DemShape.h"
#include "EllipsoidShape.h"
#include "IException.h"
#include "Interpolator.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Portal.h"
#include "Projection.h"
#include "SurfacePoint.h"
#include "IException.h"
#include "NaifStatus.h"
#include "UniqueIOCachingAlgorithm.h"

using namespace std;

namespace Isis {
  /**
   * Initialize the Isis3 Dem from projection shape model.
   *
   * @param pvl Valid Isis3 cube label.
   */
  DemShape::DemShape(Target *target, Pvl &pvl) : ShapeModel (target, pvl) {
    setName("DemShape");
    m_demProj = NULL;
    m_demCube = NULL;
    m_interp = NULL;
    m_portal = NULL;

    // m_samples.resize(4,.0);
    // m_lines.resize(4,0.);

    PvlGroup &kernels = pvl.FindGroup("Kernels", Pvl::Traverse);

    iString demCubeFile;
    if (kernels.HasKeyword("ElevationModel")) {
      demCubeFile = (std::string) kernels["ElevationModel"];
    }
    else if(kernels.HasKeyword("ShapeModel")) {
      demCubeFile = (std::string) kernels["ShapeModel"];
    }

    m_demCube = CubeManager::Open(demCubeFile);

//    Isis::PvlGroup &mapGroup = m_demCube->getLabel()->FindGroup("Mapping", Isis::Pvl::Traverse);
    // std::string proj = mapGroup["ProjectionName"];
 
     // This caching algorithm works much better for DEMs than the default,
     //   regional. This is because the uniqueIOCachingAlgorithm keeps track
     //   of a history, which for something that isn't linearly processing a
     //   cube is worth it. The regional caching algorithm tosses out results
     //   from iteration 1 of setlookdirection (first algorithm) at iteration
     //   4 and the next setimage has to re-read the data.
      m_demCube->addCachingAlgorithm(new UniqueIOCachingAlgorithm(5));
      m_demProj = m_demCube->getProjection();
      m_interp = new Interpolator(Interpolator::BiLinearType);
      m_portal = new Portal(m_interp->Samples(), m_interp->Lines(),
                            m_demCube->getPixelType(),
                            m_interp->HotSample(), m_interp->HotLine());

      // Read in the Scale of the DEM file in pixels/degree
      const PvlGroup &mapgrp = m_demCube->getLabel()->FindGroup("Mapping", Pvl::Traverse);

    // Save map scale in pixels per degree
      m_pixPerDegree = (double) mapgrp["Scale"];
  }


  /**
   * Initialize the Isis3 Dem from projection shape model.
   *
   * @param pvl Valid Isis3 cube label.
   */
  DemShape::DemShape() : ShapeModel () {
    setName("DemShape");
    m_demProj = NULL;
    m_demCube = NULL;
    m_interp = NULL;
    m_portal = NULL;
  }


  //! Destroys the DemShape
  DemShape::~DemShape() {
    if(m_demProj) {
      m_demProj = NULL;
    }

    // We do not have ownership of p_demCube
    m_demCube = NULL;

    if(m_interp) {
      delete m_interp;
      m_interp = NULL;
    }

    if (m_portal) {
      delete m_portal;
      m_portal = NULL;
    }
  }


  /** Find the intersection point with the DEM
   *
   * 
   * TIDBIT:  From the code below we have historically tested to see if we can
   * first intersect the ellipsoid. If not then we assumed that we could not
   * intersect the DEM. This of course isn't always true as the DEM could be
   * above the surface of the ellipsoid. For most images we really wouldn't
   * notice. It has recently (Aug 2011) come into play trying to intersect
   * images containing a limb and spiceinit'ed with a DEM (e.g., Vesta and soon
   * Mercury). This implies that info at the limb will not always be computed.
   * In the future we may want to do a better job handling this special case.
   */
  bool DemShape::intersectSurface(vector<double> observerPos,
                                  vector<double> lookDirection,
                                  double tol) {
    // try to intersect the target body ellipsoid as a first approximation
    // for the iterative DEM intersection method
    // (this method is in the ShapeModel base class)
    if (intersectEllipsoid(observerPos, lookDirection))
      setHasIntersection(true);
    else 
      return false;
 
    static const int maxit = 100;
    int it = 1;
    double dX, dY, dZ, dist2;
    bool done = false;
    
    // latitude, longitude in Decimal Degrees
    double latDD, lonDD;
    
    // in each iteration, the current surface intersect point is saved for
    // comparison with the new, updated surface intersect point
    SpiceDouble currentIntersectPt[3];
    SpiceDouble newIntersectPt[3];

    // initialize updated surface intersection point to the ellipsoid
    // intersection point coordinates
    newIntersectPt[0] = surfaceIntersection()->GetX().kilometers();
    newIntersectPt[1] = surfaceIntersection()->GetY().kilometers();
    newIntersectPt[2] = surfaceIntersection()->GetZ().kilometers();

    double tol2 = tol * tol;

    while (!done) {
        
      if (it > maxit) {
        setHasIntersection(false);
        done = true;
        continue;
      }

      // The lat/lon calculations are done here by hand for speed & efficiency
      //   With doing it in the SurfacePoint class using p_surfacePoint, there
      //   is a 24% slowdown (which is significant in this very tightly looped
      //   call).
      double t = newIntersectPt[0] * newIntersectPt[0] +
          newIntersectPt[1] * newIntersectPt[1];
      
      latDD = atan2(newIntersectPt[2], sqrt(t)) * RAD2DEG;
      lonDD = atan2(newIntersectPt[1], newIntersectPt[0]) * RAD2DEG;
       
      if (lonDD < 0)
        lonDD += 360;

      // Previous Sensor version used local version of this method with lat and lon doubles. ..Why Jeff???
      Distance radiusKm = localRadius(Latitude(latDD, Angle::Degrees),
                                      Longitude(lonDD, Angle::Degrees));

      if (Isis::IsSpecial(radiusKm.kilometers())) {
        setHasIntersection(false);
        return false;
      }

      // save current surface intersect point for comparison with new, updated
      // surface intersect point
      memcpy(currentIntersectPt, newIntersectPt, 3 * sizeof(double));
      
      double r = radiusKm.kilometers();
      bool status;
      surfpt_c(&observerPos[0], &lookDirection[0], r, r, r, newIntersectPt,
               (SpiceBoolean*) &status);
      setHasIntersection(status);

      if (!status)
        return status;

      dX = currentIntersectPt[0] - newIntersectPt[0];
      dY = currentIntersectPt[1] - newIntersectPt[1];
      dZ = currentIntersectPt[2] - newIntersectPt[2];
      dist2 = (dX*dX + dY*dY + dZ*dZ) * 1000 * 1000;

      // Getting resolution is a problem **TODO** How do we update the tolerance?
      // if (dist < tolerance * tolerance) {
      //   // Now recompute tolerance at updated surface point and recheck
      //   p_surfacePoint->FromNaifArray(pBOutput);
      //   tolerance = Resolution() / 100.0;
      if (dist2 < tol2)
        done = true;

      it ++;
    } // end of while loop

    surfaceIntersection()->FromNaifArray(newIntersectPt);
    setHasIntersection(true);

    return true;
  }


  /**
  * Gets the radius from the DEM, if we have one.
  * @param lat Latitude
  * @param lon Longitude
  * @return @b double Local radius from the DEM
  */
  Distance DemShape::localRadius(const Latitude &lat, const Longitude &lon) {
    
    if (!lat.isValid() || !lon.isValid())
      return Distance();
    
    m_demProj->SetUniversalGround(lat.degrees(), lon.degrees());
    
    if (!m_demProj->IsGood())
      return Distance();

    m_portal->SetPosition(m_demProj->WorldX(), m_demProj->WorldY(), 1);

    m_demCube->read(*m_portal);

    const double &radius = m_interp->Interpolate(m_demProj->WorldX(),
                                                 m_demProj->WorldY(),
                                                 m_portal->DoubleBuffer());

    return Distance(radius, Distance::Meters);
  }


  /** 
   * Return pixels per degree
   */
    double DemShape::demScale () {
      return m_pixPerDegree;
    }


  /** 
   * Calculate default normal for the DemShape
   */
  void DemShape::calculateDefaultNormal()  {
    calculateEllipsoidalSurfaceNormal();
  }


  /** 
   * Return the dem Cube object
   */
  Cube *DemShape::demCube()  {
    return m_demCube;
  }


  /** 
   * Calculate local normal
   */
  void DemShape::calculateLocalNormal (QVector<double *> cornerNeighborPoints) {
    // subtract bottom from top and left from right and store results
    double topMinusBottom[3];
    vsub_c(cornerNeighborPoints[0], cornerNeighborPoints[1], topMinusBottom);
    double rightMinusLeft[3];
    vsub_c(cornerNeighborPoints[3], cornerNeighborPoints [2], rightMinusLeft);

    // take cross product of subtraction results to get normal
    std::vector<SpiceDouble> normal(3);
    ucrss_c(topMinusBottom, rightMinusLeft, (SpiceDouble *) &normal[0]);

    // unitize normal (and do sanity check for magnitude)
    double mag;
    unorm_c((SpiceDouble *) &normal[0], (SpiceDouble *) &normal[0], &mag);

    if (mag == 0.0) {
      normal[0] = 0.;
      normal[1] = 0.;
      normal[2] = 0.;
      setHasNormal(false);
   }
    else {
      setHasNormal(true);
    } 

    // Check to make sure that the normal is pointing outward from the planet
    // surface. This is done by taking the dot product of the normal and
    // any one of the unitized xyz vectors. If the normal is pointing inward,
    // then negate it.
    double centerLookVect[3];
    SpiceDouble pB[3];
    surfaceIntersection()->ToNaifArray(pB);
    unorm_c(pB, centerLookVect, &mag);
    double dotprod = vdot_c((SpiceDouble *) &normal[0], centerLookVect);
    if (dotprod < 0.0)
      vminus_c((SpiceDouble *) &normal[0], (SpiceDouble *) &normal[0]);
  
    setNormal(normal);
  }


  /** 
   * Calculate surface normal
   */
  void DemShape::calculateSurfaceNormal()  {
    calculateEllipsoidalSurfaceNormal();
  }


}
