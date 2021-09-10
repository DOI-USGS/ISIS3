#include "NaifContext.h"

#include "IException.h"
#include "NaifContext.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);
  NaifContext naif;

  std::cout << "Unit Test for NaifStatus" << std::endl;

  std::cout << "No Errors" << std::endl;
  naif.CheckErrors();

  std::cout << std::endl << "Empty String Error" << std::endl;
  try {
    SpiceChar *tmp = new SpiceChar[128];
    tmp[0] = '\0';
    naif.erract_c("SET", (SpiceInt)0, tmp);
    naif.CheckErrors();
  }
  catch(IException &e) {
    e.print();
  }

}
