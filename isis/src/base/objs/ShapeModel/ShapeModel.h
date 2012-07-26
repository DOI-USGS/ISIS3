#ifndef ShapeModel_h
#define ShapeModel_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

#include "SurfacePoint.h"
#include "Pvl.h"

namespace Isis {
  /**
   * @brief Define shapes and provide utilities for Isis3 targets
   *
   * This class will define shapes of Isis3 target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup 
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal
   *  @history
   */
  class ShapeModel {
    public:
      //! Constructors
      ShapeModel();
      ShapeModel(Isis::Pvl &pvl);
      ShapeModel(Distance radii[3]);

      //! Initialization
      void Initialize();

      //! Destructor -- must be virtual in order to clean up properly because of virtual method below
      virtual ~ShapeModel()=0;

      //! Intersect the shape model
      virtual bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection,
                                    std::vector<double> targetPosition) = 0;

      //! Return the surface intersection
      SurfacePoint *surfaceIntersection();

      //! Calculate the surface normal of the current intersection point
      void CalculateSurfaceNormal(); 

      //! Return the surface normal of the current intersection point
      std::vector<double> SurfaceNormal();

      //! Calculate the phase angle of the current intersection point
      double PhaseAngle();

      //! Calculate the emission angle of the current intersection point
      double EmissionAngle();

      //! Calculate the incidence angle of the current intersection point
      double IncidenceAngle();

      //! Return local radius from shape model
      //      Distance LocalRadius(const SurfacePoint &pt);
      //      Distance LocalRadius(const Latitude &lat, const Longitude &lon);
      //      Distance LocalRadius() const;
      virtual Distance localRadius(const Latitude &lat, const Longitude &lon) = 0;

      //! Set tolerance for acceptance in iterative loops
      void setTolerance(const double tol);

      //! Return the tolerance
      double tolerance();

      //! Reset m_hasIntersection to false
      void reset();

      //! Return triaxial target radii from shape model TODO Put this in Target class
      Distance *targetRadii();

    protected:
      bool m_hasIntersection;              //!< Flag indicating a successful intersection has been found
      double *m_tolerance;

      bool hasIntersection();

      //! Intersect ellipse
      bool intersectEllipsoid(const std::vector<double> observerPosRelativeToTarget,
                              const std::vector<double> lookDirectionRelativeToTarget);

    private:
      void setRadii(Distance radii[3]);
      //void localRadius(const Latitude &lat, const Longitude &lon);  // Is this needed?
      // TODO Should this be a vector? If so add to destructor and modify constructor
      Distance m_radii[3];        //!< Radii of target body

      // Are these only needed for Isis3Simp model? Are they needed for photometric computations?
      SurfacePoint *m_surfacePoint;        //!< Current intersection point
      // This needs to get reset by cameras???
      bool m_hasNormal;                    //!< Flag indicating surface normal has been computed
      std::vector<double> m_surfaceNormal; //!< Surface normal of current intersection point
  };
};

#endif

