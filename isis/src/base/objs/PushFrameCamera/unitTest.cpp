#include "PushFrameCamera.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

class MyCamera : public PushFrameCamera {
  public:
    MyCamera(Isis::Pvl &lab) : Isis::PushFrameCamera(lab) { }

    virtual int CkFrameId() const {
      string msg = "CK Frame ID is unqiue to mission-specific cameras";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }

    virtual int CkReferenceId() const {
      string msg = "CK Reference ID is unique to mission-specific cameras";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }

    virtual int SpkReferenceId() const {
      string msg = "SPK Reference ID is unique to mission-specific cameras";
      throw iException::Message(iException::Camera, msg, _FILEINFO_);
    }
};

int main() {
  Isis::Preference::Preferences(true);
  string inputFile = "$mgs/testData/ab102401.lev2.cub";
  Pvl pvl(inputFile);
  MyCamera cam(pvl);

  cout << "Camera = Framing?   " << (cam.GetCameraType() == Camera::Framing) << std::endl;
  cout << "Camera = LineScan?  " << (cam.GetCameraType() == Camera::LineScan) << std::endl;
  cout << "Camera = PushFrame? " << (cam.GetCameraType() == Camera::PushFrame) << std::endl;
  cout << "Camera = Radar?     " << (cam.GetCameraType() == Camera::Radar) << std::endl;
}
