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
    Cube c("$clipper/testData/simulated_clipper_eis_wac_rolling_shutter.cub", "r");
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
