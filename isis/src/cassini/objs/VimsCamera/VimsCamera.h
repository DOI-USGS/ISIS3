#ifndef VimsCamera_h
#define VimsCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

#include <QList>
#include <QPointF>

namespace Isis {
  class Pvl;
  /**
   * @brief Cassini Vims camera model
   *
   *   This is the camera model for the Cassini Vims instrument
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Cassini-Huygens
   *
   * @see Camera
   *
   * @author 2006-03-16 Tracie Sucharski
   *
   * @internal
   *
   *   @history 2006-03-16 Tracie Sucharski Original version
   *   @history 2009-04-06 Steven Lambright Fixed problem that caused double
   *                          deletion of sky map / ground map.
   *   @history 2009-08-03 Debbie A. Cook - Added new tolerance argument to
   *                          CreateCache call to be compatible with update to
   *                          Spice class
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                          inherit directly from Camera
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                          pure virtual in Camera, implemented in mission
   *                          specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2010-08-05 Jeannie Walldren - Updated documentation. Removed
   *                          Cassini namespace wrap inside Isis namespace wrap.
   *                          Added NAIF error check to constructor.
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                          methods. Updated documentation. Removed Cassini
   *                          namespace wrap inside Isis namespace wrap. Added
   *                          Isis Disclaimer to files. Added NAIF error check
   *                          to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *                          coding standards. References #972.
   *   @history 2012-03-04 Tracie Sucharski - Added new method, PixelIfovOffsets, which will return
   *                           the ifov offsets,in x and y, from the center of the pixel in mm.  The
   *                           pixel ifov size for vims is dependent on the sampling mode.
   *   @history 2013-09-10 Tracie Sucharski, The Vims 15 Mhz clock actually oscillates closer to
   *                           14.7456 Mhz, so the IR exposure duration, interline delay and
   *                           interframe delay in the VIMS header need to be scaled by 1.01725.
   *                           This change will affect where VIMS pixels map in projected space.
   *                           Fixes #1759.
   *   @history 2013-12-04 Tracie Sucharski - Fixed bug when checking for unsupported Nyquist
   *                           cubes.  The SamplingMode keyword is actually "UNDER" not "NYQUIST".
   *                           Print appropriate error indicating that these cubes are not supported.
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2018-03-14 Adam Goins - Changed cache calculations with LoadCache() call.
   *                           This fixes an error where VimsCamera caused spiceinit to
   *                           fail when TargetName == SKY. Fixes #5353.
   *   @history 2022-07-14 Amy Stamile - Removed SpkCenterId function due to spkwriter writing
   *                           positions of Cassini relative to Titan (NAIF ID 606) but labeling
   *                           it in the kernel as the position relative to the Saturn Barycenter
   *                           (NAIF ID 6) Reference #4942.
   */
  class VimsCamera : public Camera {
    public:
      // constructors
      VimsCamera(Cube &cube);

      //! Destroys the VimsCamera object.
      ~VimsCamera() {};

      /**
       * The Vims camera is the only point camera we have.
       *
       * @return CameraType Camera::Point
       */
      virtual CameraType GetCameraType() const {
        return Point;
      }

//       void SetBand (const int physicalBand);
//       bool IsBandIndependent () { return false; };

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
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

      virtual QList<QPointF> PixelIfovOffsets();

    private:
      double m_pixelPitchX;
      double m_pixelPitchY;
  };
};
#endif
