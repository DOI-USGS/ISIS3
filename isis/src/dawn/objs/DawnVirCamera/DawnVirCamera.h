#ifndef DawnVirCamera_h
#define DawnVirCamera_h

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
   * @brief Camera model for both Danw VIR VIS and IR instruments
   *
   * This class provides the camera model for the Dawn VIR VIS and
   * IR instrumetns.  These instruments are on the Dawn spacecraft
   * which will orbit the astroids Vesta (2011) and Ceres (2013).
   *
   * The ISIS cubes must contain a table called VIRHouseKeeping
   * that contains critical information.  Stored here is a row for
   * each line in the cube which contains the time (scan lines are
   * not strictly contiguous), electrical scan angles and shutter
   * state (closed == dark current).  The VIR instrument team will
   * provide a dynamic articulation kernel that has the physical
   * scan angle of the mirror but the contents of the tabel can be
   * used to compute it should it not exist (determined by the
   * file name pattern of the CK kernels).
   *
   * Without the articulation kernel, this camera model will
   * create a CK SpiceRotation table from the contents of the
   * houskeeping table.  This table is create only when spiceinit
   * is run for the first time on the image.
   *
   * Note that it works for calibrated (1B) and uncalibrated (1A).
   * One major issue is the dark current is typically collected at
   * the start and end of an observation.  The dark current
   * appears to always slew to a specific position, crossing the
   * observation scans.  This is the apparent cause of loss of
   * mapping lat/lons to line/samp.  To fix this, a Cubic spline
   * is fit to all scan angles and  all closed shutter scan line
   * mirror angles are replaced by the (typcially extrapolated)
   * values of the spline.
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Dawn
   *
   * @author 2011-03-10 Kris Becker
   *
   * @internal
   *   @history 2011-03-10 Kris Becker Original Version
   *   @history 2011-08-23 Kris Becker - Correct length of scan
   *            line to be the EXTERNAL_REPETITION_TIME of the
   *            FRAME_PARAMETER[2] value.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
   *            coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @histroy 2016-10-27 Jesse Mapel - Modified to store exposure time instead of line rate in
   *                           line rate table.  References #4476
   *   @history 2016-10-27 Jeannie Backer - Added documentation and adjusted formatting to match
   *                            coding standards. References #4476.
   */
  class DawnVirCamera : public LineScanCamera {
    public:
      typedef TNT::Array2D<SpiceDouble> SMatrix; //!<  2-D buffer

      DawnVirCamera(Cube &cube);

      ~DawnVirCamera();

      /** CK Frame ID - Instrument Code from spacit run on CK */
      virtual int CkFrameId() const;

      /** CK Reference ID - J2000 */
      virtual int CkReferenceId() const;

      /** SPK Reference ID - J2000 */
      virtual int SpkReferenceId() const;

    private:
      void readHouseKeeping(const QString &filename, double lineRate);
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
