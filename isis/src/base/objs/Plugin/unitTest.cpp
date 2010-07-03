#include <iostream>
#include <sstream>
#include "Plugin.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
  stringstream temp;
  temp << "Group = Plugin" << endl;
  temp << "  Library = isis3" << endl;
  temp << "  Routine = PluginPlugin" << endl;
  temp << "EndGroup" << endl;

  Isis::Plugin p;
  temp >> p;
  void *ptr = p.GetPlugin("Plugin");

  int * (*module)();
  module = (int * (*)()) ptr;
  cout << *(*module)() << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}
