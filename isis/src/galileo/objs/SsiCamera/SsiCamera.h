#ifndef SsiCamera_h
#define SsiCamera_h
/**
 * @file 
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
   * This is the camera model for the Galileo Solid State Imaging Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Galileo
   *  
   * @see http://astrogeology.usgs.gov/Missions/Galileo
   * @see http://www2.jpl.nasa.gov/galileo/sepo
   * @see http://pds-imaging.jpl.nasa.gov/portal/galileo_mission.html 
   *
   *
   * @author  Jeff Anderson
   *
   * @internal
   *   @history 2007-10-25 Steven Koechle - Fixed so that it works in Isis3
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *                          method instead of CreateCache(...).
   *   @history 2009-05-04 Steven Koechle - Fixed to grab appropriate FocalLength
   *                          based on image time.
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                          inherit directly from Camera
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                          pure virtual in Camera, implemented in mission
   *                          specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                          method. Updated unitTest to test for new methods.
   *                          Updated documentation. Replaced Galileo namespace
   *                          wrap with Isis namespace. Added Isis Disclaimer to
   *                          files. Added NAIF error check to constructor.
   */
  class SsiCamera : public FramingCamera {
    public:
      SsiCamera(Pvl &lab);
      //! Destroys the SsiCamera object.
      ~SsiCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-77001); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (2); }

      /** 
       *  SPK Reference ID - J2000
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (21); }
  };
};
#endif
