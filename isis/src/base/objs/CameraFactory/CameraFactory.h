#if !defined(CameraFactory_h)
#define CameraFactory_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2009/05/12 19:31:55 $                                                                 
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

#include "Camera.h"

namespace Isis {
  class Pvl;
  class Camera;
/**                                                                       
 * @brief Initializes a Camera Model                  
 *                                                                        
 * This factory class is used to obtain a Camera Model object given a PVL which 
 * contains a valid Instrument group. The Instrument group can come from an 
 * image/cube or a hand-created PVL file. The camera is loaded based on 
 * information using the SpacecraftName and IntrumentID contained in the 
 * Instrument group. It is plugin oriented. That is, this class looks in 
 * $ISISROOT/lib/Camera.plugin to convert the SpacecraftName and IntrumentID 
 * string into a pointer to the appropriate camera class 
 * (e.g., Viking, HiRISE, etc.). This allows programmers who develop new 
 * camera models to create a plugin without the need for recompiling all the 
 * Isis applications that use camera models. 
 * 
 * @ingroup Camera                                                  
 *                                                                        
 * @author 2005-05-10 Elizabeth Ribelin                                   
 *                                                                        
 * @internal                                        
 *  @history 2005-10-06 Elizabeth Miller - added unitTest.exclude file
 *  @history 2006-05-17 Elizabeth Miller - changed CameraManager.plugin to 
 *                                         Camera.plugin
 *  @history 2009-05-12 Steven Lambright - Added CameraVersion(...) and version
 *           checking.
 */                                                                       

  class CameraFactory {
    public:
      static Camera *Create(Pvl &pvl);
      static int CameraVersion(Pvl &pvl);

    private:
     /** 
      * Constructor (Its private, so you cannot use it.  Use the Create method
      * instead
      */
      CameraFactory() {};

      //! Destroys the CameraFactory object
      ~CameraFactory() {};
  };
};

#endif


