#ifndef ClipperNacRollingShutterCamera_h
#define ClipperNacRollingShutterCamera_h
/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

 #include "RollingShutterCamera.h"

namespace Isis {
  class Cube;

  /**
   * @brief Clipper EIS Camera model
   *
   * This is the camera model for the Clipper EIS NAC Rolling Shutter Camera.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Clipper
   *
   * @author 2018-04-09 Makayla Shepherd and Ian Humphrey 
   *  
   *
   */
  class ClipperNacRollingShutterCamera : public RollingShutterCamera {
    public:
      ClipperNacRollingShutterCamera(Cube &cube);

      ~ClipperNacRollingShutterCamera();

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;
  };
};

#endif
