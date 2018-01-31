#include "LidarData.h"

#include "FileName.h"
#include "Preference.h"

using namespace Isis;


/**
 * Unit test for the LIDARData class.
 *
 * @internal
 *   @history 2018-01-29 Ian Humphrey - original version.
 */
int main(int argc, char *argv[]) {
  // Set up our unit test preferences
  Preference::Preferences(true);

  LidarData data;
  FileName csvFile("RDR_98E100E_60N62NPointPerRow_csv_table-original.csv");
  data.read(csvFile);

}
