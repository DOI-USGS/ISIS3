#ifndef RosettaVirtisCamera_h
#define RosettaVirtisCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "LineScanCamera.h"

#include <QString>

#include <tnt/tnt_array2d.h>

#include "VariableLineScanCameraDetectorMap.h"

namespace Isis {
  /**
   * @brief Camera model for both Rosetta VIRTIS-M instruments
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Rosetta
   *
   * @author 2017-08-23 Kris Becker
   *
   * @internal
   *   @history 2017-08-23 Kris Becker Original Version
   */
  class RosettaVirtisCamera : public LineScanCamera {
    public:
      typedef TNT::Array2D<SpiceDouble> SMatrix; //!<  2-D buffer

      RosettaVirtisCamera(Cube &cube);

      ~RosettaVirtisCamera();

      /** CK Frame ID - Instrument Code from spacit run on CK */
      virtual int CkFrameId() const;

      /** CK Reference ID - J2000 */
      virtual int CkReferenceId() const;

      /** SPK Reference ID - J2000 */
      virtual int SpkReferenceId() const;

    private:
      void readHouseKeeping(const QString &filename, double lineRate);
      void readSCET(const QString &filename);

      QString scrub(const QString &text) const;
      double exposureTime() const;
      double scanLineTime() const;
      int    pixelSumming() const;

      int    hkLineCount() const;
      double lineStartTime(const double midExpTime) const;
      double lineEndTime(const double midExpTime) const;

      double startTime() const;
      double endTime() const;

      Table getPointingTable(const QString &channelId,
                             const int zeroFrame);
      SMatrix getStateRotation(const QString &frame1,
                               const QString &frame2,
                               const double &et) const;

      bool hasArticulationKernel(Pvl &label) const;

      /**
       *
       * @author ????-??-?? Unknown
       * @internal
       *   @history ????-??-?? Unknown - Original version.
       */
      struct ScanMirrorInfo {
        int    m_lineNum;        //!< The line the info is for
        double m_scanLineEt;     //!< Center of line time in ET
        double m_mirrorCos;      //!< Raw mirror cosine value
        double m_mirrorSin;      //!< Raw mirror sine value
        double m_opticalAngle;   //!< Optical angle in degrees
        bool   m_isDarkCurrent;  //!< If the line is dark current data
      };


      bool   m_is1BCalibrated; //!< is determined by Archive/ProcessingLevelId
      char   m_slitMode;       //!< Slit mode of the instrument
      double m_exposureTime;   //!< Line exposure time
      int    m_summing;        //!< Summing/binnning mode
      double m_scanRate;       //!< Line scan rate

      std::vector<LineRateChange> m_lineRates;  //!< vector of timing info for each line
      std::vector<ScanMirrorInfo> m_mirrorData; //!< vector of mirror info for each line

  };
};

#endif
