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
#include <iostream>

#include "FramingCamera.h"
#include "iException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

class MyCamera : public FramingCamera {
  public:
    MyCamera(Pvl &lab) : FramingCamera(lab) { }

    /**
     * This is a pure virtual method from the Camera parent class and must be 
     * overriden. 
     */
    virtual int CkFrameId() const {
      string msg = "CK Frame ID is unqiue to mission-specific cameras";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }

    /**
     * This is a pure virtual method from the Camera parent class and must be 
     * overriden. 
     */
    virtual int CkReferenceId() const {
      string msg = "CK Reference ID is unique to mission-specific cameras";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }

    /**
     * This is a pure virtual method from the Camera parent class and must be 
     * overriden. 
     */
    virtual int SpkReferenceId() const {
      string msg = "SPK Reference ID is unique to mission-specific cameras";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }

    /**
     * This is a pure virtual method from the FramingCamera class and must be 
     * overriden. We will use the default implementation from the FramingCamera 
     * class. 
     * @author Jeannie Walldren
     * @internal
     *   @history 2011-05-03 Jeannie Walldren - Original version.
     */
    pair <iTime, iTime> ShutterOpenCloseTimes(double exposureDuration,
                                              double time) {
      return FramingCamera::ShutterOpenCloseTimes(exposureDuration, time);
    }
};

int main() {
  Preference::Preferences(true);
  //NOTE: The following cube is not from a framing camera.  The test returns 
  //true for framing camera type since MyCamera is a child class of FramingCamera
  try {
    string inputFile = "$base/testData/ab102401_ideal.cub";
    Pvl pvl(inputFile);
    MyCamera cam(pvl);
  
    // test camera type
    cout << "Camera = Framing?   " << (cam.GetCameraType() == Camera::Framing) << endl;
    cout << "Camera = LineScan?  " << (cam.GetCameraType() == Camera::LineScan) << endl;
    cout << "Camera = PushFrame? " << (cam.GetCameraType() == Camera::PushFrame) << endl;
    cout << "Camera = Radar?     " << (cam.GetCameraType() == Camera::Radar) << endl;
  
    // test ShutterOpenCloseTimes() method
    PvlGroup inst = pvl.FindGroup ("Instrument",Pvl::Traverse);
    string startTime = inst["StartTime"];
    double eTime = 0.0;
    utc2et_c(startTime.c_str(), &eTime);
    double expoDur = ((double) inst["ExposureDuration"])/1000; // in seconds       
    pair <iTime, iTime> octime = cam.ShutterOpenCloseTimes(eTime,expoDur);
    cout.precision(12);
    cout << "StartTime                 =      " << startTime << endl;
    cout << "StartTime (et)            =      " << eTime << endl;
    cout << "ExposureDuration (in sec) =      " << expoDur << endl;
    cout << "shutter open              =      " << octime.first.Et() << endl;
    cout << "shutter close             =      " << octime.second.Et() << endl;
  }
  catch (iException e) {
    cout << endl << endl;
    e.Message(iException::Programmer,
              "\n------------FramingCamera Unit Test Failed.------------",
              _FILEINFO_);
    e.Report();
  }
}
