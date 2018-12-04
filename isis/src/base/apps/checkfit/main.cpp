#include "Isis.h"

#include <cmath>
#include <iostream>

#include <QFile>
#include <QMap>
#include <QPair>
#include <QString>
#include <QTextStream>
#include <QVector>

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

QVector< QPair<double, double> > testFit(FileName inCubeFile,
                                         int positionDegree, int positionSegments,
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
  QMap<QString, QVector< QPair<double, double> > > qualityMap;

  // Setup the progress tracker
  Progress cubeProgress;
  cubeProgress.SetMaximumSteps(cubeList.size());
  cubeProgress.CheckStatus();

  // Compute a test fit for each cube
  for (int cubeIndex = 0; cubeIndex < cubeList.size(); cubeIndex++) {
    FileName cubeFileName = cubeList[cubeIndex];
    QVector< QPair<double, double> > fitQuality;
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
    outWriter << "Cube,"
              << "Position Segments,Position Fit Degree,Minimum Position Error,"
              << "Median Position Error,Maximum Position Error,RMS Position Error,"
              << "Mean Position Error,Standard Deviation of Position Error,"
              << "Chebyshev Minimum Position Error,Chebyshev Maximum Position Error,"
              << "Pointing Segments,Pointing Fit Degree,Minimum Pointing Error,"
              << "Median Pointing Error,Maximum Pointing Error,RMS Pointing Error,"
              << "Mean Pointing Error,Standard Deviation of Pointing Error,"
              << "Chebyshev Minimum Pointing Error,Chebyshev Maximum Pointing Error"
              << "\n";
    QList<QString> cubeNames = qualityMap.keys();
    for (int i = 0; i < cubeNames.size(); i++) {
      QString cubeName = cubeNames[i];
      QVector< QPair<double, double> > fitQuality = qualityMap.value(cubeName);
      outWriter << cubeName;
      // Output Position Error Stats
      for (int j = 0; j < fitQuality.size(); j++) {
        outWriter  << "," << toString(fitQuality[j].first);
      }
      // Output Pointing Error Stats
      for (int j = 0; j < fitQuality.size(); j++) {
        outWriter  << "," << toString(fitQuality[j].second);
      }
      outWriter << "\n";
    }
  }
  else {
    QString msg = "Failed opening output file [" + outFileName.expanded() + "].";
    throw IException(IException::Io, msg, _FILEINFO_);
  }
}

/**
 * <p>
 * Computes the position and pointing fit error, then outputs statistics on
 * them. The statistics are output as a vector of pairs. Each element of the
 * vector contains a pair of values for a specific statistic. The first value
 * is the value of the statistic for position error in km. The second value is
 * the value of the statistic for pointing error in radians.
 * </p>
 * <p>
 * The output statistics are:
 * </p>
 * <ol>
 *   <li>Number of Segments</li>
 *   <li>Min Error</li>
 *   <li>Median Error</li>
 *   <li>Maximum Error</li>
 *   <li>RMS Error</li>
 *   <li>Mean Error</li>
 *   <li>Standard Deviation of the Error</li>
 *   <li>Chebyshev Minimum Error</li>
 *   <li>Chebyshev Maximum Error</li>
 * </ol>
 * 
 * @param inCubeFile The cube file to test
 * @param positionDegree The degree of the position fit
 * @param positionSegments The number of segments used in the position fit.
 * @param pointingDegree The degree of the pointing fit
 * @param pointingSegments The number of segments used in the pointing fit.
 * 
 * @return @b QVector<QPair<double,double>> A vector containing pairs of
 *                                          statistic values for the position
 *                                          and pointing error in km and
 *                                          radians respectively.
 */
QVector< QPair<double, double> > testFit(FileName inCubeFile,
                                         int positionDegree, int positionSegments,
                                         int pointingDegree, int pointingSegments) {
  // TODO validate these pointers
  Cube inCube(inCubeFile);
  Camera *inCam = inCube.camera();
  SpicePosition *instPosition = inCam->instrumentPosition();
  SpiceRotation *instRotation = inCam->instrumentRotation();

  // Fit the position
  PiecewisePolynomial positionPoly;
  try {
    positionPoly = instPosition->fitPolynomial(positionDegree, positionSegments);
  }
  catch (IException &e) {
    QString msg = "Failed Fitting Instrument Position.";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  // Fit the rotation
  PiecewisePolynomial rotationPoly;
  try {
    rotationPoly = instRotation->fitPolynomial(pointingDegree, pointingSegments);
  }
  catch (IException &e) {
    QString msg = "Failed Fitting Instrument Pointing.";
    throw IException(e, IException::Unknown, msg, _FILEINFO_);
  }

  // Calculate and store the error statistics
  QVector< QPair<double, double> > fitStats;
  Histogram positionHist = instPosition->computeError(positionPoly);
  Histogram rotationHist = instRotation->computeError(rotationPoly);

  // Number of Segments
  fitStats.push_back( QPair<double, double>( positionPoly.segments(), rotationPoly.segments() ) );

  // Fit Degree
  fitStats.push_back( QPair<double, double>( positionPoly.degree(), rotationPoly.degree() ) );

  // Min Error
  fitStats.push_back( QPair<double, double>( positionHist.Minimum(), rotationHist.Minimum() ) );

  // Median Error
  fitStats.push_back( QPair<double, double>( positionHist.Median(), rotationHist.Median() ) );

  // Maximum Error
  fitStats.push_back( QPair<double, double>( positionHist.Maximum(), rotationHist.Maximum() ) );

  // RMS Error
  fitStats.push_back( QPair<double, double>( positionHist.Rms(), rotationHist.Rms() ) );

  // Mean Error
  fitStats.push_back( QPair<double, double>( positionHist.Average(), rotationHist.Average() ) );

  // Standard Deviation of the Error
  fitStats.push_back( QPair<double, double>( positionHist.StandardDeviation(), rotationHist.StandardDeviation() ) );

  // Chebyshev Minimum Error
  fitStats.push_back( QPair<double, double>( positionHist.ChebyshevMinimum(), rotationHist.ChebyshevMinimum() ) );

  // Chebyshev Maximum Error
  fitStats.push_back( QPair<double, double>( positionHist.ChebyshevMaximum(), rotationHist.ChebyshevMaximum() ) );

  return fitStats;
}

