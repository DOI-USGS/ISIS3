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

      SampleScanCameraDetectorMap(Camera *parent, const double etStart, const double sampleRate);

      virtual ~SampleScanCameraDetectorMap() {};

      void SetStartTime(const double etStart);
      void SetSampleRate(const double sampleRate);
      double SampleRate() const;

      virtual bool SetParent(const double sample, const double line);
      virtual bool SetDetector(const double sample, const double line);
      double StartTime() const;

    private:
      double p_etStart;     //!< Starting time at the left of the 1st parent sample
      double p_sampleRate;    //!< iTime between samples in parent cube
  };
};
#endif
