#ifndef HiriseCamera_h
#define HiriseCamera_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2009/08/31 15:12:31 $                                                                 
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

#include "LineScanCamera.h"

namespace Isis {
  namespace Mro {
    /**
     * @brief Hirise Camera Model
     * 
     * This class is the implementation of the camera model
     * for the MRO HiRISE instrument.
     *  
     * @ingroup SpiceInstrumentsAndCameras
     * @ingroup MarsReconnaissanceOrbiter 
     * 
     * @author  2005-02-22 Jim Torson
     * 
     * @internal
     *  @history 2005-10-03 Elizabeth Miller - Modified file to support Doxygen
     *                                         documentation
     *  @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
     *           change. Also, now using the new LoadCache(...) method instead of
     *           CreateCache(...).
     *  @history 2009-03-08 Debbie A. Cook Removed reference to obsolete LineScanCameraDetectorMap
     *           method SetXAxisTimeDependent
     *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
     *            inherit directly from Camera
     */
    class HiriseCamera : public Isis::LineScanCamera {
      public:
        // Constructs a HiriseCamera object
        HiriseCamera (Isis::Pvl &lab);
    
        // Destroys the HiriseCamera object
        ~HiriseCamera ();
    };
  };
};

#endif
