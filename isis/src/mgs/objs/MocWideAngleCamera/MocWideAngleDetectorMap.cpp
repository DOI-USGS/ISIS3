/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "MocWideAngleDetectorMap.h"

using namespace std;
namespace Isis {
  /** Constructor for MocWideAngleDetectorMap class
   *
   * @param sample
   * @param line
   */
  bool MocWideAngleDetectorMap::SetDetector(const double sample,
      const double line) {
    if(!LineScanCameraDetectorMap::SetDetector(sample, line)) return false;

    if((p_moclab->CrosstrackSumming() == 13) ||
        (p_moclab->CrosstrackSumming() == 27)) {
      int detector = (int) sample;
      if(detector < 1) detector = 1;
      if(detector >= p_moclab->Detectors()) detector = p_moclab->Detectors() - 1;
      double samp1 = p_moclab->Sample(detector - 1);
      double samp2 = p_moclab->Sample(detector);
      if(samp1 < 0.0) return false;
      if(samp2 < 0.0) return false;

      double m = (samp2 - samp1);
      p_parentSample = m * (sample - detector) + samp2;
    }

    return true;
  }


  /**
   * @param sample 
   * @param line 
   *  
   * @return @b bool 
   */
  bool MocWideAngleDetectorMap::SetParent(const double sample,
                                          const double line) {
    if(!LineScanCameraDetectorMap::SetParent(sample, line)) return false;

    // Handle variable summing if necessary
    if((p_moclab->CrosstrackSumming() == 13) ||
        (p_moclab->CrosstrackSumming() == 27)) {
      int isamp = (int) sample;
      if(isamp < 2) {
        p_detectorSample = p_moclab->StartDetector(1);
        p_detectorSample += sample - 1.0;
      }
      else if(isamp > p_camera->Samples()) {
        p_detectorSample = p_moclab->StartDetector(p_camera->Samples());
        p_detectorSample += sample - p_camera->Samples();
      }
      else {
        int ss = p_moclab->StartDetector(isamp - 1);
        int es = p_moclab->EndDetector(isamp - 1);
        double samp1 = (ss + es) / 2.0;

        ss = p_moclab->StartDetector(isamp);
        es = p_moclab->EndDetector(isamp);
        double samp2 = (ss + es) / 2.0;

        double m = (samp2 - samp1);
        p_detectorSample = m * (sample - isamp) + samp1;
      }
    }
    return true;
  }
}

