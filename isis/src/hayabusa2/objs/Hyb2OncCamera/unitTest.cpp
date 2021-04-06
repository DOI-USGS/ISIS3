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

#include "Hyb2OncCamera.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

void testCamera(Cube &c, double knownLat, double knownLon,
                double s1, double l1, 
                double s2, double l2, 
                double s3, double l3, 
                double s4, double l4);
void testLineSamp(Camera *cam, double sample, double line);


/**
 * Unit Test for the Hayabusa2 ONC camera.
 *
 * @author 2017-07-11 Kristin Berry
 *
 * @internal
 *   @history 2017-07-11 Kristin Berry
 */
int main(void) {
  Preference::Preferences(true);

  qDebug() << "Unit Test for Hyb2OncCamera...";
  try {
    // These should be lat/lon at center of image. To obtain these numbers for a new cube/camera,
    // set both the known lat and known lon to zero and copy the unit test output "Latitude off by: "
    // and "Longitude off by: " values directly into these variables.
    qDebug() << "";
    qDebug() << "----------------------------------------------";
    qDebug() << "Test for Telecopic Camera...";
    double knownLat = -47.23506367622795;
    double knownLon = 45.06713880290044;
    Cube c("$hayabusa2/testData/hyb2_onc_20151204_041012_tbf_l2a.fit.cub", "r");
    testCamera(c, knownLat, knownLon, 
               512.5, 512.5, 602.0, 334.0, 378.0, 557.0, 594.0, 580.0);
//    362.0, 352.0, 602.0, 334.0, 378.0, 557.0, 594.0, 580.0);

    qDebug() << "";
    qDebug() << "----------------------------------------------";
    qDebug() << "Test for W1 Camera...";
    knownLat = -50.66777190122887;
    knownLon = 97.68522302461859;
    Cube w1("$hayabusa2/testData/hyb2_onc_20151204_045429_w1f_l2a.fit_crop.cub", "r");
    testCamera(w1, knownLat, knownLon,
               21.0, 20.0, 31.0, 11.0, 16.0, 29.0, 32.0, 28.0);
//    16.0, 14.0, 31.0, 11.0, 16.0, 29.0, 32.0, 28.0);

    qDebug() << "";
    qDebug() << "----------------------------------------------";
    qDebug() << "Test for W2 Camera...";
//    knownLat = 4.676892803564044;
//    knownLon = -12.46121470106279;
    knownLat = 30.06610722049293;
    knownLon = 78.40416492096558;
    Cube w2("$hayabusa2/testData/hyb2_onc_20151203_072958_w2f_l2a.fit_crop.cub", "r");
    testCamera(w2, knownLat, knownLon,
               51.0, 42.0, 173.0, 21.0, 54.0, 149.0, 174.0, 155.0);

  }
  catch (IException &e) {
    e.print();
  }

}


void testCamera(Cube &c, double knownLat, double knownLon,
                double s1, double l1, 
                double s2, double l2, 
                double s3, double l3, 
                double s4, double l4) {
  Hyb2OncCamera *cam = (Hyb2OncCamera *) CameraFactory::Create(c);
  qDebug() << "FileName: " << FileName(c.fileName()).name();
  qDebug() << "CK Frame: " << cam->instrumentRotation()->Frame();
  qDebug() << "";

  // Test kernel IDs
  qDebug() << "Kernel IDs: ";
  qDebug() << "CK Frame ID      = " << cam->CkFrameId();
  qDebug() << "CK Reference ID  = " << cam->CkReferenceId();
  qDebug() << "SPK Target ID    = " << cam->SpkTargetId();
  qDebug() << "SPK Reference ID = " << cam->SpkReferenceId();
  qDebug() << "";
  
  // Test name methods
  qDebug() << "Spacecraft Name Long:  " << cam->spacecraftNameLong();
  qDebug() << "Spacecraft Name Short: " << cam->spacecraftNameShort();
  qDebug() << "Instrument Name Long:  " << cam->instrumentNameLong();
  qDebug() << "Instrument Name Short: " << cam->instrumentNameShort();
  qDebug() << "";

  // Test Shutter Open/Close 
  const PvlGroup &inst = c.label()->findGroup("Instrument", Pvl::Traverse);
  double exposureDuration = ((double) inst["ExposureDuration"])/1000;
  QString stime = inst["StartTime"];
  double et; // StartTime keyword is the center exposure time
  str2et_c(stime.toLatin1().data(), &et);
  pair <iTime, iTime> shuttertimes = cam->ShutterOpenCloseTimes(et, exposureDuration);
  qDebug() << "Shutter open  = " << toString(shuttertimes.first.Et(), 16);
  qDebug() << "Shutter close = " << toString(shuttertimes.second.Et(), 16);
  qDebug() << "";

  // Test all four corners to make sure the conversions are right
  qDebug() << "For upper left corner ...";
  testLineSamp(cam, s1, l1);

  qDebug() << "For upper right corner ...";
  testLineSamp(cam, s2, l2);

  qDebug() << "For lower left corner ...";
  testLineSamp(cam, s3, l3);

  qDebug() << "For lower right corner ...";
  testLineSamp(cam, s4, l4);

  qDebug() << "For center pixel position ...";

  if (!cam->SetImage((cam->Samples()/2.0), (cam->Lines()/2.0))) {
    throw IException(IException::Unknown, "ERROR setting image to known position.", _FILEINFO_);
  }

  if (abs(cam->UniversalLatitude() - knownLat) < 1E-10) {
    qDebug() << "Latitude OK";
  }
  else {
    qDebug() << "Latitude off by:  " << toString(cam->UniversalLatitude() - knownLat, 16);
  }

  if (abs(cam->UniversalLongitude() - knownLon) < 1E-10) {
    qDebug() << "Longitude OK";
  }
  else {
    qDebug() << "Longitude off by: " << toString(cam->UniversalLongitude() - knownLon, 16);
  }

  testLineSamp( cam, (cam->Samples()/2.0), (cam->Lines()/2.0) );
}


void testLineSamp(Camera *cam, double sample, double line) {
  bool success = cam->SetImage(sample, line);

  if (success) {
    double lat = cam->UniversalLatitude();
    double lon = cam->UniversalLongitude();
    success = cam->SetUniversalGround(lat, lon);
//    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if (success) {
    double deltaSamp = sample - cam->Sample();
    double deltaLine = line - cam->Line();
    if(fabs(deltaSamp) < 0.001) {
      qDebug() << "Delta Sample less than 0.001";
    }
    else {
      qDebug() << "Delta Sample larger than expected " << deltaSamp;
    }
    if(fabs(deltaLine) < 0.001) {
      qDebug() << "Delta Line less than 0.001";
    }
    else {
      qDebug() << "Delta Line larger than expected " << deltaLine;
    }
    qDebug() << "";
  }
  else {
    qDebug() << "DeltaSample = No Intersection";
    qDebug() << "DeltaLine   = No Intersection";
    qDebug() << "";
  }
}

