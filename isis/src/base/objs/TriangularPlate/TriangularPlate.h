#ifndef TriangularPlate_h
#define TriangularPlate_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
