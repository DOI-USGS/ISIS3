#include "Application.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void interface();

int main(int argc, char *argv[]) {
  int i_argc(argc - 1);
  Application app(i_argc, &argv[1]);
  return app.Run(interface);
}

void interface() {
  PvlObject hist = Isis::iApp->History();
  PvlGroup up = hist.findGroup("UserParameters");
  Pvl pvl;
  pvl.addGroup(up);
  cout << pvl << endl;
}
