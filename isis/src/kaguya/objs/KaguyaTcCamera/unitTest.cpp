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
#include "KaguyaTcCamera.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void TestLineSamp(Camera *cam, double samp, double line);

/**
 * Unit test for Kaguya TC Camera.
 *
 * @internal
 *   @history 2018-10-05 Jeannie Backer and Adam Goins - Original version.
 */
int main(void) {
  Preference::Preferences(true);

  qDebug() << "Unit Test for KaguyaTcCamera...";
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output
    // "Latitude off by: " and "Longitude off by: " values directly into these variables.
    double knownLat = 58.3524398749999;
    double knownLon = 311.457363494321;

    qDebug() << "Testing TC2 w L2B0 image...";
    Cube c("$kaguya/testData/TC2W2B0_01_02735N583E3115.cub", "r");
    KaguyaTcCamera *cam = (KaguyaTcCamera *) CameraFactory::Create(c);
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
    qDebug() << "For upper left corner ...";
    TestLineSamp(cam, 1.0, 1.0);

    qDebug() << "For upper right corner ...";
    TestLineSamp(cam, cam->Samples(), 1.0);

    qDebug() << "For lower left corner ...";
    TestLineSamp(cam, 1.0, cam->Lines());

    qDebug() << "For lower right corner ...";
    TestLineSamp(cam, cam->Samples(), cam->Lines());

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


    qDebug() << "";
    qDebug() << "";
    qDebug() << "Testing TC1 s L2B0 image...";
    knownLat = -82.0195024182112;
    knownLon = 46.4153943800818;

    Cube c2("$kaguya/testData/TC1S2B0_01_06691S820E0465.cub", "r");
    KaguyaTcCamera *cam2 = (KaguyaTcCamera *) CameraFactory::Create(c2);
    qDebug() << "FileName: " << FileName(c2.fileName()).name();
    qDebug() << "CK Frame: " << cam2->instrumentRotation()->Frame();
    qDebug() << "";

    // Test kernel IDs
    qDebug() << "Kernel IDs: ";
    qDebug() << "CK Frame ID = " << cam2->CkFrameId();
    qDebug() << "CK Reference ID = " << cam2->CkReferenceId();
    qDebug() << "SPK Target ID = " << cam2->SpkTargetId();
    qDebug() << "SPK Reference ID = " << cam2->SpkReferenceId();
    qDebug() << "";

    qDebug() << qSetRealNumberPrecision(18) << "Focal Length = " << cam2->FocalLength();
    qDebug() << "";

    // Test all four corners to make sure the conversions are right
    qDebug() << "For upper left corner ...";
    TestLineSamp(cam2, 1.0, 1.0);

    qDebug() << "For upper right corner ...";
    TestLineSamp(cam2, cam2->Samples(), 1.0);

    qDebug() << "For lower left corner ...";
    TestLineSamp(cam2, 1.0, cam2->Lines());

    qDebug() << "For lower right corner ...";
    TestLineSamp(cam2, cam2->Samples(), cam2->Lines());

    samp = cam2->Samples() / 2;
    line = cam2->Lines() / 2;
    qDebug() << "For center pixel position ...";

    if(!cam2->SetImage(samp, line)) {
      qDebug() << "ERROR";
      return 0;
    }

    if(abs(cam2->UniversalLatitude() - knownLat) < 1E-13) {
      qDebug() << "Latitude OK";
    }
    else {
      qDebug() << qSetRealNumberPrecision(18)
               << "Latitude off by: " << cam2->UniversalLatitude() - knownLat;
    }

    if(abs(cam2->UniversalLongitude() - knownLon) < 1E-11) {
      qDebug() << "Longitude OK";
    }
    else {
      qDebug() << qSetRealNumberPrecision(18)
               << "Longitude off by: " << cam2->UniversalLongitude() - knownLon;
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
