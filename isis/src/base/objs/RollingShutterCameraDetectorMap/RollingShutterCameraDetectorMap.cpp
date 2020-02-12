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
#include "RollingShutterCameraDetectorMap.h"

#include "iTime.h"

#include <cmath>
#include <utility>
#include <vector>


namespace Isis {
  /**
   * Constructs a RollingShutterCameraDetectorMap
   *
   * @param times A std::vector of normalized readout times
   * @param coeffs A std::vector of the coefficients of the polynomial that fit the jitter. The
   *                  coefficients are from highest degree to lowest degree.
   * @param parent The parent camera that uses the detector map.
   */
  RollingShutterCameraDetectorMap::RollingShutterCameraDetectorMap(
        Camera *parent,
        std::vector<double> times,
        std::vector<double> sampleCoeffs,
        std::vector<double> lineCoeffs) : CameraDetectorMap(parent) {
    m_times = times;
    m_sampleCoeffs = sampleCoeffs;
    m_lineCoeffs = lineCoeffs;
  }


  /**
   * Destructor
   */
  RollingShutterCameraDetectorMap::~RollingShutterCameraDetectorMap() {

  }


  /**
   * Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position from the parent
   * line/sample coordinate
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image
   *
   * @return conversion successful
   */
  bool RollingShutterCameraDetectorMap::SetParent(const double sample,
                                                  const double line) {
    return RollingShutterCameraDetectorMap::SetParent(sample, line, 0.0);
  }


  /**
   * Compute detector position from a parent image coordinate
   *
   * This method will compute the detector position from the parent
   * line/sample coordinate and an offset from the currently set time
   * in seconds. If the time has not already been set, the input
   * offset is not applied.
   *
   * @param sample Sample number in the parent image
   * @param line Line number in the parent image
   * @param deltaT option time offset from center of exposure in seconds
   *
   * @return conversion successful
   */
  bool RollingShutterCameraDetectorMap::SetParent(const double sample,
                                                  const double line,
                                                  const double deltaT) {
    std::pair<double, double> jittered = removeJitter(sample, line);
    p_parentSample = jittered.first;
    p_parentLine = jittered.second;
    p_detectorSample = (p_parentSample - 1.0) * p_detectorSampleSumming + p_ss;
    p_detectorLine   = (p_parentLine   - 1.0) * p_detectorLineSumming + p_sl;
    if (p_camera->isTimeSet()) {
      p_camera->setTime(p_camera->time().Et() + deltaT);
    }
    return true;
  }


  /**
   * Compute parent position from a detector coordinate
   *
   * This method will compute a parent sample/line given a
   * detector coordinate
   *
   * @param sample Sample number in the detector
   * @param line Line number in the detector
   *
   * @return conversion successful
   */
  bool RollingShutterCameraDetectorMap::SetDetector(const double sample, const double line) {
    p_detectorSample = sample;
    p_detectorLine   = line;
    p_parentSample   = (p_detectorSample - p_ss) / p_detectorSampleSumming + 1.0;
    p_parentLine     = (p_detectorLine   - p_sl) / p_detectorLineSumming   + 1.0;
    std::pair<double, double> jittered = applyJitter(p_parentSample, p_parentLine);
    p_parentSample += jittered.first;
    p_parentLine += jittered.second;
    return true;
  }


  /**
   * Iteratively finds a solution to "apply" jitter to an image coordinate.
   *
   * @param sample Image sample to apply jitter to.
   * @param line Image line to apply jitter to.
   *
   * @return std::pair<double,double> Returns the image coordinate with jitter applied to it.
   */
  std::pair<double, double> RollingShutterCameraDetectorMap::applyJitter(const double sample,
                                                                         const double line) {
    std::pair<double, double> jittered = removeJitter(sample, line);
    double currentSample = sample;
    double currentLine = line;

    int iterations = 0;
    int maxIterations = 50;

    p_detectorSample = sample;
    p_detectorLine   = line;
    p_parentSample   = (p_detectorSample - p_ss) / p_detectorSampleSumming + 1.0;
    p_parentLine     = (p_detectorLine   - p_sl) / p_detectorLineSumming   + 1.0;

    while((abs(sample - currentSample) < 1e-7) &&
          (abs(line - currentLine) < 1e-7)) {
      
      currentSample = (currentSample - jittered.first);
      currentLine = (currentLine - jittered.second);
      
      jittered = removeJitter(currentSample, currentLine);

      iterations++;
      if (iterations > maxIterations) {
        QString message = "Max Iterations reached.";
        throw IException(IException::Unknown, message, _FILEINFO_);
      }

    }
    
    return std::pair<double, double>(sample + currentSample, line + currentLine);
  }

  /**
   * Remove the distortion from the image (parent) coordinates.
   *
   * This is a helper function for the SetParent method.
   *
   * @param sample Input (parent) image sample coordinate.
   * @param line   Input (parent) image line coordinate.
   *
   * @return std::pair<double,double> Returns a pair of doubles corresponding to the de-jittered
   *         sample and line. This is in image coordinates.
   */
  std::pair<double, double> RollingShutterCameraDetectorMap::removeJitter(const double sample,
                                                                          const double line) {
    // De-jitter equation in form:
    //   c1(t^n) + c2(t^(n-2)) + ... + cn(t)
    double sampleDejitter = 0.0;
    double lineDejitter = 0.0;
    // Note that # sample coeffs == # line coeffs
    for (unsigned int n = 1; n <= m_lineCoeffs.size(); n++) {
      double sampleCoeff = m_sampleCoeffs[n - 1];
      double lineCoeff = m_lineCoeffs[n - 1];
      // Round to nearest line
      int timeEntry = 0;
      // Any lines that round past last time entry will use last time entry as index
      if (line > m_times.size()) {
        timeEntry = m_times.size();
      }
      else {
        timeEntry = (int)round(line);
      }
      double time = m_times[timeEntry - 1];
      int exponent = m_lineCoeffs.size() - (n - 1);
      time = pow(time, exponent);
      sampleDejitter += (sampleCoeff * time);
      lineDejitter += (lineCoeff * time);
    }
    return std::pair<double, double>(sample - sampleDejitter, line - lineDejitter);
  }
}
