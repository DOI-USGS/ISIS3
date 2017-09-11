/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/04/08 02:32:55 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the
 *   USGS as to the accuracy and functioning of such
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
