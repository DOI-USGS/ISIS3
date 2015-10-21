#ifndef VoyagerCamera_h
#define VoyagerCamera_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.0 $ 
 * $Date: 2009/05/27 12:08:01 $ 
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
   * @brief Voyager Camera Model                
   *                                        
   * This is the camera model for Voyager 1 and 2 wide and narrow 
   * angle cameras. 
   * 
   * @ingroup SpiceInstrumentsAndCameras                                   
   * @ingroup Voyager
   *  
   *  
   * @see 
   *      http://pds-imaging.jpl.nasa.gov/data/vg2-n-iss-2-edr-v1.0/vg_0009/document/volinfo.txt
   * @see http://voyager.jpl.nasa.gov 
   * @see http://pds-imaging.jpl.nasa.gov/portal/voyager_mission.html 
   * @see http://astrogeology.usgs.gov/Missions/Voyager 
   *                                                                         
   * @author 2010-07-19 Mackenzie Boyd
   * 
   * @internal 
   *   @history 2010-07-19 Mackenzie Boyd - Original Version
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Added Isis Disclaimer to
   *                           files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2015-08-14 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test for
   *                           name methods and added new data for Voyager1 WAC, Voyager2 NAC and
   *                           and WAC.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument 
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */                                                                       
  class VoyagerCamera : public FramingCamera {
    public:
      VoyagerCamera (Cube &cube);
      //! Destroys the VoyagerCamera object.
      ~VoyagerCamera () {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);
      /** 
       * CK frame ID -
       * Voyager 1 instrument code (VG1_SCAN_PLATFORM) = -31100
       * Voyager 2 instrument code (VG1_SCAN_PLATFORM) [sic] = -32100
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return p_ckFrameId; }

      /** 
       * CK Reference ID - B1950
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (2); }

      /** 
       * SPK Target Body ID -
       * VOYAGER 1 = -31
       * VOYAGER 2 = -32
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return p_spkTargetId; }

      /** 
       * SPK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

    private:
      int p_ckFrameId;       //!< "Camera-matrix" Kernel Frame ID
      int p_spkTargetId;     //!< Spacecraft Kernel Target ID
  };
};
#endif
