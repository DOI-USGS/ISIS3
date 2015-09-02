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
#include "Preference.h"
#include "Pvl.h"
#include "ThemisVisCamera.h"

using namespace std;
using namespace Isis;

void TestSampLine(Camera *cam, double samp, double line);

int main(void) {
  Preference::Preferences(true);

  qDebug() << "Unit Test for ThemisVisCamera...";
  qDebug();
  /**
   * unit test changed for themis vis camera
   */
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    //double knownCenterLat =  48.53477763371114; 
    //double knownCenterLon = 332.0435632412456;
    // double knownCenterLat = 48.5338707313522;
    // double knownCenterLon = 332.0434591311045;
    // 2014-06-10 Jeannie Backer - Improved distortion model.  New lat/lon
    double knownCenterLat =  48.515167642355;
    double knownCenterLon = 332.03690329802;

    Cube evenCube("$odyssey/testData/V14093008RDR.even.cub", "r");
//    Cube evenCube("./V14093008RDR.even.cub", "r");
    Camera *evenCam = CameraFactory::Create(evenCube);
    qDebug() << "FileName: " << FileName(evenCube.fileName()).name();
    qDebug() << "CK Frame: " << evenCam->instrumentRotation()->Frame();
    qDebug();

    // Test kernel IDs
    qDebug() << "Kernel IDs: ";
    qDebug() << "CK Frame ID      = " << evenCam->CkFrameId();
    qDebug() << "CK Reference ID  = " << evenCam->CkReferenceId();
    qDebug() << "SPK Target ID    = " << evenCam->SpkTargetId();
    qDebug() << "SPK Reference ID = " << evenCam->SpkReferenceId();
    qDebug();
    qDebug() << "Is Band Independent: " << evenCam->IsBandIndependent();
    qDebug();

    // Test all four corners to make sure the conversions are right
    qDebug() << "For upper left corner ...";
    TestSampLine(evenCam, 1.0, 192.0 / 2 + 1.5);  // omit framelet 1 for even

    qDebug() << "For upper right corner ...";
    TestSampLine(evenCam, evenCam->Samples(), 192.0 / 2 + 1.5); // omit framelet 1 for even

    qDebug() << "For lower left corner ...";
    TestSampLine(evenCam, 1.0, evenCam->Lines());

    qDebug() << "For lower right corner ...";
    TestSampLine(evenCam, evenCam->Samples(), evenCam->Lines());
    
    qDebug() << "For center framelet 14 pixel position ...";
    double samp = evenCam->Samples() / 2;
    double line = evenCam->Lines() / 2 + 192.0 /2.0 / 2.0; // add half of summed framelet to get
                                                           // center of framelet 14

    if (!evenCam->SetImage(samp, line)) {
      qDebug() << "ERROR";
      return 0;
    }

    if (abs(evenCam->UniversalLatitude() - knownCenterLat) < 1e-10) {
      qDebug() << "Latitude:     OK";
    }
    else {
      qDebug() << "Latitude:     off by " 
               << QString::number(evenCam->UniversalLatitude() - knownCenterLat,  'f',  16);
    }

    if (abs(evenCam->UniversalLongitude() - knownCenterLon) < 2e-10) {
      qDebug() << "Longitude:    OK";
    }
    else {
      qDebug() << "Longitude:    off by " 
               << QString::number(evenCam->UniversalLongitude() - knownCenterLon,  'f',  16);
    }
    TestSampLine(evenCam, samp, line);

    qDebug();
    qDebug();
    qDebug();
    Cube oddCube("$odyssey/testData/V14093008RDR.odd.cub", "r");
    Camera *oddCam = CameraFactory::Create(oddCube);
    qDebug()<< "FileName: " << FileName(oddCube.fileName()).name();
    qDebug()<< "CK Frame: " << oddCam->instrumentRotation()->Frame();
    qDebug();

    // Test kernel IDs
    qDebug()<< "Kernel IDs: ";
    qDebug()<< "CK Frame ID      = " << oddCam->CkFrameId();
    qDebug()<< "CK Reference ID  = " << oddCam->CkReferenceId();
    qDebug()<< "SPK Target ID    = " << oddCam->SpkTargetId();
    qDebug()<< "SPK Reference ID = " << oddCam->SpkReferenceId();
    qDebug();

    // Test all four corners to make sure the conversions are right
    qDebug()<< "For upper left corner ...";
    TestSampLine(oddCam, 1.0, 1.0);

    qDebug()<< "For upper right corner ...";
    TestSampLine(oddCam, oddCam->Samples(), 1.0);

    qDebug()<< "For lower left corner ...";
    TestSampLine(oddCam, 1.0, oddCam->Lines() - 96.0); // omit framelet 26 for odd

    qDebug()<< "For lower right corner ...";
    TestSampLine(oddCam, oddCam->Samples(), oddCam->Lines() - 96.0); // omit framelet 26 for odd
    
    qDebug() << "For center framelet 13 pixel position ...";
    knownCenterLat =  48.563958771636;
    knownCenterLon = 332.04676929446;
    samp = oddCam->Samples()/ 2;
    line = oddCam->Lines() / 2 - 192.0 / 2.0 / 2.0;  // subtract half of summed framelet to
                                                     // get center of framelet 13
    if (!oddCam->SetImage(samp, line)) {
      qDebug() << "ERROR";
      return 0;
    }

    if (abs(oddCam->UniversalLatitude() - knownCenterLat) < 1e-10) {
      qDebug() << "Latitude:     OK";
    }
    else {
      qDebug() << "Latitude:     off by "
          << QString::number(oddCam->UniversalLatitude() - knownCenterLat,  'f',  16);
    }

    if (abs(oddCam->UniversalLongitude() - knownCenterLon) < 2e-10) {
      qDebug() << "Longitude:    OK";
    }
    else {
      qDebug() << "Longitude:    off by "
          << QString::number(oddCam->UniversalLongitude() - knownCenterLon,  'f',  16);
    }
    TestSampLine(oddCam, samp, line);

    qDebug();
    
    // Test name methods
    qDebug() << "Testing name methods ...";
    qDebug() << "Spacecraft Name Long: " << evenCam->spacecraftNameLong();
    qDebug() << "Spacecraft Name Short: " << evenCam->spacecraftNameShort();
    qDebug() << "Instrument Name Long: " << evenCam->instrumentNameLong();
    qDebug() << "Instrument Name Short: " << evenCam->instrumentNameShort();
    qDebug();
    qDebug();
    
    qDebug() << "Testing errors";
    try {
      Cube irCube("$odyssey/testData/I00831002RDR.cub", "r");
      ThemisVisCamera irImage(irCube);
    } 
    catch (IException &e) {
      e.print();
    }

  }
  catch(IException &e) {
    e.print();
  }
}

void TestSampLine(Camera *cam, double samp, double line) {
  bool success = cam->SetImage(samp, line);

  if (success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if (success) {
    double deltaSamp = samp - cam->Sample();
    double deltaLine = line - cam->Line();

    if (fabs(deltaSamp) < 0.0001) {
      qDebug() << "DeltaSample:  OK";
    }
    else {
      qDebug() << "DeltaSample = " << QString::number(deltaSamp,  'f',  16);
    }

    if (fabs(deltaLine) < 0.0001) {
      qDebug() << "DeltaLine:    OK";
    }
    else {
      qDebug() << "DeltaLine   = " << QString::number(deltaLine,  'f',  16);
    }

  }
  else {
    qDebug() << "DeltaSample = ERROR";
    qDebug() << "DeltaLine   = ERROR";
  }

  qDebug();
}

