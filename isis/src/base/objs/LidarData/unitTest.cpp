#include "LidarData.h"

#include <QDebug>
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

void print(const LidarData &lidarData);

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

  // Test write() JSON format
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
    for (int j = 0; j < 2; j++) {
      ControlMeasure *measure = new ControlMeasure();
      measure->SetCoordinate((double) i, (double) j);
      measure->SetCubeSerialNumber("SN_" + QString::number(i) + "-" + QString::number(j));
      lcp->Add(measure);
    }
    mockData.insert(lcp);
  }
  FileName outputFile("./test.json");
  cout << outputFile.extension() << endl;
  mockData.write(outputFile, LidarData::Json);
  cout << endl;

  // Test write() with no data

  // Test read() with no data

  // Test write() binary format
  cout << outputFile.extension() << endl;
  outputFile = outputFile.setExtension("dat");
  cout << outputFile.extension() << endl;
  cout << outputFile.expanded().toStdString() << endl;
  mockData.write(outputFile, LidarData::Binary);

  // Test read() binary format
  cout << "Testing read(FileName) from binary data... " << endl;
  LidarData fromBinary;
  fromBinary.read(outputFile);
  print(fromBinary);
  cout << endl;

  // Test read()
  cout << "Testing read(FileName) from JSON data... " << endl;
  LidarData fromJson;
  outputFile = outputFile.setExtension(".json");
  fromJson.read(outputFile);
  print(fromJson);
  cout << endl;
}


void print(const LidarData &lidarData) {
  QList< QSharedPointer<LidarControlPoint> > points = lidarData.points();
  std::cout << "LidarData:" << std::endl;
  foreach (QSharedPointer<LidarControlPoint> point, points) {
    std::cout << "\tLidarControlPoint:" << std::endl;
    std::cout << "\t\tid: " << point->GetId() << std::endl;;
    SurfacePoint sp = point->GetAprioriSurfacePoint();
    double lat, lon, rad;
    lat = sp.GetLatitude().planetocentric(Angle::Units::Degrees);
    lon = sp.GetLongitude().positiveEast(Angle::Units::Degrees);
    rad = sp.GetLocalRadius().kilometers();
    std::cout << "\t\tlatitude:  " << lat << std::endl;
    std::cout << "\t\tlongitude: " << lon << std::endl;
    std::cout << "\t\tradius:    " << rad << std::endl;
    std::cout << "\t\trange:     " << point->range() << std::endl;
    std::cout << "\t\tsigmaRange:" << point->sigmaRange() << std::endl;
    std::cout << "\t\ttime:      " << point->time().Et() << std::endl;
    QList<ControlMeasure *> measures = point->getMeasures();
    foreach (ControlMeasure *measure, measures) {
      std::cout << "\t\tControlMeasure: " << std::endl;
      std::cout << "\t\t\tline:   " << measure->GetLine() << std::endl;
      std::cout << "\t\t\tsample: " << measure->GetSample() << std::endl;
      std::cout << "\t\t\tSN:     " << measure->GetCubeSerialNumber() << std::endl;
      std::cout << "\t\t#END_ControlMeasure." << std::endl;
    }
    std::cout << "\t#END_LidarControlPoint." << std::endl << std::endl;
  }
  std::cout << std::endl;
}
