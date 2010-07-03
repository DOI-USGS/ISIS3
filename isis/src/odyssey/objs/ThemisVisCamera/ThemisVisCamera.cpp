using namespace std;
#include <iomanip>
#include "ThemisVisCamera.h"
#include "PushFrameCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "ThemisVisDistortionMap.h"
#include "PushFrameCameraGroundMap.h"
#include "CameraSkyMap.h"

namespace Isis {
  namespace Odyssey {

   /**
    * Constructor for the Themis Vis Camera Model
    *
    * @param lab Pvl Label to create the camera model from
    *
    * @throws Isis::iException::User - The image does not appear to be a Themis
    *                                  VIS image
    */
    ThemisVisCamera::ThemisVisCamera (Isis::Pvl &lab) : Isis::PushFrameCamera(lab) {
      // Set up the camera characteristics
      // LoadFrameMounting("M01_SPACECRAFT","M01_THEMIS_VIS");
      SetFocalLength(203.9);
      SetPixelPitch(0.009);

      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      // make sure it is a themis vis image
      if (inst["InstrumentId"][0] != "THEMIS_VIS") {
        string msg = "The image does not appear to be a Themis Vis Image";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }

      // Get necessary variables
      p_exposureDur = inst["ExposureDuration"];
      p_interframeDelay = inst["InterframeDelay"];
      int sumMode = inst["SpatialSumming"];

      // Get the start and end time
      double et;
      string stime = inst["SpacecraftClockCount"];
      scs2e_c (NaifSclkCode(),stime.c_str(),&et);
      double offset = inst["SpacecraftClockOffset"];
      p_etStart = et + offset - ((p_exposureDur / 1000.0) / 2.0);
      p_nframes = inst["NumFramelets"];

      // Get the keywords from labels
      Isis::PvlGroup &bandBin = lab.FindGroup("BandBin",Isis::Pvl::Traverse);
      Isis::PvlKeyword &orgBand = bandBin["OriginalBand"];
      for (int i=0; i<orgBand.Size(); i++) {
        p_originalBand.push_back(orgBand[i]);
      }

      // Setup detector map
      double frameRate = p_interframeDelay;
      PushFrameCameraDetectorMap *dmap =
        new PushFrameCameraDetectorMap(this,p_etStart,frameRate,192);
      dmap->SetDetectorSampleSumming(sumMode);
      dmap->SetDetectorLineSumming(sumMode);

      // Setup focal plane map
      CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this,NaifIkCode());
      focalMap->SetDetectorOrigin(512.5,512.5);

      // Setup distortion map
      new ThemisVisDistortionMap(this);

      // Setup the ground and sky map
      bool evenFramelets = (inst["Framelets"][0] == "Even");
      new PushFrameCameraGroundMap(this, evenFramelets);
      new CameraSkyMap(this);

      LoadCache();
    }

   /**
    * Sets the band in the camera model
    *
    * @param vband The band number to set
    */
    void ThemisVisCamera::SetBand (const int vband) {
      Camera::SetBand(vband);

      // Set the et
      SetEphemerisTime(p_etStart + BandEphemerisTimeOffset(vband));
      PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *)this->DetectorMap();
      dmap->SetStartTime(p_etStart + BandEphemerisTimeOffset(vband));
    }

    double ThemisVisCamera::BandEphemerisTimeOffset(int vband) {
      int waveToTimeBand[] = {2,5,3,4,1};
      int visBandFirstRow[] = {4,203,404,612,814};

      // Lookup the original band from the band bin group.  Unless there is
      // a reference band which means the data has all been aligned in the
      // band dimension
      int band = p_originalBand[vband-1];
      if (HasReferenceBand()) {
        band = ReferenceBand();
      }

      // convert wavelength band the time band
      band = waveToTimeBand[band-1];

      // Compute the time offset for this detector line
      p_bandTimeOffset = ((band-1) * p_interframeDelay) -
        ((p_exposureDur / 1000.0) / 2.0);

      PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *)this->DetectorMap();
      dmap->SetBandFirstDetectorLine(visBandFirstRow[band-1]);

      return p_bandTimeOffset;
    }
  }
}

// Plugin
extern "C" Isis::Camera *ThemisVisCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Odyssey::ThemisVisCamera(lab);
}
