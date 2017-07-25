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
#include "Histogram.h"
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
  PiecewisePolynomial positionPoly = instPosition->fitPolynomial(positionDegree, positionSegments);

  // Fit the rotation
  instRotation->SetPolynomialDegree(pointingDegree);
  instRotation->setPolynomialSegments(pointingSegments);
  PiecewisePolynomial rotationPoly = instRotation->fitPolynomial(pointingDegree, pointingSegments);

  // Compute the position RMS
  Histogram positionHist = instPosition->computeError(positionPoly);
  double positionRMS = positionHist.Rms();

  // Compute the rotation RMS
  Histogram rotationHist = instRotation->computeError(rotationPoly);
  double rotationRMS = rotationHist.Rms();

  return QPair<double, double>(positionRMS, rotationRMS);
}

