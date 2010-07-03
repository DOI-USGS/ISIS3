#ifndef Mariner10Camera_h
#define Mariner10Camera_h
/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/06/29 18:16:39 $
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
   * @brief Mariner10 Camera Model
   *
   * This is the camera model for both mariner10, both cameras A (wide angle)
   * and B (narrow angle).
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Mariner10
   *
   * @author 2009-04-13 Tracie Sucharski
   *
   * @internal
   * @history 2009-12-18 Tracie Sucharski - Changed inheritance from Camera to
   *                        FramingCamera.
   * @history 2010-02-22 Tracie Sucharski - Preface naif includes with /naif.
   * @history 2010-03-04 Tracie Sucharski - Removed couts.
   * @history 2010-03-04 Tracie Sucharski - Added throw when creation of
   *                        ReseauDistortion fails.
   */
  class Mariner10Camera : public Isis::FramingCamera {
    public:
      Mariner10Camera(Isis::Pvl &lab);

      //! Destroys the Mariner10Camera Object
      ~Mariner10Camera() {};
  };
};
#endif

