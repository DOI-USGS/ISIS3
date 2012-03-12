#include "NaifStatus.h"
#include "naif/SpiceUsr.h"
#include "Preference.h"
#include "IException.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "Unit Test for NaifStatus" << std::endl;

  std::cout << "No Errors" << std::endl;
  NaifStatus::CheckErrors();

  std::cout << std::endl << "Empty String Error" << std::endl;
  try {
    SpiceChar *tmp = new SpiceChar[128];
    tmp[0] = '\0';
    erract_c("SET", (SpiceInt)0, tmp);
    NaifStatus::CheckErrors();
  }
  catch(IException &e) {
    e.print();
  }

}
