#include "Application.h"
#include "UserInterface.h"

using namespace std; 
using namespace Isis;

void interface ();

int main (int argc, char *argv[]) {
  int i_argc(argc-1);
  Application app(i_argc,&argv[1]);
  return app.Exec(interface);
}

void interface () {
  PvlObject hist = Isis::iApp->History();
  PvlGroup up = hist.FindGroup("UserParameters");
  Pvl pvl;
  pvl.AddGroup(up);
  cout << pvl << endl;
}
