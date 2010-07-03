#include "LineScanCamera.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

class MyCamera : public LineScanCamera {
  public:
    MyCamera(Isis::Pvl& lab) : Isis::LineScanCamera(lab) { }
};

int main() {
  Isis::Preference::Preferences(true);
  string inputFile = "$ISIS3DATA/mgs/testData/ab102401.lev2.cub";
  Pvl pvl(inputFile);
  MyCamera cam(pvl);

  cout << "Camera = Framing?   " << (cam.GetCameraType() == Camera::Framing) << std::endl;
  cout << "Camera = LineScan?  " << (cam.GetCameraType() == Camera::LineScan) << std::endl;
  cout << "Camera = PushFrame? " << (cam.GetCameraType() == Camera::PushFrame) << std::endl;
  cout << "Camera = Radar?     " << (cam.GetCameraType() == Camera::Radar) << std::endl;
}
