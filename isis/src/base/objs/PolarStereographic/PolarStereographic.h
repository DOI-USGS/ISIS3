#ifndef PolarStereographic_h
#define PolarStereographic_h
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
   * @brief Stereographic Map Projection for Polar Aspect
   *
   * This class provides methods for the forward and inverse equations of a 
   * Polar Stereographic map projection (for an ellipsoid).
   *  
   *  The Stereographic projection is used for polar aspect maps.  This
   *  projection is azimuthal, conformal, and maps onto a plane. The
   *  center longitude is a straight line. All other longitudes are represented
   *  by arcs of circles. The antipodal point can not be projected.
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
   * @author 2004-02-24 Jeff Anderson
   *
   * @internal
   *   @history 2004-02-24 Jeff Anderson - Fixed a bug in TrueScaleLatitude and
   *                           changed default computation for CenterLatitude
   *   @history 2005-03-01 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2005-03-11 Elizabeth Ribelin - added TrueScaleLatitude method 
   *                           test to unitTest
   *   @history 2006-06-14 Elizabeth Miller - Added error check to make sure the
   *                           center latitude is not zero
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes 
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file. Moved
   *                           Name, Version, and TrueScaleLatitude to the
   *                           implementation file. Minor modifications to
   *                           comply with some coding standards. References
   *                           #928.
   *   @history 2012-08-01 Kimberly Oyama and Steven Lambright - Modified SetGround()
   *                           so that it returns false when the lat parameter is the
   *                           pole opposite of the center latitude. Also updated the
   *                           unit tests to exercise this change. References #604.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   */
  class PolarStereographic : public TProjection {
    public:
      PolarStereographic(Pvl &label, bool allowDefaults = false);
      ~PolarStereographic();
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

      double m_e4;           //!< Convenience variable for calculations.
      double m_t;            //!< Snyder's t-value from equation (15-19).
      double m_m;            //!< Snyder's m-value from equation (14-15).
      double m_signFactor;   /**< If the center latitude is positive, the sign 
                                  factor is 1.  Otherwise, it is -1.*/
      double m_poleFlag;     /**< Indicates whether the center latitude is at a
                                  pole.  If not, the pole flag is set to true.
                                  If the center latitude is at a pole, this flag
                                  is set to false.*/
  };
};

#endif

