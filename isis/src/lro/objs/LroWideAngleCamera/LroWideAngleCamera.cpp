#include <sstream>
#include <iomanip>

#include "LroWideAngleCamera.h"
#include "PushFrameCameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "PushFrameCameraGroundMap.h"
#include "CameraSkyMap.h"
#include "LroWideAngleCameraDistortionMap.h"
#include "CollectorMap.h"

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
             << filtNames.Size() << ") - required for band-dependent geoemtry";
        throw iException::Message(iException::User, mess.str(), _FILEINFO_);
      }

      // Is the data flipped?
      bool dataflipped = (inst["DataFlipped"][0].UpCase() == "YES");

      //  Now create detector offsets
      iString instCode = "INS" + iString((int) NaifIkCode());
      iString ikernKey = instCode + "_FILTER_BANDCENTER";
      std::vector<int> fbc = GetVector(ikernKey);
      ikernKey = instCode + "_FILTER_OFFSET";
      std::vector<int> foffset = GetVector(ikernKey);


      // Create a map of filter wavelength to offset.  Also needs a reverse
      // lookup to order the offset into the CCD (ascending sort provided
      // automagically be CollectorMap).
      CollectorMap<int, int> filterToDetectorOffset, wavel;
      for(unsigned int i = 0 ; i < foffset.size() ; i++) {
        filterToDetectorOffset.add(fbc[i], foffset[i]);
        wavel.add(foffset[i], fbc[i]);
      }

      // Construct special format for framelet offsets into CCD.  Uses the above
      // reverse map.  Need only get the value (wavelength) of the map as the
      // key (offset) is sorted above.
      int frameletOffsetFactor = inst["ColorOffset"];
      if(dataflipped) frameletOffsetFactor *= -1;
      CollectorMap<int, int> filterToFrameletOffset;
      for(int j = 0 ; j < wavel.size() ; j++) {
        int wavelen = wavel.getNth(j);
        filterToFrameletOffset.add(wavelen, j * frameletOffsetFactor);
      }

      //  Now map the actual filter that exist in cube
      for(int i = 0; i < filtNames.Size(); i++) {
        if(!filterToDetectorOffset.exists(filtNames[i])) {
          string msg = "Unrecognized filter name [" + filtNames[i] + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }

        p_detectorStartLines.push_back(filterToDetectorOffset.get(filtNames[i]));
        p_frameletOffsets.push_back(filterToFrameletOffset.get(filtNames[i]));
      }

      // Setup detector map
      double frameletRate = (double) inst["InterframeDelay"] / 1000.0;
      PushFrameCameraDetectorMap *dmap = new PushFrameCameraDetectorMap(this,
          p_etStart, frameletRate, frameletSize);
      dmap->SetDetectorSampleSumming(sumMode);
      dmap->SetDetectorLineSumming(sumMode);

      // flipping disabled if already flipped
      bool flippedFramelets = dataflipped;
      dmap->SetFlippedFramelets(flippedFramelets, p_nframelets);

      // Setup focal plane map
      new CameraFocalPlaneMap(this, NaifIkCode());

      // The line detector origin varies based on instrument mode
      double detectorOriginLine;
      double detectorOriginSamp;

      dmap->SetGeometricallyFlippedFramelets(false);

      ikernKey = instCode + "_BORESIGHT_SAMPLE";
      double sampleBoreSight = GetDouble(ikernKey);

      ikernKey = instCode + "_BORESIGHT_LINE";
      double lineBoreSight = GetDouble(ikernKey);

      //  get instrument-specific sample offset
      iString instModeId = ((iString)(string) inst["InstrumentModeId"]).UpCase();
      // For BW mode, add the mode (0,1 (non-polar) or 2,3 (polar)) used to
      // acquire image 
      if (instModeId == "BW") {
        instModeId += inst["Mode"][0];
        // There are no offsets for BW mode.. there can only be 1 filter
        //   and there must be 1 filter.
        p_frameletOffsets[0] = 0;
      }
      ikernKey = instCode + "_" + instModeId + "_SAMPLE_OFFSET";
      int sampOffset = GetInteger(ikernKey);

      detectorOriginSamp = sampleBoreSight + 1;
      detectorOriginLine = lineBoreSight + 1;

      FocalPlaneMap()->SetDetectorOrigin(detectorOriginSamp,
                                         detectorOriginLine);
      dmap->SetStartingDetectorSample(sampOffset+1);

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

    int LroWideAngleCamera::PoolKeySize(const std::string &key) const {
      SpiceBoolean found;
      SpiceInt n;
      SpiceChar ctype[1];
      dtpool_c(key.c_str(), &found, &n, ctype);
      if(!found) n = 0;
      return (n);
    }

    std::vector<int> LroWideAngleCamera::GetVector(const std::string &key) 
                                                   const {
      int nvals = PoolKeySize(key);
      if(nvals <= 0) {
        string mess = "Kernel pool keyword " + key + " not found!";
        throw iException::Message(iException::Programmer, mess, _FILEINFO_);
      }

      std::vector<int> parms;
      for(int i = 0 ; i < nvals ; i++) {
        parms.push_back(GetInteger(key, i));
      }

      return (parms);
    }

  }
}

// Plugin
extern "C" Isis::Camera *LroWideAngleCameraPlugin(Isis::Pvl &lab) {
  return new Isis::Lro::LroWideAngleCamera(lab);
}
