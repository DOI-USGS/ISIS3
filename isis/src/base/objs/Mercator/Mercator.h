#ifndef Mercator_h
#define Mercator_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "TProjection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief Mercator Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * Mercator map projection (for an ellipsoid). 
   *  
   * The Mercator projection is cylindrical and conformal, preserving angles and
   * shapes of small objects and distorting the shape of large objects. The 
   * cylinder wraps the planet along the equator, with the poles at 
   * infinity. Latitudes and longitudes are straight lines, crossing at right 
   * angles. However, latitudes are unequally spaced and longitudes are equally 
   * spaced. 
   *  
   * The code was converted to C++ from the C version of the USGS General 
   * Cartographic Transformation Package (GCTP). This class inherits Projection 
   * and provides the two virtual methods SetGround (forward) and SetCoordinate 
   * (inverse) and a third virtual method, XYRange, for obtaining projection 
   * coordinate coverage for a latitude/longitude window. 
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *
   * @ingroup MapProjection
   *
   * @author 2005-02-25 Elizabeth Ribelin
   *
   * @internal
   *   @history 2005-03-18 Elizabeth Ribelin - added TrueScaleLatitude method to
   *                           class and tested in unitTest
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version,
   *                           IsEquatorialCylindrical methods
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file.  Moved
   *                           Name, Version, IsEquatorialCylindrical, and
   *                           TrueScaleLatitude to the implementation file.
   *                           Minor modifications to comply with some coding
   *                           standards. References #928.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   */
  class Mercator : public TProjection {
    public:
      Mercator(Pvl &label, bool allowDefaults = false);
      ~Mercator();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;
      double TrueScaleLatitude() const;
      bool IsEquatorialCylindrical();

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerLatitude;  //!< The center latitude for the map projection
      double m_scalefactor;      //!< Scaling factor
  };
};

#endif
