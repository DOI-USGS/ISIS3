#ifndef Planar_h
#define Planar_h
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
   * @brief Planar Map Projection
   *
   * This class provides methods for the forward and inverse equations of an
   * Planar map projection.
   *  
   * The Orthographic projection is an azimuthal projection.  Latitudes and 
   * longitudes are ellipses, circles, or straight lines.  Only one hemisphere 
   * can be projected.  Scale is true at the point (center latitude, center 
   * longitude). 
   *  
   *  
   * See the Projection class for a full accounting of all the methods
   * available. 
   *
   * @ingroup MapProjection
   *
   * @author 2012-09-09 Ken Edmundson
   *
   * @internal
   *   @history 2012-08-09 Ken Edmundson - initial version
   */
  class Planar : public Projection {
    public:
      Planar(Pvl &label, bool allowDefaults = false);
      ~Planar();
      bool operator== (const Projection &proj);

      std::string Name() const;
      std::string Version() const;
//      double TrueScaleLatitude() const;

      bool SetGround(const double radius, const double lon);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingRadii();
      PvlGroup MappingLongitudes();

      double Radius() const;

  protected:
      double m_radius;
      double m_minimumRadius;
      double m_maximumRadius;

    private:
      double m_centerLongitude; //!< The center longitude for the map projection
      double m_centerRadius;    //!< The center radius for the map projection
  };
};
#endif
