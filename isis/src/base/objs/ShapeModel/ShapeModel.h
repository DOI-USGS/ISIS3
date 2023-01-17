#ifndef ShapeModel_h
#define ShapeModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
   * @brief Define shapes and provide utilities for Isis targets
   *
   * This base class will define shapes of Isis target bodies as well as provide utilities to
   * retrieve radii and photometric information.
   *
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal
   *   @history 2010-07-30 Debbie A. Cook - Original version.
   *   @history 2012-10-25 Jeannie Backer - Changed resolution() method and call to
   *                           Spice::resolution() method to lower camel case. Added resolution()
   *                           method test to improve unitTest code coverage. References #1181.
   *   @history 2012-10-31 Ken Edmundson - Added another SetNormal method and fixed original to
   *                           set the m_hasNormal to true.
   *   @history 2012-11-14 Jeannie Backer - Removed cout lines left in while testing code.
   *                           References #1181.
   *   @history 2012-12-21 Debbie A. Cook - Added new members m_hasEllipsoidIntersection and
   *                           method hasEllipsoidIntersection(). Fixes Mantis ticket #1343
   *   @history 2015-04-30 Jeannie Backer - Added pure virtual isDEM() method. References #2243.
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                           were signaled. References #2248
   *   @history 2015-10-01 Jeannie Backer - Made improvements to documentation and brought code
   *                           closer to ISIS coding standards. References #1438
   *   @history 2016-06-13 Kelvin Rodriguez - Removed redundant contructor ShapeModel(Target, Pvl).
   *                           Fixes #2214
   *   @history 2017-05-19 Tyler Wilson - Removed the calculateEllipsoidalSurfaceNormal() function,
   *                           as this is now being handled in the EllipsoidShape class.
   *                           References #1028.
   *   @history 2017-03-23 Kris Becker - added isVisibleFrom() and two
   *                            intersectSurface() methods to address real
   *                            occlusions. It is recommended that derived
   *                            models reimplement this method! Also made
   *                            setSurfacePoint() & clearSurfacePoint() virtual
   *                            to give some hope of a consistent internal state
   *                            in derived models.
   */
  class ShapeModel {
    public:
      // Constructors
      ShapeModel();
      ShapeModel(Target *target);

      // Initialization
      void Initialize();

      // Destructor -- must be virtual in order to clean up properly because of
      // virtual method below
      virtual ~ShapeModel()=0;

      // Intersect the shape model
      virtual bool intersectSurface(std::vector<double> observerPos,
                                    std::vector<double> lookDirection)=0;

      // These two methods are for optional testing of occlusions when checking
      // specific locations on the body from the observer. The first uses
      // localRadius() by default and so may be OK as is.
      virtual bool intersectSurface(const Latitude &lat, const Longitude &lon,
                                    const std::vector<double> &observerPos,
                                    const bool &backCheck = true);
      virtual bool intersectSurface(const SurfacePoint &surfpt,
                                    const std::vector<double> &observerPos,
                                    const bool &backCheck = true);



      // Return the surface intersection
      virtual SurfacePoint *surfaceIntersection() const;

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
      virtual void clearSurfacePoint();

      // Calculate the emission angle of the current intersection point
      virtual double emissionAngle(const std::vector<double> & sB);

      // Calculate the incidence angle of the current intersection point
      virtual double incidenceAngle(const std::vector<double> &uB);

      // Calculate the phase angle of the current intersection point
      virtual double phaseAngle(const std::vector<double> &sB,
                                const std::vector<double> &uB);

      // Return local radius from shape model
      virtual Distance localRadius(const Latitude &lat, const Longitude &lon) = 0;

      /**
       * Indicates whether this shape model is from a DEM. This method is used to
       * determine whether the Camera class will calculate the local normal using
       * neighbor points. This method is pure virtual and must be implemented by
       * all ShapeModel classes.  The parent implementation returns false.
       *
       * @return bool Indicates whether this is a DEM shape model.
       */
      virtual bool isDEM() const = 0;

      // Get shape name
      QString name() const;

      // Set m_hasIntersection
      void setHasIntersection(bool b);

      // Set current surface point
      virtual void setSurfacePoint(const SurfacePoint &surfacePoint);

      // Return the normal (surface or local) of the current intersection point
      virtual std::vector<double>  normal();

      // Determine if the internal intercept is occluded from the observer/lookdir
      virtual bool isVisibleFrom(const std::vector<double> observerPos,
                                 const std::vector<double> lookDirection);

    protected:

      // Set the normal (surface or local) of the current intersection point
      void setNormal(const std::vector<double>);
      void setNormal(const double a, const double b, const double c);

      // Set shape name
      void setName(QString name);

      void calculateEllipsoidalSurfaceNormal();
      bool hasEllipsoidIntersection();

      // Intersect ellipse
      bool intersectEllipsoid(const std::vector<double> observerPosRelativeToTarget,
                              const std::vector<double> &observerLookVectorToTarget);
      bool hasValidTarget() const;
      std::vector<Distance> targetRadii() const;
      void setHasNormal(bool status);
      double resolution();

    private:
      bool m_hasEllipsoidIntersection; //!< Indicates the ellipsoid was successfully intersected
      bool m_hasIntersection;          //!< indicates good intersection exists
      bool m_hasNormal;                //!< indicates normal has been computed
      std::vector<double> m_normal;    //!< Local normal of current intersection point
      QString *m_name;                 //! < Name of the shape
      SurfacePoint *m_surfacePoint;    //!< Current intersection point

      Target *m_target;
  };
};

#endif
