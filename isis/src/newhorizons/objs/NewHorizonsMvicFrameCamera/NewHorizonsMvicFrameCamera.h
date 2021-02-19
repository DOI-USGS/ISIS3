#ifndef NewHorizonsMvicFrameCamera_h
#define NewHorizonsMvicFrameCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FramingCamera.h"

#include <QString>

namespace Isis {
  /**
   * This is the camera model for the New Horizons MVIC Frame mode Camera
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup New Horizons
   *
   * @author 2014-03-31 Tracie Sucharski
   *
   * @internal
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           added methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class NewHorizonsMvicFrameCamera : public FramingCamera {
    public:
      //! Create a NewHorizonsMvicFrameCamera object
      NewHorizonsMvicFrameCamera(Cube &cube);


      //! Destroys the NewHorizonsMvicFrameCamera object
      ~NewHorizonsMvicFrameCamera() {};


      // Sets the band to the band number given
      void SetBand(const int vband);


//      /**
//       * The camera model is band dependent, so this method returns false
//       *
//       * @return bool False
//       */
////    bool IsBandIndependent() {
////      return false;
////    };


      virtual std::pair <iTime, iTime> ShutterOpenCloseTimes(double time,
                                                             double exposureDuration);


      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-98000); }


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

    private:
      QList<int> m_originalBand;
      QList<QString> m_utcTime;
      double m_etStart;
      double m_exposure;
  };
};
#endif
