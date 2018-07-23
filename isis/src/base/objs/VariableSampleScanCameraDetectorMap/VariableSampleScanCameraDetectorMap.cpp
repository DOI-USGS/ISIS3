/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/08/08 22:02:36 $
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

#include "VariableSampleScanCameraDetectorMap.h"
#include "Affine.h"
#include "CameraFocalPlaneMap.h"
#include "iTime.h"

namespace Isis {
  VariableSampleScanCameraDetectorMap::VariableSampleScanCameraDetectorMap(Camera *parent,
      std::vector< SampleRateChange > &sampleRates, Affine *fiducialMap) :
      SampleScanCameraDetectorMap(parent, sampleRates[0].GetStartEt(),
                                  sampleRates[0].GetSampleScanRate()),
                                  p_sampleRates(sampleRates) {

    // transx, transy are affine coefficients transforming from parent (Sp,Lp) to fiducial coordinate
    // system (Sf, Lf)
    // get coefficients of forward transform
    m_transx = fiducialMap->Coefficients(1);
    m_transy = fiducialMap->Coefficients(2);

    // Correct the Affine order - move the constant to the front
    m_transx.insert(m_transx.begin(), m_transx[2]);
    m_transx.pop_back();
    m_transy.insert(m_transy.begin(), m_transy[2]);
    m_transy.pop_back();

    // transs, transl are affine coefficients transforming from detector (FSC) (S,L) to parent (S,L)
    // get coefficients of inverse transform
    m_transs = fiducialMap->InverseCoefficients(1);
    m_transl = fiducialMap->InverseCoefficients(2);

    // Correct the Affine order - move the constant to the front
    m_transs.insert(m_transs.begin(), m_transs[2]);
    m_transs.pop_back();
    m_transl.insert(m_transl.begin(), m_transl[2]);
    m_transl.pop_back();
  };


  /** Compute parent image sample/line from a detector sample/line
   *
   * This method computes parent sample/line from detector sample/line.
   *
   * TODO: modify/remove this comment - This method will compute a parent sample given a
   * detector coordinate. The parent sample will be computed using the
   * the time in the parent camera
   *
   * @param sample detector sample
   * @param line   detector line
   *
   * @return conversion successful
   *
   * NOTE: Calling base classes results in unnecessary computations. Just set detector sample/line
   *       directly.
   */
  bool VariableSampleScanCameraDetectorMap::SetDetector(const double sample, const double line) {

    p_detectorSample = sample;
    p_detectorLine   = line;

    // currentEt is our known ephemeris time
    double currentEt = p_camera->time().Et();
    int rateIndex = p_sampleRates.size() - 1;

    while(rateIndex >= 0 && currentEt > p_sampleRates[rateIndex].GetStartEt()) {
      rateIndex --;
    }

    if(rateIndex < 0) {
      return false;
    }

    int rateStartSample = p_sampleRates[rateIndex].GetStartSample();
    double rateStartEt = p_sampleRates[rateIndex].GetStartEt();
    double rate = p_sampleRates[rateIndex].GetSampleScanRate();

    double etDiff = -(currentEt - rateStartEt);

    double fiducialSample = etDiff / rate + rateStartSample;

    // affine transformation from detector S/L to parent S/L
    // detector is fiducial coordinate system in pixels
    p_parentSample = m_transs[0] + fiducialSample * m_transs[1] + line * m_transs[2];
    p_parentLine   = m_transl[0] + fiducialSample * m_transl[1] + line * m_transl[2];

    SetSampleRate(rate);

    return true;
  }

  /** Compute detector sample/line from a parent image sample/line
   *
   * This method computes detector sample/line from parent sample/line. The detector sample is used
   * to retrieve time from the samplescantimes table stored in cube.
   *
   * TODO: samplescantimes table in cube needs to change to contain detector
   *       samples in fiducial coordinate system (FCS) instead of parent image samples
   *
   * @param sample parent image sample
   * @param line   parent image line
   *
   * @return conversion successful
   *
   * NOTE: Calling base classes results in unnecessary computations. Just set parent sample/line
   *       directly, then determine time.
   */
  bool VariableSampleScanCameraDetectorMap::SetParent(const double sample, const double line) {

    p_parentSample = sample;
    p_parentLine = line;

    // affine transformation from parent S/L to detector S/L
    // detector is fiducial coordinate system in pixels
    p_detectorSample = m_transx[0] + sample * m_transx[1] + line * m_transx[2];
    p_detectorLine   = m_transy[0] + sample * m_transy[1] + line * m_transy[2];

    int rateIndex = p_sampleRates.size() - 1;

//    while (rateIndex >= 0 && sample < p_sampleRates[rateIndex].GetStartSample() - 0.5) {
//      rateIndex --;
//    }
    while (rateIndex >= 0 && p_detectorSample < p_sampleRates[rateIndex].GetStartSample() - 0.5) {
      rateIndex --;
    }

    if (rateIndex < 0) {
      return false;
    }

    int rateStartSample = p_sampleRates[rateIndex].GetStartSample();
    double rateStartEt = p_sampleRates[rateIndex].GetStartEt();
    double rate = p_sampleRates[rateIndex].GetSampleScanRate();

    // TODO: confirm calculation of et, questions about application of 1/2 pixel here.
    //  double et = rateStartEt - (sample - rateStartSample + 0.5) * rate;
//  double et = rateStartEt - (sample - rateStartSample) * rate;
    double et = rateStartEt - (p_detectorSample - rateStartSample) * rate;

    // TODO: see Orrin's comment in ApolloPanoramicDetectorMap::SetParent()
    p_detectorSample = 0.0;

    SetSampleRate(rate);

    p_camera->setTime(et);

    return true;
  }
}
