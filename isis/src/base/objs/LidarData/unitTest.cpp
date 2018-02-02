#include "LidarData.h"

#include <QString>

#include "Angle.h"
#include "ControlMeasure.h"
#include "Distance.h"
#include "FileName.h"
#include "iTime.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "Longitude.h"
#include "Preference.h"
#include "SurfacePoint.h"

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
  lcp->SetId("testLidarControlPoint");
  defaultData.insert(lcp);
  cout << "\tnumber of points: " << defaultData.points().size() << endl;
  cout << "\tname of point:    " << defaultData.points().first()->GetId() << endl;
  cout << endl;

  // Test read()
  cout << "Testing read(FileName)... " << endl;
  cout << endl;

  // Test write()
  cout << "Testing write(FileName)... " << endl;
  LidarData mockData;
  double lat, lon, rad;
  lat = 100.0;
  lon = 50.0;
  rad = 1000.0;
  for (int i = 1; i < 11; i++) {
    time += 60.0;
    range += 10.0;
    lcp = QSharedPointer<LidarControlPoint>(new LidarControlPoint(time, range, sigmaRange));
    lcp->SetId("testLidarControlPoint" + QString::number(i));
    lat += 1.0;
    lon += 1.0;
    SurfacePoint sp(Latitude(lat, Angle::Units::Degrees),
                    Longitude(lon, Angle::Units::Degrees),
                    Distance(rad, Distance::Units::Kilometers));
    lcp->SetAprioriSurfacePoint(sp);
    double line, sample;
    line = 1.0;
    sample = 1.0;
    for (int j = 0; j < 2; j++) {
      ControlMeasure *measure = new ControlMeasure();
      sample += 1.0;
      line += 1.0;
      measure->SetCoordinate(sample, line);
      measure->SetCubeSerialNumber("SN_" + QString::number(i) + "-" + QString::number(j));
      lcp->Add(measure);
    }
    mockData.insert(lcp);
  }
  FileName outFile("./test.json");
  mockData.write(outFile);
  cout << endl;
}
