#ifndef SimpleCylindrical_h
#define SimpleCylindrical_h
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
   * @brief Simple Cylindrical Map Projection
   *
   * This class provides methods for the forward and inverse equations of a 
   * Simple Cylindrical map projection (for a sphere). 
   *  
   * The Simple Cylindrical projection is an Equidistant Cylindrical projection 
   * with the standard parallel at the equator of the target planet. Poles, 
   * latitudes and longitudes are represented as straight lines.  The longitudes 
   * and latitudes are  equally spaced and intersect at right angles. 
   *  
   * The code was converted to C++ from the Fortran version of the USGS General 
   * Cartographic Transformation Package (GCTP). In particular it was modified 
   * from the Equidistant Cylindrical code. This class inherits Projection and 
   * provides the two virtual methods SetGround (forward) and SetCoordinate 
   * (inverse) and a third virtual method, XYRange, for obtaining projection 
   * coordinate coverage for a latitude/longitude window. 
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *
   * @ingroup MapProjection
   * @see Equirectangular
   *
   * @author 2003-01-29 Jeff Anderson
   *
   * @internal
   *   @history 2003-01-30 Jeff Anderson - Removed IsisWorldMapper argument from
   *                           the constructor
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2003-06-05 Jeff Anderson - Changed SetCoordinate method so it
   *                           did not adjust longitude into the longitude
   *                           domain
   *   @history 2003-09-26 Jeff Anderson - Provided virtual methods for Name and
   *                           operator==
   *   @history 2003-11-13 Jeff Anderson - Modified constructor to allow for
   *                           computation for default value for CenterLongitude
   *                           keyword.
   *   @history 2005-02-15 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version,
   *                           IsEquatorialCylindrical methods
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file.  Moved
   *                           Name, Version, IsEquatorialCylindrical to the
   *                           implementation file. Minor modifications to
   *                           comply with some coding standards. References
   *                           #928.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   */
  class SimpleCylindrical : public TProjection {
    public:
      SimpleCylindrical(Pvl &label, bool allowDefaults = false);
      ~SimpleCylindrical();
      bool operator==(const Projection &proj);

      QString Name() const;
      QString Version() const;
      bool IsEquatorialCylindrical();

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

