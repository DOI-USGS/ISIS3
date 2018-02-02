#include "Isis.h"

#include <QString>

#include "Angle.h"
#include "Camera.h"
#include "CSVReader.h"
#include "Cube.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "LidarData.h"
#include "Longitude.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;


struct LidarCube {
  FileName name;
  QString sn;
  iTime startTime;
  iTime endTime;
}


void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  FileName dataFile = ui.GetFileName("FROM");
  SerialNumberList cubeList = SerialNumberList(ui.GetFileName("FROM2"));
  FileName output = ui.GetFileName("TO");

  QList<LidarCube> images;
  
  for (i = 0; i < cubeList.size(); i++) {
    LidarCube lidarCube;
    QString serialNumber = cubeList.serialNumber(i);
    FileName fileName = FileName(cubeList.fileName(serialNumber));
    Cube cube(fileName);
    
    lidarCube.name = fileName;
    lidarCube.sn = serialNumber;
    std::pair< double, double> startEndTime = cube.camera().StartEndEphemerisTimes();
    lidarCube.startTime = iTime(startEndTime.first);
    lidarCube.endTime = iTime(startEndTime.second);
    
    images.append(lidarCube);
  }
  
  CSVReader lidarData(dataFile.expanded(), true, 1);
  LidarData lidarData();
  
  for (i = 0; i < lidarDataFile.size(); i++) {
    CSVReader::CSVAxis row = lidarDataFile.getRow(i);
    
    iTime time(row[0].toDouble());
    Latitude lat(row[1].toDouble(), Angle::Units::Degrees);
    Longitude lon(row[2].toDouble(), Angle::Units::Degrees);
    Distance radius(row[3].toDouble(), Distance::Units::Kilometers);
    double range = row[4].toDouble();
    QString id = "POINT" + QString::number(i);
    double sigma = 0; //TODO figure out how/where to calculate this
//     QString quality = row[]; //TODO figure out how/where to find this value
    
    LidarControlPoint lidarPoint();
    lidarPoint.SetId(id);
    lidarPoint.setTime(time);
    lidarPoint.setRange(range);
    lidarPoint.setSigmaRadius(sigma);
    lidarPoint.SetAprioriSurfacePoint(SurfacePoint(lat, lon, radius));
    
    for (j = 0; j < images.size(); j++) {
      if (images[j].startTime <= time || time <= images[j].endTime) {
        Cube cube(images[j].name);
        Camera camera = cube.camera();
        camera.SetGround(lat, lon);
        
        ControlMeasure measure();
        measure.SetCoordinate(camera.Line(), camera.Sample()); 
        measure.SetCubeSerialNumber(images[j].sn);
        
        lidarPoint.Add(measure);
      }
    }
    
    lidarData.insert(lidarPoint);
  }

  return;
}


void fetchCSVData(FileName &csvFile) {

  QFile data(csvFile.expanded());

  if (data.open(QIODevice::ReadOnly)) {
    qDebug() << "Could not open:  " << csvFile.expanded();
  }


  return;
}


