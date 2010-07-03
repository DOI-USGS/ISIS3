#ifndef MiCalibration_h
#define MiCalibration_h

#include "Cube.h"
#include "Pvl.h"
#include "Statistics.h"

namespace Isis{
  namespace Mer{
    class MiCalibration {
    public:
      MiCalibration(Cube &image, Pvl &kernel);
      ~MiCalibration(){};

      inline double ExposureDuration() const {return p_exposureDuration;};
      int InstrumentSerialNumber() const {return p_instrumentSerialNumber;};
      inline std::string ShutterEffectCorrectionFlag() 
             const {return p_shuttereffectcorrectionflag;};
      inline std::string ReferencePixelImage() const {return p_ReferencePixelImage;};
      inline std::string ZeroExposureImage() const {return p_ZeroExposureImage;};
      inline std::string ActiveAreaImage() const {return p_ActiveAreaImage;};
      inline std::string FlatImageOpen() const {return p_FlatImageOpen;};
      inline std::string FlatImageClosed() const {return p_FlatImageClosed;};
      inline std::string FilterName() const {return p_filterName;};
      inline std::string StartTime() const {return p_startTime;};
      inline double TransferTime() const {return p_transfertime;};
      inline double OffsetModeID() const {return p_OffsetModeId;};
      inline double OmegaNaught() const {return p_OmegaNaught;};
      inline double CCDTemperatureCorrect() const {return p_CCDTemperatureCorrect;};
      inline double PCBTemperature() const {return p_PCBTemperature;};
      inline double ReferencePixelModel() const {return p_ReferencePixelModel;};
      inline double ZeroExposureValue() const {return p_ZeroExposureValue;};
      inline double ActiveAreaValue() const {return p_ActiveAreaValue;};

      void SetReferencePixelModel();
      void SetZeroExposureValue();
      void SetActiveAreaValue();
      void SetCCDTemperature (double temperature);
      void SetPCBTemperature (double temperature);
      void SetOmegaNaught ();

    private:
      double p_exposureDuration; //expousr duration in mill sec.
      int p_instrumentSerialNumber;
      double p_CCDTemperature;
      double p_PCBTemperature;
      double p_CCDTemperatureCorrect;
      double p_OffsetModeId;
      std::string p_shuttereffectcorrectionflag;
      std::string p_filterName;
      std::string p_startTime;

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
      std::string p_ReferencePixelImage;
      std::string p_ZeroExposureImage;
      std::string p_ActiveAreaImage;
      std::string p_FlatImageOpen;
      std::string p_FlatImageClosed;
      double p_RPswitch;
      double p_ZEswitch;
      double p_AAswitch;
      double p_OmegaNaught;

      double p_calPixel;
      double p_DesmearPixel;
      double p_outputPixel;

      void ReadLabels (Cube &image);
      void ReadKernel (Pvl &kernel);

 
    };
  };
};
#endif

