/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/07/09 16:40:59 $
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

#include "RadarPulseMap.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {
  /** Compute alpha position from a detector coordinate
   *
   * This method will compute a alpha sample given a detector
   * coordinate.  The alpha line will be computed using the the
   * time in the parent camera
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector (ignored)
   *
   * @return conversion successful
   */
  bool RadarPulseMap::SetDetector(const double sample,
                                  const double line) {
    if (!CameraDetectorMap::SetDetector(sample,line)) return false;
    double etDiff = p_camera->EphemerisTime() - p_etStart;
    p_parentLine = etDiff / p_lineRate + 1.0;
    return true;
  }

  /** Compute radar (sample/time)from a alpha image coordinate
   *
   * This method will compute the radar position from the alpha
   * line/sample coordinate.  The alpha line will be used to set
   * the appropriate time in the parent camera.
   *
   * @param sample Sample number in the alpha image
   * @param line Line number in the alpha image
   *
   * @return conversion successful
   */
  bool RadarPulseMap::SetParent(const double sample,
                                const double line) {
    // Apply base class summing/first sample corrections
    if (!CameraDetectorMap::SetParent(sample,line)) return false;

    // line is really a function of time so set detector line to zero
    p_detectorLine = 0;
    double etLine = p_etStart + p_lineRate * (line - 1.0);
    p_camera->SetEphemerisTime(etLine);
    return true;
  }
}
