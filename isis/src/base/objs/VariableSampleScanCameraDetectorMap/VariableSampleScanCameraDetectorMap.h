/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/03/07 18:02:33 $
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

#ifndef VariableSampleScanCameraDetectorMap_h
#define VariableSampleScanCameraDetectorMap_h

#include "SampleScanCameraDetectorMap.h"

using namespace std;

namespace Isis {
  class Affine;
  class SampleRateChange;

  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent detector coordinates
   * (sample/line) and detector coordinates for a sample scan camera.
   *
   * @author 2016-09-07 Ken Edmundson
   *
   * @ingroup Camera
   * @see Camera
   *
   * @internal
   *   @history 2008-08-08 Ken Edmundson Original version
   *
   */
  class VariableSampleScanCameraDetectorMap : public SampleScanCameraDetectorMap {
    public:
      /**
       * Constructs a VariableSampleScanCameraDetectorMap.
       *
       * @param parent The camera
       * @param p_sampleRates This should be a vector with an entry for every
       *          scan rate change in it. The pair consists of the sample number and
       *          ET of the changed time; the first entry should be sample 1 and the last
       *          entry should be one line past the end of the image. See
       *          HrscCamera for an example of a VariableLineScanCamera and Apollo Panoramic
       *          for an example of a VariableSampleScanCamera.
       */
      VariableSampleScanCameraDetectorMap(Camera *parent,
                                          std::vector< SampleRateChange > &sampleRates,
                                          Affine *fiducialMap);

      //! Destructor
      virtual ~VariableSampleScanCameraDetectorMap() {};

      virtual bool SetParent(const double sample, const double line);

      virtual bool SetDetector(const double sample, const double line);

    private:
      std::vector< SampleRateChange > &p_sampleRates;

      vector<double> m_transx; /**< affine transformation from parent S/L to detector S/L */
      vector<double> m_transy; /**< detector is fiducial coordinate system in pixels */

      vector<double> m_transs; /**< affine transformation from detector S/L to parent S/L */
      vector<double> m_transl;
  };


  /**
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class SampleRateChange {
    public:
      SampleRateChange(int sample, double stime, double rate) {
        p_sample = sample;
        p_stime = stime;
        p_rate = rate;
      };

      int GetStartSample() {
        return p_sample;
      }
      double GetStartEt() {
        return p_stime;
      }
      double GetSampleScanRate() {
        return p_rate;
      }

    private:
      int p_sample;
      double p_stime;
      double p_rate;
  };
};
#endif
