#ifndef UvvisCamera_h
#define UvvisCamera_h
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

#include <QString>

namespace Isis {
  /**
   * This is the camera model for the Clementine Ultraviolet/Visible Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Clementine
   *  
   * @see 
   *      http://astrogeology.usgs.gov/Projects/Clementine/nasaclem/sensors/uvvis/uvvis.html
   * @see 
   *      http://astrogeology.usgs.gov/Projects/Clementine/nasaclem/clemhome.html
   * @see http://pds-imaging.jpl.nasa.gov/portal/clementine_mission.html 
   * @see http://astrogeology.usgs.gov/Missions/Clementine
   *  
   * @author  2007-07-10 Tracie Sucharski
   *
   * @internal
   *   @history 2007-01-11 Tracie Sucharski - Original version.
   *   @history 2007-01-30 Elizabeth Ribelin - Renamed String to QString and
   *                           Time to iTime.
   *   @history 2007-07-10 Steven Lambright - Added unitTest.  Read boresight
   *                           from addendum.  Added time to 1/2 exposure
   *                           duration to get center time of image.
   *   @history 2007-07-11 Steven Koechle - Casted NaifIkCode to int before
   *                           istring to fix Linux 32bit build error
   *   @history 2008-08-08 Steven Lambright - Made the unit test work with a
   *                           Sensor change. Also, now using the new
   *                           LoadCache(...) method instead of CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera.  Camera is now pure
   *                           virtual, parent class is FramingCamera.
   *   @history 2010-09-16 Steven Lambright - Updated unitTest to not use a DEM.
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                           pure virtual in Camera, implemented in mission
   *                           specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                           method. Updated unitTest to test for new methods.
   *                           Updated documentation. Replaced Clementine
   *                           namespace wrap with Isis namespace. Added Isis
   *                           Disclaimer to files. Added NAIF error check to
   *                           constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   */
  class UvvisCamera : public FramingCamera {
    public:
      UvvisCamera(Cube &cube);
      //! Destroys the UvvisCamera object.
      ~UvvisCamera() {};
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
      
      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;
      
    private:
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};
#endif
