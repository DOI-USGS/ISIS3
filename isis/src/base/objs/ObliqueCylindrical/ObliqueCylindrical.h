#ifndef ObliqueCylindrical_h
#define ObliqueCylindrical_h
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
   * @brief Oblique Cylindrical Map Projection
   *
   * This class provides methods for the forward and inverse equations of an 
   * Oblique Cylindrical map projection (for a sphere).
   *  
   * This projection works by moving the north pole of the simple cylindrical 
   * projection. The pole latitude and longitude are the location of the new 
   * north pole, and the rotation is the equivalent to the center longitude in 
   * simple cylindrical.
   *  
   * The code was converted to C++ from the Fortran version of the USGS General 
   * Cartographic Transformation Package (GCTP). In particular it was modified 
   * from the Simple Cylindrical code. This class inherits Projection and 
   * provides the four virtual methods Name(), SetGround (forward) and 
   * SetCoordinate (inverse), XYRange (for obtaining projection coordinate 
   * coverage for a latitude/longitude window) and the == operator. 
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available.
   *
   * 
   *
   * @ingroup MapProjection
   *
   * @author 2000-02-09 Jeff Anderson
   *
   * @internal
   *   @history 2007-06-19 Steven Lambright, Converted to ISIS3 and created XY
   *                           Range search implementation
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version methods
   *   @history 2010-02-08 Sharmila Prasad  - Removed testing m_latitude and
   *                           m_longitude  in operator "=="
   *   @history 2012-06-06 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl, PvlGroup to header file.
   *                           Ordered includes in implementation file. Moved
   *                           Name() and Version() to the implementation file.
   *                           Minor modifications to comply with some coding
   *                           standards.
   *   @history 2012-06-15 Jeannie Backer - Moved the following methods to
   *                           Projection class for generalized
   *                           xyRangeOblique() method - doSearch(),
   *                           findExtreme(), setSearchGround(). Minor
   *                           modifications to comply with some coding
   *                           standards. References #928.
   *   @history 2012-01-20 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                           were signaled. References #2248.
   */
  class ObliqueCylindrical : public Isis::TProjection {
    public:
      ObliqueCylindrical(Isis::Pvl &label, bool allowDefaults = false);
      ~ObliqueCylindrical();
      bool operator==(const Isis::Projection &proj);

      QString Name() const;
      QString Version() const;

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingLatitudes();
      PvlGroup MappingLongitudes();

      double poleLatitude() const;
      double poleLongitude() const;
      double poleRotation() const;

    private:
      void init();

      // These are the oblique projection pole values in degrees.
      double m_poleLatitude;   //!< The Oblique Pole Latitude
      double m_poleLongitude;  //!< The Oblique Pole Longitude
      double m_poleRotation;   //!< The Oblique Pole Rotation

      // These vectors are not used by the projection
      std::vector<double> m_xAxisVector; /**< The x-axis vector, read from the 
                                               mapping group in the label.*/
      std::vector<double> m_yAxisVector; /**< The y-axis vector, read from the 
                                               mapping group in the label.*/
      std::vector<double> m_zAxisVector; /**< The z-axis vector, read from the 
                                               mapping group in the label.*/
  };
};

#endif

