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
   *  @history 2007-10-18 Debbie A. Cook Corrected coding error with TransS0 and TransL0 and
   *  @history 2008-06-18 Stuart Sides - Fixed doc error
   *  @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *           change. Also, now using the new LoadCache(...) method instead of
   *           CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *            inherit directly from Camera
   */
  class IdealCamera : public Isis::Camera {
    public:
      // Constructs a HiriseCamera object
      IdealCamera (Isis::Pvl &lab);

      // Destroys the HiriseCamera object
      ~IdealCamera ();

      /**
       * Returns the type of camera that was created.
       * 
       * @return CameraType
       */
      virtual CameraType GetCameraType() const {
        if(p_framing) return Framing;
        return LineScan;
      }

    private:
      bool p_framing; //!< true if framing camera
  };
};

#endif
