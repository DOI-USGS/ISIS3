#ifndef DawnVirCamera_h
#define DawnVirCamera_h

#include "LineScanCamera.h"
#include "VariableLineScanCameraDetectorMap.h"

#include "tnt/tnt_array2d.h"

namespace Isis {
    /** 
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
