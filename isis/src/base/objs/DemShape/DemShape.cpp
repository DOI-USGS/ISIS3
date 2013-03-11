#include "DemShape.h"

// Qt third party includes
#include <QVector>

// c standard library third party includes
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

// naif third party includes
#include <naif/SpiceUsr.h>
#include <naif/SpiceZfc.h>
#include <naif/SpiceZmc.h>

#include "Cube.h"
#include "CubeManager.h"
#include "EllipsoidShape.h"
#include "IException.h"
#include "Interpolator.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Portal.h"
#include "Projection.h"
#include "Pvl.h"
#include "SurfacePoint.h"
#include "UniqueIOCachingAlgorithm.h"

using namespace std;

namespace Isis {
  /**
   * Construct a DemShape object. This method creates a ShapeModel object 
   * named "DemShape" and initializes member variables from the projection 
   * shape model using the given Target and Pvl. 
   *
   * @param pvl Valid Isis3 cube label.
   */
  DemShape::DemShape(Target *target, Pvl &pvl) : ShapeModel (target, pvl) {
    setName("DemShape");
    m_demProj = NULL;
    m_demCube = NULL;
    m_interp = NULL;
    m_portal = NULL;

    PvlGroup &kernels = pvl.findGroup("Kernels", Pvl::Traverse);

    QString demCubeFile;
    if (kernels.hasKeyword("ElevationModel")) {
      demCubeFile = (QString) kernels["ElevationModel"];
    }
    else if(kernels.hasKeyword("ShapeModel")) {
      demCubeFile = (QString) kernels["ShapeModel"];
    }

    m_demCube = CubeManager::Open(demCubeFile);
 
    // This caching algorithm works much better for DEMs than the default,
    //   regional. This is because the uniqueIOCachingAlgorithm keeps track
    //   of a history, which for something that isn't linearly processing a
    //   cube is worth it. The regional caching algorithm tosses out results
    //   from iteration 1 of setlookdirection (first algorithm) at iteration
    //   4 and the next setimage has to re-read the data.
    m_demCube->addCachingAlgorithm(new UniqueIOCachingAlgorithm(5));
    m_demProj = m_demCube->projection();
    m_interp = new Interpolator(Interpolator::BiLinearType);
    m_portal = new Portal(m_interp->Samples(), m_interp->Lines(),
                            m_demCube->pixelType(),
                            m_interp->HotSample(), m_interp->HotLine());

    // Read in the Scale of the DEM file in pixels/degree
    const PvlGroup &mapgrp = m_demCube->label()->findGroup("Mapping", Pvl::Traverse);

    // Save map scale in pixels per degree
    m_pixPerDegree = (double) mapgrp["Scale"];
  }


  /**
   * Construct a DemShape object. This method creates a ShapeModel object named 
   * "DemShape". The member variables are set to Null. 
   *
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
    m_demProj = NULL;

    // We do not have ownership of p_demCube
    m_demCube = NULL;

    delete m_interp;
    m_interp = NULL;

    delete m_portal;
    m_portal = NULL;
  }


  /** 
   * Find the intersection point with the DEM
   * 
   * TIDBIT:  From the code below we have historically tested to see if we can
   * first intersect the ellipsoid. If not then we assumed that we could not
   * intersect the DEM. This of course isn't always true as the DEM could be
   * above the surface of the ellipsoid. For most images we really wouldn't
   * notice. It has recently (Aug 2011) come into play trying to intersect
   * images containing a limb and spiceinit'ed with a DEM (e.g., Vesta and soon
   * Mercury). This implies that info at the limb will not always be computed.
   * In the future we may want to do a better job handling this special case. 
   *  
   * @param observerPos
   * @param lookDirection
   *  
   * @return Indicates whether the intersection was found.
   */
  bool DemShape::intersectSurface(vector<double> observerPos,
                                  vector<double> lookDirection) {
    // try to intersect the target body ellipsoid as a first approximation
    // for the iterative DEM intersection method
    // (this method is in the ShapeModel base class)
    if (!intersectEllipsoid(observerPos, lookDirection))
      return false;
 
    double tol = resolution()/100;  // 1/100 of a pixel
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

      // Previous Sensor version used local version of this method with lat and lon doubles. 
      // Steven made the change to improve speed.  He said the difference was negilgible.  
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
      surfpt_c((SpiceDouble *) &observerPos[0], &lookDirection[0], r, r, r, newIntersectPt,
               (SpiceBoolean*) &status);
      setHasIntersection(status);

      if (!status)
        return status;

      dX = currentIntersectPt[0] - newIntersectPt[0];
      dY = currentIntersectPt[1] - newIntersectPt[1];
      dZ = currentIntersectPt[2] - newIntersectPt[2];
      dist2 = (dX*dX + dY*dY + dZ*dZ) * 1000 * 1000;

      // Now recompute tolerance at updated surface point and recheck
      if (dist2 < tol2) {
        surfaceIntersection()->FromNaifArray(newIntersectPt);
        tol = resolution() / 100.0;
        tol2 = tol * tol;
        if (dist2 < tol2) {
          setHasIntersection(true);
          done = true;
        }
      }

      it ++;
    } // end of while loop

    return hasIntersection();
  }


  /**
   * Gets the radius from the DEM, if we have one.
   *  
   * @param lat Latitude
   * @param lon Longitude
   *  
   * @return @b double Local radius from the DEM
   */
  Distance DemShape::localRadius(const Latitude &lat, const Longitude &lon) {
    
    Distance distance=Distance();

    if (lat.isValid() && lon.isValid()) {
      m_demProj->SetUniversalGround(lat.degrees(), lon.degrees());
    
      // The next if statement attempts to do the same as the previous one, but not as well so 
      // it was replaced.
      // if (!m_demProj->IsGood())
      //   return Distance();

      m_portal->SetPosition(m_demProj->WorldX(), m_demProj->WorldY(), 1);

      m_demCube->read(*m_portal);

      distance = Distance(m_interp->Interpolate(m_demProj->WorldX(),
                                                m_demProj->WorldY(),
                                                m_portal->DoubleBuffer()),
                                                Distance::Meters);
    }

    return distance;
  }


  /** 
   * Return the scale of the DEM shape, in pixels per degree.
   *  
   * @return The scale of the DEM
   */
  double DemShape::demScale() {
    return m_pixPerDegree;
  }


  /** 
   * This method calculates the default normal (Ellipsoid for backwards 
   * compatability) for the DemShape. 
   */
  void DemShape::calculateDefaultNormal() {
    calculateEllipsoidalSurfaceNormal();
  }


  /** 
   * Returns the DEM Cube object.
   *  
   * @return The DEM cube associated with this shape model.
   */
  Cube *DemShape::demCube() {
    return m_demCube;
  }


  /** 
   * This method calculates the local surface normal of the current intersection 
   * point. 
   *  
   * @param neighborPoints
   */
  void DemShape::calculateLocalNormal(QVector<double *> neighborPoints) {
    // subtract bottom from top and left from right and store results
    double topMinusBottom[3];
    vsub_c(neighborPoints[0], neighborPoints[1], topMinusBottom);
    double rightMinusLeft[3];
    vsub_c(neighborPoints[3], neighborPoints [2], rightMinusLeft);

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
      return;
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
   * This method calculates the surface normal of the current intersection 
   * point. 
   */
  void DemShape::calculateSurfaceNormal() {
    calculateEllipsoidalSurfaceNormal();
  }


}
