/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ClipperNacRollingShutterCamera.h"

#include <QString>

#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "Cube.h"
#include "iTime.h"
#include "NaifContext.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "RollingShutterCamera.h"
#include "RollingShutterCameraDetectorMap.h"
#include "Table.h"
#include "TableRecord.h"

namespace Isis {
  /**
   * Constructs a ClipperNacRollingShutterCamera object using the image labels.
   *
   * @param Cube &cube Clipper EIS image.
   */
  ClipperNacRollingShutterCamera::ClipperNacRollingShutterCamera(Cube &cube) : 
    RollingShutterCamera(cube) {

    m_spacecraftNameLong = "Europa Clipper";
    m_spacecraftNameShort = "Clipper";

    m_instrumentNameLong  = "Europa Imaging System Rolling Shutter Narrow Angle Camera";
    m_instrumentNameShort = "EIS-RSNAC";

    auto naif = NaifContext::acquire();

    naif->CheckErrors();

    SetFocalLength(naif);
    SetPixelPitch(naif);

    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);

    // Set up start time and exposure duration
    QString startTime = inst["StartTime"];
    iTime etStart(startTime); 

    // Use to calculate center time when exposure duration is available
    // double exposureDuration = ((double) inst["ExposureDuration"]);

    // Grab the coefficients for the polynomial that fit the jitter and the normal readout times of
    // the lines of the image
    
    // The two keywords and table below are _all_ required for remove/addJitter in the 
    // RollingShutterCameraDetectorMap to work. If any are missing, just leave the below arrays
    //  unitialized, and add/removeJitter will default to 0. 

    std::vector<double> sampleCoeffs, lineCoeffs, readoutTimes;

    if ( (inst.hasKeyword("JitterSampleCoefficients") && inst.hasKeyword("JitterLineCoefficients") )
        && cube.hasTable("Normalized Main Readout Line Times")) {
      PvlKeyword sampleCoefficients = inst.findKeyword("JitterSampleCoefficients"); 
      for (int i = 0; i < sampleCoefficients.size(); i++) {
        sampleCoeffs.push_back(sampleCoefficients[i].toDouble());
      }
      
      PvlKeyword lineCoefficients = inst.findKeyword("JitterLineCoefficients");
      for (int i = 0; i < lineCoefficients.size(); i++) {
        lineCoeffs.push_back(lineCoefficients[i].toDouble());
      }
      
      Table normalizedReadoutTimes("Normalized Main Readout Line Times", lab.fileName(), lab); 
      
      for (int i = 0; i < normalizedReadoutTimes.Records(); i++) {
        TableRecord record = normalizedReadoutTimes[i];
        readoutTimes.push_back((double) record["time"]);
      }
    }

    // Set up camera detector map with the coefficients and readout times
    new RollingShutterCameraDetectorMap(this, readoutTimes, sampleCoeffs, lineCoeffs);

    // Set up focal plane map
    new CameraFocalPlaneMap(this, naifIkCode());

    // Set up distortion map (use default for now)
    new CameraDistortionMap(this);

    // Set up the ground and sky map
    new CameraGroundMap(this);
    new CameraSkyMap(this);

    setTime(etStart, naif); // Consider changing to center in future. 
    LoadCache(naif);
    naif->CheckErrors();
  }


  /**
   * Destructor for a ClipperNacRollingShutterCamera object.
   *
   * Note that the memory allocated by the constructor is owned by Camera. Each of the Camera
   * utility objects created (CameraFocalPlaneMap, CameraDetectorMap, etc.) give ownership to
   * the Camera* parent passed to them.
   */
  ClipperNacRollingShutterCamera::~ClipperNacRollingShutterCamera() {
  }


  /**
   * CK frame ID
   *
   * CK frame ID obtained from the CK kernel for Europa Clipper by using spacit.
   * @see Camera::CkFrameId()

   * <tt>spacit -> R -> ck/europa_sa_17F12v2_tour_eom_ecr3018.bc -> INSTRUMENT_ID</tt>

   * @note Could not use spacit's S option, we do not have a SCLK for Clipper EIS yet.
   *   
   * @return int The appropriate instrument code for the "Camera-Matrix" Kernel Frame ID.
   */
  int ClipperNacRollingShutterCamera::CkFrameId() const {
    return (-159011);
  }


  /**
   * CK Reference ID - J2000
   *
   * CK reference ID obtained from the CK kernel for Europa Clipper by using spacit.
   * @see Camera::CkReferenceId()
   *
   * <tt>spacit -> R -> ck/europa_sa_17F12v2_tour_eom_ecr3018.bc -> REFERENCE_FRAME_NAME</tt>
   * Look up 'EUROPAM_SA_BASE' in FK kernel for Clipper EIS.
   *
   * @note Could not use spacit's S option, we do not have a SCLK for Clipper EIS yet.
   *
   * @return int The appropriate instrument code for the "Camera-matrix" Kernel Reference ID.
   */
  int ClipperNacRollingShutterCamera::CkReferenceId() const {
    return (-159010);
  }


  /**
   * SPK Reference ID - J2000
   *
   * SPK reference ID obtained from the SPK kernel for Europa Clipper by using spacit.
   * @see Camera::SpkReferenceId()
   *
   * <tt>spacit -> S -> spk/17F12_DIR_L220604_A241223_V2_scpse.bsp, naif0012.tls -> B -> 502</tt>
   * (502 is the NAIF ID for 'EUROPA')
   *
   * @return int The appropriate intstrument code for the Spacecraft Kernel Reference ID.
   */
  int ClipperNacRollingShutterCamera::SpkReferenceId() const {
    return (1);
  }
}


/**
 * This is the function that is called in order to instantiate an ClipperNacRollingShutterCamera 
 * object.
 *
 * @param Isis::Cube &cube Clipper EIS image.
 *
 * @return Isis::Camera* ClipperNacRollingShutterCamera
 */
extern "C" Isis::Camera *ClipperNacRollingShutterCameraPlugin(Isis::Cube &cube) {
  return new Isis::ClipperNacRollingShutterCamera(cube);
}
