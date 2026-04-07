/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>
#include <QStringBuilder>

#include "NaifStatus.h"
#include "Spice.h"

#include "Camera.h"
#include "CameraFocalPlaneMap.h"
#include "Cube.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "LineScanCamera.h"
#include "LineScanCameraDetectorMap.h"
#include "LineScanCameraGroundMap.h"
#include "LineScanCameraSkyMap.h"
#include "ShadowCamDistortionMap.h"
#include "ShadowCamUtilities.h"

#include "ShadowCamCamera.h"

namespace Isis {

  ShadowCamCamera::ShadowCamCamera(Cube &cube) : LineScanCamera(cube) {
    m_spacecraftNameLong = "KOREA PATHFINDER LUNAR ORBITER";
    m_spacecraftNameShort = "KPLO";
    // SHADOWCAM instrument kernel code = -155151
    if (naifIkCode() != -155151) {
      QString msg = "File does not appear to be a Korea Pathfinder Lunar Orbiter ShadowCam Image: "
                  % QString::number(naifIkCode())
                  % " is not a supported instrument kernel code for Korea Pathfinder Lunar Orbiter.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    m_instrumentNameLong = "KOREA PATHFINDER LUNAR ORBITER SHADOWCAM";
    m_instrumentNameShort = "KPLO SHADOWCAM";

    NaifStatus::CheckErrors();

    /** kernels *************************************************************************/
    // Set the camera up using info from ik/iak kernels
    SetFocalLength();
    SetPixelPitch();

    const QString constantTimeOffsetIkKey = "INS" % toString(naifIkCode()) % "_CONSTANT_TIME_OFFSET";
    const double constantTimeOffset = getDouble(constantTimeOffsetIkKey);

    const QString additiveLineErrorIkKey = "INS" % toString(naifIkCode()) % "_ADDITIVE_LINE_ERROR";
    const double additiveLineError = getDouble(additiveLineErrorIkKey);

    const QString multiplicativeLineErrorIkKey = "INS" % toString(naifIkCode()) % "_MULTIPLI_LINE_ERROR";
    const double multiplicativeLineError = getDouble(multiplicativeLineErrorIkKey);
    
    // Get the start time from labels
    const Pvl &label = *cube.label();
    const PvlGroup &instrument = label.findGroup("Instrument", Pvl::Traverse);

    const QString executionSpacecraftTimeClockString = (ShadowCam::GetFromLabels(instrument, "ExecutionSpacecraftTime"));
    const SpiceDouble executionSpacecraftTimeSecs = getClockTime(executionSpacecraftTimeClockString, -155, false).Et();

    //SpiceDouble etStart = getClockTime(executionSpacecraftTimeClockString, -155, false).Et();
    //static double prerollLines = (GetFromLabels(instrument, "PrerollLines")).toDouble();

    // ShadowCam linerate is in milliseconds, so convert it to seconds first
    const double lineRateSecs = ((ShadowCam::GetFromLabels(instrument, "LineRate")).toDouble() / 1000.0)
                        * (1.0 + multiplicativeLineError)
                        + additiveLineError;

    // Start time offset is in seconds, so no conversion needed
    const double startTimeOffsetSecs = (ShadowCam::GetFromLabels(instrument, "StartTimeOffset")).toDouble();

    // TDI direction offset from IAK
    /*keywordExist
      INS-155151_TDI_A_Offset 
      INS-155151_TDI_B_Offset 
    */
    double tdiOffset = 0.0;
    if(!instrument.hasKeyword("TDIDirection")){
      const QString msg = "Error: keyword: TDIDirection was not found in labels.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    // Get TDI factor from TDI direction
    if(QString::compare(instrument["TDIDirection"], "A", Qt::CaseInsensitive) != 0){
      const QString tdiAOffsetIkKey = "INS" % toString(naifIkCode()) % "_TDI_A_OFFSET";
      tdiOffset = getDouble(tdiAOffsetIkKey);
    }
    else if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) != 0) {
      const QString tdiBOffsetIkKey = "INS" % toString(naifIkCode()) % "_TDI_B_OFFSET";
      tdiOffset = getDouble(tdiBOffsetIkKey);
    }
    else {
      const QString msg = "Error: TDIDirection value in labels is invalid. Expected value of A or B, but got: " % QString(instrument["TDIDirection"]);
      throw IException(IException::User, msg, _FILEINFO_);
    }
    const double tdiOffsetSecs = tdiOffset * lineRateSecs;

    const double ss = (ShadowCam::GetFromLabels(instrument, "SampleFirstPixel")).toDouble() + 1.0;

    /*/===========================================================================
      startTimeOffsetSecs will come from labels instead but code is left commented  
      for information purposes

      
      double preroll_delay = prerollLines * lineRateSecs;
      double command_delay = 0.0043656;
      double mystery_offset = 0.79539;

      preroll_delay + command_delay + mystery_offset;
      if (startTimeOffsetSecs != startTimeOffsetSecsFromLabels){
        QString msg = "Error: startTimeOffset in camera model doesn't match offset in labels";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      startTimeOffSetSecs += tdi_offset_secs;

    ====================================*/
    const double etStartTime = executionSpacecraftTimeSecs + startTimeOffsetSecs + constantTimeOffset + tdiOffsetSecs;
    setTime(etStartTime);

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, etStartTime, lineRateSecs);
    detectorMap->SetStartingDetectorSample(ss);

    // Setup focal plane map
    CameraFocalPlaneMap *focalMap = new CameraFocalPlaneMap(this, naifIkCode());

    //  Retrieve boresight location from instrument kernel (IK) (addendum?)
    const QString boresightSampleIkKey = "INS" % toString(naifIkCode()) % "_BORESIGHT_SAMPLE";
    const double boresightSample = getDouble(boresightSampleIkKey);
    const QString boresightLineIkKey = "INS" % toString(naifIkCode()) % "_BORESIGHT_LINE";
    const double boresightLine = getDouble(boresightLineIkKey);

    focalMap->SetDetectorOrigin(boresightSample, boresightLine);
    focalMap->SetDetectorOffset(0.0, 0.0);

    // Setup distortion map
    ShadowCamDistortionMap *distMap = new ShadowCamDistortionMap(this);
    distMap->SetDistortion(naifIkCode());

    // Setup the ground and sky maps
    new LineScanCameraGroundMap(this);
    new LineScanCameraSkyMap(this);

    LoadCache();
    NaifStatus::CheckErrors();
  }
}

/**
 * Instantiate a KPLO ShadowCam Camera object.
 *
 * @param cube The cube for which to construct a KPLO ShadowCam Camera object.
 *
 * @return Isis::Camera* KPLO ShadowCam Camera
 *
 * @internal
 *    @history 2022-10-12 Victor Silva - original object
 */
extern "C" Isis::Camera *ShadowCamCameraPlugin(Isis::Cube &cube) {
  return new Isis::ShadowCamCamera(cube);
}
