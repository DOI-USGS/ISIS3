#ifndef ReseauDistortionMap_h
#define ReseauDistortionMap_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:09 $                                                                 
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
     
#include <vector>
#include "CameraDistortionMap.h"

namespace Isis {    
  /** 
   * Distort/undistort focal plane coordinates
   * 
   * Creates a map for adding/removing optical distortions 
   * from the focal plane of the camera.  
   * 
   * @ingroup Camera
   * 
   * @author 2005-06-08 Elizabeth Ribelin
   * 
   * @internal
   *   @history 2005-12-07 Elizabeth Miller - Added check for colinearity in 
   *                  closest reseaus to fix a bug
   */
  class ReseauDistortionMap : public CameraDistortionMap {
    public:
      ReseauDistortionMap(Camera *parent, Pvl &labels, const std::string &fname);

      virtual bool SetFocalPlane(const double dx, const double dy);

      virtual bool SetUndistortedFocalPlane(const double ux, const double uy);  

    private:
      std::vector<double> p_rlines, p_rsamps;        //!<Refined Reseau Locations
      std::vector<double> p_mlines, p_msamps;        //!<Master Reseau Locations
      double p_distortedLines, p_distortedSamps;     /**<Dimensions of distorted 
                                                        cube*/
      double p_undistortedLines, p_undistortedSamps; /**<Dimensions of 
                                                        undistorted cube*/
      int p_numRes;                                  //!<Number of Reseaus
      double p_pixelPitch;                           /**<Pixel Pitch of parent 
                                                        Camera*/ 
  };
};
#endif

