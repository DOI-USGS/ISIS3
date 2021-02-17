/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#ifndef CameraDetectorMap_h
#define CameraDetectorMap_h

#include "Camera.h"

namespace Isis {
  /** Convert between parent image coordinates and detector coordinates
   *
   * This base class is used to convert between parent detector coordinates
   * (sample/line) and detector coordinates for the camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2005-02-03 Jeff Anderson
   *
   * @internal
   *   @history 2009-04-02 Debbie A. Cook Removed obsolete methods IsXAxisTimeDependent,
   *                           IsYAxisTimeDependent, XAxisDirection, YAxisDirection,
   *                           SetXAxisDirection, and SetYAxisDirection
   *   @history 2012-07-25 Kris Becker - Corrected bug in SetStartingDetectorLine() method
   *                           in that it applied this value to the sample starting detector
   *                           rather than the line starting detector.  It appeared to only
   *                           affect the MESSENGER/MDIS camera model, however.
   *   @history 2013-02-11 E. Howington-Kraus - Added accessor methods:
   *                           AdjustedStartingSample() and AdjustedStartingLine().
   *                           These are tested by application socetlinescankeywords
   *                           since no unitTest exists. Fixed indentation
   *                           of history entries.  References #1490.
   *   @history 2014-04-17 Jeannie Backer - Added padding for ISIS coding standards
   *                           compliance. References #1659.
   *   @history 2016-10-18 Kristin Berry - Added additional SetParent with a time offset and
   *                           added unitTest coverage. References #4476.
   *   @history 2016-10-19 Jesse Mapel - Added exposureDuration method for accessing
   *                           a pixel's exposure duration. Updated test. References #4476.
   *   @history 2016-10-27 Jeannie Backer - Moved method implementation to cpp file.
   *                           Added some variable documentation. References #4476.
   *   @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   */
  class CameraDetectorMap {
    public:
      CameraDetectorMap(Camera *parent = 0);

      virtual ~CameraDetectorMap();

      virtual bool SetParent(const double sample, 
                             const double line);
      virtual bool SetParent(const double sample, 
                             const double line, 
                             const double deltaT);

      virtual bool SetDetector(const double sample, 
                               const double line);

      double AdjustedStartingSample() const;

      double AdjustedStartingLine() const;

      double ParentSample() const;
      double ParentLine() const;
      double DetectorSample() const;
      double DetectorLine() const;

      /** Set the starting detector sample
       *
       * Use this method to specify the starting detector that represents
       * the first image sample in the cube.  If not set the default is 1.
       *
       * @param sample Starting detector sample
       *
       */
      inline void SetStartingDetectorSample(const double sample) {
        p_startingDetectorSample = sample;
        Compute();
      }


      /** Set the starting detector line
       *
       * Use this method to specify the starting detector that represents
       * the first image line in the cube.  If not set the default is 1.
       *
       * @param line Starting detector line
       *
       */
      inline void SetStartingDetectorLine(const double line) {
        p_startingDetectorLine = line;
        Compute();
      }


      /** Set sample summing mode
       *
       * Use this method to specify if detector samples are summed/averaged.
       * That is, one image sample represents the average of N detectors.
       * If not set the default is 1.
       *
       * @param summing Sample summing mode
       *
       */
      inline void SetDetectorSampleSumming(const double summing) {
        p_detectorSampleSumming = summing;
        Compute();
      }


      /** Set line summing mode
       *
       * Use this method to specify if detector lines are summed/averaged.
       * That is, one image lines represents the average of N detectors.
       * If not set the default is 1.
       *
       * @param summing Line summing mode
       *
       */
      inline void SetDetectorLineSumming(const double summing) {
        p_detectorLineSumming = summing;
        Compute();
      }


      virtual double SampleScaleFactor() const;
      virtual double LineScaleFactor() const;
      virtual double LineRate() const;

      virtual double exposureDuration(const double sample,
                                      const double line,
                                      const int band) const;

    protected:
      Camera *p_camera;               /**< Pointer to the camera.*/

      double p_parentSample;          //!< The parent sample calculated from the detector.
      double p_parentLine;            //!< The parent line calculated from the detector.
      double p_detectorLine;          /**< Detector coordinate line value.*/
      double p_detectorSample;        /**< Detector coordinate sample value.*/

      double p_detectorSampleSumming; //!< The scaling factor for computing sample resolution
      double p_detectorLineSumming;   //!< The scaling factor for computing line resolution
      double p_startingDetectorSample;/**< Detector start coordinate sample value.*/
      double p_startingDetectorLine;  /**< Detector start coordinate line value.*/

      double p_ss;                    /**< Start sample.*/
      double p_sl;                    /**< Start line.*/

    private:
      void Compute();
  };
};
#endif
