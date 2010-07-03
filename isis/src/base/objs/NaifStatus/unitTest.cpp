#include "NaifStatus.h"
#include "naif/SpiceUsr.h"
#include "Preference.h"
#include "iException.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "Unit Test for NaifStatus" << std::endl;

  std::cout << "No Errors" << std::endl;
  NaifStatus::CheckErrors();

  std::cout << std::endl << "Empty String Error" << std::endl;
  try {
    erract_c("SET", (SpiceInt)0, "");
    NaifStatus::CheckErrors();
  }
  catch (iException &e) {
    e.Report(false);
  }

}
