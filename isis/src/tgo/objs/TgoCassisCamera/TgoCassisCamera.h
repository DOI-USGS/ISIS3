#ifndef TgoCassisCamera_h
#define TgoCassisCamera_h
/**
 * @file
 * $Revision: $
 * $Date: $
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
   * @brief TGO Cassis camera model
   *
   * This is the camera model for the Trace Gas Orbiter CaSSIS instrument. This
   * instrument is technically a pushframe instrument, but it is treated as a
   * framing instrument because the framelet size is 256 lines or more. This is
   * also a more flexible camera model since it will make controlling the
   * individual framelets alot easier.
   * 
   * The CaSSIS frame hierarchy is as follows:
   * <pre>
   *         J2000
   *           |
   *           | ck
   *           |
   *           V
   *     TGO_SPACECRAFT
   *           |
   *           | fixed
   *           |
   *           V
   *     TGO_CASSIS_CRU
   *           |
   *           | ck
   *           |
   *           V
   *     TGO_CASSIS_TEL
   *           |
   *           | fixed
   *           |
   *           V
   *     TGO_CASSIS_FSA
   * </pre>
   *  
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Tgo
   * @author 2017-01-26 Kris Becker
   *
   * @internal
   *   @history 2017-01-26 Kris Becker - Original implementation. Fixes #4593.
   *   @history 2017-02-06 Jesse Mapel & Kristin Berry - Updated ck frame and
   *                           documentation. Added a unitTest. References #4593.
   */
  class TgoCassisCamera : public FramingCamera {
    public:
      TgoCassisCamera(Cube &cube);
      ~TgoCassisCamera();

      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time, 
                                                             double exposureDuration);
      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkTargetId() const;
      virtual int SpkReferenceId() const;

  };
};
#endif
