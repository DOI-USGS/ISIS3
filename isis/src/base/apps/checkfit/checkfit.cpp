#include "Isis.h"

#include <cmath>
#include <iostream>

#include <QFile>
#include <QMap>
#include <QPair>
#include <QString>
#include <QTextStream>

#include "Camera.h"
#include "Cube.h"
#include "FileList.h"
#include "FileName.h"
#include "IException.h"
#include "PiecewisePolynomial.h"
#include "Progress.h"
#include "SpicePosition.h"
#include "SpiceRotation.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

QPair<double, double> testFit(FileName inCubeFile, int positionDegree, int positionSegments,
                              int pointingDegree, int pointingSegments);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Read in the list of cubes to check
  FileList cubeList;
  cubeList.read( ui.GetFileName("FROMLIST") );

  // Get the fit parameters
  int positionDegree = ui.GetInteger("SPKDEGREE");
  int positionSegments = ui.GetInteger("SPKSEGMENTS");
  int pointingDegree = ui.GetInteger("CKDEGREE");
  int pointingSegments = ui.GetInteger("CKSEGMENTS");

  // Setup the map for storing fit quality
  QMap<QString, QPair<double, double> > qualityMap;

  // Setup the progress tracker
  Progress cubeProgress;
  cubeProgress.SetMaximumSteps(cubeList.size());
  cubeProgress.CheckStatus();

  // Compute a test fit for each cube
  for (int cubeIndex = 0; cubeIndex < cubeList.size(); cubeIndex++) {
    FileName cubeFileName = cubeList[cubeIndex];
    QPair<double, double> fitQuality;
    try {
      cubeProgress.CheckStatus();
      fitQuality = testFit(cubeFileName,
                           positionDegree, positionSegments,
                           pointingDegree, pointingSegments);
    }
    catch(IException &e) {
      QString warning = "**WARNING** Failed checking cube [" + cubeFileName.expanded() + "].";
      std::cerr << warning << std::endl << e.toString() << std::endl;
      continue;
    }
    qualityMap.insert(cubeFileName.expanded(), fitQuality);
  }

  // Open the TO file for writing
  FileName outFileName = ui.GetFileName("TO");
  QFile outFile(outFileName.expanded());
  if (outFile.open(QFile::WriteOnly |QFile::Truncate)) {
    QTextStream outWriter(&outFile);
    // Output the header
    outWriter << "Cube, Position Error (km), Pointing Error (Rad)\n";
    QList<QString> cubeNames = qualityMap.keys();
    for (int i = 0; i < cubeNames.size(); i++) {
      QString cubeName = cubeNames[i];
      QPair<double, double> fitQuality = qualityMap.value(cubeName);
      outWriter << cubeName << ", "
                << toString(fitQuality.first) << ", "
                << toString(fitQuality.second) << "\n";
    }
  }
  else {
    QString msg = "Failed opening output file [" + outFileName.expanded() + "].";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
}

QPair<double, double> testFit(FileName inCubeFile, int positionDegree, int positionSegments,
                              int pointingDegree, int pointingSegments) {
  Cube inCube(inCubeFile);
  Camera *inCam = inCube.camera();
  SpicePosition *instPosition = inCam->instrumentPosition();
  SpiceRotation *instRotation = inCam->instrumentRotation();

  // Fit the position
  instPosition->SetPolynomialDegree(positionDegree);
  instPosition->setPolynomialSegments(positionSegments);
  PiecewisePolynomial positionPoly = instPosition->testFit();

  // Fit the rotation
  instRotation->SetPolynomialDegree(pointingDegree);
  instRotation->setPolynomialSegments(pointingSegments);
  PiecewisePolynomial rotationPoly = instRotation->testFit();

  // Compute the position RMS
  double sumSquaredPositionError = 0;
  double positionBaseTime = instPosition->GetBaseTime();
  double positionTimeScale = instPosition->GetTimeScale();
  std::vector<double> positionSampleTimes = instPosition->timeCache();
  int positionSampleCount = positionSampleTimes.size();
  for (int i = 0; i < positionSampleCount; i++) {
    double error = 0;
    double scaledTime = (positionSampleTimes[i] - positionBaseTime) / positionTimeScale;
    std::vector<double> measuredCoord = instPosition->SetEphemerisTime(positionSampleTimes[i]);
    std::vector<double> estimatedCoord = positionPoly.evaluate(scaledTime);
    error += (measuredCoord[0] - estimatedCoord[0]) * (measuredCoord[0] - estimatedCoord[0]);
    error += (measuredCoord[1] - estimatedCoord[1]) * (measuredCoord[1] - estimatedCoord[1]);
    error += (measuredCoord[2] - estimatedCoord[2]) * (measuredCoord[2] - estimatedCoord[2]);
    sumSquaredPositionError += sqrt(error);
  }
  double positionRMS = sqrt(sumSquaredPositionError / positionSampleCount);

  // Compute the rotation RMS
  double sumSquaredRotationError = 0;
  double rotationBaseTime = instRotation->GetBaseTime();
  double rotationTimeScale = instRotation->GetTimeScale();
  std::vector<double> rotationSampleTimes = instRotation->timeCache();
  int rotationSampleCount = rotationSampleTimes.size();
  double start1 = 0.; // value of 1st angle1 in cache
  double start3 = 0.; // value of 1st angle3 in cache
  for (int i = 0; i < rotationSampleCount; i++) {
    double error = 0;
    double scaledTime = (rotationSampleTimes[i] - rotationBaseTime) / rotationTimeScale;
    instRotation->SetEphemerisTime(rotationSampleTimes[i]);
    std::vector<double> measuredAngles = instRotation->Angles(3, 1, 3);
    // Fix the angles crossing the domain bound
    if (i == 0) {
      start1 = measuredAngles[0];
      start3 = measuredAngles[2];
    }
    else {
      measuredAngles[0] = instRotation->WrapAngle(start1, measuredAngles[0]);
      measuredAngles[2] = instRotation->WrapAngle(start3, measuredAngles[2]);
    }
    std::vector<double> estimatedAngles = rotationPoly.evaluate(scaledTime);
    error += (measuredAngles[0] - estimatedAngles[0]) * (measuredAngles[0] - estimatedAngles[0]);
    error += (measuredAngles[1] - estimatedAngles[1]) * (measuredAngles[1] - estimatedAngles[1]);
    error += (measuredAngles[2] - estimatedAngles[2]) * (measuredAngles[2] - estimatedAngles[2]);
    sumSquaredRotationError += sqrt(error);
  }
  double rotationRMS = sqrt(sumSquaredRotationError / rotationSampleCount);

  return QPair<double, double>(positionRMS, rotationRMS);
}

