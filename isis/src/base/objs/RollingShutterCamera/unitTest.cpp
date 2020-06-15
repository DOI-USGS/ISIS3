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

#include "RollingShutterCamera.h"
#include "IException.h"
#include "iTime.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"

using namespace std;
using namespace Isis;

class TestRollingShutterCamera : public RollingShutterCamera {
      public:
        TestRollingShutterCamera(Cube &cube) : RollingShutterCamera(cube) {}

        /**
         * This is a pure virtual method from the Camera parent class and must be
         * overriden.
         */
        virtual int CkFrameId() const {
          string msg = "CK Frame ID is unqiue to mission-specific cameras";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        /**
         * This is a pure virtual method from the Camera parent class and must be
         * overriden.
         */
        virtual int CkReferenceId() const {
          string msg = "CK Reference ID is unique to mission-specific cameras";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        /**
         * This is a pure virtual method from the Camera parent class and must be
         * overriden.
         */
        virtual int SpkReferenceId() const {
          string msg = "SPK Reference ID is unique to mission-specific cameras";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
        
        // These are pure virtual within Sensor that must be overriden
        virtual QString instrumentNameLong() const { return QString("RollingShutting"); }
        virtual QString instrumentNameShort() const { return QString("RS"); }
        virtual QString spacecraftNameLong() const { return QString("RollingShutterSpaceCraft"); }
        virtual QString spacecraftNameShort() const { return QString("SC"); }
    };


int main() {
  Preference::Preferences(true);
  //NOTE: The following cube is not from a framing camera.  The test returns
  //true for framing camera type since MyCamera is a child class of FramingCamera
  try {
    Cube cube("$clipper/testData/simulated_clipper_eis_nac_rolling_shutter.cub", "r");
    TestRollingShutterCamera cam(cube);

    // test camera type
    cout << "Camera = Framing?         " << (cam.GetCameraType() == Camera::Framing) << endl;
    cout << "Camera = LineScan?        " << (cam.GetCameraType() == Camera::LineScan) << endl;
    cout << "Camera = PushFrame?       " << (cam.GetCameraType() == Camera::PushFrame) << endl;
    cout << "Camera = Radar?           " << (cam.GetCameraType() == Camera::Radar) << endl;
    cout << "Camera = RollingShutter?  " << (cam.GetCameraType() == Camera::RollingShutter) << endl;
  }
  catch (IException &e) {
    cout << endl << endl;
    IException(e, IException::Programmer,
              "\n------------RollingShutterCamera Unit Test Failed.------------",
              _FILEINFO_).print();
  }
}
