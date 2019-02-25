#include "Isis.h"

#include <QSharedPointer>
#include <QString>

#include "Angle.h"
#include "Camera.h"
#include "CSVReader.h"
#include "Cube.h"
#include "CubeManager.h"
#include "Distance.h"
#include "FileName.h"
#include "ID.h"

#include "IException.h"
#include "iTime.h"
#include "Latitude.h"
#include "LidarControlPoint.h"
#include "LidarData.h"
#include "Longitude.h"
#include "Target.h"
#include "SerialNumberList.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;


struct LidarCube {
  FileName name;
  QString sn;
  iTime startTime;
  iTime endTime;
};


void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  FileName dataFile = ui.GetFileName("FROM");
  SerialNumberList cubeList = SerialNumberList(ui.GetFileName("CUBES"));
  double threshold = ui.GetDouble("THRESHOLD");
  double rangeSigma = ui.GetDouble("POINT_RANGE_SIGMA");

  double latSigma = Isis::Null;
  double lonSigma = Isis::Null;
  double radiusSigma = Isis::Null;

  if (ui.WasEntered("POINT_LATITUDE_SIGMA")) {
    latSigma = ui.GetDouble("POINT_LATITUDE_SIGMA");
  }
  if (ui.WasEntered("POINT_LONGITUDE_SIGMA")) {
    lonSigma = ui.GetDouble("POINT_LONGITUDE_SIGMA");
  }
  if (ui.WasEntered("POINT_RADIUS_SIGMA")) {
    radiusSigma = ui.GetDouble("POINT_RADIUS_SIGMA");
  }

  QList<LidarCube> images;
  
  for (int i = 0; i < cubeList.size(); i++) {
    LidarCube lidarCube;
    QString serialNumber = cubeList.serialNumber(i);
    FileName fileName = FileName(cubeList.fileName(serialNumber));
    Cube cube(fileName);
    
    lidarCube.name = fileName;
    lidarCube.sn = serialNumber;
    std::pair< double, double> startEndTime = cube.camera()->StartEndEphemerisTimes();
    lidarCube.startTime = iTime(startEndTime.first);
    lidarCube.endTime = iTime(startEndTime.second);
    
    images.append(lidarCube);
  }
  
  CSVReader lidarDataFile;
  lidarDataFile.read(dataFile.expanded());
  LidarData lidarDataSet;
  CubeManager cubeMgr;
  Distance  majorAx;
  Distance minorAx;
  Distance polarAx;
  
  // Set up an automatic id generator for the point ids
  ID pointId = ID(ui.GetString("POINTID"));
  
  //Start at 1 because there is a header. TODO actually set a header in lidarDataFile
  for (int i = 1; i < lidarDataFile.rows(); i++) {
    CSVReader::CSVAxis row = lidarDataFile.getRow(i);

    iTime time(row[0]);
    Latitude lat(row[2].toDouble(), Angle::Units::Degrees);
    Longitude lon(row[1].toDouble(), Angle::Units::Degrees);
    Distance radius(row[3].toDouble(), Distance::Units::Kilometers);
    double range = row[4].toDouble();
//     QString quality = row[]; //TODO figure out how/where to find this value
    
    LidarControlPoint *lidarPoint = new LidarControlPoint;
    lidarPoint->SetId(pointId.Next());

    lidarPoint->setTime(time);
    lidarPoint->setRange(range);
    lidarPoint->setSigmaRange(rangeSigma);

    // Just set the point coordinates for now.  We need to wait until we set
    // the target radii to be able to set the coordinate sigmas.  The sigmas
    // will be converted to angles and the target radii are needed to do that.
    SurfacePoint spoint(lat, lon, radius);
    // lidarPoint->SetAprioriSurfacePoint(SurfacePoint(lat, lon, radius));
    
    bool setSurfacePointRadii = true;
      
    for (int j = 0; j < images.size(); j++) {
      Cube *cube = cubeMgr.OpenCube(images[j].name.expanded());
        
      if (cube != NULL) {
          
        Camera *camera = cube->camera();
          
        if (camera != NULL) {
          if (camera->SetGround(spoint)) {
            double samp = camera->Sample();
            double line = camera->Line();
              
            if (samp > 0.5 - threshold   &&   line > 0.5 - threshold
                && samp < camera->Samples() + .5  &&  line < camera->Lines() + .5) {
        
              ControlMeasure *measure = new ControlMeasure;

              measure->SetCoordinate(camera->Sample(), camera->Line()); 
              measure->SetCubeSerialNumber(images[j].sn);

              if (setSurfacePointRadii) {
              // Get the radii and set the radii in the SurfacePoint
                std::vector<Distance>  targetRadii;
                targetRadii = camera->target()->radii();
                majorAx = targetRadii[0];
                minorAx = targetRadii[1];
                polarAx = targetRadii[2];
                setSurfacePointRadii = false;
                spoint.SetSphericalSigmasDistance(
                                     Distance(latSigma, Distance::Units::Meters),
                                     Distance(lonSigma, Distance::Units::Meters),
                                     Distance(radiusSigma, Distance::Units::Meters));
                lidarPoint->SetAprioriSurfacePoint(spoint);
                // if (camera->target()->shape()->hasValidTarget()) {
                //   targetRadii = camera->target()->shape()->targetRadii();
                // }
                // else {
                //   QString msg = "Valid target not defined in shape model ";
                //   throw IException(IException::Unknown, msg, _FILEINFO_);
                // }
                  
                // targid = camera->SpkTargetId();
                // Distance  targetRadii[3];
                // camera0>getDouble(
                // camera->radii(targetRadii);
                // majorAx = targetRadii[0];
                // minorAx = targetRadii[1];
                // polarAx = targetRadii[2];
                // setSurfacePointRadii = false;
              }
          
              lidarPoint->Add(measure);
              if (time >= images[j].startTime && time <= images[j].endTime) {
                QString newSerial = measure->GetCubeSerialNumber();
                lidarPoint->addSimultaneous(newSerial);
              }
            }
          }
        }
        else {
          QString msg = "Unable to create a camera from " + images[j].name.expanded();
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }
      }
      else {
        QString msg = "Unable to open a cube from " + images[j].name.expanded();
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }        
    }
    // end image loop
    
      if (lidarPoint->GetNumMeasures() <= 0 ||
          lidarPoint->snSimultaneous().size() <=0) {
      continue;
    }
    
    lidarDataSet.insert(QSharedPointer<LidarControlPoint>(lidarPoint));
  }

  
  if (ui.GetString("OUTPUTTYPE") == "JSON") {
    lidarDataSet.write(ui.GetFileName("TO"), LidarData::Format::Json);
  }
  else if (ui.GetString("OUTPUTTYPE") == "TEST") {
    lidarDataSet.write(ui.GetFileName("TO"), LidarData::Format::Test);
  }
  else {
    lidarDataSet.write(ui.GetFileName("TO"), LidarData::Format::Binary);
  }
}
