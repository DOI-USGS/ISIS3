/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCameraDetectorMap.h"

#include "CameraFocalPlaneMap.h"
#include "iTime.h"

namespace Isis {

  /** Construct a detector map for line scan cameras
   *
   * @param parent    The parent camera model for the detector map
   * @param etStart   starting ephemeris time in seconds
   *                  at the top of the first line
   * @param lineRate  the time in seconds between lines
   *
   */
  LineScanCameraDetectorMap::LineScanCameraDetectorMap(Camera *parent, 
                                                       const double etStart,
                                                       const double lineRate) :
                                                       CameraDetectorMap(parent) {
    p_etStart = etStart;
    p_lineRate = lineRate;
  }


  //! Destructor
  LineScanCameraDetectorMap::~LineScanCameraDetectorMap() {
  }


  /** Reset the starting ephemeris time.
   *
   * Use this method to reset the starting time of the top edge of
   * the first line in the parent image.  That is the time, prior
   * to cropping, scaling, or padding.  Usually this will not need
   * to be done unless the time changes between bands.
   *
   * @param etStart starting ephemeris time in seconds
   *
   */
  void LineScanCameraDetectorMap::SetStartTime(const double etStart) {
    p_etStart = etStart;
  }


  /** Reset the line rate.
   *
   * Use this method to reset the time between lines.  Usually this
   * will not need to be done unless the rate changes between bands.
   *
   * @param lineRate the time in seconds between lines
   *
   */
  void LineScanCameraDetectorMap::SetLineRate(const double lineRate) {
    p_lineRate = lineRate;
  }


  /**
   * Access the time, in seconds, between scan lines.
   * 
   * @return Line rate.
   */
  double LineScanCameraDetectorMap::LineRate() const {
    return p_lineRate;
  }


  /** Compute parent position from a detector coordinate
   *
   * This method will compute a parent sample given a
   * detector coordinate.  The parent line will be computed using the
   * the time in the parent camera
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector
   *
   * @return conversion successful
   */
  bool LineScanCameraDetectorMap::SetDetector(const double sample, 
                                              const double line) {

    if(!CameraDetectorMap::SetDetector(sample, line)) {
      return false;
    }
    double etDiff = p_camera->time().Et() - p_etStart;
    p_parentLine = etDiff / p_lineRate + 0.5;
    return true;

  }


  /** Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position from the parent
   * line/sample coordinate.  The parent line will be used to set the
   * appropriate time in the parent camera.
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image
   *
   * @return @b bool conversion successful
   */
  bool LineScanCameraDetectorMap::SetParent(const double sample, 
                                            const double line) {
    return SetParent(sample, line, 0.0);
  }


  /** Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position from the parent
   * line/sample coordinate.  The parent line and input deltaT offset 
   * will be used to set the appropriate time in the parent camera. 
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image 
   * @param deltaT offset in seconds from current exposure time  
   *
   * @return @b bool conversion successful
   */
  bool LineScanCameraDetectorMap::SetParent(const double sample, 
                                            const double line, 
                                            const double deltaT) {

    if(!CameraDetectorMap::SetParent(sample, line)) {
      return false;
    }
    p_detectorLine = p_camera->FocalPlaneMap()->DetectorLineOffset();
    double etLine = p_etStart + p_lineRate * (line - 0.5);
    p_camera->setTime(etLine + deltaT);
    return true;

  }


  /**
   * Access the starting time at the top edge of the first line in the parent
   * image.
   *  
   * @return The start ephemeris time for the image. 
   */
  double LineScanCameraDetectorMap::StartTime() const {
    return p_etStart;
  }


  /**
   * @breif This virtual method is for returning the exposure duration of a pixel.
   * 
   * For a fixed rate line scan camera this will return the line scan rate.  Note that this may
   * not be the exact same thing as the exposure duration.  If there is some amount of padding
   * between exposures, then the line scan rate is actually the exposure duration plus that
   * padding.
   * 
   * @param sample The sample of the desired pixel.
   * @param line The line of the desired pixel.
   * @param band The band of the desired pixel.
   * 
   * @return @b double The exposure duration for the desired pixel in seconds.
   * 
   * @see LineRate
   * 
   * @TODO Add a way to account for the padding described above.  Push frame cameras handle this
   *       by having a separate exposure duration member that can be set by individual camera
   *       models.  Then, if the exposure duration member is not it's default value (0)
   *       return it instead of the line rate.
   */
  double LineScanCameraDetectorMap::exposureDuration(const double sample,
                                                     const double line,
                                                     const int band) const {
    return LineRate();
  }

}
