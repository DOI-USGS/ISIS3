#ifndef TMCCamera_h
#define TMCCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

namespace Isis {
  /**
   * @brief Chandrayaan2 TMC (Terrain Mapping Camera) Camera Model
   *
   * This is the camera model for the Chandrayaan2 Terrain Mapping Camera
   */
  class Chandrayaan2TmcCamera : public LineScanCamera {
    public:
      Chandrayaan2TmcCamera(Cube &cube);

      //! Destroys the CTXCamera object.
      ~Chandrayaan2TmcCamera() {};

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-152001); }

      /**
       *  CK Reference ID - MRO_MME_OF_DATE
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
