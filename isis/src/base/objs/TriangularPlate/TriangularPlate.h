#ifndef TriangularPlate_h
#define TriangularPlate_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/05/09 18:49:25 $
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

#include "AbstractPlate.h"

#include "NaifDskApi.h"

namespace Isis {

  class Angle;
  class Distance;
  class Intercept;
  class Latitude;
  class Longitude;
  class SurfacePoint;
  
  /**
   * @brief Specification for an abstract triangular plate 
   *  
   * This class implements the abstract concept of a triangular plate. It provides 
   * the basic elements of the plate to compute intersections and property values.
   *  
   * @author 2013-12-05 Kris Becker 
   * @internal 
   *   @history 2013-12-05 Kris Becker  Original Version 
   *   @history 2015-03-08 Jeannie Backer - Added documentation and test. Added class to ISIS trunk.
   *                           References #2035
   */
  class TriangularPlate : public AbstractPlate {
    public:
      TriangularPlate(const NaifTriangle &plate, const int &plateId = 0);
      virtual ~TriangularPlate();

      int id() const;
      QString name() const;

      Distance minRadius() const;
      Distance maxRadius() const;

      double area() const;
      NaifVector normal() const;
      NaifVector center() const;

      Angle separationAngle(const NaifVector &raydir) const;

      bool hasIntercept(const NaifVertex &vertex, const NaifVector &raydir) const;
      bool hasPoint(const Latitude &lat, const Longitude &lon) const;

      SurfacePoint *point(const Latitude &lat, const Longitude &lon) const;
      Intercept *intercept(const NaifVertex &vertex, const NaifVector &raydir) const;

      NaifVertex vertex(int v) const;

      AbstractPlate *clone() const;

    protected:
      bool findPlateIntercept(const NaifVertex &obs, const NaifVector &raydir, 
                              NaifVertex &point) const;
  
    private:
      TriangularPlate();  // Disallow an empty instantiation of the class
      NaifTriangle m_plate;   /**< Tetrahedron, defined by the coordinate system origin and 3 
                                   vertices, used to represent the TriangularPlate. */
      int          m_plateId; //!< ID for this plate on the ShapeModel.
  
  };
};

#endif
