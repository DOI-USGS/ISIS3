/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "VariableLineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "iTime.h"

namespace Isis {
  /**
   * Constructs a VariableLineScanCameraDetectorMap.
   *
   * @param parent The camera
   * @param p_lineRates This should be a vector with an entry for every
   *          scan rate change in it. The pair consists of the line number and
   *          ET of the changed time; the first entry should be line 1 and the last
   *          entry should be one line past the end of the image. See
   *          HrscCamera for an example.
   */
  VariableLineScanCameraDetectorMap::VariableLineScanCameraDetectorMap(
      Camera *parent, 
      std::vector< LineRateChange > &lineRates) :
      LineScanCameraDetectorMap(parent, 
                                lineRates[0].GetStartEt(), 
                                lineRates[0].GetLineScanRate()), 
                                p_lineRates(lineRates) {
  }


  /**
   * Destructor
   */
  VariableLineScanCameraDetectorMap::~VariableLineScanCameraDetectorMap() {
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
  bool VariableLineScanCameraDetectorMap::SetDetector(const double sample,
                                                      const double line) {
    // Use the parent SetDetector for the sample, which should work fine
    if (!CameraDetectorMap::SetDetector(sample, line)) {
      return false;
    }

    // currEt is our known et time
    double currEt = p_camera->time().Et();
    int rateIndex = p_lineRates.size() - 1;

    while (rateIndex >= 0 && currEt < p_lineRates[rateIndex].GetStartEt() - 0.5) {
      rateIndex --;
    }

    if (rateIndex < 0) {
      return false;
    }

    int rateStartLine = p_lineRates[rateIndex].GetStartLine();
    double rateStartEt = p_lineRates[rateIndex].GetStartEt();
    double rate = p_lineRates[rateIndex].GetLineScanRate();

    double etDiff = currEt - rateStartEt;
    p_parentLine = etDiff / rate + (rateStartLine - 0.5);

    //std::cout << "p_parentLine = " << p_parentLine << " = " << etDiff << "/" << rate << " + " << rateStartLine << std::endl;

    SetLineRate(rate);

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
   * @return conversion successful
   */
  bool VariableLineScanCameraDetectorMap::SetParent(const double sample, 
                                                    const double line) {
    return SetParent(sample, line, 0.0); 
  }


  /** Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position from the parent
   * line/sample coordinate.  The parent line will be used to set the
   * appropriate time in the parent camera.
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image 
   * @param deltaT offset in seconds from center exposure time 
   *
   * @return @b bool conversion successful
   */
  bool VariableLineScanCameraDetectorMap::SetParent(const double sample, 
                                                    const double line, 
                                                    const double deltaT) {
    if (!CameraDetectorMap::SetParent(sample, line, deltaT)) {
      return false;
    } //check to make sure we're not doubling the "DeltaT" 

    p_detectorLine = p_camera->FocalPlaneMap()->DetectorLineOffset();

    int rateIndex = p_lineRates.size() - 1;

    while (rateIndex >= 0 && line < p_lineRates[rateIndex].GetStartLine() - 0.5) {
      rateIndex --;
    }

    if (rateIndex < 0) {
      return false;
    }

    int rateStartLine = p_lineRates[rateIndex].GetStartLine();
    double rateStartEt = p_lineRates[rateIndex].GetStartEt();
    double rate = p_lineRates[rateIndex].GetLineScanRate();

    /**
     * The following time calculation has some potential pitfalls.  If the line rate and
     * exposure duration are not the same, such as with the Dawn VIR camera, then this will
     * not return the true center pixel time.  If there is a difference, then the calculation
     * should be rateStartEt + (line-rateStartLine) * rate + exposureDuration()/2.  See
     * exposureDuration()'s documentation for more information.
     */
    double et = rateStartEt + (line - (rateStartLine - 0.5)) * rate;
    //printf("et = %.8f = %.8f + (%.3f - %i) * %.8f\n", et, rateStartEt, line, rateStartLine, rate);

    SetLineRate(rate);

    p_camera->setTime(et + deltaT);

    return true;
  }


  /**
   * @brief This virtual method is for returning the exposure duration of a given pixel.
   * 
   * For a variable line scan camera, the exposure duration is assumed to be the line scan
   * rate for the given line.  Note, this may not be the actual exposure duration.  The line scan
   * rate is the time from the beginning of one line to the beginning of the next.  The exposure
   * duration is the time from the beginning of a line to the end of that line.  So, if the end
   * of a line is not the beginning of the next line, these two values will not be the same.
   * 
   * @param sample The sample of the desired pixel.
   * @param line The line of the desired pixel.
   * @param band The band of the desired pixel.
   * 
   * @return @b double The exposure duration for the desired pixel in seconds.
   * 
   * @throws IException::Programmer "line was not found in the line scan rate table."
   * 
   * @TODO How do we get the actual exposure duration?
   */
  double VariableLineScanCameraDetectorMap::exposureDuration(const double sample,
                                                             const double line,
                                                             const int band) const {
    return lineRate(line).GetLineScanRate();
  }


  /**
   * Get the line rate information for a given line.
   * 
   * @param line The line to find the line rate information for.
   * 
   * @return @b LineRateChange& The LineRateChange object that is used for time calculations
   *                            with the input line.
   * 
   * @TODO How to handle if rateIndex < 0?
   */
  LineRateChange &VariableLineScanCameraDetectorMap::lineRate(const double line) const{
    int rateIndex = p_lineRates.size() - 1;

    while (rateIndex >= 0 && line < p_lineRates[rateIndex].GetStartLine() - 0.5) {
      rateIndex --;
    }

    if (rateIndex < 0) {
      //This may not be a good idea
      //  SetParent used to return false here
      //  exposureDuration used to throw an exception here
      rateIndex = 0;
    }

    return p_lineRates[rateIndex];
  }

}
