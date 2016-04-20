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

#include "OsirisRexOcamsCamera.h"

#include <QString>

#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {

  /**
   * Constructs an OSIRIS-REx MapCam Model using the image labels.
   *  
   * @param lab Pvl label from an Osiris Rex MapCam image. 
   *  
   * @internal 
   *   @history 2014-04-04 Janet Barrett - Original version.
   *   @history 2015-09-28 Stuart C. Sides - Updated to work with OCams data delivery 2015-07
   *
   */
  OsirisRexOcamsCamera::OsirisRexOcamsCamera(Cube &cube) : FramingCamera(cube) {

    NaifStatus::CheckErrors();

    m_spacecraftNameLong = "OSIRIS-REx";
    m_spacecraftNameShort = "OSIRIS-REx";

    int ikCode = naifIkCode();

    if (ikCode == -64361) {
      m_instrumentNameLong = "Mapping Camera";
      m_instrumentNameShort = "MapCam";
    }
    else if (ikCode <= -64362) {
      m_instrumentNameLong = "Sampling Camera";
      m_instrumentNameShort = "SamCam";
    }
    else if (ikCode <= -64360) {
      m_instrumentNameLong = "PolyMath Camera";
      m_instrumentNameShort = "PolyCam";
    }

    SetFocalLength();

    // The instrument kernel contains pixel pitch in microns, so convert it to mm.
    QString pitch = "INS" + toString(naifIkCode()) + "_PIXEL_SIZE";
    SetPixelPitch(getDouble(pitch) / 1000.0);

    // Get the start time in et
    Pvl &lab = *cube.label();
    PvlGroup inst = lab.findGroup("Instrument", Pvl::Traverse);

    // Set the observation time and exposure duration
    QString clockCount = inst["SpacecraftClockStartCount"];
    double startTime = getClockTime(clockCount).Et();
    double exposureDuration = ((double) inst["ExposureDuration"]) / 1000.0;
    pair<iTime, iTime> shuttertimes = ShutterOpenCloseTimes(startTime, exposureDuration);

    // Add half exposure duration to get time at center of image
    iTime centerTime = shuttertimes.first.Et() + exposureDuration / 2.0;

    // Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    // The instrument kernel contains a CCD_CENTER keyword instead of BORESIGHT_LINE
    // and BORESIGHT_SAMPLE keywords.
    focalMap->SetDetectorOrigin(
        Spice::getDouble("INS" + toString(naifIkCode()) + "_CCD_CENTER", 0),
        Spice::getDouble("INS" + toString(naifIkCode()) + "_CCD_CENTER", 1));

    // Setup distortion map
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    setTime(centerTime);
    LoadCache();
    NaifStatus::CheckErrors();
  }


  //! Destroys the MapCamera object.
  OsirisRexOcamsCamera::~OsirisRexOcamsCamera() {
  }


  /**
   * Returns the shutter open and close times. The user should pass in the
   * ExposureDuration keyword value, converted to seconds, and the StartTime
   * keyword value, converted to ephemeris time. The StartTime keyword value
   * from the labels represents the time at the start of the observation, as
   * noted in the Osiris Rex EDR image SIS. This method uses the FramingCamera
   * class implementation, returning the given time value as the shutter open
   * and the sum of the time value and exposure duration as the shutter close.
   *
   * @param exposureDuration ExposureDuration keyword value from the labels,
   *                         converted to seconds.
   * @param time The StartTime keyword value from the labels, converted to
   *             ephemeris time.
   *
   * @return @b pair < @b iTime, @b iTime > The first value is the shutter
   *         open time and the second is the shutter close time.
   *
   * @see http://pds-imaging.jpl.nasa.gov/documentation/clementine_edrsis.pdf
   * @author 2011-05-03 Jeannie Walldren
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Original version.
   */
  pair<iTime, iTime> OsirisRexOcamsCamera::ShutterOpenCloseTimes(double time,
                                                               double exposureDuration) {
    return FramingCamera::ShutterOpenCloseTimes(time, exposureDuration);
  }


  /**
   * CK frame ID -  - Instrument Code from spacit run on CK
   *  
   * @return @b int The appropriate instrument code for the "Camera-matrix" 
   *         Kernel Frame ID
   */
  int OsirisRexOcamsCamera::CkFrameId() const { 
    return -64361; 
  }


  /** 
   * CK Reference ID - J2000
   * Obtain by running /usgs/pkgs/local/isis3.2.2.0/src/naif/cspice/exe/spacit.
   * Choose "Summarize binary file" option and enter:
   * Binary file     : /work/projects/progteam/jbarrett/OsirisRex/kernels/ck/osiris-rex/orx_approach_1s.bc
   * Leapseconds file: /work/projects/progteam/jbarrett/OsirisRex/kernels/lsk/naif0010.tls
   * SCLK file       : /work/projects/progteam/jbarrett/OsirisRex/kernels/sclk/ORX_SCLKSCET.00000.example.tsc
   * Choose "Summarize entire file" option.
   * Locate instrument code -64361 in the summary to get information for MapCam.
   * Get the "Reference frame" value and put it here.
   * NOTE: The current SPICE kernels do not have an entry for the MapCam, so the value used here may not
   * be correct.
   * 
   * @return @b int The appropriate instrument code for the "Camera-matrix"
   *         Kernel Reference ID
   */
  int OsirisRexOcamsCamera::CkReferenceId() const { 
    return 1; 
  }


  /** 
   * SPK Reference ID - J2000
   * Obtain by running /usgs/pkgs/local/isis3.2.2.0/src/naif/cspice/exe/spacit.
   * Choose "Summarize binary file" option and enter:
   * Binary file     : /work/projects/progteam/jbarrett/OsirisRex/kernels/spk/osiris-rex/orx_approach.bsp
   * Leapseconds file: /work/projects/progteam/jbarrett/OsirisRex/kernels/lsk/naif0010.tls
   * Choose "Summarize entire file" option.
   * Get the "Reference frame" value and put it here.
   * 
   * @return @b int The appropriate instrument code for the Spacecraft
   *         Kernel Reference ID
   */
  int OsirisRexOcamsCamera::SpkReferenceId() const { 
    return 1; 
  }
}


/**
 * This is the function that is called in order to instantiate a MapCamera
 * object.
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* OsirisRexOcamsCamera
 */
extern "C" Isis::Camera *OsirisRexOcamsCameraPlugin(Isis::Cube &cube) {
  return new Isis::OsirisRexOcamsCamera(cube);
}
