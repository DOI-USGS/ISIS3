/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iomanip>
#include "IException.h"
#include "Cube.h"
#include "OverlapStatistics.h"
#include "Preference.h"
#include "PvlObject.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  try {
    cout << "UnitTest for Overlap Statistics" << endl;
    Cube cube1, cube2;
    cube1.open("$ISISTESTDATA/isis/src/odyssey/unitTestData/I00824006RDR.lev2.cub");
    cube2.open("$ISISTESTDATA/isis/src/odyssey/unitTestData/I02609002RDR.lev2.cub");
    cout << setprecision(9);

    // Check to make sure the overlap calculation is working correctly
    OverlapStatistics oStats(cube1, cube2);
    cout << "For Overlap of I00824006RDR.lev2.cub & I02609002RDR.lev2.cub..."
         << endl;
    cout << "Has Overlap? = " << oStats.HasOverlap() << endl << endl;
    cout << "Overlap Dimensions: " << endl;
    cout << "  Samples = " << oStats.Samples() << endl;
    cout << "  Lines = " << oStats.Lines() << endl;
    cout << "  Bands = " << oStats.Bands() << endl << endl;

    cout << "Overlap Areas:" << endl;
    cout << "  Filename = " << FileName(oStats.FileNameX()).name() << endl;
    cout << "    Start Sample = " << oStats.StartSampleX() << endl;
    cout << "    End Sample = " << oStats.EndSampleX() << endl;
    cout << "    Start Line = " << oStats.StartLineX() << endl;
    cout << "    End Line = " << oStats.EndLineX() << endl << endl;

    cout << "  Filename = " << FileName(oStats.FileNameY()).name() << endl;
    cout << "    Start Sample = " << oStats.StartSampleY() << endl;
    cout << "    End Sample = " << oStats.EndSampleY() << endl;
    cout << "    Start Line = " << oStats.StartLineY() << endl;
    cout << "    End Line = " << oStats.EndLineY() << endl << endl;


    cout << endl << "Testing Pvl serialization methods..." << endl;
    PvlObject toStats = oStats.toPvl();
    OverlapStatistics fromStats(toStats);
    cout << "Has Overlap? = " << fromStats.HasOverlap() << endl << endl;
    cout << "Overlap Dimensions: " << endl;
    cout << "  Samples = " << fromStats.Samples() << endl;
    cout << "  Lines = " << fromStats.Lines() << endl;
    cout << "  Bands = " << fromStats.Bands() << endl << endl;

    cout << "Overlap Areas:" << endl;
    cout << "  Filename = " << FileName(fromStats.FileNameX()).name() << endl;
    cout << "    Start Sample = " << fromStats.StartSampleX() << endl;
    cout << "    End Sample = " << fromStats.EndSampleX() << endl;
    cout << "    Start Line = " << fromStats.StartLineX() << endl;
    cout << "    End Line = " << fromStats.EndLineX() << endl << endl;

    cout << "  Filename = " << FileName(fromStats.FileNameY()).name() << endl;
    cout << "    Start Sample = " << fromStats.StartSampleY() << endl;
    cout << "    End Sample = " << fromStats.EndSampleY() << endl;
    cout << "    Start Line = " << fromStats.StartLineY() << endl;
    cout << "    End Line = " << fromStats.EndLineY() << endl << endl;
  }
  catch(IException &e) {
    e.print();
  }
}







