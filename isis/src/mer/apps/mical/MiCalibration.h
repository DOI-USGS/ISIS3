#ifndef MiCalibration_h
#define MiCalibration_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Cube.h"
#include "Pvl.h"
#include "Statistics.h"

namespace Isis {
  namespace Mer {
    /**
     * @author ????-??-?? Unknown
     *
     * @internal
     * @history 2016-08-28 Kelvin Rodriguez - Removed usused private member variables to
     *                              eliminate unused member variable warnings in clang.
     *                              Part of porting to OS X 10.11.
     */
    class MiCalibration {
      public:
        MiCalibration(Cube &image, Pvl &kernel);
        ~MiCalibration() {};

        inline double ExposureDuration() const {
          return p_exposureDuration;
        };
        int InstrumentSerialNumber() const {
          return p_instrumentSerialNumber;
        };
        inline QString ShutterEffectCorrectionFlag()
        const {
          return p_shuttereffectcorrectionflag;
        };
        inline QString ReferencePixelImage() const {
          return p_ReferencePixelImage;
        };
        inline QString ZeroExposureImage() const {
          return p_ZeroExposureImage;
        };
        inline QString ActiveAreaImage() const {
          return p_ActiveAreaImage;
        };
        inline QString FlatImageOpen() const {
          return p_FlatImageOpen;
        };
        inline QString FlatImageClosed() const {
          return p_FlatImageClosed;
        };
        inline QString FilterName() const {
          return p_filterName;
        };
        inline QString StartTime() const {
          return p_startTime;
        };
        inline double TransferTime() const {
          return p_transfertime;
        };
        inline double OffsetModeID() const {
          return p_OffsetModeId;
        };
        inline double OmegaNaught() const {
          return p_OmegaNaught;
        };
        inline double CCDTemperatureCorrect() const {
          return p_CCDTemperatureCorrect;
        };
        inline double PCBTemperature() const {
          return p_PCBTemperature;
        };
        inline double ReferencePixelModel() const {
          return p_ReferencePixelModel;
        };
        inline double ZeroExposureValue() const {
          return p_ZeroExposureValue;
        };
        inline double ActiveAreaValue() const {
          return p_ActiveAreaValue;
        };

        void SetReferencePixelModel();
        void SetZeroExposureValue();
        void SetActiveAreaValue();
        void SetCCDTemperature(double temperature);
        void SetPCBTemperature(double temperature);
        void SetOmegaNaught();

      private:
        double p_exposureDuration; //expousr duration in mill sec.
        int p_instrumentSerialNumber;
        double p_CCDTemperature;
        double p_PCBTemperature;
        double p_CCDTemperatureCorrect;
        double p_OffsetModeId;
        QString p_shuttereffectcorrectionflag;
        QString p_filterName;
        QString p_startTime;

        double p_ReferencePixelModel;
        double p_ZeroExposureValue;
        double p_ActiveAreaValue;

        double p_DELCCDTa;
        double p_DELCCDTb;
        double p_RPVOFFa;
        double p_RPVOFFb;
        double p_RPPCBTa;
        double p_RPPCBTb;
        double p_RPPCBTc;
        double p_RPCCDTa;
        double p_RPCCDTb;
        double p_RPCCDTc;
        double p_ZEROEXPa;
        double p_ZEROEXPb;
        double p_ACTAREAa;
        double p_ACTAREAb;
        double p_temperatureOffset;
        double p_transfertime;
        QString p_ReferencePixelImage;
        QString p_ZeroExposureImage;
        QString p_ActiveAreaImage;
        QString p_FlatImageOpen;
        QString p_FlatImageClosed;
        double p_OmegaNaught;

        void ReadLabels(Cube &image);
        void ReadKernel(Pvl &kernel);


    };
  };
};
#endif
