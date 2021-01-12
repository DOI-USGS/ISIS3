#include <iomanip>

#include "Camera.h"
#include "CameraStatistics.h"
#include "Cube.h"
#include "IException.h"
#include "Pvl.h"
#include "Preference.h"
#include "Statistics.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  try {
    cout << "UnitTest for Camera Statistics" << endl;
    Cube cube;
    cube.open("$Clementine1/testData/lna1391h.cub");
    Camera *cam = cube.camera();
    cout << setprecision(6);

    CameraStatistics camStats(cam, 1, 1);
    Pvl statsPvl = camStats.toPvl();
    cout << endl;

    for (int i = 0; i < statsPvl.groups(); i++) {
      PvlGroup &group = statsPvl.group(i);
      cout << group.name() << ":" << endl;

      for (int j = 0; j < group.keywords(); j++) {
        PvlKeyword &keyword = group[j];
        cout << "  " << keyword.name() << " = " << toDouble(keyword[0]) << endl;
      }

      cout << endl;
    }
  }
  catch(IException &e) {
    e.print();
  }
}
