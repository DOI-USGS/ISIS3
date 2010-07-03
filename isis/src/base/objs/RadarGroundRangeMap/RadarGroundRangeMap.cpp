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
#include "RadarGroundRangeMap.h"

namespace Isis {
  /** Construct mapping between detectors and focal plane x/y
   *
   * @param parent      parent camera that will use this map
   * @param naifIkCode  code of the naif instrument for reading coefficients
   *
   */
  RadarGroundRangeMap::RadarGroundRangeMap(Camera *parent, const int naifIkCode)
    : CameraFocalPlaneMap(parent,naifIkCode) {
  }

  void RadarGroundRangeMap::setTransform(int naifIkCode,
                                         double groundRangeResolution,
                                         int samples, Radar::LookDirection ldir) {
    // Setup map from radar(sample,time) to radar(groundRange,time)
    double transx[3], transy[3];
    double transs[3], transl[3];

    // There is no change for Left and Right look because the RangeCoefficientSet
    // takes the look direction into account
    transx[0] = -1.0 * groundRangeResolution;
    transx[1] = groundRangeResolution;
    transx[2] = 0.0;

    transs[0] = 1.0;
    transs[1] = 1.0 / groundRangeResolution;
    transs[2] = 0.0;

    transy[0] = 0.0;
    transy[1] = 0.0;
    transy[2] = 0.0;

    transl[0] = 0.0;
    transl[1] = 0.0;
    transl[2] = 0.0;

    std::string icode = "INS" + iString(naifIkCode);
    pdpool_c((icode+"_TRANSX").c_str(), 3, transx);
    pdpool_c((icode+"_TRANSY").c_str(), 3, transy);
    pdpool_c((icode+"_ITRANSS").c_str(), 3, transs);
    pdpool_c((icode+"_ITRANSL").c_str(), 3, transl);
  }
}

