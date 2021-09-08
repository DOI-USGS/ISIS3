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

void testCamera(Cube &c, double knownLat, double knownLon);
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
    double knownLat = -54.63487131147738;
    double knownLon = 40.43436155430055;
    Cube c("$hayabusa2/testData/hyb2_onc_20151204_041012_tbf_l2a.fit.cub", "r");
    testCamera(c, knownLat, knownLon);

    qDebug() << "";
    qDebug() << "----------------------------------------------";
    qDebug() << "Test for W1 Camera...";
    knownLat = -50.11857108654684;
    knownLon = 91.03535388676204;
    Cube w1("$hayabusa2/testData/hyb2_onc_20151204_045429_w1f_l2a.fit_crop.cub", "r");
    testCamera(w1, knownLat, knownLon);

    qDebug() << "";
    qDebug() << "----------------------------------------------";
    qDebug() << "Test for W2 Camera...";
    knownLat = 25.38911363842043;
    knownLon = 90.86547761107917;
    Cube w2("$hayabusa2/testData/hyb2_onc_20151203_072958_w2f_l2a.fit_crop.cub", "r");
    testCamera(w2, knownLat, knownLon);

  }
  catch(IException &e) {
    e.print();
  }

}


void testCamera(Cube &c, double knownLat, double knownLon) {
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
  testLineSamp(cam, 1.0, 1.0);

  qDebug() << "For upper right corner ...";
  testLineSamp(cam, cam->Samples(), 1.0);

  qDebug() << "For lower left corner ...";
  testLineSamp(cam, 1.0, cam->Lines());

  qDebug() << "For lower right corner ...";
  testLineSamp(cam, cam->Samples(), cam->Lines());

  qDebug() << "For center pixel position ...";

  if(!cam->SetImage((cam->Samples()/2.0), (cam->Lines()/2.0))) {
    throw IException(IException::Unknown, "ERROR setting image to known position.", _FILEINFO_);
  }

  if(abs(cam->UniversalLatitude() - knownLat) < 1E-10) {
    qDebug() << "Latitude OK";
  }
  else {
    qDebug() << "Latitude off by:  " << toString(cam->UniversalLatitude() - knownLat, 16);
  }

  if(abs(cam->UniversalLongitude() - knownLon) < 1E-10) {
    qDebug() << "Longitude OK";
  }
  else {
    qDebug() << "Longitude off by: " << toString(cam->UniversalLongitude() - knownLon, 16);
  }

  testLineSamp( cam, (cam->Samples()/2.0), (cam->Lines()/2.0) );
}


void testLineSamp(Camera *cam, double sample, double line) {
  bool success = cam->SetImage(sample, line);

  if(success) {
    success = cam->SetUniversalGround(cam->UniversalLatitude(), cam->UniversalLongitude());
  }

  if(success) {
    double deltaSamp = sample - cam->Sample();
    double deltaLine = line - cam->Line();
    if(fabs(deltaSamp) < 0.001) deltaSamp = 0;
    if(fabs(deltaLine) < 0.001) deltaLine = 0;
    qDebug() << "DeltaSample = " << deltaSamp;
    qDebug() << "DeltaLine   = " << deltaLine;
    qDebug() << "";
  }
  else {
    qDebug() << "DeltaSample = No Intersection";
    qDebug() << "DeltaLine   = No Intersection";
    qDebug() << "";
  }
}

