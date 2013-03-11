#ifndef IdealCamera_h
#define IdealCamera_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2009/08/31 15:12:28 $
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

#include "Camera.h"

namespace Isis {
  /**
   * @brief Ideal Camera Model
   *
   * This class is the implementation of a generic camera model
   * with no optical distortion.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author  2006-10-17 Jeff Anderson and Debbie Cook
   *
   * @internal
   *   @history 2007-10-18 Debbie A. Cook  -Corrected coding error with TransS0
   *                           and TransL0 and added ldir and sdir.
   *   @history 2008-06-18 Stuart Sides - Fixed doc error
   *   @history 2008-08-08 Steven Lambright - Made the unit test work with a
   *                           Sensor change. Also, now using the new
   *                           LoadCache(...) method instead of CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2010-09-16 Jeannie Walldren - Modified test cube to run properly
   *                           with ShapeModel changes to Sensor class and
   *                           updated unitTest known lat/lon values.
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Updated documentation. Added Isis
   *                           Disclaimer to files. Added NAIF error check to
   *                           constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2012-09-10 Steven Lambright - Added support for reading ideal information from
   *                           the NaifKeywords object (via readValue()). This camera now
   *                           works in 3 ways: detached spice (ideal keywords, e.g. TransX0, must
   *                           be in the instrument group), legacy attached spice (ideal keywords
   *                           are in the instrument group, but spice is attached so kernels will
   *                           not be furnished), and attached spice (ideal keywords are already in
   *                           the NaifKeywords object). The first method (detached spice) works by
   *                           pushing the ideal keywords into the naif kernel pool. The second
   *                           method (legacy attached spice) works by pushing the keywords on the
   *                           fly into the NaifKeywords object (kernels won't need to be
   *                           furnished). The third method works automatically. References #1094.
   *  
   */
  class IdealCamera : public Camera {
    public:
      // Constructs a HiriseCamera object
      IdealCamera(Cube &cube);

      // Destroys the HiriseCamera object
      ~IdealCamera();

      /**
       * Returns the type of camera that was created.
       *
       * @return CameraType
       */
      virtual CameraType GetCameraType() const {
        if(p_framing) return Framing;
        return LineScan;
      }

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkTargetId() const;
      virtual int SpkCenterId() const;
      virtual int SpkReferenceId() const;

    private:
      bool p_framing; //!< true if framing camera
  };
};

#endif
