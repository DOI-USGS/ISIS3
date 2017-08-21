#ifndef EllipsoidShape_h
#define EllipsoidShape_h
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

#include "ShapeModel.h"

template<class T> class QVector;

namespace Isis {
  class Pvl;

  /**
   * @brief Define shapes and provide utilities for Isis3 targets
   *
   * This class will define shapes of Isis3 target bodies as well as
   * provide utilities to retrieve radii and photometric information.
   *
   *
   * @ingroup
   *
   * @author 2010-08-02 Debbie A. Cook
   *
   * @internal
   *   @history 2012-12-21 Debbie A. Cook - Cleaned up intersectSurface method to
   *                           reflect changes made to ShapeModel class.  References #1343.
   *   @history 2015-04-30 Jeannie Backer - Added isDEM() method. References #2243.
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                           were signaled. References #2248.
   *   @history 2016-06-13 Kelvin Rodriguez - Removed redundant contructor 
   *                           EllipsoidShape(Target, Pvl). References #2214
   *   @history 2017-05-19 Tyler Wilson - calculateSurfaceNormal() and calculateDefaultNormal()
   *                           now call calculateLocalNormal(), which calculates the normal
   *                           vector to an ellipsoid.  Prior to this, they were calling
   *                           ShapeModel::calculateEllipsoidalSurfaceNormal() function
   *                           which was incorrectly returning the normal vector to a sphere and not
   *                           an ellipsoid.  Fixes #1028.
   *   @history 2017-06-07 Kristin Berry - Added a using declaration so that the new 
   *                            intersectSurface methods in ShapeModel are accessible by
   *                            EllipsoidShape.
   */
  class EllipsoidShape : public Isis::ShapeModel {
    public:
      //! Constructors
      EllipsoidShape(Target *target);
      EllipsoidShape();

      //! Destructor
      ~EllipsoidShape() { };

      // Make parent functions visible
      using Isis::ShapeModel::intersectSurface; 

      //! Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

      //! Calculate the default normal of the current intersection point
      virtual void calculateDefaultNormal();

      // implement pure virtual method from ShapeModel class
      bool isDEM() const;

      //! Calculate the local surface normal of the current intersection point
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints);

      //! Calculate the surface normal of the current intersection point
      void calculateSurfaceNormal();

      //! Get the local radius for a point on the surface
      Distance localRadius(const Latitude &lat, const Longitude &lon);

    private:
  };
};

#endif
