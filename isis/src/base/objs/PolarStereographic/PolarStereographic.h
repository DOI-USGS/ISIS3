#ifndef PolarStereographic_h
#define PolarStereographic_h
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

#include "Projection.h"

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
   */
  class PolarStereographic : public Projection {
    public:
      PolarStereographic(Pvl &label, bool allowDefaults = false);
      ~PolarStereographic();
      bool operator== (const Projection &proj);

      std::string Name() const;
      std::string Version() const;
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

