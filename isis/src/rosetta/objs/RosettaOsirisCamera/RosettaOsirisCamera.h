#ifndef RosettaOsirisCamera_h
#define RosettaOsirisCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "FramingCamera.h"

#include <QXmlStreamReader>

#include "LinearAlgebra.h"
#include "RosettaOsirisCameraDistortionMap.h"

namespace Isis {
  /**
   * This is the camera model for the Osiris NAC Framing Camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Rosetta
   *
   * @author 2015-05-21 Sasha Brownsberger
   *
   * @internal
   *   @history 2015-05-21 Sasha Brownsberger - Original Version.
   *   @history 2017-06-02 Jesse Mapel - Added a distortion map Fixes #4496.
   *   @history 2017-04-11 Jesse Mapel - Added subwindowing. Fixes #5394.
   *   @history 2018-09-25 Kaj Williams - Added binning. Due to the difficulty in obtaining test images which are binned, this is currently untested.
   */
  class RosettaOsirisCamera : public FramingCamera {
    public:
      //! Create a OsirisNacCamera object
      RosettaOsirisCamera(Cube &cube);

      //! Destroys the NewHorizonsLorriCamera object
      ~RosettaOsirisCamera() {};

    /**
     * Reimplemented from FrameCamera
     *
     * @author Stuart Sides
     *
     * @internal
     * @history modified Sasha Brownsberger (2015/05/21)
     *
     * @param time Start time of the observation
     * @param exposureDuration The exposure duration of the observation
     *
     * @return std::pair<iTime,iTime> The start and end times of the observation
     */
      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-226000); } //Code for Rosetta orbitter; no specific code for Osiris in ck files.

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

      void initDistortion(QString ikCode, RosettaOsirisCameraDistortionMap *distortionMap);
  };
};
#endif
