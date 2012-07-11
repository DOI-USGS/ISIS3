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
#include "ThemisVisCamera.h"

#include <iomanip>

#include "CameraFocalPlaneMap.h"
#include "CameraSkyMap.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "PushFrameCameraDetectorMap.h"
#include "PushFrameCameraGroundMap.h"
#include "ThemisVisDistortionMap.h"


using namespace std;

namespace Isis {

  /**
   * Constructor for the Themis Vis Camera Model
   *
   * @param lab Pvl label from an Odyssey Themis VIS image.
   *
   * @throws IException::User - The image does not appear to be a Themis
   *                                  VIS image
   * @internal
   *   @history 2010-08-04 Jeannie Walldren - Added NAIF error check.
   */
  ThemisVisCamera::ThemisVisCamera(Pvl &lab) : PushFrameCamera(lab) {
    NaifStatus::CheckErrors();
    // Set up the camera characteristics
    // LoadFrameMounting("M01_SPACECRAFT","M01_THEMIS_VIS");
    // Changed Focal Length from 203.9 (millimeters????) to 202.059, per request from
    // Christopher Edwards (Christopher.Edwards@asu.edu) at ASU, on 2/18/11.
    SetFocalLength(202.059);
    SetPixelPitch(0.009);

    PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);
    // make sure it is a themis vis image
    if(inst["InstrumentId"][0] != "THEMIS_VIS") {
      string msg = "The image does not appear to be a Themis Vis Image";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Get necessary variables
    p_exposureDur = inst["ExposureDuration"];
    p_interframeDelay = inst["InterframeDelay"];
    int sumMode = inst["SpatialSumming"];

    // Get the start and end time
    double et;
    string stime = inst["SpacecraftClockCount"];
    et = getClockTime(stime).Et();

    double offset = inst["SpacecraftClockOffset"];
    p_etStart = et + offset - ((p_exposureDur / 1000.0) / 2.0);
    p_nframes = inst["NumFramelets"];

    // Get the keywords from labels
    PvlGroup &bandBin = lab.FindGroup("BandBin", Pvl::Traverse);
    PvlKeyword &orgBand = bandBin["OriginalBand"];
    for(int i = 0; i < orgBand.Size(); i++) {
      p_originalBand.push_back(orgBand[i]);
    }

    // Setup detector map
    double frameRate = p_interframeDelay;
    PushFrameCameraDetectorMap *dmap =
      new PushFrameCameraDetectorMap(this, p_etStart, frameRate, 192);
    dmap->SetDetectorSampleSumming(sumMode);
    dmap->SetDetectorLineSumming(sumMode);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());
    focalMap->SetDetectorOrigin(512.5, 512.5);

    // Setup distortion map
    new ThemisVisDistortionMap(this);

    // Setup the ground and sky map
    bool evenFramelets = (inst["Framelets"][0] == "Even");
    new PushFrameCameraGroundMap(this, evenFramelets);
    new CameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }

  /**
   * Sets the band in the camera model
   *
   * @param vband The band number to set
   */
  void ThemisVisCamera::SetBand(const int vband) {
    Camera::SetBand(vband);

    // Set the et
    setTime(p_etStart + BandEphemerisTimeOffset(vband));
    PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *)this->DetectorMap();
    dmap->SetStartTime(p_etStart + BandEphemerisTimeOffset(vband));
  }

  /**
   * Calculates time offset for the given band.
   *
   * @param vband The band number.
   *
   * @return @b double The time offset value.
   *
   */
  double ThemisVisCamera::BandEphemerisTimeOffset(int vband) {
    int waveToTimeBand[] = {2, 5, 3, 4, 1};
    int visBandFirstRow[] = {4, 203, 404, 612, 814};

    // Lookup the original band from the band bin group.  Unless there is
    // a reference band which means the data has all been aligned in the
    // band dimension
    int band = p_originalBand[vband-1];
    if(HasReferenceBand()) {
      band = ReferenceBand();
    }

    // convert wavelength band the time band
    band = waveToTimeBand[band-1];

    // Compute the time offset for this detector line
    p_bandTimeOffset = ((band - 1) * p_interframeDelay) -
                       ((p_exposureDur / 1000.0) / 2.0);

    PushFrameCameraDetectorMap *dmap = (PushFrameCameraDetectorMap *)this->DetectorMap();
    dmap->SetBandFirstDetectorLine(visBandFirstRow[band-1]);

    return p_bandTimeOffset;
  }
}


// Plugin
/**
 * This is the function that is called in order to instantiate a
 * ThemisVisCamera object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* ThemisVisCamera
 * @internal
 *   @history 2011-05-03 Jeannie Walldren - Removed Odyssey namespace.
 */
extern "C" Isis::Camera *ThemisVisCameraPlugin(Isis::Pvl &lab) {
  return new Isis::ThemisVisCamera(lab);
}
