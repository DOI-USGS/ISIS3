#include <iomanip>

#include "MarciCamera.h"
#include "PushFrameCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "PushFrameCameraGroundMap.h"
#include "CameraSkyMap.h"
#include "MarciDistortionMap.h"

using namespace std;

namespace Isis {
  namespace Mro {

   /**
    * Constructor for the Marci Camera Model
    *
    * @param lab Pvl Label to create the camera model from
    *
    * @throws Isis::iException::User - The image does not appear to be a Marci 
    *             image
    */
    MarciCamera::MarciCamera (Isis::Pvl &lab) : Isis::PushFrameCamera(lab) {
      Isis::PvlGroup &inst = lab.FindGroup ("Instrument",Isis::Pvl::Traverse);
      // make sure it is a marci image
      if (inst["InstrumentId"][0] != "Marci") {
        string msg = "The image does not appear to be a Marci Image";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }

      // Set up the camera characteristics
      SetFocalLength();

      string pixelPitchKey = "INS" + iString((int)NaifIkCode()) + "_PIXEL_SIZE";
      SetPixelPitch(GetDouble(pixelPitchKey));

      // Get necessary variables
      p_exposureDur = inst["ExposureDuration"];
      p_interframeDelay = inst["InterframeDelay"];
      int sumMode = inst["SummingMode"];

      // Get the start and end time
      double et;
      string stime = inst["SpacecraftClockCount"];
      scs2e_c (NaifSclkCode(),stime.c_str(),&et);
      p_etStart = et - ((p_exposureDur / 1000.0) / 2.0);
      p_nframelets = ParentLines() / sumMode;

      // These numbers came from "MARCI_CTX_Cal_Report_v1.5.pdf" page 7 (Bandpasses & downlinked detector rows)
      std::map<std::string, int> filterToDetectorOffset;
      filterToDetectorOffset.insert( std::pair<std::string, int>("BLUE",     709) );
      filterToDetectorOffset.insert( std::pair<std::string, int>("GREEN",    734) );
      filterToDetectorOffset.insert( std::pair<std::string, int>("ORANGE",   760) );
      filterToDetectorOffset.insert( std::pair<std::string, int>("RED",      786) );
      filterToDetectorOffset.insert( std::pair<std::string, int>("NIR",      811) );
      filterToDetectorOffset.insert( std::pair<std::string, int>("LONG_UV",  266) );
      filterToDetectorOffset.insert( std::pair<std::string, int>("SHORT_UV", 293) );

      std::map<std::string, int> filterToFilterNumbers;
      filterToFilterNumbers.insert( std::pair<std::string, int>("BLUE",     0) );
      filterToFilterNumbers.insert( std::pair<std::string, int>("GREEN",    1) );
      filterToFilterNumbers.insert( std::pair<std::string, int>("ORANGE",   2) );
      filterToFilterNumbers.insert( std::pair<std::string, int>("RED",      3) );
      filterToFilterNumbers.insert( std::pair<std::string, int>("NIR",      4) );
      filterToFilterNumbers.insert( std::pair<std::string, int>("LONG_UV",  5) );
      filterToFilterNumbers.insert( std::pair<std::string, int>("SHORT_UV", 6) );

      int frameletOffsetFactor = inst["ColorOffset"];

      if((int)inst["DataFlipped"] != 0) frameletOffsetFactor *= -1;
      std::map<std::string, int> filterToFrameletOffset;
      filterToFrameletOffset.insert( std::pair<std::string, int>("NIR",      0*frameletOffsetFactor) );
      filterToFrameletOffset.insert( std::pair<std::string, int>("RED",      1*frameletOffsetFactor) );
      filterToFrameletOffset.insert( std::pair<std::string, int>("ORANGE",   2*frameletOffsetFactor) );
      filterToFrameletOffset.insert( std::pair<std::string, int>("GREEN",    3*frameletOffsetFactor) );
      filterToFrameletOffset.insert( std::pair<std::string, int>("BLUE",     4*frameletOffsetFactor) );
      filterToFrameletOffset.insert( std::pair<std::string, int>("LONG_UV",  5*frameletOffsetFactor) );
      filterToFrameletOffset.insert( std::pair<std::string, int>("SHORT_UV", 6*frameletOffsetFactor) );

      // Get the keywords from labels
      const PvlGroup &bandBin = lab.FindGroup("BandBin",Isis::Pvl::Traverse);
      const PvlKeyword &filtNames = bandBin["FilterName"];

      for (int i=0; i<filtNames.Size(); i++) {
        if(filterToDetectorOffset.find(filtNames[i]) == filterToDetectorOffset.end()) {
          string msg = "Unrecognized filter name [" + filtNames[i] + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        p_detectorStartLines.push_back(filterToDetectorOffset.find(filtNames[i])->second);
        p_filterNumbers.push_back(filterToFilterNumbers.find(filtNames[i])->second);
        p_frameletOffsets.push_back(filterToFrameletOffset.find(filtNames[i])->second);
      }

      // Setup detector map
      double frameletRate = p_interframeDelay;
      PushFrameCameraDetectorMap *dmap =
        new PushFrameCameraDetectorMap(this,p_etStart,frameletRate,16);
      dmap->SetDetectorSampleSumming(sumMode);
      dmap->SetDetectorLineSumming(sumMode);
      dmap->SetGeometricallyFlippedFramelets(false);

      int numFramelets = ParentLines() / (16 / sumMode);
      bool flippedFramelets = (int)inst["DataFlipped"] != 0;
      dmap->SetFlippedFramelets(flippedFramelets, numFramelets);

      // Setup focal plane map
      new CameraFocalPlaneMap(this, -74400);

      if((int)NaifIkCode() == -74410) {
        // The line detector origin is in the middle of the orange framelet
        FocalPlaneMap()->SetDetectorOrigin(512.5, 760.0 + 8.5);
      }
      else if((int)NaifIkCode() == -74420) {
        FocalPlaneMap()->SetDetectorOrigin(512.5, 288.5);
      }
      else {
        string msg = "Unrecognized NaifIkCode [" + iString((int)NaifIkCode()) + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    
      // Setup distortion map
      new MarciDistortionMap(this, NaifIkCode());

      // Setup the ground and sky map
      bool evenFramelets = (inst["Framelets"][0] == "Even");
      new PushFrameCameraGroundMap(this, evenFramelets);
      new CameraSkyMap(this);
      LoadCache();

      if(sumMode == 1) {
        SetGeometricTilingHint(16, 4);
      }
      else if(sumMode == 2) {
        SetGeometricTilingHint(8, 4);
      }
      else if(sumMode == 4) {
        SetGeometricTilingHint(4, 4);
      }
      else {
        SetGeometricTilingHint(2, 2);
      }
    }

   /**
    * Sets the band in the camera model
    *
    * @param vband The band number to set
    */
    void MarciCamera::SetBand (const int vband) {
      Camera::SetBand(vband);

      PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *)DetectorMap();
      dmap->SetBandFirstDetectorLine(p_detectorStartLines[vband-1]);
      dmap->SetFrameletOffset(p_frameletOffsets[vband-1]);

      MarciDistortionMap *distmap = (MarciDistortionMap *)DistortionMap();
      distmap->SetFilter(p_filterNumbers[vband-1]);
    }
  }
}

// Plugin
extern "C" Isis::Camera *MarciCameraPlugin (Isis::Pvl &lab) {
  return new Isis::Mro::MarciCamera(lab);
}
