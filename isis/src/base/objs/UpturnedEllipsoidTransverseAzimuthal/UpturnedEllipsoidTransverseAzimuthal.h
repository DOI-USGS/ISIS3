#ifndef UpturnedEllipsoidTransverseAzimuthal_h
#define UpturnedEllipsoidTransverseAzimuthal_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2009/03/20 22:30:23 $
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
   * @brief Upturned Ellipsoid Transverse Azimuthal Map Projection
   *
   * This class provides methods for the forward and inverse formulas of a 
   * Upturned Ellipsoid Transverse Azimuthal map projection 
   *  
   *  
   * This class inherits Projection and implements the virtual methods 
   * <UL> 
   *   <LI> SetGround - forward projection takes an lat/lon value from the given
   *   planet and calculates the corresponding x/y value on the projected plane
   *   <LI> SetCoordinate - inverse projection takes an x/y coordinate from the
   *   plane and calculates the lat/lon value on the planet
   *   <LI> XYRange - obtains projection coordinate coverage for a
   *   latitude/longitude window
   * </UL>
   *  
   * Please see the Projection class for a full accounting of all the methods 
   * available. 
   *  
   * @see <em>Carographic Projections For Small Bodies of the Solar System</em> 
   *      by Maria E. Fleis, Michael M. Borisov, Michael V. Alexandrovich,
   *      Philip Stooke, and Kira B Shingareva
   *  
   * @ingroup MapProjection
   *
   * @author 2016-03-18 Jeannie Backer
   *
   * @internal
   *   @history 2016-03-18 Jeannie Backer - Original version. Fixes #3877.
   *   @history 2016-12-28 Kristin Berry - Minor coding standards and documentation updates for
   *                                       checkin. 
   */
  class UpturnedEllipsoidTransverseAzimuthal : public TProjection {
    public:
      UpturnedEllipsoidTransverseAzimuthal(Pvl &label, bool allowDefaults = false);
      ~UpturnedEllipsoidTransverseAzimuthal();
      bool operator== (const Projection &proj);

      virtual QString Name() const;
      virtual QString Version() const;

      virtual bool SetGround(const double lat, const double lon);
      virtual bool SetCoordinate(const double x, const double y);
      virtual bool XYRange(double &minX, double &maxX, double &minY, double &maxY);

      virtual PvlGroup Mapping();
      virtual PvlGroup MappingLatitudes();
      virtual PvlGroup MappingLongitudes();

    private:
      void init(double centerLongitude);
      void checkLongitude(double longitude);


      double m_a;       /**< Semi-major axis of the ellipse. For prolate bodies,
                             this will be the polar radius.*/
      double m_b;       /**< Semi-minor axis of the ellipse. For prolate bodies,
                             this will be the equatorial radius.*/
      double m_e;       /**< Eccentricity of the ellipse.*/
      double m_lambda0; /**< The longitude of the center of the projection.
                             This is value is 0 or 180 degrees.*/ 
      double m_t;       /**< Auxiliary value used to reduce calculations. t = 1 - eccentricity^2. */
      double m_t1;      /**< Auxiliary value used to reduce calculations. 
                             t1 = e / sqrt( 1 - eccentricity^2 ) */
      double m_k;       /**< The radius of the Equator of the transverse graticule on the 
                             Azimuthal projection under the condition of no distortion in the
                             center of the projection.*/
  };
};

#endif
