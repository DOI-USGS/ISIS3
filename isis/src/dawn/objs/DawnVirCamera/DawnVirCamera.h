#ifndef DawnVirCamera_h
#define DawnVirCamera_h

#include "LineScanCamera.h"
#include "VariableLineScanCameraDetectorMap.h"

#include "tnt/tnt_array2d.h"

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
     * @internal
     *
     *   @history 2011-03-10 Kris Becker Original Version
     */
    class DawnVirCamera : public LineScanCamera {
      public:
        DawnVirCamera(Isis::Pvl &lab);

        ~DawnVirCamera() {};

        /** CK Frame ID - Instrument Code from spacit run on CK */
        virtual int CkFrameId() const;

        /** CK Reference ID - J2000 */
        virtual int CkReferenceId() const;

        /** SPK Reference ID - J2000 */
        virtual int SpkReferenceId() const;

      private:
        typedef TNT::Array2D<SpiceDouble> SMatrix;       //!<  2-D buffer

        struct ScanMirrorInfo {
          int    m_lineNum;
          double m_scanLineEt;     // Center of line time in ET
          double m_mirrorCos;      // Raw mirror cosine value
          double m_mirrorSin;      // Raw mirror sine value
          double m_opticalAngle;   // Optical angle in degrees
          bool   m_isDarkCurrent;
        };

        bool   m_is1BCalibrated; ///!< is determined by Archive/ProcessingLevelId
        char   m_slitMode;       ///!< Slit mode of the instrument
        double m_lineRate;       ///!< Scan line rate
        int    m_summing;        ///!< Summing/binnning mode
        std::vector<LineRateChange> m_lineRates;
        std::vector<ScanMirrorInfo> m_mirrorData;
        int    m_nDarkCurrent;   ///!< Number of dark current lines in table

        void readHouseKeeping(const std::string &filename, double lineRate);
        std::string scrub(const std::string &text) const;
        double exposureTime() const;
        int    pixelSumming() const;
        double startTime() const;
        double endTime() const;
        int    hkLineCount() const;

        Table getPointingTable(const std::string &channelId, 
                               const int zeroFrame);
        SMatrix getStateRotation(const std::string &frame1, 
                                 const std::string &frame2, 
                                 const double &et) const;

        bool hasArticulationKernel(Pvl &label) const;
    };
};

#endif
