#ifndef Mollweide_h
#define Mollweide_h
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

#include "TProjection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @description Mollweide Map Projection
   *
   * This class provides methods for the forward and inverse equations of a
   * Mollweide Equal-Area map projection (for a sphere).
   *  
   *  
   * The Mollweide projection is an equal-area, pseudo-cylindrical projection
   * presented by Carl B. Mollweide (1774-1825) of Halle, Germany in 1805.
   * The sphere is projected as an ellipse where it's Equator (the major axis)
   * is twice as long as as the prime meridian (the minor axis).  All other
   * meridians are elliptical arcs.   Combined with their opposite members, they form 
   * ellipses where they meet at the two poles.  The meridians 90 degrees to the east
   * and west of the prime meridian form a perfect circle.  
   *
   * The lines of latitude are straight lines, but they are not qquidistant from
   * each other.  The regions along the Equator are stretched 23 percent in
   * a north-south direction relative to the east-west directions.
   *
   * The only two points of the projection free of distortion are where the 
   * prime meridian crosses the latitudinal lines at 44 degrees, 44 minutes to the N and S.  
   * North and south of these latitudes, the stretching turns into compression near the poles.
   *
   * The Mollweide projection is usually applied at a small scale.
   *
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *
   * @ingroup MapProjection
   *
   * @author 2016-08-24 Tyler Wilson
   *
   * @internal
   *
   */
  class Mollweide : public TProjection {
    public:
      Mollweide(Pvl &label, bool allowDefaults = false);
      ~Mollweide();
      bool operator== (const Projection &proj);

      QString Name() const;
      QString Version() const;

      bool newton_rapheson(double gamma, double &result);
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

