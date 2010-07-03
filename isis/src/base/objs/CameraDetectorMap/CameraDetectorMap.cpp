/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2009/04/08 02:32:55 $
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
#include "CameraDetectorMap.h"

namespace Isis {
  /** Default constructor assumes no summing and starting detector offsets
   *
   * @param parent Camera that will use this detector map
   */
  CameraDetectorMap::CameraDetectorMap(Camera *parent) {
    p_camera = parent;
    p_startingDetectorSample = 1.0;
    p_startingDetectorLine = 1.0;
    p_detectorSampleSumming = 1.0;
    p_detectorLineSumming = 1.0;
    Compute();
    if (parent != 0) {
      p_camera->SetDetectorMap(this);
    }
  }

  /** Compute parent position from a detector coordinate
   *
   * This method will compute a parent sample/line given a
   * detector coordinate
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector
   *
   * @return conversion successful
   */
  bool CameraDetectorMap::SetDetector(const double sample, const double line) {
    p_detectorSample = sample;
    p_detectorLine = line;
    p_parentSample = (p_detectorSample - p_ss) / p_detectorSampleSumming + 1.0;
    p_parentLine   = (p_detectorLine   - p_sl) / p_detectorLineSumming   + 1.0;
    return true;
  }

  /** Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position from the parent
   * line/sample coordinate
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image
   *
   * @return conversion successful
   */
  bool CameraDetectorMap::SetParent(const double sample, const double line) {
    p_parentSample = sample;
    p_parentLine = line;
    p_detectorSample = (p_parentSample - 1.0) * p_detectorSampleSumming + p_ss;
    p_detectorLine   = (p_parentLine   - 1.0) * p_detectorLineSumming + p_sl;
    return true;
  }

  //! Compute new offsets whenenver summing or starting sample/lines change
  void CameraDetectorMap::Compute() {
    p_ss = (p_detectorSampleSumming / 2.0) + 0.5 +
           (p_startingDetectorSample - 1.0);

    p_sl = (p_detectorLineSumming / 2.0) + 0.5 +
           (p_startingDetectorLine - 1.0);
  }

}

