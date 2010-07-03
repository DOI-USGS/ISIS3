/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/07/09 16:40:19 $
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
#ifndef RadarGroundRangeMap_h
#define RadarGroundRangeMap_h

#include "Camera.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {

#ifndef RADAR_LOOK_DIR
namespace Radar {
  enum LookDirection { Left, Right };
}
#define RADAR_LOOK_DIR
#endif

  /** Construct a mapping between image sample and Radar ground range
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @internal
   *
   * @author 2008-06-17 Jeff Anderson
   * Original version
   *
   * @history 2009-07-01 Janet Barrett - Corrected the transformation
   *                     calculations
   *
   */
  class RadarGroundRangeMap : public CameraFocalPlaneMap {
    public:
      RadarGroundRangeMap(Camera *parent, const int naifIkCode);

      static void setTransform(int naifIkCode, double groundRangeResolution,
                               int samples, Radar::LookDirection ldir);
  };
};
#endif
