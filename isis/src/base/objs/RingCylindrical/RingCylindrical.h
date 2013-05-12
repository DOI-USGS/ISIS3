#ifndef RingCylindrical_h
#define RingCylindrical_h
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

#include "RingPlaneProjection.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  /**
   * @brief Ring Cylindrical Map Projection
   *
   * This class provides methods for the forward and inverse equations of a 
   * Ring Cylindrical map projection (for a sphere). 
   *  
   * The Ring Cylindrical projection is an
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *
   * @ingroup MapProjection
   * @see Equirectangular
   *
   * @author 2013-03-10 Debbie A. Cook
   *
   * @internal 
   *   @history 2013-05-09 Jeannie Backer - Added comments and documentation.
   *            References #775.
   */
  // or Rectilinear projection?? scale azimuth with 1/(2*pi) * radius maybe
  class RingCylindrical : public RingPlaneProjection {
    public:
      RingCylindrical(Pvl &label, bool allowDefaults = false);
      ~RingCylindrical();
      bool operator==(const Projection &proj);

      QString Name() const;
      QString Version() const;
      bool IsEquatorialCylindrical();
      double TrueScaleRingRadius() const;

      double CenterRingLongitude() const;
      double CenterRingRadius() const;

      bool SetGround(const double ringRadius, const double ringLongitude);
      bool SetCoordinate(const double x, const double y);
      bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      PvlGroup Mapping();
      PvlGroup MappingRingRadii();
      PvlGroup MappingRingLongitudes();

    protected:

    private:
      double m_centerRingLongitude; /**< The center ring longitude (azimuth) for the map projection,
                                         in radians*/
      double m_centerRingRadius;    //!< The center ring radius for the map projection
  };
};

#endif

