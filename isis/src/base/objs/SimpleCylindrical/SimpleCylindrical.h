#ifndef SimpleCylindrical_h
#define SimpleCylindrical_h
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
   */
  class SimpleCylindrical : public Projection {
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

