#ifndef ApolloPanoramicCamera_h
#define ApolloPanoramicCamera_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.7 $                                                             
 * $Date: 2005/10/03 22:43:39 $                                                                 
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

#include "LineScanCamera.h"


namespace Isis {
  namespace Apollo {
    /**                                                                       
     * @brief Brief coming soon
     *                                                                        
     * Description coming soon
     *                                                                        
     * @author 2011-09-19 Orrin Thomas
     *                                                                        
     * @internal                                                              
     *   @history 2011-09-19 Orrin Thomas - Original version
     */        
    class ApolloPanoramicCamera : public LineScanCamera {
    public:
      ApolloPanoramicCamera(Isis::Pvl &lab);

      ~ApolloPanoramicCamera() {};

      /**
      * CK frame ID -  - Instrument Code from spacit run on CK
      *  
      * @return @b int The appropriate instrument code for the "Camera-matrix" 
      *         Kernel Frame ID
      */
      virtual int CkFrameId() const {return p_CkFrameId; }  //this sensor was used on multiple missions so it is necessary to check which Apollo 

      /** 
      * CK Reference ID - J2000
      * 
      * @return @b int The appropriate instrument code for the "Camera-matrix"
      *         Kernel Reference ID
      */
      virtual int CkReferenceId() const { return (1); }

      /** 
      *  SPK Reference ID - J2000
      *  
      * @return @b int The appropriate instrument code for the Spacecraft 
      *         Kernel Reference ID
      */
      virtual int SpkReferenceId() const { return (1); }
    private:
      int p_CkFrameId;
    };
  };
};

#endif
