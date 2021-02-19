/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
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
    Cube cube("$ISISTESTDATA/isis/src/base/unitTestData/simulated_clipper_eis_nac_rolling_shutter.cub", "r");
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
