/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/03/07 17:57:27 $
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

#ifndef SampleScanCameraDetectorMap_h
#define SampleScanCameraDetectorMap_h

#include "CameraDetectorMap.h"

namespace Isis {
  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent detector coordinates
   * (sample/line) and detector coordinates for a sample scan camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2016-09-07 Ken Edmundson
   *
   * @internal
   *   @history 2016-09-07 Ken Edmundson Original version.
   *
   */
  class SampleScanCameraDetectorMap : public CameraDetectorMap {
    public:

      /** Construct a detector map for sample scan cameras
       *
       * @param parent    The parent camera model for the detector map
       * @param etStart   starting ephemeris time in seconds
       *                  at the left of the first sample
       * @param sampleRate  the time in seconds between samples
       *
       */
      SampleScanCameraDetectorMap(Camera *parent, const double etStart,
                                const double sampleRate) :
        CameraDetectorMap(parent) {
        p_etStart = etStart;
        p_sampleRate = sampleRate;

//        p_detectorSample = 0.0;
      }


      //! Destructor
      virtual ~SampleScanCameraDetectorMap() {};

      /** Reset the starting ephemeris time
       *
       * Use this method to reset the starting time of the left edge of
       * the first sample in the parent image. That is the time, prior
       * to cropping, scaling, or padding. Usually this will not need
       * to be done unless the time changes between bands.
       *
       * @param etStart starting ephemeris time in seconds
       *
       */
      void SetStartTime(const double etStart) {
        p_etStart = etStart;
      };


      /** Reset the sample rate
       *
       * Use this method to reset the time between samples. Usually this
       * will not need to be done unless the rate changes between bands.
       *
       * @param sampleRate the time in seconds between samples
       *
       */
      void SetSampleRate(const double sampleRate) {
        p_sampleRate = sampleRate;
      };


      //! Return the time in seconds between scan columns
      double SampleRate() const {
        return p_sampleRate;
      };


      virtual bool SetParent(const double sample, const double line);
      virtual bool SetDetector(const double sample, const double line);
      double StartTime() const;

    private:
      double p_etStart;     //!< Starting time at the left of the 1st parent sample
      double p_sampleRate;    //!< iTime between samples in parent cube
  };
};
#endif
