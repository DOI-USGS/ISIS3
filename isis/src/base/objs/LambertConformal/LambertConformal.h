#ifndef LambertConformal_h
#define LambertConformal_h
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
   * @brief Lambert Conformal Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * Lambert Conformal map projection (for an ellipsoid). 
   *  
   * The Lambert conformal projection is a conic projection around a center 
   * latitude and longitude.  For this projection, latitudes are circles and 
   * longitudes are equally spaced lines, intersecting the latitudes at right 
   * angles. Scale is true along the two standard parallels. 
   * 
   *  
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
   * @author 2005-03-29 Elizabeth Ribelin
   *
   * @internal
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes 
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
   *   @history 2008-08-15 Stuart Sides - Modified to allow standard parallels
   *                           to be in any order. Modified to not accept
   *                           center latitudes too close to either pole.
   *   @history 2009-03-20 Stuart Sides - Modified to not accept center 
   *                           latitudes near the pole opposite the apex of the
   *                           cone
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file. Moved
   *                           Name, Version, and TrueScaleLatitude to the
   *                           implementation file. Minor modifications to
   *                           comply with some coding standards. References
   *                           #928.
   *   @history 2012-01-20 Debbie A. Cook - Changed to uTProjection instead of Projection.
   *                           References #775.
   */
  class LambertConformal : public TProjection {
    public:
      LambertConformal(Pvl &label, bool allowDefaults = false);
      ~LambertConformal();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;
      double TrueScaleLatitude() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerLatitude;  //!< The center latitude for the map projection
      double m_par1;            //!< The first standard parallel
      double m_par2;            //!< The second standard parallel
      double m_n;               //!< Snyder's n variable
      double m_f;               //!< Snyder's f variable
      double m_rho;             //!< Snyder's rho variable

  };
};

#endif
