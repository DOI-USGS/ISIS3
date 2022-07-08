/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:08 $
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
  bool LineScanCameraDetectorMap::SetParent(NaifContextPtr naif, 
                                            const double sample, 
                                            const double line) {
    return SetParent(naif, sample, line, 0.0);
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
  bool LineScanCameraDetectorMap::SetParent(NaifContextPtr naif, 
                                            const double sample, 
                                            const double line, 
                                            const double deltaT) {

    if(!CameraDetectorMap::SetParent(naif, sample, line)) {
      return false;
    }
    p_detectorLine = p_camera->FocalPlaneMap()->DetectorLineOffset();
    double etLine = p_etStart + p_lineRate * (line - 0.5);
    p_camera->setTime(etLine + deltaT, naif);
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
