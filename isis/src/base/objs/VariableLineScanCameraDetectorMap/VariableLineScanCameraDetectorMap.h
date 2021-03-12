#ifndef VariableLineScanCameraDetectorMap_h
#define VariableLineScanCameraDetectorMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCameraDetectorMap.h"

namespace Isis {
  class LineRateChange;

  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a line scan camera.
   *
   * @author 2008-08-08 Steven Lambright
   *
   * @ingroup Camera
   * @see Camera
   *
   * @internal
   *   @history 2008-08-08 Steven Lambright Original version
   *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute CameraDetectorMap methods
   *   @history 2012-06-20 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           
   *                           coding standards
   *   @history 2016-10-18 Kristin Berry - Added new SetParent method to include time offset.
   *                           Updated test. References #4476.
   *   @history 2016-10-19 Jesse Mapel - Added exposureDuration method for accessing
   *                           a pixel's exposure duration. Updated test. References #4476.
   *   @history 2016-10-21 Jesse Mapel - Moved the LineRateChange look up to a separate method.
   *                           References #4476.
   *   @history 2016-10-21 Jesse Mapel and Kristin Berry - Fixed bug in SetDetector() and
   *                           SetParent() methods so that we now subtract 1/2 pixel from the
   *                           rateStartLine to calculate the p_parentLine and et variables,
   *                           respectively. This was needed since rateStartLine is the integer
   *                           value for the first line that uses the current rate. The integer
   *                           value indicates the center of the line, vertically, so we
   *                           subtract 0.5 to get the top of the needed start pixel. References
   *                           #4476.
   *   @history 2016-10-27 Jeannie Backer - Moved constructor documentation and destructor to
   *                           cpp file. References #4476.
   *
   */
  class VariableLineScanCameraDetectorMap : public LineScanCameraDetectorMap {
    public:
      VariableLineScanCameraDetectorMap(Camera *parent, std::vector< LineRateChange > &lineRates);

      virtual ~VariableLineScanCameraDetectorMap();

      virtual bool SetParent(const double sample, 
                             const double line);
      virtual bool SetParent(const double sample, 
                             const double line, 
                             const double deltaT);

      virtual bool SetDetector(const double sample, 
                               const double line);

      virtual double exposureDuration(const double sample,
                                      const double line,
                                      const int band) const;
      LineRateChange &lineRate(const double line) const;

    private:
      std::vector< LineRateChange > &p_lineRates; /**!< Timing information for the
                                                        sections of the image.*/
  };


  /**
   * Container class for storing timing information for a section of an image.
   * 
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2016-10-28 Jesse Mapel - Added documentation.
   */
  class LineRateChange {
    public:
      LineRateChange(int line, double stime, double rate) {
        p_line = line;
        p_stime = stime;
        p_rate = rate;
      };

      int GetStartLine() {
        return p_line;
      }
      double GetStartEt() {
        return p_stime;
      }
      double GetLineScanRate() {
        return p_rate;
      }

    private:
      int p_line;     //!< The first line in the section.
      double p_stime; //!< The time at the beginning of exposure of the first line.
      double p_rate;  //!< The time between lines in the section.
  };
};
#endif
