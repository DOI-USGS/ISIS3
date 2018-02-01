#include "LidarData.h"

#include "FileName.h"
#include "iTime.h"
#include "LidarControlPoint.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

/**
 * Unit test for the LIDARData class.
 *
 * @internal
 *   @history 2018-01-29 Ian Humphrey - original version.
 *   @history 2018-01-31 Ian Humphrey - added tests for points() and insert().
 */
int main(int argc, char *argv[]) {
  // Set up our unit test preferences
  Preference::Preferences(true);

  // Test LidarData()
  cout << "Testing default constructor... " << endl;
  LidarData defaultData;
  cout << "\tnumber of points: " << defaultData.points().size() << endl;
  cout << endl;

  // Test LidarData(FileName)
  cout << "Testing LidarData(FileName)... " << endl;
  cout << endl;

  // Test insert(QSharedPointer<LidarControlPoint>)
  cout << "Testing insert(QSharedPointer<LidarControlPoint>)... " << endl;
  iTime time("2018-01-31T14:05:00.1234");
  double range = 55.0;
  double sigmaRange = 0.1;
  QSharedPointer<LidarControlPoint> lcp =
      QSharedPointer<LidarControlPoint>(new LidarControlPoint(time, range, sigmaRange));
  lcp->SetId("testLidarControlPoint1");
  defaultData.insert(lcp);
  cout << "\tnumber of points: " << defaultData.points().size() << endl;
  cout << "\tname of point:    " << defaultData.points().first()->GetId() << endl;
  cout << endl;

  // Test read()
  cout << "Testing read(FileName)... " << endl;
  cout << endl;

  // Test write()
  cout << "Testing write(FileName)... " << endl;
  cout << endl;
}
