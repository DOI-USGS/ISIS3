#include <sstream>
#include <iomanip>

#include "LroWideAngleCamera.h"
#include "PushFrameCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "PushFrameCameraGroundMap.h"
#include "CameraSkyMap.h"
#include "LroWideAngleCameraDistortionMap.h"

using namespace std;

namespace Isis {
  namespace Lro {
    /**
     * Constructor for the LRO WAC Camera Model
     *
     * @param lab Pvl Label to create the camera model from
     *
     * @throws Isis::iException::User - The image does not appear to be a Lunar
     *             Reconaissance Orbiter Wide Angle Camera image
     */
    LroWideAngleCamera::LroWideAngleCamera(Isis::Pvl &lab) :
      Isis::PushFrameCamera(lab) {
      // Set up the camera characteristics
      InstrumentRotation()->SetFrame(-85620);
      SetFocalLength();
      SetPixelPitch();

      // Get the ephemeris time from the labels
      double et;
      Isis::PvlGroup &inst = lab.FindGroup("Instrument", Isis::Pvl::Traverse);
      string stime = inst["SpacecraftClockStartCount"];
      scs2e_c(NaifSclkCode(), stime.c_str(), &et);

      p_exposureDur = inst["ExposureDuration"];
      // TODO:  Changed et - exposure to et + exposure.
      //   Think about if this is correct
      p_etStart = et + ((p_exposureDur / 1000.0) / 2.0);

      // Compute the framelet size and number of framelets
      iString instId = iString((string) inst["InstrumentId"]).UpCase();

      int frameletSize = 14;
      int sumMode = 1;

      if(instId == "WAC-UV") {
        sumMode = 4;
        frameletSize = 16;
      }
      else if(instId == "WAC-VIS") {
        sumMode = 1;
        frameletSize = 14;
      }
      else {
        string msg = "Invalid value [" + instId
                     + "] for keyword [InstrumentId]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      p_nframelets = ParentLines() / (frameletSize / sumMode);

      // Setup the line detector offset map for each filter
      int nbands = (int) lab.FindKeyword("Bands", PvlObject::Traverse);
      const PvlGroup &bandBin = lab.FindGroup("BandBin", Isis::Pvl::Traverse);
      const PvlKeyword &filtNames = bandBin["Center"];
      // Sanity check
      if(nbands != filtNames.Size()) {
        ostringstream mess;
        mess << "Number bands in (file) label (" << nbands
             << ") do not match number of values in BandBin/Center keyword ("
             << filtNames.Size() << ") - required for band-dependant geoemtry";
        throw iException::Message(iException::User, mess.str(), _FILEINFO_);
      }

      //  Now create detector offsets
      std::map<int, int> filterToDetectorOffset;

      filterToDetectorOffset.insert(std::pair<int, int>(315, 244));
      filterToDetectorOffset.insert(std::pair<int, int>(321, 244));

      filterToDetectorOffset.insert(std::pair<int, int>(360, 302));

      filterToDetectorOffset.insert(std::pair<int, int>(415, 702));

      filterToDetectorOffset.insert(std::pair<int, int>(565, 727));
      filterToDetectorOffset.insert(std::pair<int, int>(566, 727));

      filterToDetectorOffset.insert(std::pair<int, int>(600, 753));
      filterToDetectorOffset.insert(std::pair<int, int>(604, 753));

      filterToDetectorOffset.insert(std::pair<int, int>(640, 780));
      filterToDetectorOffset.insert(std::pair<int, int>(643, 780));

      filterToDetectorOffset.insert(std::pair<int, int>(680, 805));
      filterToDetectorOffset.insert(std::pair<int, int>(689, 805));

      int frameletOffsetFactor = inst["ColorOffset"];

      if(inst["DataFlipped"][0].UpCase() == "YES")
        frameletOffsetFactor *= -1;

      std::map<int, int> filterToFrameletOffset;
      // the UV order is based on position in the cube
      filterToFrameletOffset.insert(
        std::pair<int, int>(321, 0 * frameletOffsetFactor)
      );
      filterToFrameletOffset.insert(
        std::pair<int, int>(315, 0 * frameletOffsetFactor)
      );

      filterToFrameletOffset.insert(
        std::pair<int, int>(360, 1 * frameletOffsetFactor)
      );

      filterToFrameletOffset.insert(
        std::pair<int, int>(415, 0 * frameletOffsetFactor));

      filterToFrameletOffset.insert(
        std::pair<int, int>(566, 1 * frameletOffsetFactor)
      );
      filterToFrameletOffset.insert(
        std::pair<int, int>(565, 1 * frameletOffsetFactor)
      );

      filterToFrameletOffset.insert(
        std::pair<int, int>(604, 2 * frameletOffsetFactor)
      );
      filterToFrameletOffset.insert(
        std::pair<int, int>(600, 2 * frameletOffsetFactor)
      );

      filterToFrameletOffset.insert(
        std::pair<int, int>(643, 3 * frameletOffsetFactor)
      );
      filterToFrameletOffset.insert(
        std::pair<int, int>(640, 3 * frameletOffsetFactor)
      );

      filterToFrameletOffset.insert(
        std::pair<int, int>(689, 4 * frameletOffsetFactor)
      );
      filterToFrameletOffset.insert(
        std::pair<int, int>(680, 4 * frameletOffsetFactor)
      );

      for(int i = 0; i < filtNames.Size(); i++) {
        if(filterToDetectorOffset.find((int) filtNames[i]) ==
            filterToDetectorOffset.end()) {
          string msg = "Unrecognized filter name [" + filtNames[i] + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        p_detectorStartLines.push_back(
          filterToDetectorOffset.find(filtNames[i])->second
        );
        p_frameletOffsets.push_back(
          filterToFrameletOffset.find(filtNames[i])->second
        );
      }

      // Setup detector map
      double frameletRate = (double) inst["InterframeDelay"] / 1000.0;
      PushFrameCameraDetectorMap *dmap = new PushFrameCameraDetectorMap(this,
          p_etStart, frameletRate, frameletSize);
      dmap->SetDetectorSampleSumming(sumMode);
      dmap->SetDetectorLineSumming(sumMode);

      bool flippedFramelets = false;

      // flipping disabled
      if(iString((string) inst["DataFlipped"]).UpCase() == "YES")
        flippedFramelets = true;

      dmap->SetFlippedFramelets(flippedFramelets, p_nframelets);

      // Setup focal plane map
      new CameraFocalPlaneMap(this, NaifIkCode());

      // The line detector origin varies based on instrument mode
      double detectorOriginLine;
      double detectorOriginSamp;

      dmap->SetGeometricallyFlippedFramelets(false);

      iString ikernKey;
      ikernKey = "INS" + iString((int) NaifIkCode()) + "_BORESIGHT_SAMPLE";
      double sampleBoreSight = GetDouble(ikernKey);

      ikernKey = "INS" + iString((int) NaifIkCode()) + "_BORESIGHT_LINE";
      double lineBoreSight = GetDouble(ikernKey);

      if(instId == "WAC-UV") {
        /**
         * The detector origin sample is from
         *   LRO Wide Angle Geometric Calibration
         *   document by Peter Thomas and accounts
         *   for the 8 pixels off the left side of
         *   the detector and the 256 other ignored
         *   pixels off the left side of the detector
         *   which are not in the image.
         */
        detectorOriginSamp = sampleBoreSight - 8 - 256;
        detectorOriginLine = lineBoreSight;
      }
      else {
        iString instModeId;
        instModeId = ((iString)(string) inst["InstrumentModeId"]).UpCase();

        if(instModeId == "COLOR" || instModeId == "VIS") {
          /**
           * The detector origin sample is from
           *   LRO Wide Angle Geometric Calibration
           *   document by Peter Thomas and accounts
           *   for the 8 pixels off the left side of
           *   the detector and the 160 other ignored
           *   pixels off the left side of the detector
           *   which are not in the image.
           */
          detectorOriginSamp = sampleBoreSight - 8 - 160;
          detectorOriginLine = lineBoreSight;
        }
        else if(instModeId == "BW") {

          iString mode = ((iString)(string) inst["Mode"]).UpCase();

          /**
           * The detector origin sample is from
           *   LRO Wide Angle Geometric Calibration
           *   document by Peter Thomas and accounts
           *   for the 8 pixels off the left side of
           *   the detector.
           */

          // Mode 0 and 1 are not Polar (i.e. they are only 704 pixels wide)
          if(mode == "0" || mode == "1") {
            // The 160 other ignored pixels off the left side of the detector
            //   which are not in the image. These numbers added to compensate
            //   for the non-polar mode.
            detectorOriginSamp = sampleBoreSight - 8 - 160;
            detectorOriginLine = lineBoreSight;
          }
          // Mode 2 and 3 are Polar (i.e. they are 1024 pixels wide)
          else if(mode == "2" || mode == "3") {
            detectorOriginSamp = sampleBoreSight - 8;
            detectorOriginLine = lineBoreSight;
          }
          else {
            string msg = "Invalid value [" + mode + "] for keyword [Mode]";
            throw iException::Message(iException::User, msg, _FILEINFO_);
          }

          // There are no offsets for BW mode.. there can only be 1 filter
          //   and there must be 1 filter.
          p_frameletOffsets[0] = 0;
        }
        else {
          string msg = "Invalid value [" + instModeId +
                       "] for keyword [InstrumentModeId]";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }
      }

      FocalPlaneMap()->SetDetectorOrigin(detectorOriginSamp,
                                         detectorOriginLine);

      // Setup distortion map
      new LroWideAngleCameraDistortionMap(this, NaifIkCode());

      // Setup the ground and sky map
      bool evenFramelets = (iString((string) inst["Framelets"][0]).UpCase()
                            == "EVEN");

      new PushFrameCameraGroundMap(this, evenFramelets);

      new CameraSkyMap(this);
      LoadCache();

      if(instId == "WAC-UV") {
        // geometric tiling is not worth trying for 4-line framelets
        SetGeometricTilingHint(2, 2);
      }
      else {
        SetGeometricTilingHint(8, 4);
      }
    }

    /**
     * Sets the band in the camera model
     *
     * @param vband The band number to set
     */
    void LroWideAngleCamera::SetBand(const int vband) {

      // Sanity check on requested band
      int maxbands = min(p_detectorStartLines.size(), p_frameletOffsets.size());
      if((vband <= 0) || (vband > maxbands)) {
        ostringstream mess;
        mess << "Requested virtual band (" << vband
             << ") outside valid (BandBin/Center) limits (1 - " << maxbands
             <<  ")";
        throw iException::Message(iException::Programmer, mess.str(), _FILEINFO_);
      }

      //  Set up valid band access
      Camera::SetBand(vband);
      PushFrameCameraDetectorMap *dmap = NULL;
      dmap = (PushFrameCameraDetectorMap *) DetectorMap();
      dmap->SetBandFirstDetectorLine(p_detectorStartLines[vband - 1]);
      dmap->SetFrameletOffset(p_frameletOffsets[vband - 1]);
    }
  }
}

// Plugin
extern "C" Isis::Camera *LroWideAngleCameraPlugin(Isis::Pvl &lab) {
  return new Isis::Lro::LroWideAngleCamera(lab);
}
