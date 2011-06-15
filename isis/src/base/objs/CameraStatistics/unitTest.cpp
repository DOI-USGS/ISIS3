#include <iomanip>

#include "Camera.h"
#include "CameraStatistics.h"
#include "Cube.h"
#include "iException.h"
#include "Pvl.h"
#include "Preference.h"
#include "Statistics.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    cout << "UnitTest for Camera Statistics" << endl;
    Isis::Cube cube;
    cube.Open("$Clementine1/testData/lna1391h.cub");
    Isis::Camera *cam = cube.Camera();
    cout << setprecision(9);

    Isis::CameraStatistics camStats(cam, 1, 1);
    Isis::Pvl test = camStats.toPvl();
    cout << test << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}
