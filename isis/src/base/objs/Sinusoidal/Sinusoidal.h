#ifndef Sinusoidal_h
#define Sinusoidal_h
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
   * @brief Sinusoidal Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * Sinusoidal Equal-Area map projection (for a sphere). 
   *  
   *  
   * The Sinusoidal projection is an equal-area, psuedo-cylindrical projection. 
   * The poles are represented as points, the center longitude and all latitudes 
   * as straight lines and all other longitudes as sinusoidal curves. The 
   * latitudes are equally spaced and parallel and the longitudes are equally 
   * spaced.  True scale is found along the center longitude and all latitudes. 
   *  
   * The code was converted to C++ from the Fortran version of the USGS General 
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
   * @author 2003-01-29 Jeff Anderson
   *
   * @internal
   *   @history 2003-01-30 Jeff Anderson - Removed WorldMapper argument from the
   *                           constructor
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-06-05 Jeff Anderson - Changed SetCoordinate method so it
   *                           does not adjust the longitude into the longitude
   *                           domain
   *   @history 2003-09-26 Jeff Anderson - Provided virtual methods for Name and
   *                           operator==
   *   @history 2003-11-13 Jeff Anderson - Modified constructor to allow for
   *                           computation of default center longitude using the
   *                           longitude range.
   *   @history 2004-02-07 Jeff Anderson - Added plugin routine and file
   *   @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
   *   @history 2012-03-30 Steven Lambright and Stuart Sides - SetCoordinate
   *                           will now fail if the resulting longitude is
   *                           becoming too large to work with in double
   *                           precision. Fixes #656.
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Added includes in implementation file.  Moved
   *                           Name and Version to the implementation file.
   *                           Minor modifications to comply with some coding
   *                           standards. References #928.
   *   @history 2012-11-22 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   */

  class Sinusoidal : public TProjection {
    public:
      Sinusoidal(Pvl &label, bool allowDefaults = false);
      ~Sinusoidal();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
  };
};

#endif

