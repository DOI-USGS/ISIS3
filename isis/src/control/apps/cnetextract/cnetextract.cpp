#include "Isis.h"

#include <set>
#include <sstream>

#include <QMap>
#include <QSet>
#include <QVector>

#include "Angle.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Cube.h"
#include "CubeManager.h"
#include "FileList.h"
#include "IException.h"
#include "IString.h"
#include "Longitude.h"
#include "Latitude.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "SurfacePoint.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void ExtractPointList(ControlNet &outNet, QVector<QString> &nonListedPoints);
void ExtractLatLonRange(ControlNet &outNet, QVector<QString> nonLatLonPoints,
                        QVector<QString> cannotGenerateLatLonPoints,
                        QMap<QString, QString> sn2filename);
bool NotInLatLonRange(SurfacePoint surfacePt, Latitude minlat,
                      Latitude maxlat, Longitude minlon, Longitude maxlon);
void WriteCubeOutList(ControlNet cnet, 
                      QMap<QString, QString> sn2file,
                      PvlGroup &summary);
void WriteResults(QString filename, 
                  QVector<QString> notExtracted, 
                  PvlGroup &results);
void omit(ControlNet &cnet, int cp);
void omit(ControlPoint *point, int cm);


// Main program
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  if(!ui.WasEntered("FROMLIST") && ui.WasEntered("TOLIST")) {
    QString msg = "To create a [TOLIST] the [FROMLIST] parameter must be provided.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  bool noIgnore          = ui.GetBoolean("NOIGNORE");
  bool noMeasureless     = ui.GetBoolean("NOMEASURELESS");
  bool noSingleMeasure   = ui.GetBoolean("NOSINGLEMEASURES");
  bool reference         = ui.GetBoolean("REFERENCE");
  bool fixed             = ui.GetBoolean("FIXED");
  bool noTolerancePoints = ui.GetBoolean("TOLERANCE");
  bool pointsEntered     = ui.WasEntered("POINTLIST");
  bool cubePoints        = ui.GetBoolean("CUBES");
  bool cubeMeasures      = ui.GetBoolean("CUBEMEASURES");
  bool retainReference   = ui.GetBoolean("RETAIN_REFERENCE");
  bool latLon            = ui.GetBoolean("LATLON");

  if(!(noIgnore || noMeasureless || noSingleMeasure || reference || fixed ||
       noTolerancePoints || pointsEntered || cubePoints || latLon)) {
    QString msg = "At least one filter must be selected [";
    msg += "NOIGNORE,NOMEASURELESS,NOSINGLEMEASURE,REFERENCE,FIXED,TOLERANCE,";
    msg += "POINTLIST,CUBES,LATLON]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(cubeMeasures || reference) {
    noMeasureless = true;
  }

  // Gets the input parameters
  ControlNet outNet(ui.GetFileName("CNET"));
  FileList inList;
  if(ui.WasEntered("FROMLIST")) {
    //inList = ui.GetFileName("FROMLIST");
    inList.read(ui.GetFileName("FROMLIST"));
  }

  int inputPoints = outNet.GetNumPoints();
  int inputMeasures = 0;
  for (int cp = 0; cp < outNet.GetNumPoints(); cp++)
    inputMeasures += outNet.GetPoint(cp)->GetNumMeasures();

  // Set up the Serial Number to FileName mapping
  QMap<QString, QString> sn2filename;
  for(int cubeIndex = 0; cubeIndex < (int)inList.size(); cubeIndex ++) {
    QString sn = SerialNumber::Compose(inList[cubeIndex].toString());
    sn2filename[sn] = inList[cubeIndex].toString();
  }


  Progress progress;
  progress.SetMaximumSteps(outNet.GetNumPoints());
  progress.CheckStatus();

  // Set up vector records of how points/measures are removed
  QVector<QString> ignoredPoints;
  QVector<QString> ignoredMeasures;
  QVector<QString> singleMeasurePoints;
  QVector<QString> measurelessPoints;
  QVector<QString> tolerancePoints;
  QVector<QString> nonReferenceMeasures;
  QVector<QString> nonFixedPoints;
  QVector<QString> nonCubePoints;
  QVector<QString> noCubeMeasures;
// This is commented out since this does not correspond to any filters or the
//  documentation of this application. I did not delete the code in case we find
//  that we need it later. J.Backer 2012-06-22
// 
//  QVector<QString> noMeasurePoints;
  QVector<QString> nonListedPoints;
  QVector<QString> nonLatLonPoints;
  QVector<QString> cannotGenerateLatLonPoints;

  // Set up comparison data
  QVector<QString> serialNumbers;
  if(cubePoints) {
    FileList cubeList(ui.GetFileName("CUBELIST"));
    for(int cubeIndex = 0; cubeIndex < (int)cubeList.size(); cubeIndex ++) {
      QString sn = SerialNumber::Compose(cubeList[cubeIndex].toString());
      serialNumbers.push_back(sn);
    }
  }

  double tolerance = 0.0;
  if(noTolerancePoints) {
    tolerance = ui.GetDouble("PIXELTOLERANCE");
  }

  // Set up extracted network values
  if(ui.WasEntered("NETWORKID"))
    outNet.SetNetworkId(ui.GetString("NETWORKID"));

  outNet.SetUserName(Isis::Application::UserName());
  outNet.SetDescription(ui.GetString("DESCRIPTION"));

  for(int cp = outNet.GetNumPoints() - 1; cp >= 0; cp --) {
    progress.CheckStatus();

    ControlPoint *controlpt = outNet.GetPoint(cp);

    // Do preliminary exclusion checks
    if(noIgnore && controlpt->IsIgnored()) {
      ignoredPoints.append(controlpt->GetId());
      omit(outNet, cp);
      continue;
    }
    if(fixed && !(controlpt->GetType() == ControlPoint::Fixed)) {
      nonFixedPoints.append(controlpt->GetId());
      omit(outNet, cp);
      continue;
    }

    if(noSingleMeasure) {
      bool invalidPoint = false;
      invalidPoint |= noIgnore && (controlpt->GetNumValidMeasures() < 2);
      invalidPoint |= controlpt->GetNumMeasures() < 2 && (controlpt->GetType() != ControlPoint::Fixed);

      if(invalidPoint) {
        singleMeasurePoints.append(controlpt->GetId());
        omit(outNet, cp);
        continue;
      }
    }

    // Change the current point into a new point by manipulation of its control measures
    ControlPoint *newPoint = outNet.GetPoint(cp);
    bool replaceLock = false;
    if (newPoint->IsEditLocked()) {
      newPoint->SetEditLock(false);
      replaceLock = true;
    }

    for(int cm = newPoint->GetNumMeasures() - 1; cm >= 0; cm --) {
      const ControlMeasure *newMeasure = newPoint->GetMeasure(cm);

      if(noIgnore && newMeasure->IsIgnored()) {
        //New error with deleting Reference Measures
        QString msg = newPoint->GetId() + "," + newMeasure->GetCubeSerialNumber();
        if(newPoint->GetRefMeasure() != newMeasure) {
          omit(newPoint, cm);
        }
        else{
          msg += ", Ignored measure but extracted since it is Reference";
        }
        ignoredMeasures.append(msg);
      }
      else if(reference && newPoint->GetRefMeasure() != newMeasure) {
        nonReferenceMeasures.append(newPoint->GetId() + "," + newMeasure->GetCubeSerialNumber());
        omit(newPoint, cm);
      }
      else if(cubeMeasures) {
        bool hasSerialNumber = false;
        QString serialNum = newMeasure->GetCubeSerialNumber();
        for(int sn = 0; sn < serialNumbers.size(); sn ++) {
          if(serialNumbers[sn] == serialNum) {
            hasSerialNumber = true;
            break;
          }
        }

        // doesn't have serial number if measure is not associated with cubelist
        // we need to omit appropriate measures
        if(!hasSerialNumber) {
          QString msg = newPoint->GetId() + "," + newMeasure->GetCubeSerialNumber();
          // if this measure is not reference, omit it
          // if this measure is a reference, but retainReference is off, omit it
          if(newPoint->GetRefMeasure() != newMeasure ||
             (newPoint->GetRefMeasure() == newMeasure && !retainReference))
            omit(newPoint, cm);
          else {
            //this is unnecessary if statement. only other possibility for else...
            // if(newPoint->GetRefMeasure() == newMeasure && retainReference) {
              msg += ", Reference not in the cubelist but extracted since "
                     "RETAIN_REFERENCE=true";
            // }
          }
          noCubeMeasures.append(msg);
        }
      }
    }

    if (replaceLock)
      newPoint->SetEditLock(true);

    //outNet.UpdatePoint(newPoint); // Fixed by redesign

    // Check for line/sample errors above provided tolerance
    if(noTolerancePoints) {
      bool hasLowTolerance = true;

      for(int cm = 0; cm < newPoint->GetNumMeasures() && hasLowTolerance; cm ++) {
        const ControlMeasure *newMeasure = newPoint->GetMeasure(cm);
        if(newMeasure->GetSampleResidual() >= tolerance ||
            newMeasure->GetLineResidual() >= tolerance) {
          hasLowTolerance = false;
        }
      }

      if(hasLowTolerance) {
        tolerancePoints.append(newPoint->GetId());
        omit(outNet, cp);
        continue;
      }
    }

    // Do not add outPoint if it has too few measures
    if(noSingleMeasure) {
      bool invalidPoint = false;
      invalidPoint |= noIgnore && (newPoint->GetNumValidMeasures() < 2);
      invalidPoint |= newPoint->GetNumMeasures() < 2 && newPoint->GetType() != ControlPoint::Fixed;

      if(invalidPoint) {
        singleMeasurePoints.append(controlpt->GetId());
        omit(outNet, cp);
        continue;
      }
    }

    // Do not add outPoint if it does not have a cube in CUBELIST as asked
    if(cubePoints) {
      bool hasSerialNumber = false;

      for(int cm = 0; cm < newPoint->GetNumMeasures() && !hasSerialNumber; cm ++) {
        for(int sn = 0; sn < serialNumbers.size() && !hasSerialNumber; sn ++) {
          if(serialNumbers[sn] == newPoint->GetMeasure(cm)->GetCubeSerialNumber())
            hasSerialNumber = true;
        }
      }

      if(!hasSerialNumber) {
        nonCubePoints.append(newPoint->GetId());
        omit(outNet, cp);
        continue;
      }
    }

    if(noMeasureless && newPoint->GetNumMeasures() == 0) {
      measurelessPoints.append(newPoint->GetId());
      omit(outNet, cp);
      continue;
    }
  } // Finished with simple comparisons

   // Use another pass to check for Ids
  if(pointsEntered) {
    ExtractPointList(outNet, nonListedPoints);
  }


   // Use another pass on outNet, because this is by far the most time consuming
   // process, and time could be saved by using the reduced size of outNet
  if(latLon) {
    ExtractLatLonRange(outNet, nonLatLonPoints, cannotGenerateLatLonPoints, sn2filename);
  }

  int outputPoints = outNet.GetNumPoints();
  int outputMeasures = 0;
  for (int cp = 0; cp < outNet.GetNumPoints(); cp++){
    outputMeasures += outNet.GetPoint(cp)->GetNumMeasures();
  }

  // if the output control net is not empty, write it out.
  Progress outProgress;
  outProgress.SetMaximumSteps(3);


  // Adds the remove history to the summary and results group
  PvlGroup summary("ResultSummary");
  PvlGroup results("Results");

  summary.AddKeyword(PvlKeyword("InputPoints", toString(inputPoints)));
  summary.AddKeyword(PvlKeyword("InputMeasures", toString(inputMeasures)));
  summary.AddKeyword(PvlKeyword("OutputPoints", toString(outputPoints)));
  summary.AddKeyword(PvlKeyword("OutputMeasures", toString(outputMeasures)));

  if (outputPoints != 0) {
    // Write the filenames associated with outNet
    if (ui.WasEntered("TOLIST") ) {
      WriteCubeOutList(outNet, sn2filename, summary);
    }

    outProgress.SetText("Writing Control Network");
    outProgress.CheckStatus();

    // Write the extracted Control Network
    outNet.Write(ui.GetFileName("ONET"));
    outProgress.CheckStatus();
  }
  else {
    summary.AddComment("The output control network file, ["
                       + ui.GetFileName("ONET") +
                       "],  was not created. "
                       "The provided filters have resulted in no points or "
                       "measures extracted.");
    if (ui.WasEntered("TOLIST")) {
      summary.AddComment("The output cube list file, ["
                         + ui.GetFileName("TOLIST") + 
                         "], was not created. "
                         "The provided filters have resulted in an empty"
                         "Control Network.");
    }
  }


  if(noIgnore) {
    summary.AddKeyword(PvlKeyword("IgnoredPoints", toString((int)ignoredPoints.size())));
    summary.AddKeyword(PvlKeyword("IgnoredMeasures", toString((int)ignoredMeasures.size())));
  }
  if(noSingleMeasure) {
    summary.AddKeyword(PvlKeyword("SingleMeasurePoints", toString((int)singleMeasurePoints.size())));
  }
  if(noMeasureless) {
    summary.AddKeyword(PvlKeyword("MeasurelessPoints", toString((int)measurelessPoints.size())));
  }
  if(noTolerancePoints) {
    summary.AddKeyword(PvlKeyword("TolerancePoints", toString((int)tolerancePoints.size())));
  }
  if(reference) {
    summary.AddKeyword(PvlKeyword("NonReferenceMeasures", toString((int)nonReferenceMeasures.size())));
  }
  if(fixed) {
    summary.AddKeyword(PvlKeyword("NonFixedPoints", toString((int)nonFixedPoints.size())));
  }
  if(cubePoints) {
    summary.AddKeyword(PvlKeyword("NonCubePoints", toString((int)nonCubePoints.size())));
  }
// This is commented out since this does not correspond to any filters or the
//  documentation of this application. I did not delete the code in case we find
//  that we need it later. J.Backer 2012-06-22
// 
//  if(noMeasurePoints.size() != 0) {
//    summary.AddKeyword(PvlKeyword("NoCubeMeasure", toString((int)noMeasurePoints.size())));
//  }
  if(cubeMeasures) {
    summary.AddKeyword(PvlKeyword("NonCubeMeasures", toString((int)noCubeMeasures.size())));
  }
  if(pointsEntered) {
    summary.AddKeyword(PvlKeyword("NonListedPoints", toString((int)nonListedPoints.size())));
  }
  if(latLon) {
    summary.AddKeyword(PvlKeyword("LatLonOutOfRange", toString((int)nonLatLonPoints.size())));
    summary.AddKeyword(PvlKeyword("NoLatLonPoints", toString((int)cannotGenerateLatLonPoints.size())));
  }

  outProgress.CheckStatus();

  // Log Control Net results
  Application::Log(summary);

  outProgress.CheckStatus();

  Progress resultsProgress;
  if(ui.WasEntered("PREFIX")) {
    if (outputPoints == inputPoints && outputMeasures == inputMeasures) {
      results.AddComment("No filter reports were created since all points and "
                         "measures from the input control network were "
                         "extracted into the output control network.");
    }
    else {
      resultsProgress.SetText("Writing Results");
      resultsProgress.SetMaximumSteps(11);
      resultsProgress.CheckStatus();

      QString prefix = ui.GetString("PREFIX");

      if(noIgnore) {
        QString namecp = FileName(prefix + "IgnoredPoints.txt").expanded();
        WriteResults(namecp, ignoredPoints, results);
        QString namecm = FileName(prefix + "IgnoredMeasures.txt").expanded();
        WriteResults(namecm, ignoredMeasures, results);
      }

      resultsProgress.CheckStatus();

      if(noSingleMeasure) {
        QString name = FileName(prefix + "SingleMeasurePoints.txt").expanded();
        WriteResults(name, singleMeasurePoints, results);
      }

      resultsProgress.CheckStatus();

      if(noMeasureless) {
        QString name = FileName(prefix + "MeasurelessPoints.txt").expanded();
        WriteResults(name, measurelessPoints, results);
      }

      resultsProgress.CheckStatus();

      if(noTolerancePoints) {
        QString name = FileName(prefix + "TolerancePoints.txt").expanded();
        WriteResults(name, tolerancePoints, results);
      }

      resultsProgress.CheckStatus();

      if(reference) {
        QString name = FileName(prefix + "NonReferenceMeasures.txt").expanded();
        WriteResults(name, nonReferenceMeasures, results);
      }
  
      resultsProgress.CheckStatus();
  
      if(fixed) {
        QString name = FileName(prefix + "NonFixedPoints.txt").expanded();
        WriteResults(name, nonFixedPoints, results);
      }
  
      resultsProgress.CheckStatus();
  
      if(cubePoints) {
        QString name = FileName(prefix + "NonCubePoints.txt").expanded();
        WriteResults(name, nonCubePoints, results);
      }
  
      resultsProgress.CheckStatus();

// This is commented out since this does not correspond to any filters or the
//  documentation of this application. I did not delete the code in case we find
//  that we need it later. J.Backer 2012-06-22
//   
//      if(noMeasurePoints.size() != 0) {
//        QString name = FileName(prefix + "NoMeasurePoints.txt").expanded();
//        WriteResults(name, noMeasurePoints);
//      }
  
      resultsProgress.CheckStatus();
  
      if(cubeMeasures) {
        QString name = FileName(prefix + "NonCubeMeasures.txt").expanded();
        WriteResults(name, noCubeMeasures, results);
      }
  
      resultsProgress.CheckStatus();
  
      if(pointsEntered) {
        QString name = FileName(prefix + "NonListedPoints.txt").expanded();
        WriteResults(name, nonListedPoints, results);
      }
  
      resultsProgress.CheckStatus();
  
      if(latLon) {
        QString namenon = FileName(prefix + "LatLonOutOfRange.txt").expanded();
        WriteResults(namenon, nonLatLonPoints, results);
        QString namegen = FileName(prefix + "NoLatLonPoints.txt").expanded();
        WriteResults(namegen, cannotGenerateLatLonPoints, results);
      }

      results.AddComment("Each keyword represents a filter parameter used. "
                         "Check the documentation for specific keyword descriptions.");
    }
    Application::Log(results);

    resultsProgress.CheckStatus();
  }

}


/**
 * Removes control points not listed in POINTLIST
 *
 * @param outNet The output control net being removed from
 * @param nonListedPoints The keyword recording all of the control points
 *                        removed due to not being listed
 * @history 2012-06-22 - Jeannie Backer - Changed nonListedPoints parameter to 
 *                           reference so that this information could be
 *                           accessed in the main program.
 */
void ExtractPointList(ControlNet &outNet, QVector<QString> &nonListedPoints) {
  UserInterface &ui = Application::GetUserInterface();

  // Use the file list class for functionality 
  // (even though this is a list of point IDs)
  FileList listedPoints(ui.GetFileName("POINTLIST"));

  // loop through control point indices, backwards
  for(int cp = outNet.GetNumPoints() - 1; cp >= 0; cp--) {
    ControlPoint *controlpt = outNet.GetPoint(cp);
    // flag to determine if this controlpoint is in the pointlist
    bool isInList = false;
    // loop through line numbers of POINTLIST until we find a point ID in the 
    // list that matches this control point's ID
    for(int i = 0; i < (int)listedPoints.size()  &&  !isInList; i++) {
      QString pointId = listedPoints[i].toString();
      // isInList is true if these strings are equal
      isInList = pointId.toLower() == controlpt->GetId().toLower();
    }
    if(!isInList) {
      nonListedPoints.append(controlpt->GetId());
      omit(outNet, cp);
    }
  }
  return;
}


/**
 * Removes control points not in the lat/lon range provided in the unput
 * parameters.
 *
 * @param outNet The output control net being removed from
 * @param noLanLonPoint The keyword recording all of the control points removed
 *                      due to the provided lat/lon range
 * @param noLanLonPoint The keyword recording all of the control points removed
 *                      due to the inability to calculate the lat/lon for that
 *                      point
 */
void ExtractLatLonRange(ControlNet &outNet, QVector<QString> nonLatLonPoints,
                        QVector<QString> cannotGenerateLatLonPoints,  QMap<QString, QString> sn2filename) {
  if(outNet.GetNumPoints() == 0) {
    return;
  }

  UserInterface &ui = Application::GetUserInterface();

  // Get the lat/lon and fix the range for the internal 0/360
  Latitude minlat(ui.GetDouble("MINLAT"), Angle::Degrees);
  Latitude maxlat(ui.GetDouble("MAXLAT"), Angle::Degrees);
  Longitude minlon = Longitude(ui.GetDouble("MINLON"), Angle::Degrees);
  Longitude maxlon = Longitude(ui.GetDouble("MAXLON"), Angle::Degrees);

  Progress progress;
  progress.SetText("Calculating lat/lon");
  progress.SetMaximumSteps(outNet.GetNumPoints());
  progress.CheckStatus();

  CubeManager manager;
  manager.SetNumOpenCubes(50);   //Should keep memory usage to around 1GB

  bool hasFromList = ui.WasEntered("FROMLIST");
  for(int cp = outNet.GetNumPoints() - 1; cp >= 0; cp --) {
    progress.CheckStatus();
    const ControlPoint *controlPt = outNet.GetPoint(cp);
    SurfacePoint surfacePt = controlPt->GetBestSurfacePoint();

    // If the Contorl Network takes priority, use it
    if(surfacePt.Valid()) {
      if(NotInLatLonRange(surfacePt, minlat, maxlat, minlon, maxlon)) {
        nonLatLonPoints.push_back(controlPt->GetId());
        omit(outNet, cp);
      }
    }

    /**
     * If the lat/lon cannot be determined from the point, then we need to calculate
     * lat/lon on our own
     */
    else if(hasFromList) {

      // Find a cube in the Control Point to get the lat/lon from
      int cm = 0;
      QString sn = "";
      Latitude lat;
      Longitude lon;
      Distance radius;

      // First check the reference Measure
      //if(!sn2filename[controlPt[cm].GetCubeSerialNumber()].length() == 0) {
      if(!sn2filename[controlPt->GetReferenceSN()].length() == 0) {
        sn = controlPt->GetReferenceSN();
      }

      // Search for other Control Measures if needed
      if(sn.isEmpty()) {
        // Find the Serial Number if it exists
        for(int cm = 0; (cm < controlPt->GetNumMeasures()) && sn.isEmpty(); cm ++) {
          if(!sn2filename[controlPt->GetReferenceSN()].length() == 0) {
            sn = controlPt->GetReferenceSN();
          }
        }
      }

      // Connot fine a cube to get the lat/lon from
      if(sn.isEmpty()) {
        cannotGenerateLatLonPoints.push_back(controlPt->GetId());
        omit(outNet, cp);
      }

      // Calculate the lat/lon and check for validity
      else {
        bool remove = false;

        Cube *cube = manager.OpenCube(sn2filename[sn]);
        Camera *camera = cube->getCamera();

        if(camera == NULL) {
          try {
            Projection *projection =
              ProjectionFactory::Create((*(cube->getLabel())));

            if(!projection->SetCoordinate(controlPt->GetMeasure(cm)->GetSample(),
                                          controlPt->GetMeasure(cm)->GetLine())) {
              nonLatLonPoints.push_back(controlPt->GetId());
              remove = true;
            }

            lat = Latitude(projection->Latitude(), Angle::Degrees);
            lon = Longitude(projection->Longitude(), Angle::Degrees);
            radius = Distance(projection->LocalRadius(), Distance::Meters);

            delete projection;
            projection = NULL;
          }
          catch(IException &) {
            remove = true;
          }
        }
        else {
          if(!camera->SetImage(controlPt->GetMeasure(cm)->GetSample(),
                               controlPt->GetMeasure(cm)->GetLine())) {
            nonLatLonPoints.push_back(controlPt->GetId());
            remove = true;
          }

          lat = camera->GetLatitude();
          lon = camera->GetLongitude();
          radius = camera->LocalRadius();

          camera = NULL;
        }

        cube = NULL;

        bool notInRange = false;
        bool validLatLonRadius = lat.isValid() && lon.isValid() && radius.isValid();
        if(validLatLonRadius) {
          SurfacePoint sfpt(lat, lon, radius);
          notInRange = NotInLatLonRange(sfpt, minlat, maxlat, minlon, maxlon);
        }

        if(remove || notInRange) {
          nonLatLonPoints.push_back(controlPt->GetId());
          omit(outNet, cp);
        }
        else if(validLatLonRadius) { // Add the reference lat/lon/radius to the Control Point
          outNet.GetPoint(cp)->SetAprioriSurfacePoint(SurfacePoint(lat, lon, radius));
        }
      }
    }
    else {
      cannotGenerateLatLonPoints.push_back(controlPt->GetId());
      omit(outNet, cp);
    }

  }

  manager.CleanCubes();
}


/**
 * Checks whether the given surface point is in the given lat/lon range, 
 * handling the meridian correctly 
 *
 * @param lat The latitude to check
 * @param lon The longitude to check
 * @param minlat Minimum Latitude Minimum valid latitude
 * @param maxlat Maximum Latitude Maximum valid latitude
 * @param minlon Minimum Longitude Minimum valid longitude
 * @param maxlon Maximum Longitude Maximum valid longitude
 *
 * @return bool True when the range is valid
 */
bool NotInLatLonRange(SurfacePoint surfacePtToTest, Latitude minlat,
                      Latitude maxlat, Longitude minlon, Longitude maxlon) {
  Latitude lat = surfacePtToTest.GetLatitude();
  Longitude lon = surfacePtToTest.GetLongitude();

  bool outRange = false;
  try {
    outRange = !lat.inRange(minlat, maxlat) || !lon.inRange(minlon, maxlon);
  }
  catch (IException &e) {
    QString msg = "Cannot complete lat/lon range test with given filters";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  return outRange;
}


/**
 * Creates the output list, [TOLIST], if the parameter is entered. This method
 * finds all cubes contained within the given Control Network and lists the 
 * corresponding file names for these cubes in the [TOLIST] output file. 
 *
 * @param cnet The Control Network from which the cube list will be found and 
 *             created.
 * @param sn2file The map for converting the Control Network's serial numbers
 *                to filenames.
 */
void WriteCubeOutList(ControlNet cnet, 
                      QMap<QString, QString> sn2file, 
                      PvlGroup &summary) {
  UserInterface &ui = Application::GetUserInterface();

  Progress p;
  p.SetText("Writing Cube List");
  try {
    p.SetMaximumSteps(cnet.GetNumPoints());
    p.CheckStatus();
  }
  catch(IException &e) {
    QString msg = "Unable to write the output cube list, [TOLIST].";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  std::set<QString> outputsn;
  for(int cp = 0; cp < cnet.GetNumPoints(); cp ++) {
    for(int cm = 0; cm < cnet.GetPoint(cp)->GetNumMeasures(); cm ++) {
      QString sn = cnet.GetPoint(cp)->GetMeasure(cm)->GetCubeSerialNumber();
      if (sn2file[sn].length() != 0) {
        outputsn.insert(sn);
        }
      }
    p.CheckStatus();
  }
  // Don't create file if it will be empty
  if (outputsn.size() == 0) {
    summary.AddComment("The output cube list file, ["
                       + ui.GetFileName("TOLIST") + 
                       "], was not created. "
                       "The provided filters have resulted in an empty"
                       "Control Network.");
    return;
  }

  QString toList = ui.GetFileName("TOLIST");
  std::ofstream out_stream;
  out_stream.open(toList.toAscii().data(), std::ios::out);
  out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

  for(std::set<QString>::iterator sn = outputsn.begin(); sn != outputsn.end(); sn ++) {
    // moved the "if" statement to the previous for loop to prevent creating
    // an empty file
    //if(!sn2file[(*sn)].length() == 0) { 
      out_stream << sn2file[(*sn)] << std::endl;
    //}
  }

  out_stream.close();
  return;
}


/**
 * Creates the results report using the given file name and vector of measures 
 * or points that were not extracted. 
 *
 * @param filename The name of the report file to create. 
 * @param notExtracted  A vector of points and/or measures to be listed in the 
 *                      report. 
 * @param results  A reference to the results PvlGroup address.
 */
void WriteResults(QString filename, 
                  QVector<QString> notExtracted, 
                  PvlGroup &results) {
  // if no points or measures are being extracted,
  // we will not create the filter report.
  if(notExtracted.size() == 0) {
    results.AddComment("The output report ["+ filename +"] was not created. "
                       "The corresponding filter found no points/measures that "
                       "would not be extracted.");
    return;
  }

  // Set up the output file for writing
  std::ofstream out_stream;
  out_stream.open(filename.toAscii().data(), std::ios::out);
  out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

  out_stream << notExtracted[0];
  for(int index = 1; index < notExtracted.size(); index ++) {
    out_stream << std::endl << notExtracted[index];
  }

  out_stream.close();
  results.AddKeyword(PvlKeyword("ReportCreated", filename));
  return;
}

/** 
 * This method removes the given control point from the given control network.
 */ 
void omit(ControlNet &cnet, int cp) {
  ControlPoint *point = cnet.GetPoint(cp);
  if (point->IsEditLocked()) point->SetEditLock(false);
  cnet.DeletePoint(cp);
  return;
}

/**
 * This method removes the given control measure from the given control point.
 */
void omit(ControlPoint *point, int cm) {
  ControlMeasure *measure = point->GetMeasure(cm);
  if (measure->IsEditLocked()) measure->SetEditLock(false);
  point->Delete(cm);
  return;
}


//   Modified [PREFIX]IgnoredMeasures.txt output to exclude
//      ignored reference measures since these measures will be extracted.
