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

#define RTD 180.0/PI

using namespace std;

namespace Isis {
  /**
   * Initialize the Isis3 Dem from projection shape model.
   *
   * @param pvl Valid Isis3 cube label.
   */
  DemShape::DemShape(Pvl &pvl) : ShapeModel (pvl) {
    std::cout << "Making Isis3 Dem shape" << std::endl;
    setName("DemShape");
    m_demProj = NULL;
    m_demCube = NULL;
    m_interp = NULL;
    m_portal = NULL;
    m_demCubeFile = "";

    m_samples.resize(4,.0);
    m_lines.resize(4,0.);

    PvlGroup &kernels = pvl.FindGroup("Kernels", Pvl::Traverse);

    if (kernels.HasKeyword("ElevationModel")) {
      m_demCubeFile = (std::string) kernels["ElevationModel"];
    }
    else if(kernels.HasKeyword("ShapeModel")) {
      m_demCubeFile = (std::string) kernels["ShapeModel"];
    }

    m_demCube = CubeManager::Open(m_demCubeFile);

    m_demLabel = m_demCube->getLabel();
//    Isis::PvlGroup &mapGroup = m_demLabel->FindGroup("Mapping", Isis::Pvl::Traverse);
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
      const PvlGroup &mapgrp = m_demLabel->FindGroup("Mapping", Pvl::Traverse);

    // Save map scale in pixels per degree
      m_pixPerDegree = (double) mapgrp["Scale"];
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
                                  vector<double> lookDirection) {

    // try to intersect the target body ellipsoid as a first approximation
    // for the iterative DEM intersection method
    // (this method is in the ShapeModel base class)
    if (intersectEllipsoid(observerPos, lookDirection))
      m_hasIntersection = true;
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

    double tol2 = tolerance();
    tol2 *= tol2;

    while (!done) {
        
      if (it > maxit) {
        m_hasIntersection = false;
        done = true;
        continue;
      }

      // The lat/lon calculations are done here by hand for speed & efficiency
      //   With doing it in the SurfacePoint class using p_surfacePoint, there
      //   is a 24% slowdown (which is significant in this very tightly looped
      //   call).
      double t = newIntersectPt[0] * newIntersectPt[0] +
          newIntersectPt[1] * newIntersectPt[1];
      
      latDD = atan2(newIntersectPt[2], sqrt(t)) * RTD;
      lonDD = atan2(newIntersectPt[1], newIntersectPt[0]) * RTD;
       
      if (lonDD < 0)
        lonDD += 360;

      Distance radiusKm = localRadius(Latitude(latDD, Angle::Degrees),
                                      Longitude(lonDD, Angle::Degrees));

      if (Isis::IsSpecial(radiusKm.kilometers())) {
        m_hasIntersection = false;
        return m_hasIntersection;
      }

      // save current surface intersect point for comparison with new, updated
      // surface intersect point
      memcpy(currentIntersectPt, newIntersectPt, 3 * sizeof(double));
      
      double r = radiusKm.kilometers();
      surfpt_c(&observerPos[0], &lookDirection[0], r, r, r, newIntersectPt,
               (SpiceBoolean*) &m_hasIntersection);

      if (!m_hasIntersection)
        return m_hasIntersection;

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
    m_hasIntersection = true;

    return m_hasIntersection;
  }


  /** Return pixels per degree
   *
   */
    double DemShape::demScale () {
      return m_pixPerDegree;
    }


}
