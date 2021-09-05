#include <SpiceUsr.h>

#include "IException.h"
#include "NaifStatus.h"
#include "NaifContext.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);
  NaifContext naif;

  std::cout << "Unit Test for NaifStatus" << std::endl;

  std::cout << "No Errors" << std::endl;
  NaifStatus::CheckErrors(&naif);

  std::cout << std::endl << "Empty String Error" << std::endl;
  try {
    SpiceChar *tmp = new SpiceChar[128];
    tmp[0] = '\0';
    erract_c(naif.get(), "SET", (SpiceInt)0, tmp);
    NaifStatus::CheckErrors(&naif);
  }
  catch(IException &e) {
    e.print();
  }

}
