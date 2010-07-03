/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/07/09 16:41:34 $
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

#ifndef RadarSkyMap_h
#define RadarSkyMap_h

#include "Camera.h"
#include "CameraSkyMap.h"

namespace Isis {
  /** Convert between slantrange/groundrange and ra/dec
   *  coordinates
   *
   * Radar can never paint a star so this routine alway returns
   * false for a sky intersection
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @internal
   *
   * @history 2008-06-17 Jeff Anderson
   * Original version
   *
   */
  class RadarSkyMap : public CameraSkyMap {
    public:
      RadarSkyMap(Camera *parent);

      //! Destructor
      virtual ~RadarSkyMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy,
                                 const double uz);

      virtual bool SetSky(const double ra, const double dec);

  };
};
#endif
