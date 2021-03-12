#ifndef CrismCamera_h
#define CrismCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

#include <QString>

#include "VariableLineScanCameraDetectorMap.h"

namespace Isis {
  /**
   * @brief MRO CRISM camera model
   *
   *   This is the camera model for the MRO CRISM instrument.  It provides
   *   support for band independent geometry for the instrument.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @see Camera
   *
   * @author 2013-02-21 Kris Becker
   *
   * @internal
   *
   *   @history 2013-02-21 Kris Becker Original version
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-09-14 Kelvin Rodriguez - Enforced the order in which BORESIGHT_LINE and
   *                           BORESIGHT_SAMPLE are added to the PVL. Part of porting to
   *                           OSX 10.11
   */
  class CrismCamera : public LineScanCamera {
    public:
      // constructors
      CrismCamera(Cube &cube);

      //! Destroys the CrismCamera object.
      virtual ~CrismCamera() {  }

      void SetBand (const int physicalBand);

      /**
       * @brief This is a band-dependant instrument
       *
       * @author kbecker (4/12/2012)
       *
       * @return bool
       */
      bool IsBandIndependent ();

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-74000); }

      /**
       * CK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (-74900); }

      /**
       *  SPK Reference ID - J2000
       *
       * @return @b int The appropriate instrument code for the Spacecraft
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

  private:
    std::vector<LineRateChange> m_lineRates;
    bool m_isBandDependent;

    double getEtTime(const QString &sclk);
  };
};
#endif
