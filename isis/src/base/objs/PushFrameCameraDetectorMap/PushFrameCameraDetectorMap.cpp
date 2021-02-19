/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "PushFrameCameraDetectorMap.h"

#include <QDebug>

#include <iomanip>

#include "CameraFocalPlaneMap.h"
#include "iTime.h"

namespace Isis {
  /** Construct a detector map for push frame cameras
   *
   * @param parent    The parent camera model for the detector map
   * @param etStart   starting ephemeris time in seconds
   *                  at the first framelet (not including padded
   *                  framelets).
   * @param frameletRate  the time in seconds between framelets
   * @param frameletHeight Physical height of framelet in lines
   *                       (don't account for summing)
   *
   */
  PushFrameCameraDetectorMap::PushFrameCameraDetectorMap(Camera *parent, const double etStart, 
                             const double frameletRate, int frameletHeight):
  CameraDetectorMap(parent) {
    p_etStart = etStart;
    p_exposureDuration = 0.0;
    p_frameletRate = frameletRate;
    p_frameletHeight = frameletHeight;
    p_frameletOffset = 0;
    p_flippedFramelets = true;
    p_timeAscendingFramelets = true;
    p_nframelets = 0;
    p_bandStartDetector = 0;
  }

  //! Destructor
  PushFrameCameraDetectorMap::~PushFrameCameraDetectorMap() {
  }



  /** Compute parent position from a detector coordinate
   *
   * This method will compute a parent sample given a
   * detector coordinate. The parent line and framelet line
   * will be computed.
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector
   *
   * @return @b bool conversion successful
   */
  bool PushFrameCameraDetectorMap::SetDetector(const double sample,
                                               const double line) {
    // Sometime folks want to write the framelets flipped in the EDR so
    // features match.  Take care of this.
    double unsummedFrameletLine;
    // first we need to determine the cube line number for the framelet that contains the given line
    // ??? comment this section...
    if (p_flippedFramelets) {
      unsummedFrameletLine = p_bandStartDetector + p_frameletHeight - line;
    }
    else {
      unsummedFrameletLine = line - p_bandStartDetector;
    }

    double unsummedFrameletSample = sample;

    // Convert framelet sample/line to summed framelet sample/line,
    // parent sample will be computed correctly
    if (!CameraDetectorMap::SetDetector(unsummedFrameletSample, unsummedFrameletLine)) {
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
    if (p_frameletLine > p_frameletHeight + 0.5) {
      return false;
    }

    if (p_frameletLine < 0.5) {
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
   * @return @b bool conversion successful
   */
  bool PushFrameCameraDetectorMap::SetParent(const double sample, const double line) {
    return SetParent(sample, line, 0.0);
  }


  /** Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position and framelet position
   * from the parent line/sample coordinate. The parent line will be used
   * to set the appropriate time in the parent camera.
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image 
   * @param deltaT offset from center time in seconds
   *
   * @return @b bool conversion successful
   */
  bool PushFrameCameraDetectorMap::SetParent(const double sample,
                                             const double line, 
                                             const double deltaT) {
    // Compute the height of a framelet taking into account the summing mode
    int actualFrameletHeight = (int)(p_frameletHeight / LineScaleFactor());

    // Compute the framelet number.  We could have padded with null framelets
    // at the top of the image so take that into account.  Setting the framelet
    // changes the time for the observation. Line starts at 0.5 (top of first framelet)
    // and framelet needs to start at 1.
    int framelet             = (int)((line - 0.5) / actualFrameletHeight) + 1;
    SetFramelet(framelet, deltaT);

    // Convert the parent line/sample to a framelet line/sample
    p_frameletLine = line - actualFrameletHeight * (framelet - 1);
    p_frameletSample = sample;

    // Convert the framelet line/sample to an unsummed framelet line/sample
    if (!CameraDetectorMap::SetParent(p_frameletSample, p_frameletLine, deltaT)) return false;
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
   * @param deltaT offset from center time in seconds
   */
  void PushFrameCameraDetectorMap::SetFramelet(int framelet, const double deltaT) {
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
    p_camera->setTime(etTime + deltaT);
  }



  /** 
   * Reset the starting ephemeris time
   *
   * Use this method to reset the starting time of the top edge of
   * the first line in the parent image.  That is the time, prior
   * to cropping, scaling, or padding.  Usually this will not need
   * to be done unless the time changes between bands.
   *
   * @param etStart starting ephemeris time in seconds
   *
   */
  void PushFrameCameraDetectorMap::SetStartTime(const double etStart) {
    p_etStart = etStart;
  }



  /** Change the exposure duration in seconds
   *
   *  Use this method to change the exposure duration of each
   *  framelet which may be different than the framelet rate.
   *
   * @param exposureDuration The time from the start of a framelet's exposure
   *                         to the end of the framlet's exposure duration.
   */
  void PushFrameCameraDetectorMap::SetExposureDuration(double exposureDuration) {
    p_exposureDuration = exposureDuration;
  }



  /** Reset the frame rate
   *
   * Use this method to reset the time between framelets.  Usually
   * this will not need to be done unless the rate changes between
   * bands.
   *
   * @param frameletRate the time from the start of a framelet to the start of the next framlet.
   *
   */
  void PushFrameCameraDetectorMap::SetFrameletRate(const double frameletRate) {
    p_frameletRate = frameletRate;
  }



  /**
   * Return the time in seconds between framelets
   * 
   * @return @b double The time from the start of a framelet's exposure
   *                   to the end of the framlet's exposure duration.
   */
  double PushFrameCameraDetectorMap::FrameletRate() const {
    return p_frameletRate;
  }



  /** Reset the frame offset
  *
  * Use this method to reset the frame offset.  Usually this will
  * not need to be done unless the offset changes between bands.
  *
  * @param frameletOffset Number of frames offset in cube
  *
  */
  void PushFrameCameraDetectorMap::SetFrameletOffset(int frameletOffset) {
    p_frameletOffset = frameletOffset;
  }



  /**
   * Return the frame offset
   * 
   * @return @b int The number of framelets padding the top of the band.
   */
  int PushFrameCameraDetectorMap::FrameletOffset() const {
    return p_frameletOffset;
  }


  /**
   * This method returns the current framelet. This framelet is
   * calculated when SetParent is called.
   *
   * @return @b int The current framelet
   */
  int PushFrameCameraDetectorMap::Framelet() {
    return p_framelet;
  }



  /** Change the starting line in the detector based on band
   *
   *  Use this method to change which line is read out of the
   *  CCD for any given band.  That is, as the virtual SetBand
   *  method for the specfic camera is invoked this method should
   *  be called.
   *
   * @param firstLine 0-based offset to the first line (first line of
   *                  detector = 0)
   */
  void PushFrameCameraDetectorMap::SetBandFirstDetectorLine(int firstLine) {
    p_bandStartDetector = firstLine;
  }



  /**
   * Return the starting line in the detector for the current band
   * 
   * @return @b int The starting line in the detector for the current band
   */
  int PushFrameCameraDetectorMap::GetBandFirstDetectorLine() {
    return p_bandStartDetector;
  }



  /** 
   *  Changes the direction of the framelets
   *
   *  Use this method to change which direction the framelets are ordered.
   *  In some cases, the top framelet from the raw instrument data has been
   *  moved to the bottom of the image and this compensates for that.
   *  
   *  If not set, the default is to not flip the framelet order
   *  
   *  @param frameletOrderReversed Indicates whether the order of the
   *                              framelets is should be flipped
   *  
   *  @param nframelets Number of framelets in each band, ignored
   *                    if frameletsReversed is set to false
   */
  void PushFrameCameraDetectorMap::SetFrameletOrderReversed(bool frameletOrderReversed, int nframelets) {
    p_timeAscendingFramelets = !frameletOrderReversed;
    p_nframelets = nframelets;
  }



  /** 
   *  Mirrors the each framelet in the file
   *
   *  Use this method to change which direction the framelets are geometrically
   *  placed. If the first line in the framelet has been changed to the last line
   *  in the framelet, then this should be true (DEFAULT).
   *  
   *  If not set, the default is true.
   *  
   * @param frameletsFlipped Indicates whether each framelet is flipped 
   *                         over a horizontal axis
   */
  void PushFrameCameraDetectorMap::SetFrameletsGeometricallyFlipped(bool frameletsFlipped) {
    p_flippedFramelets = frameletsFlipped;
  }



  /**
   * This returns the starting ET of this band
   *
   * @return @b double Starting time (often band-dependant)
   */
  double PushFrameCameraDetectorMap::StartEphemerisTime() const {
    return p_etStart;
  }



  /**
   * Return the total number of framelets including padding
   *
   * @return @b int The total number of framelets including padding
   */
  int PushFrameCameraDetectorMap::TotalFramelets() const {
    return (int)(p_camera->ParentLines() / (p_frameletHeight / LineScaleFactor()));
  }



  /**
   * This returns the calculated framelet sample
   *
   * @return @b double Current framelet sample
   */
  double PushFrameCameraDetectorMap::frameletSample() const {
    return p_frameletSample;
  }



  /**
   * This returns the calculated framelet line
   *
   * @return @b double Current framelet line
   */
  double PushFrameCameraDetectorMap::frameletLine() const {
    return p_frameletLine;
  }



  /**
   * This returns how many lines are considered a single framelet
   *
   * @return @b int Number of lines in a framelet
   */
  int PushFrameCameraDetectorMap::frameletHeight() const {
    return p_frameletHeight;
  }



  /**
   * Returns if the framelets are reversed from top-to-bottom.
   * 
   * @return @b bool if the framelets are reversed from top-to-bottom.
   */
  bool PushFrameCameraDetectorMap::timeAscendingFramelets() {
    return p_timeAscendingFramelets;
  }


  /**
   * This virtual method is for returning the exposure duration of a given pixel.
   * 
   * @param sample The sample of the desired pixel.
   * @param line The line of the desired pixel.
   * @param band The band of the desired pixel.
   * 
   * @return @b double The exposure duration for the desired pixel in seconds.
   */
  double PushFrameCameraDetectorMap::exposureDuration(const double sample,
                                                      const double line,
                                                      const int band) const {
    if (p_exposureDuration > 0) {
      return p_exposureDuration;
    }
    return p_frameletRate;
  }

}
