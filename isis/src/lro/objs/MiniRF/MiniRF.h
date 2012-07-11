#ifndef MiniRF_h
#define MiniRF_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/03/27 06:46:58 $
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
#include "RadarCamera.h"

namespace Isis {
  /**
   * @brief LRO Mini-RF SAR and Chandrayaan 1 Mini-RF SAR
   *
   * This is the camera model for both LRO Mini-RF SAR and
   * Chandrayaan 1 Mini-RF SAR radar systems.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @author 2008-07-01 Jeff Anderson
   *
   * @internal
   *   @history 2009-07-01 Janet Barrett - Updated with code to handle
   *            weighting of slant range and Doppler shift; fixed so that
   *            the frequency and wavelength of the instrument are made
   *            available to Radar classes.
   *   @history 2009-07-31 Debbie A. Cook and Jeannie Walldren - Added
   *            new tolerance argument to LoadCache call to be compatible
   *            with update to Spice class
   *   @history 2009-08-05 Debbie A. Cook - corrected altitude in tolerance
   *            calculation
   *   @history 2009-10-16 Debbie A. Cook - fixed the rotation of the velocity vector
   *   @history 2009-11-03 Debbie A. Cook - added RadarGroundMap method calls to set radar parameters
   *   @history 2010-03-19 Debbie A. Cook - removed unit change of rangeResolution so range_sigma works
   *            on meters instead of km
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *            methods. Updated documentation. Removed Lro namespace wrap
   *            inside Isis namespace wrap. Added Isis Disclaimer to files.
   *            Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   */
  class MiniRF : public RadarCamera {
    public:
      MiniRF(Pvl &lab);

      //! Destroys the MiniRF object
      ~MiniRF() {};

      virtual int CkFrameId() const;

      virtual int CkReferenceId() const;

      /** 
       *  SPK Target Body ID - Lunar Reconnaissance Orbiter spacecraft
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Target ID
       */
      virtual int SpkTargetId() const { return (-85); }

      /** 
       *  SPK Reference ID - J2000
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
