/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2009/04/06 15:23:27 $                                                                 
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

#ifndef VimsSkyMap_h
#define VimsSkyMap_h

#include "CameraSkyMap.h"


namespace Isis {    
  namespace Cassini {
    /** Convert between undistorted focal plane and ground coordinates
     * 
     * This base class is used to convert between undistorted focal plane
     * coordinates (x/y) in millimeters and ground coordinates lat/lon.  
     * This class handles the case of framing cameras.
     * 
     * @ingroup Camera
     * 
     * @see Camera
     * 
     * @internal
     * 
     * @history 2006-04-05 Tracie Sucharski
     * Original version
     * @history 2009-04-06 Steven Lambright Fixed problem that caused double 
     *          deletion of sky map / ground map.
     *
     */
    class VimsSkyMap : public CameraSkyMap {
    public:
      VimsSkyMap(Camera *parent, Pvl &lab);

      //! Destructor
      virtual ~VimsSkyMap() {};

      virtual bool SetFocalPlane(const double ux, const double uy, 
                                 const double uz);

      virtual bool SetSky(const double ra, const double dec);

      void Init(Pvl &lab);

    protected:

    private:
      SpiceDouble p_etStart;

      double p_exposureDuration;
      double p_interlineDelay;

      std::string p_channel;
      double p_visExp;
      double p_irExp;
      int    p_nsUv;
      int    p_nlUv;
      int    p_swathWidth;
      int    p_swathLength;
      int    p_camSampOffset;
      int    p_camLineOffset;

      double p_unitVector[192][192][3];

      double p_minRa;
      double p_maxRa;
      double p_minDec;
      double p_maxDec;
      double p_raMap[64][64];
      double p_decMap[64][64];

    };
  };
};
#endif
