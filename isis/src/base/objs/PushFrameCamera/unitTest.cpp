/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "IException.h"
#include "FileName.h"
#include "PushFrameCamera.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

class MyCamera : public PushFrameCamera {
  public:
    MyCamera(Cube &cube) : PushFrameCamera(cube) { }

    virtual int CkFrameId() const {
      string msg = "CK Frame ID is unqiue to mission-specific cameras";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    virtual int CkReferenceId() const {
      string msg = "CK Reference ID is unique to mission-specific cameras";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    virtual int SpkReferenceId() const {
      string msg = "SPK Reference ID is unique to mission-specific cameras";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    // These are pure virtual within Sensor that must be overriden
    virtual QString instrumentNameLong() const { return QString("Push Frame"); }
    virtual QString instrumentNameShort() const { return QString("PF"); }
    virtual QString spacecraftNameLong() const { return QString("Push Frame 1"); }
    virtual QString spacecraftNameShort() const { return QString("PF1"); }
};

int main() {
  Preference::Preferences(true);
  Cube cube("$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.lev2.cub", "r");
  MyCamera cam(cube);

  cout << "Camera = Framing?   " << (cam.GetCameraType() == Camera::Framing) << endl;
  cout << "Camera = LineScan?  " << (cam.GetCameraType() == Camera::LineScan) << endl;
  cout << "Camera = PushFrame? " << (cam.GetCameraType() == Camera::PushFrame) << endl;
  cout << "Camera = Radar?     " << (cam.GetCameraType() == Camera::Radar) << endl;
}
