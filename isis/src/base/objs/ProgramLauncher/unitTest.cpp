#include "Isis.h"

#include <iostream>

#include "IException.h"
#include "IString.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "QRegularExpression"

using namespace Isis;
using namespace std;

void IsisMain() {
  Preference::Preferences(true);

  cerr << "Testing ProgramLauncher Class ... " << endl;
  cerr << endl;

  cerr << "Testing ls, grep, sed and pipes ... " << endl;
  cerr << endl;
  ProgramLauncher::RunSystemCommand("ls -l * | grep -e 'ProgramLauncher\\.' | "
      "sed 's/\\(.*\\)\\(ProgramLauncher\\..*\\)/\\2/'");

  cerr << "Testing stats ... " << endl;
  cerr << endl;
  ProgramLauncher::RunSystemCommand("greyscale to=unitTest.cub enddn=50.0 samples=50 lines=50 "
        "-preference=$ISISROOT/TestPreferences");

  ProgramLauncher::RunIsisProgram("stats",
        "from=unitTest.cub "
        "-preference=$ISISROOT/TestPreferences");

  cerr << endl;
  cerr << "Testing non-existant Isis program... " << endl;
  cerr << endl;
  try {
    ProgramLauncher::RunIsisProgram("chocolatelab",
                                    "from=$ISISTESTDATA/isis/src/base/unitTestData/ab102401_ideal.cub");
  }
  catch(IException &e) {
    e.print();
  }


  cerr << endl;
  cerr << "Testing using Isis program as a system program without pid... "
       << endl;
  cerr << endl;
  try {
    ProgramLauncher::RunSystemCommand("$ISISROOT/bin/stats "
        "from=unitTest.cub "
        "-preference=\\$ISISROOT/TestPreferences");
  }
  catch(IException &e) {
    e.print();
  }
}
