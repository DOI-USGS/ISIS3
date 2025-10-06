/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ShadowCamCamera.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include "CameraFocalPlaneMap.h"
#include "IException.h"
#include <string.h>
#include "iTime.h"
#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlContainer.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "ShadowCamDistortionMap.h"
#include "ShadowCamUtilities.h"
#include "NaifStatus.h"

using namespace std;
namespace Isis {
  /**
   * Constructor for the ShadowCam Camera Model
   *
   * @param lab Pvl Label to create the camera model from
   *
   * @internal
   *   @history 2022-10-12 Victor Silva - original object
   *   @history 2024-07-12 Victor Silva - updated to use SpacecraftClockPrerollCount
   *                                      and use getClockTime to use SCLKS
   *   @history 2025-09-08 Victor Silva - updated to use SpacecraftStartTime + StartTimeOffset
   *                                      from labels instead of deriving in kernel. 
   */
  ShadowCamCamera::ShadowCamCamera(Cube &cube) : LineScanCamera(cube) {
    m_spacecraftNameLong = "KOREA PATHFINDER LUNAR ORBITER";
    m_spacecraftNameShort = "KPLO";
    // SHADOWCAM instrument kernel code = -155151
    if (naifIkCode() == -155151) {
      m_instrumentNameLong = "KOREA PATHFINDER LUNAR ORBITER SHADOWCAM";
      m_instrumentNameShort = "KPLO SHADOWCAM";
    }
    else {
      QString msg = "File does not appear to be a Korea Pathfinder Lunar Orbiter ShadowCam Image: ";
      msg += QString::number(naifIkCode());
      msg += " is not a supported instrument kernel code for Korea Pathfinder Lunar Orbiter.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    NaifStatus::CheckErrors();

    /** kernels *************************************************************************/
    // Set the camera up using info from ik/iak kernels
    SetFocalLength();
    SetPixelPitch();

    double constantTimeOffset = 0.0,
           additiveLineTimeError = 0.0,
           multiplicativeLineTimeError = 0.0;

    QString ikernKey = "INS" + toString(naifIkCode()) + "_CONSTANT_TIME_OFFSET";
    constantTimeOffset = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_ADDITIVE_LINE_ERROR";
    additiveLineTimeError = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_MULTIPLI_LINE_ERROR";
    multiplicativeLineTimeError = getDouble(ikernKey);
    
    // Get the start time from labels
    Pvl &label = *cube.label();
    PvlGroup instrument = label.findGroup("Instrument", Pvl::Traverse);

    static QString sTimeQStr = (GetFromLabels(instrument, "ExecutionSpacecraftTime"));
    SpiceDouble executionSpacecraftTimeFromLabels = getClockTime(sTimeQStr,-155, false).Et();
    //SpiceDouble etStart = getClockTime(sTimeQStr,-155, false).Et();
    //static double prerollLines = (GetFromLabels(instrument, "PrerollLines")).toDouble();
    // Shadwowcam linerate info in milliseconds, make in secs
    static double lineRateSecs = (GetFromLabels(instrument, "LineRate")).toDouble() / 1000.0;
    // start time offset in secs
    static double startTimeOffsetSecsFromLabels = (GetFromLabels(instrument, "StartTimeOffset")).toDouble();
    // this value is usually 0 then set to one in two lines below
    static double ss = (GetFromLabels(instrument, "SampleFirstPixel")).toDouble();
    ss += 1.0;
    // TDI direction offset from IAK
    /*keywordExist
      INS-155151_TDI_A_Offset 
      INS-155151_TDI_B_Offset 
    */
    double tdiOffset = 0.0,
           tdiOffsetSecs = 0.0,
           etStartTime = 0.0;

    if(!instrument.hasKeyword("TDIDirection")){
      QString msg = "Error: keyword: ";
              msg += "TDIDirection";
              msg += " was not found in labels.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    // get tdi factor from tdi direction
    if(QString::compare(instrument["TDIDirection"], "A", Qt::CaseInsensitive) != 0){
      ikernKey = "INS" + toString(naifIkCode()) + "_TDI_A_OFFSET";
      tdiOffset = getDouble(ikernKey);
    }
    else{
      ikernKey = "INS" + toString(naifIkCode()) + "_TDI_B_OFFSET";
      tdiOffset = getDouble(ikernKey);
    }
    /*/===========================================================================
      startTimeOffsetSecs will come from labels instead but code is left commented  
      for information purposes

      
      double preroll_delay = prerollLines * lineRateSecs;
      double command_delay = 0.0043656;
      double mystery_offset = 0.79539;

      preroll_delay + command_delay + mystery_offset;
      if (startTimeOffsetSecs != startTimeOffsetSecsFromLabels){
        QString msg = "Error: startTimeOffset in camera model doens't match offset in labels";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      startTimeOffSetSecs += tdi_offset_secs;

    ====================================*/
    lineRateSecs *= 1.0 + multiplicativeLineTimeError;
    lineRateSecs += additiveLineTimeError;
    tdiOffsetSecs = tdiOffset * lineRateSecs;
    etStartTime = executionSpacecraftTimeFromLabels + startTimeOffsetSecsFromLabels + constantTimeOffset + tdiOffsetSecs;
    setTime(etStartTime);
    // Setup detector map
    LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, etStartTime, lineRateSecs);
    detectorMap->SetStartingDetectorSample(ss);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
    ikernKey = "INS" + toString(naifIkCode()) + "_BORESIGHT_SAMPLE";
    double sampleBoreSight = getDouble(ikernKey);

    ikernKey = "INS" + toString(naifIkCode()) + "_BORESIGHT_LINE";
    double lineBoreSight = getDouble(ikernKey);

    focalMap->SetDetectorOrigin(sampleBoreSight, lineBoreSight);
    focalMap->SetDetectorOffset(0.0, 0.0);

    // Setup distortion map
    ShadowCamDistortionMap *distMap = new ShadowCamDistortionMap(this);
    distMap->SetDistortion(naifIkCode());

    // Setup the ground and sky map
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}

/**
 * This is the function that is called in order to instantiate a
 * KPLO ShadowCam object.
 *
 * @param lab Cube 
 *
 * @return Isis::Camera* KploShadowCam Camera
 * @internal
 *    @history 2022-10-12 Victor Silva - original object
 */
extern "C" Isis::Camera *ShadowCamCameraPlugin(Isis::Cube &cube) {
  return new Isis::ShadowCamCamera(cube);
}
