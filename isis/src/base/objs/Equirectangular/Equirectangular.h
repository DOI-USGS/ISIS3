#ifndef Equirectangular_h
#define Equirectangular_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/11/13 15:56:28 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Projection.h"

namespace Isis {
  class Pvl;
  /**
   * @brief Equirectangular Map Projection
   * 
   * This class provides methods for the forward and inverse equations of a
   * Equirectangular map projection (for a sphere). 
   *  
   * The Equirectangluar projection is a cylindrical projection in which the 
   * poles, latitudes, and longitudes are represented as straight, equidistant, 
   * lines.  The poles and latitudes are horizontal lines and the longitudes are 
   * vertical lines.  These lines intersect at right angles. 
   *  
   * The code was converted to C++ from the Fortran version of the USGS General 
   * Cartographic Transformation Package (GCTP).  In particular it was modified 
   * from the Equidistant Cylindrical code. This class inherits IsisProjection 
   * and provides the two virtual methods SetGround (forward) and SetCoordinate 
   * (inverse) and a third virtual method, XYRange, for obtaining projection 
   * coordinate coverage for a latitude/longitude window.  
   *
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *  
   * @ingroup MapProjection
   *  
   * @author 2003-11-13 Jeff Anderson
   *
   * @internal
   *   @history 2004-02-07 Jeff Anderson - added plug-in capability.
   *   @history 2004-02-24 Jeff Anderson - Modified forward and inverse methods
   *                           to use the local radius at the center latitude
   *                           instead of the equitorial radius.
   *   @history 2005-03-11 Elizabeth Ribelin - added TrueScaleLatitude method
   *                           test to the unitTest
   *   @history 2007-06-29 Steven Lambright - Added Mapping, MappingLatitudes
   *                           and MappingLongitudes methods.
   *   @history 2008-05-09 Steven Lambright - Added Name, Version,
   *                           IsEquatorialCylindrical methods
   *   @history 2008-05-09 Steven Lambright - Fixed test for being too close to
   *                           a pole
   *   @history 2008-11-12 Steven Lambright - Commented some unclear code
   *                           (CenterLatitudeRadius keyword)
   *   @history 2012-06-15 Jeannie Backer - Added documentation.  Added forward
   *                           declaration of Pvl to header file.  Ordered
   *                           includes in implementation file. Moved Name,
   *                           Version, IsEquatorialCylindrical to the
   *                           implementation file. Minor modifications to
   *                           comply with some coding standards. References
   *                           #928.
   */
  class Equirectangular : public Projection {
    public:
      Equirectangular(Pvl &label, bool allowDefaults = false);
      ~Equirectangular();
      bool operator==(const Projection &proj);

      std::string Name() const;
      std::string Version() const;
      double TrueScaleLatitude() const;
      bool IsEquatorialCylindrical();

      bool SetGround(const double lat, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      virtual PvlGroup Mapping();
      virtual PvlGroup MappingLatitudes();
      virtual PvlGroup MappingLongitudes();

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerLatitude;  //!< The center latitude for the map projection
      double m_cosCenterLatitude;//!< Cosine of the center latitude
      double m_clatRadius;       /**< The radius of the target planet at the 
                                      center latitude.*/
  };
};

#endif

