#ifndef NirCamera_h
#define NirCamera_h
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
   * This is the camera model for the Clementine Near Infrared Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Clementine
   *  
   * @see 
   *      http://astrogeology.usgs.gov/Projects/Clementine/nasaclem/sensors/nir/nir.html
   * @see 
   *      http://astrogeology.usgs.gov/Projects/Clementine/nasaclem/clemhome.html
   * @see http://pds-imaging.jpl.nasa.gov/portal/clementine_mission.html 
   * @see http://astrogeology.usgs.gov/Missions/Clementine
   *      
   * @author  2007-07-10 Steven Lambright
   *
   * @internal
   *   @history 2007-07-10 Steven Lambright - Original Version
   *   @history 2007-07-11 Steven Koechle - casted NaifIkCode to int before
   *                          istring to fix Linux 32bit build error
   *   @history 2008-02-06 Steven Koechle - Fixed Name keyword.
   *   @history 2008-02-20 Christopher Austin - BandBin Name to FilterName.
   *   @history 2008-08-08 Steven Lambright - Made the unit test work with a
   *                          Sensor change. Also, now using the new
   *                          LoadCache(...) method instead of CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                          inherit directly from Camera.  Camera is now pure
   *                          virtual, parent class is FramingCamera.
   *   @history 2010-09-16 Steven Lambright - Updated unitTest to not use a DEM.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                          pure virtual in Camera, implemented in mission
   *                          specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                          method. Updated unitTest to test for new methods.
   *                          Updated documentation. Replaced Clementine
   *                          namespace wrap with Isis namespace. Added Isis
   *                          Disclaimer to files. Added NAIF error check to
   *                          constructor.
   */
  class NirCamera : public FramingCamera {
    public:
      NirCamera(Pvl &lab);
      //! Destroys the NirCamera object
      ~NirCamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-40000); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /** 
       * SPK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }
  };
};
#endif
