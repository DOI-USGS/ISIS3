/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:08 $                                                                 
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

#include "LineScanCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"

namespace Isis {
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
  bool LineScanCameraDetectorMap::SetDetector(const double sample, 
                                              const double line) {
    if (!CameraDetectorMap::SetDetector(sample,line)) return false;
    double etDiff = p_camera->EphemerisTime() - p_etStart;
    p_parentLine = etDiff / p_lineRate + 0.5;
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
  bool LineScanCameraDetectorMap::SetParent(const double sample, 
                                            const double line) {
    if (!CameraDetectorMap::SetParent(sample,line)) return false;
    p_detectorLine = p_camera->FocalPlaneMap()->DetectorLineOffset();
    double etLine = p_etStart + p_lineRate * (line - 0.5);
    p_camera->SetEphemerisTime(etLine);
    return true;
  }
}
