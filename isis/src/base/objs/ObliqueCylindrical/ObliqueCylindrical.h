#ifndef ObliqueCylindrical_h
#define ObliqueCylindrical_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/02/08 19:02:07 $
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

#include "Projection.h"

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
   */
  class ObliqueCylindrical : public Isis::Projection {
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

