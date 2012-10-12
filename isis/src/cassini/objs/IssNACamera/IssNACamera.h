// $Id: IssNACamera.h,v 1.7 2009/08/31 15:12:29 slambright Exp $
#ifndef IssNACamera_h
#define IssNACamera_h
/**
 * @file
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
   * @brief Cassini ISS Narrow Angle Camera Model
   *
   *     * This is the camera model for the Cassini Imaging Science Subsystem Narrow 
   * Angle Camera 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Cassini-Huygens
   *  
   * @see 
   *      http://saturn.jpl.nasa.gov/spacecraft/cassiniorbiterinstruments/instrumentscassiniiss
   * @see http://pds-imaging.jpl.nasa.gov/portal/cassini_mission.html
   * @see http://astrogeology.usgs.gov/Missions/Cassini
   *  
   * @author  2007-07-10 Steven Koechle
   *
   * @internal
   *   @history 2007-07-10 Steven Koechle - Original Version
   *   @history 2007-07-10 Steven Koechle - Removed hardcoding of NAIF
   *                          Instrument number
   *   @history 2007-07-11 Steven Koechle - casted NaifIkCode to int before
   *                          IString to fix problem on Linux 32bit
   *   @history 2008-08-08 Steven Lambright Now using the new LoadCache(...)
   *                          method instead of CreateCache(...).
   *   @history 2009-01-22 Kris Becker Added new frame rotation to the CK frame
   *                          hierarchy to convert to detector coordinates.
   *                          This is essentially a 180 degree rotation.  The
   *                          frame definition is actually contained in the IAK.
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                          inherit directly from Camera
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                          pure virtual in Camera, implemented in mission
   *                          specific cameras
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *                          documentation. 
   *   @history 2011-05-03 Jeannie Walldren - Added ShutterOpenCloseTimes()
   *                          method. Updated unitTest to test for new methods.
   *                          Updated documentation. Replaced Cassini namespace
   *                          wrap with Isis namespace. Added Isis Disclaimer
   *                          to files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   */
  class IssNACamera : public FramingCamera {
    public:
      IssNACamera(Pvl &lab);
      //! Destroys the IssNACamera object.
      ~IssNACamera() {};
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-82000); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /** 
       *  SPK Center ID - 6 (Saturn)
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Center ID
       */
      virtual int SpkCenterId() const { return 6; }

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
