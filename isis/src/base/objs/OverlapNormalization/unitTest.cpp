/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iomanip>
#include "IException.h"
#include "Cube.h"
#include "OverlapStatistics.h"
#include "Statistics.h"
#include "OverlapNormalization.h"
#include "Preference.h"

using namespace std;

void PrintResults(string, const unsigned, Isis::OverlapNormalization &);

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try {
    cout << "UnitTest for Overlap Normalization" << endl;
    Isis::Cube cube1, cube2, cube3;
    cube1.open("$ISISTESTDATA/isis/src/odyssey/unitTestData/I00824006RDR.lev2.cub");
    cube2.open("$ISISTESTDATA/isis/src/odyssey/unitTestData/I01523019RDR.lev2.cub");
    cube3.open("$ISISTESTDATA/isis/src/odyssey/unitTestData/I02609002RDR.lev2.cub");
    Isis::Statistics *stats1 = cube1.statistics();
    Isis::Statistics *stats2 = cube2.statistics();
    Isis::Statistics *stats3 = cube3.statistics();

    vector<Isis::Statistics *> statsList;
    statsList.push_back(stats1);
    statsList.push_back(stats2);
    statsList.push_back(stats3);
    cout << "statsList size: " << statsList.size() << endl;

    Isis::OverlapNormalization oNorm(statsList);

    cout << "oNorm creation == SUCCESS" << endl;
    cout << setprecision(13);

    Isis::OverlapStatistics oStats1(cube1, cube2);
    Isis::OverlapStatistics oStats2(cube1, cube3);
    Isis::OverlapStatistics oStats3(cube2, cube3);

    Isis::MultivariateStatistics mStats1;
    Isis::MultivariateStatistics mStats2;
    Isis::MultivariateStatistics mStats3;
    mStats1 = oStats1.GetMStats(1);
    mStats2 = oStats2.GetMStats(1);
    mStats3 = oStats3.GetMStats(1);

    Isis::Statistics overlap11 = mStats1.X();
    Isis::Statistics overlap12 = mStats1.Y();
    oNorm.AddOverlap(overlap11, 0,
                     overlap12, 1, overlap11.ValidPixels());
    Isis::Statistics overlap21 = mStats2.X();
    Isis::Statistics overlap22 = mStats2.Y();
    oNorm.AddOverlap(overlap21, 0,
                     overlap22, 2, overlap21.ValidPixels());
    Isis::Statistics overlap31 = mStats3.X();
    Isis::Statistics overlap32 = mStats3.Y();
    oNorm.AddOverlap(overlap31, 1,
                     overlap32, 2, overlap31.ValidPixels());
    oNorm.AddHold(1);

    oNorm.Solve();

    PrintResults("I00824006RDR.lev2.cub", 0, oNorm);
    PrintResults("I01523019RDR.lev2.cub", 1, oNorm);
    PrintResults("I02609002RDR.lev2.cub", 2, oNorm);
  }
  catch(Isis::IException &e) {
    e.print();
  }
}


void PrintResults(string filename, const unsigned index,
                  Isis::OverlapNormalization &oNorm) {
  double offset = oNorm.Offset(index);
  double gain = oNorm.Gain(index);

  if(fabs(offset) < 1e-15) {
    offset = 0.0;
  }
  if(fabs(gain) < 1e-15) {
    gain = 0.0;
  }

  cout << filename << " : Gathered Offset: " << offset << endl;
  cout << filename << " : Gathered Gain: " << gain << endl;
}
