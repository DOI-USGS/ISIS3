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

#ifndef LineScanCameraSkyMap_h
#define LineScanCameraSkyMap_h

#include "CameraSkyMap.h"

namespace Isis {    
  /** Convert between undistorted focal plane and ra/dec coordinates
   * 
   * This class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and sky (ra/dec).  This 
   * class handles the case of line scan cameras.
   * 
   * @ingroup Camera
   * 
   * @see Camera
   * 
   * @internal
   * 
   * @history 2005-02-08 Jeff Anderson
   * Original version
   *
   * @history 2005-10-13 Jeff Anderson
   * Fixed a bug.  Removed failure comparsion test on 
   * Sensor::SetRightAscensionDeclination calls.  A failure indicated we
   * didn't hit the target but we don't care here since the target is the
   * sky.
   * 
   */
  class LineScanCameraSkyMap : public CameraSkyMap {
    public:
      //! Constructor
      LineScanCameraSkyMap(Camera *parent) : CameraSkyMap(parent) {};
  
      //! Destructor
      virtual ~LineScanCameraSkyMap() {};

      virtual bool SetSky(const double ra, const double dec);
  };
};
#endif
