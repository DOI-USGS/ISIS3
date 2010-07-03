#include <iomanip>
#include "iException.h"
#include "Cube.h"
#include "OverlapStatistics.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  try{
    cout << "UnitTest for Overlap Statistics" << endl;
    Isis::Cube cube1,cube2;
    cube1.Open("$odyssey/testData/I00824006RDR.lev2.cub");
    cube2.Open("$odyssey/testData/I02609002RDR.lev2.cub");
    cout << setprecision(9);

    // Check to make sure the overlap calculation is working correctly
    Isis::OverlapStatistics oStats(cube1,cube2);
    cout << "For Overlap of I00824006RDR.lev2.cub & I02609002RDR.lev2.cub..." 
      << endl;
    cout << "Has Overlap? = " << oStats.HasOverlap() << endl << endl;
    cout << "Overlap Dimensions: " << endl;
    cout << "  Samples = " << oStats.Samples() << endl;
    cout << "  Lines = " << oStats.Lines() << endl;
    cout << "  Bands = " << oStats.Bands() << endl << endl;

    cout << "Overlap Areas:" << endl;
    cout << "  Filename = " << Isis::Filename(oStats.FilenameX()).Name() << endl;
    cout << "    Start Sample = " << oStats.StartSampleX() << endl;
    cout << "    End Sample = " << oStats.EndSampleX() << endl;
    cout << "    Start Line = " << oStats.StartLineX() << endl;
    cout << "    End Line = " << oStats.EndLineX() << endl << endl;

    cout << "  Filename = " << Isis::Filename(oStats.FilenameY()).Name() << endl;
    cout << "    Start Sample = " << oStats.StartSampleY() << endl;
    cout << "    End Sample = " << oStats.EndSampleY() << endl;
    cout << "    Start Line = " << oStats.StartLineY() << endl;
    cout << "    End Line = " << oStats.EndLineY() << endl << endl;
  }
  catch (Isis::iException &e) {
   e.Report();
 }
}







