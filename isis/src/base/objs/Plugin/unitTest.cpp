#include <iostream>
#include <sstream>
#include "Plugin.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  std::cout << "This unit test is not currently capable of testing Plugin"
            << std::endl;
}
