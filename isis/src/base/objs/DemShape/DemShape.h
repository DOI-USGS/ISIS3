#ifndef DemShape_h
#define DemShape_h
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
  class Cube;
  class Interpolator;
  class Portal;
  class Projection;

  /**
   * @brief Define shapes and provide utilities for targets stored as Isis3 maps
   *
   * This class will define shapes of Isis3 target bodies with the shape defined by an 
   * Isis 3 map fille (level 2 image), as well as provide utilities to retrieve radii and 
   * photometric information for the intersection point.
   *
   * @author 2010-07-30 Debbie A. Cook
   *
   * @internal 
   *   @history 2010-07-30 - Debbie A. Cook - Original version.
   *   @history 2012-10-25 - Jeannie Backer - Changed call to Spice::resolution() method
   *                             to lower camel case. Added documentation. Ordered includes.
   *                             References #1181.
   *   @history 2015-04-30 Jeannie Backer - Added isDEM() method. References #2243.
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                             were signaled. References #2248.
   */
  class DemShape : public ShapeModel {
    public:
      // Constructor
      DemShape(Target *target, Pvl &pvl);

      // Constructor
      DemShape();

      // Destructor
      ~DemShape();

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

      Distance localRadius(const Latitude &lat, const Longitude &lon);

      // Return dem scale in pixels/degree
      double demScale();

      // Calculate the default normal of the current intersection point
      virtual void calculateDefaultNormal(); 

      // implement pure virtual method from ShapeModel class
      bool isDEM() const;

      // To compute the surface normal, you must call setLocalAreaPoint on top,
      // bottom, left, and right surrounding points in the image.  Then call
      // calculateSurfaceNormal and directSurfaceNormal to calculate the normal.
      // Use removeLOcalAreaPoints to clean up as needed.  See Camera for an
      // example, or use its GetLocalNormal method.
      
      // Calculate the surface normal of the current intersection point
     void calculateLocalNormal(QVector<double *> cornerNeighborPoints); 
     void calculateSurfaceNormal(); 

    protected:
     Cube *demCube();         //!< Returns the cube defining the shape model.  

    private:
      Cube *m_demCube;        //!< The cube containing the model
      Projection *m_demProj;  //!< The projection of the model
      double m_pixPerDegree;  //!< Scale of DEM file in pixels per degree
      Portal *m_portal;       //!< Buffer used to read from the model
      Interpolator *m_interp; //!< Use bilinear interpolation from dem
  };
}

#endif

