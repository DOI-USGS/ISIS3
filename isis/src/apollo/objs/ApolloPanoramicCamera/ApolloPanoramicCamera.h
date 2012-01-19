#ifndef ApolloPanoramicCamera_h
#define ApolloPanoramicCamera_h

#include "LineScanCamera.h"


/**
 *
 * @internal
 *    @history 2011-11-21 Orrin Thomas - Original Version
 */

namespace Isis {
  namespace Apollo {
    /**
     * @internal
     *   @history 2011-09-19 Created Orrin Thomas
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
