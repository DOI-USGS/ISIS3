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

#include <vector>

template<class T> class QVector;

class QString;

namespace Isis {
  class Distance;
  class Latitude;
  class Longitude;
  class Pvl;
  class Spice;
  class SurfacePoint;
  class Target;

  /**
   * @brief Define shapes and provide utilities for Isis3 targets
   *
   * This base class will define shapes of Isis3 target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal
   *   @history 2010-07-30 - Debbie A. Cook - Original version.
   *   @history 2012-10-25 - Jeannie Backer - Changed resolution() method and
   *                             call to Spice::resolution() method to lower
   *                             camel case. Added resolution() method test to
   *                             improve unitTest code coverage. References
   *                             #1181.
   *   @history 2012-11-14 - Jeannie Backer - Removed cout lines left in while
   *                             testing code. References #1181.
   *   @history 2012-12-21 - Debbie A. Cook - Added new members m_hasEllipsoidIntersection
   *                             and method hasEllipsoidIntersection().   Fixes Mantis ticket #1343
   */
  class ShapeModel {
    public:
      // Constructors
      ShapeModel();
      ShapeModel(Target *target, Isis::Pvl &pvl);
      ShapeModel(Target *target);

      // Initialization
      void Initialize();

      // Destructor -- must be virtual in order to clean up properly because of 
      // virtual method below
      virtual ~ShapeModel()=0;

      // Intersect the shape model
      virtual bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection)=0;

      // Return the surface intersection
      SurfacePoint *surfaceIntersection() const;

      bool hasIntersection();
      bool hasNormal() const;

      // Calculate the default normal of the current intersection point
      virtual void calculateDefaultNormal() = 0; 

      // Calculate the local normal of the current intersection point 
      // (relative to neighbor points)
      virtual void calculateLocalNormal(QVector<double *> neighborPoints) = 0; 

      // Calculate the surface normal of the current intersection point 
      // (relative to ellipsoid)
      virtual void calculateSurfaceNormal() = 0; 

      // Clear current point
      void clearSurfacePoint();

      // Calculate the emission angle of the current intersection point
      virtual double emissionAngle(const std::vector<double> & sB);

      // Calculate the incidence angle of the current intersection point
      virtual double incidenceAngle(const std::vector<double> &uB);

      // Calculate the phase angle of the current intersection point
      virtual double phaseAngle(const std::vector<double> &sB, 
                                const std::vector<double> &uB);

      // Return local radius from shape model
      virtual Distance localRadius(const Latitude &lat, const Longitude &lon) = 0;

      // Get shape name
      QString name() const;

      // Set m_hasIntersection
      void setHasIntersection(bool b);

      // Set current surface point
      void setSurfacePoint(const SurfacePoint &surfacePoint);

      // Return the normal (surface or local) of the current intersection point
      std::vector<double>  normal();

    protected:

      // Set the normal (surface or local) of the current intersection point
      void setNormal(const std::vector<double>);

      // Set shape name
      void setName(QString name);
 
      void calculateEllipsoidalSurfaceNormal();
      bool hasEllipsoidIntersection();

      // Intersect ellipse
      bool intersectEllipsoid(const std::vector<double> observerPosRelativeToTarget,
                              const std::vector<double> &observerLookVectorToTarget);
      std::vector<Distance> targetRadii() const;
      void setHasNormal(bool status);
      double resolution();

    private:
      bool m_hasEllipsoidIntersection;  // Indicates the ellipsoid was successfully intersected
      bool m_hasIntersection;       //!< indicates good intersection exists
      bool m_hasNormal;             //!< indicates normal has been computed
      std::vector<double> m_normal; //!< Local normal of current intersection point
      QString *m_name;
      SurfacePoint *m_surfacePoint; //!< Current intersection point

      Target *m_target;
  };
};

#endif

