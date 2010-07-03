/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/10/21 18:37:02 $
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

#include <iomanip>
#include "PushFrameCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {
  /** Compute parent position from a detector coordinate
   *
   * This method will compute a parent sample given a
   * detector coordinate. The parent line and framelet line
   * will be computed.
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector
   *
   * @return conversion successful
   */
  bool PushFrameCameraDetectorMap::SetDetector(const double sample,
                                               const double line) {
    // Sometime folks want to write the framelets flipped in the EDR so
    // features match.  Take care of this.
    double unsummedFrameletLine;
    if (p_flippedFramelets) {
      unsummedFrameletLine = p_bandStartDetector + p_frameletHeight - line;
    }
    else {
      unsummedFrameletLine = line - p_bandStartDetector;
    }

    double unsummedFrameletSample = sample;

    // Convert framelet sample/line to summed framelet sample/line,
    // parent sample will be computed correctly
    if (!CameraDetectorMap::SetDetector(unsummedFrameletSample,unsummedFrameletLine)) {
      return false;
    }

    p_frameletSample = p_detectorSample;
    p_frameletLine = p_detectorLine;

    // Compute the height of a framelet taking into account the summing mode
    int actualFrameletHeight = (int)(p_frameletHeight / LineScaleFactor());

    p_parentLine = (p_framelet - 1) * actualFrameletHeight + p_parentLine;

    // Save the detector sample/line
    p_detectorSample = sample;
    p_detectorLine = line;

    // Didn't succeed if framelet line doesn't make sense
    if(p_frameletLine > p_frameletHeight + 0.5) {
      return false;
    }

    if(p_frameletLine < 0.5) {
      return false;
    }

    return true;
  }

  /** Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position and framelet position
   * from the parent line/sample coordinate. The parent line will be used 
   * to set the appropriate time in the parent camera.
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image
   *
   * @return conversion successful
   */
  bool PushFrameCameraDetectorMap::SetParent(const double sample,
                                             const double line) {
    // Compute the height of a framelet taking into account the summing mode
    int actualFrameletHeight = (int)(p_frameletHeight / LineScaleFactor());

    // Compute the framelet number.  We could have padded with null framelets
    // at the top of the image so take that into account.  Setting the framelet
    // changes the time for the observation. Line starts at 0.5 (top of first framelet)
    // and framelet needs to start at 1.
    int framelet = (int)((line - 0.5) / actualFrameletHeight) + 1;
    SetFramelet(framelet);

    // Convert the parent line/sample to a framelet line/sample
    p_frameletLine = line - actualFrameletHeight * (framelet - 1);
    p_frameletSample = sample;

    // Convert the framelet line/sample to an unsummed framelet line/sample
    if (!CameraDetectorMap::SetParent(p_frameletSample, p_frameletLine)) return false;
    double unsummedFrameletLine = p_detectorLine;

    // Sometime folks want to write the framelets flipped in the EDR so
    // features match.  Take care of this. p_bandStartDetector is 0-based and
    // unsummedFrameletLine is the correct base for p_detectorLine so these calculations 
    // are valid.
    if (p_flippedFramelets) {
      p_detectorLine = p_bandStartDetector + p_frameletHeight - unsummedFrameletLine;
    }
    else {
      p_detectorLine = p_bandStartDetector + unsummedFrameletLine;
    }

    // Save the parent line/sample
    p_parentSample = sample;
    p_parentLine = line;

    return true;
  }

  /**
   * This method changes the current framelet. The camera's ephemeris time
   *   will be updated to the center of the framelet.
   *
   * @param framelet Current Framelet
   */
  void PushFrameCameraDetectorMap::SetFramelet(int framelet) {
    p_framelet = framelet;

    // We can add framelet padding to each band.  Compute the adjusted framelet
    // number
    int adjustedFramelet = (int) framelet - p_frameletOffset;
    double etTime = 0.0;

    // Use this information to compute the time of the framelet
    if (p_timeAscendingFramelets) {
      etTime = p_etStart + (adjustedFramelet - 1) * p_frameletRate;
    }
    else {
      etTime = p_etStart + (p_nframelets - adjustedFramelet) * p_frameletRate;
    }

    etTime += p_exposureDuration / 2.0;
    p_camera->SetEphemerisTime(etTime);
  }
}
