#include <iomanip>

#include "Camera.h"
#include "CameraStatistics.h"
#include "Cube.h"
#include "IException.h"
#include "Pvl.h"
#include "Preference.h"
#include "Statistics.h"

using namespace std;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    cout << "UnitTest for Camera Statistics" << endl;
    Isis::Cube cube;
    cube.open("$Clementine1/testData/lna1391h.cub");
    Isis::Camera *cam = cube.getCamera();
    cout << setprecision(6);

    Isis::CameraStatistics camStats(cam, 1, 1);
    Isis::Pvl statsPvl = camStats.toPvl();
    cout << endl;

    for (int i = 0; i < statsPvl.Groups(); i++) {
      Isis::PvlGroup &group = statsPvl.Group(i);
      cout << group.Name() << ":" << endl;

      for (int j = 0; j < group.Keywords(); j++) {
        Isis::PvlKeyword &keyword = group[j];
        cout << "  " << keyword.Name() << " = " << (double) keyword[0] << endl;
      }

      cout << endl;
    }
  }
  catch(Isis::IException &e) {
    e.print();
  }
}
