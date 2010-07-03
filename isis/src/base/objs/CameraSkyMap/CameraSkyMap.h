/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/07/15 15:07:19 $                                                                 
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

#ifndef CameraSkyMap_h
#define CameraSkyMap_h

#include "Camera.h"

namespace Isis {    
  /** Convert between undistorted focal plane and ra/dec coordinates
   * 
   * This base class is used to convert between undistorted focal plane
   * coordinates (x/y) in millimeters and sky (ra/dec).  This 
   * class handles the case of framing cameras.
   * 
   * @ingroup Camera
   * 
   * @see Camera
   * 
   * @internal
   *  @history 2005-02-08 Jeff Anderson Original version
   *  @history 2008-07-14 Steven Lambright Added NaifStatus calls
   * 
   */
  class CameraSkyMap {
    public:
      CameraSkyMap(Camera *parent);
  
      //! Destructor
      virtual ~CameraSkyMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy, 
                                 const double uz);

      virtual bool SetSky(const double ra, const double dec);

      //! Return undistorted focal plane x
      inline double FocalPlaneX() const { return p_focalPlaneX; };

      //! Return undistorted focal plane y
      inline double FocalPlaneY() const { return p_focalPlaneY; };

    protected:
      Camera *p_camera;
      double p_focalPlaneX;             
      double p_focalPlaneY;
  };
};
#endif
