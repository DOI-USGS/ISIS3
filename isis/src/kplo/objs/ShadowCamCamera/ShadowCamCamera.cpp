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
    constexpr int shadowCamIkCode = -155151;
    if (naifIkCode() != shadowCamIkCode) {
      QString msg = QString(
        "File does not appear to be a Korea Pathfinder Lunar Orbiter ShadowCam Image: %1 is not a supported instrument kernel code for Korea Pathfinder Lunar Orbiter."
      ).arg(QString::number(naifIkCode()));
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
    
    const Pvl &label = *cube.label();
    const PvlGroup &instrument = label.findGroup("Instrument", Pvl::Traverse);

    // ShadowCam linerate is in milliseconds, so convert it to seconds first
    const double lineRateSecs = ((ShadowCam::GetFromLabels(instrument, "LineRate")).toDouble() / 1000.0)
                              * (1.0 + multiplicativeLineError)
                              + additiveLineError;

    const double startingSample = (ShadowCam::GetFromLabels(instrument, "SampleFirstPixel")).toDouble() + 1.0;

    double tdiOffsetSecs = 0.0;
    if (!instrument.hasKeyword("TDIDirection")) {
      const QString msg = "Error: keyword TDIDirection was not found in label.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    if (QString::compare(instrument["TDIDirection"], "A", Qt::CaseInsensitive) == 0) {
      const QString tdiAOffsetKey = "INS" % toString(naifIkCode()) % "_TDI_A_OFFSET";
      tdiOffsetSecs = getDouble(tdiAOffsetKey) * lineRateSecs;
    }
    else if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) == 0) {
      const QString tdiBOffsetKey = "INS" % toString(naifIkCode()) % "_TDI_B_OFFSET";
      tdiOffsetSecs = getDouble(tdiBOffsetKey) * lineRateSecs;
    }
    else {
      const QString msg = QString("Error: TDIDirection value in label is invalid. Expected A or B but got: %1").arg(
        QString(instrument["TDIDirection"]));
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Start time offset is in seconds, so no conversion needed
    const double startTimeOffsetSecs = (ShadowCam::GetFromLabels(instrument, "StartTimeOffset")).toDouble();

    const QString executionSpacecraftTimeClockString = (ShadowCam::GetFromLabels(instrument, "ExecutionSpacecraftTime"));
    const iTime executionSpacecraftTime = getClockTime(executionSpacecraftTimeClockString, -155, false);
    const iTime startTime = executionSpacecraftTime + startTimeOffsetSecs + constantTimeOffset + tdiOffsetSecs;
    const double startTimeSecsEt = startTime.Et();

    setTime(startTime);

    // Setup detector map
    LineScanCameraDetectorMap *detectorMap = new LineScanCameraDetectorMap(this, startTimeSecsEt, lineRateSecs);
    detectorMap->SetStartingDetectorSample(startingSample);

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
