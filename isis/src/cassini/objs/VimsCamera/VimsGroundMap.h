/**                                                                       
 * @file                                                                  
 * $Revision: 1.6 $                                                             
 * $Date: 2009/08/07 22:08:33 $                                                                 
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

#ifndef VimsGroundMap_h
#define VimsGroundMap_h

#include "CameraGroundMap.h"


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
     * @history 2006-03-16 Tracie Sucharski Original version 
     * @history 2008-02-05 Tracie Sucharski, Replaced unitVector files with 
     *                          Rick McCloskey's code to calculate look direction. 
     * @history 2008-06-18 Steven Lambright Fixed documentation 
     * @history 2009-04-06 Steven Lambright Fixed problem that caused double 
     *          deletion of sky map / ground map.
     * @history 2009-08-06 Tracie Sucharski, Bug in unit vector change made 
     *                         on 2008-02-05, had the incorrect boresight for
     *                         VIS Hires.
     *
     */
    class VimsGroundMap : public CameraGroundMap {
    public:
      VimsGroundMap(Camera *parent, Pvl &lab);

      //! Destructor
      virtual ~VimsGroundMap() { };

      virtual bool SetFocalPlane(const double ux, const double uy, 
                                 const double uz);

      virtual bool SetGround(const double lat, const double lon);

      void Init(Pvl &lab);

    protected:

    private:
      void LookDirection (double v[3]);

      SpiceDouble p_etStart;

      double p_exposureDuration;
      double p_interlineDelay;

      double p_ux;
      double p_uy;
      double p_uz;

      double p_xPixSize;
      double p_yPixSize;
      double p_xBore;
      double p_yBore;

      std::string p_channel;
      double p_visExp;
      double p_irExp;
      int    p_nsUv;
      int    p_nlUv;
      int    p_swathWidth;
      int    p_swathLength;
      int    p_camSampOffset;
      int    p_camLineOffset;

      double p_minLat;
      double p_maxLat;
      double p_minLon;
      double p_maxLon;
      double p_latMap[64][64];
      double p_lonMap[64][64];

    };
  };
};
#endif
