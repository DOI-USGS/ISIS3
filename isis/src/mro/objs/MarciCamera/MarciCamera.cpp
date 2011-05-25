/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <iomanip>

#include "MarciCamera.h"
#include "MarciDistortionMap.h"

#include "CameraFocalPlaneMap.h"
#include "CameraSkyMap.h"
#include "iException.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "PushFrameCameraDetectorMap.h"
#include "PushFrameCameraGroundMap.h"

using namespace std;
namespace Isis {

  /**
   * Constructor for the Marci Camera Model
   *
   * @param lab Pvl Label to create the camera model from
   *
   * @throws Isis::iException::User - The image does not appear to be a Marci
   *             image
   * @internal 
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   */
  MarciCamera::MarciCamera(Pvl &lab) : PushFrameCamera(lab) {
    NaifStatus::CheckErrors();
    PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);
    // make sure it is a marci image
    if(inst["InstrumentId"][0] != "Marci") {
      string msg = "The image does not appear to be a Marci Image";
      throw iException::Message(iException::User, msg, _FILEINFO_);
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
    et = getClockTime(stime).Et();
    p_etStart = et - ((p_exposureDur / 1000.0) / 2.0);
    p_nframelets = ParentLines() / sumMode;

    // These numbers came from "MARCI_CTX_Cal_Report_v1.5.pdf" page 7 (Bandpasses & downlinked detector rows)
    std::map<string, int> filterToDetectorOffset;
    filterToDetectorOffset.insert(pair<string, int>("BLUE",     709));
    filterToDetectorOffset.insert(pair<string, int>("GREEN",    734));
    filterToDetectorOffset.insert(pair<string, int>("ORANGE",   760));
    filterToDetectorOffset.insert(pair<string, int>("RED",      786));
    filterToDetectorOffset.insert(pair<string, int>("NIR",      811));
    filterToDetectorOffset.insert(pair<string, int>("LONG_UV",  266));
    filterToDetectorOffset.insert(pair<string, int>("SHORT_UV", 293));

    std::map<string, int> filterToFilterNumbers;
    filterToFilterNumbers.insert(pair<string, int>("BLUE",     0));
    filterToFilterNumbers.insert(pair<string, int>("GREEN",    1));
    filterToFilterNumbers.insert(pair<string, int>("ORANGE",   2));
    filterToFilterNumbers.insert(pair<string, int>("RED",      3));
    filterToFilterNumbers.insert(pair<string, int>("NIR",      4));
    filterToFilterNumbers.insert(pair<string, int>("LONG_UV",  5));
    filterToFilterNumbers.insert(pair<string, int>("SHORT_UV", 6));

    int frameletOffsetFactor = inst["ColorOffset"];

    if((int)inst["DataFlipped"] != 0) frameletOffsetFactor *= -1;
    std::map<string, int> filterToFrameletOffset;
    filterToFrameletOffset.insert(pair<string, int>("NIR",      0 * frameletOffsetFactor));
    filterToFrameletOffset.insert(pair<string, int>("RED",      1 * frameletOffsetFactor));
    filterToFrameletOffset.insert(pair<string, int>("ORANGE",   2 * frameletOffsetFactor));
    filterToFrameletOffset.insert(pair<string, int>("GREEN",    3 * frameletOffsetFactor));
    filterToFrameletOffset.insert(pair<string, int>("BLUE",     4 * frameletOffsetFactor));
    filterToFrameletOffset.insert(pair<string, int>("LONG_UV",  5 * frameletOffsetFactor));
    filterToFrameletOffset.insert(pair<string, int>("SHORT_UV", 6 * frameletOffsetFactor));

    // Get the keywords from labels
    const PvlGroup &bandBin = lab.FindGroup("BandBin", Pvl::Traverse);
    const PvlKeyword &filtNames = bandBin["FilterName"];

    for(int i = 0; i < filtNames.Size(); i++) {
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
      new PushFrameCameraDetectorMap(this, p_etStart, frameletRate, 16);
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
    NaifStatus::CheckErrors();

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
  void MarciCamera::SetBand(const int vband) {
    Camera::SetBand(vband);

    PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *)DetectorMap();
    dmap->SetBandFirstDetectorLine(p_detectorStartLines[vband-1]);
    dmap->SetFrameletOffset(p_frameletOffsets[vband-1]);

    MarciDistortionMap *distmap = (MarciDistortionMap *)DistortionMap();
    distmap->SetFilter(p_filterNumbers[vband-1]);
  }
}


/**
 * This is the function that is called in order to instantiate a MarciCamera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* MarciCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Mro namespace.
 */
extern "C" Isis::Camera *MarciCameraPlugin(Isis::Pvl &lab) {
  return new Isis::MarciCamera(lab);
}
