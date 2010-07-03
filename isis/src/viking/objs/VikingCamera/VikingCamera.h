#ifndef VikingCamera_h
#define VikingCamera_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2009/08/31 15:12:32 $                                                                 
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

#include "FramingCamera.h"

namespace Isis {
  /**                                                                       
   * @brief Viking Camera Model                
   *                                        
   * This is the camera model for both viking orbiter 1 and viking orbiter 2, 
   * both cameras A and B.
   * 
   * @ingroup SpiceInstrumentsAndCameras                                   
   * @ingroup Viking
   *                                                                        
   * @author 2005-06-09 Elizabeth Ribelin  
   * 
   * @internal  
   *   @history 2005-11-15 Elizabeth Miller - Fixed problems caused by viking 
   *                                          data area split
   *   @history 2006-05-17 Elizabeth Miller - Depricated CameraManager to 
   *                                          CameraFactory
   *   @history 2006-06-14 Elizabeth Miller - Changed format of unitTest to
   *                                          fix problems with minor naif
   *                                          changes
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *            method instead of CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no
   *           longer inherit directly from Camera
   */                                                                       
  class VikingCamera : public Isis::FramingCamera {
    public:
      VikingCamera (Isis::Pvl &lab);

      //! Destroys the VikingCamera Object
      ~VikingCamera () {};      
  };
};
#endif

