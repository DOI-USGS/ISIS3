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

#include "VariableLineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {
  VariableLineScanCameraDetectorMap::VariableLineScanCameraDetectorMap(
            Camera *parent, std::vector< LineRateChange > &lineRates) :
            LineScanCameraDetectorMap(parent, lineRates[0].GetStartEt(), lineRates[0].GetLineScanRate()), p_lineRates(lineRates) {
  };
       
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
    if (!CameraDetectorMap::SetDetector(sample,line)) {
      return false;
    }

    // currEt is our known et time
    double currEt = p_camera->EphemerisTime();
    int rateIndex = p_lineRates.size()-1;

    while(rateIndex >= 0 && currEt < p_lineRates[rateIndex].GetStartEt() - 0.5) {
      rateIndex --;
    }

    if(rateIndex < 0) {
      return false;
    }

    int rateStartLine = p_lineRates[rateIndex].GetStartLine();
    double rateStartEt = p_lineRates[rateIndex].GetStartEt();
    double rate = p_lineRates[rateIndex].GetLineScanRate();

    double etDiff = currEt - rateStartEt;
    p_parentLine = etDiff / rate + rateStartLine;
 
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
    if (!CameraDetectorMap::SetParent(sample,line)) {
      return false;
    }

    p_detectorLine = p_camera->FocalPlaneMap()->DetectorLineOffset();

    int rateIndex = p_lineRates.size()-1;

    while(rateIndex >= 0 && line < p_lineRates[rateIndex].GetStartLine() - 0.5) {
      rateIndex --;
    }

    if(rateIndex < 0) {
      return false;
    }

    int rateStartLine = p_lineRates[rateIndex].GetStartLine();
    double rateStartEt = p_lineRates[rateIndex].GetStartEt();
    double rate = p_lineRates[rateIndex].GetLineScanRate();

    double et = rateStartEt + (line - rateStartLine) * rate; 
    //printf("et = %.8f = %.8f + (%.3f - %i) * %.8f\n", et, rateStartEt, line, rateStartLine, rate);

    SetLineRate(rate);

    p_camera->SetEphemerisTime(et);

    return true;
  }
}
