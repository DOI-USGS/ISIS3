/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>

#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "ClipperWacFcCamera.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

/**
 * Unit test for Clipper Wac Framing Camera
 *
 * @internal
 *   @history 2020-01-24 Kristin Berry - Original version. At the time this was written, many values
 *                       were preliminary or set to arbitrary numbers for testing reasons. These
 *                       will need to be updated in the future.
 */

// IMPORTANT NOTE: This test is believed to be failing because the test data has an arbitrary date
// for the StartTime, which means that the spice probably shows the spacecraft as not being near and
// pointed at Europa. If the spacecraft isn't near and pointed at Europa, there will be no intersection
// and SetImage will fail.
int main(void) {
  Preference::Preferences(true);

  qDebug() << "Unit Test for ClipperWacFcCamera...";
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output
    // "Latitude off by: " and "Longitude off by: " values directly into these variables.
    double knownLat = 0;
    double knownLon = 0;

    qDebug() << "Testing with test image...";
    Cube c("$ISISTESTDATA/isis/src/clipper/unitTestData/simulated_clipper_eis_wac_rolling_shutter.cub", "r");
    ClipperWacFcCamera *cam = (ClipperWacFcCamera *) CameraFactory::Create(c);
    qDebug() << "FileName: " << FileName(c.fileName()).name();
    qDebug() << "CK Frame: " << cam->instrumentRotation()->Frame();
    qDebug() << "";

    // Test kernel IDs
    qDebug() << "Kernel IDs: ";
    qDebug() << "CK Frame ID = " << cam->CkFrameId();
    qDebug() << "CK Reference ID = " << cam->CkReferenceId();
    qDebug() << "SPK Target ID = " << cam->SpkTargetId();
    qDebug() << "SPK Reference ID = " << cam->SpkReferenceId();
    qDebug() << "";

    qDebug() << qSetRealNumberPrecision(18) << "Focal Length = " << cam->FocalLength();
    qDebug() << "";

    // Test all four corners to make sure the conversions are right
    // The actual four corners are not on the body, so shifting a little
    qDebug() << "For upper left corner ...";
    TestLineSamp(cam, 145.0, 161.0);

    qDebug() << "For upper right corner ...";
    TestLineSamp(cam, 3655.0, 157.0);

    qDebug() << "For lower left corner ...";
    TestLineSamp(cam, 289, 1767);

    qDebug() << "For lower right corner ...";
    TestLineSamp(cam, 3767, 1579);

    double samp = cam->Samples() / 2;
    double line = cam->Lines() / 2;
    qDebug() << "For center pixel position ...";

    if(!cam->SetImage(samp, line)) {
      qDebug() << "ERROR";
      return 0;
    }

    if(abs(cam->UniversalLatitude() - knownLat) < 1E-13) {
      qDebug() << "Latitude OK";
    }
    else {
      qDebug() << qSetRealNumberPrecision(18)
               << "Latitude off by: " << cam->UniversalLatitude() - knownLat;
    }

    if(abs(cam->UniversalLongitude() - knownLon) < 1E-11) {
      qDebug() << "Longitude OK";
    }
    else {
      qDebug() << qSetRealNumberPrecision(18)
               << "Longitude off by: " << cam->UniversalLongitude() - knownLon;
    }
  }
  catch(IException &e) {
    e.print();
  }
}

void TestLineSamp(Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if(success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();
    if(fabs(deltaSamp) < 1.1e-2) deltaSamp = 0.0;
    if(fabs(deltaLine) < 1.0e-2) deltaLine = 0.0;
    qDebug() << "DeltaSample = " << deltaSamp;
    qDebug() << "DeltaLine = " << deltaLine;
    qDebug() << "";
  }
  else {
    qDebug() << "DeltaSample = ERROR";
    qDebug() << "DeltaLine = ERROR";
    qDebug() << "";
  }
}
