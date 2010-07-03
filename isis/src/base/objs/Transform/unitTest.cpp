#include <iostream>
#include "Preference.h"

using namespace std;
int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);

  cout << "This class currently contains only pure virtual functions," <<
          "so this is a dummy unit test." << endl;
}
