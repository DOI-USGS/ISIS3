#include <iomanip>

#include "Equalization.h"
#include "iException.h"
#include "OverlapNormalization.h"
#include "OverlapStatistics.h"
#include "PvlGroup.h"
#include "Preference.h"

using namespace std;

void PrintResults(string, const unsigned, Isis::OverlapNormalization &);

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    cout << "UnitTest for Equalization" << endl;
    Isis::Equalization equalizer("FromList.lst");
    equalizer.addHolds("HoldList.lst");

    double percent = 100.0;
    int mincount = 1000;
    bool weight = false;
    Isis::OverlapNormalization::SolutionType adjust =
      Isis::OverlapNormalization::Both;

    equalizer.calculateStatistics(percent, mincount, weight, adjust);
    Isis::PvlGroup results = equalizer.getResults();
    cout << results << endl;
  }
  catch(Isis::iException &e) {
    e.Report();
  }
}

