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
#include "iException.h"
#include "iString.h"
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

void ExtractPointList(ControlNet &outNet, QVector<iString> nonListedPoints);
void ExtractLatLonRange(ControlNet &outNet, QVector<iString> nonLatLonPoints,
                        QVector<iString> cannotGenerateLatLonPoints,
                        QMap<iString, iString> sn2filename);
bool NotInLatLonRange(SurfacePoint surfacePt, Latitude minlat,
                      Latitude maxlat, Longitude minlon, Longitude maxlon);
void WriteCubeOutList(ControlNet cnet, QMap<iString, iString> sn2file);
void WriteResults(iString filename, QVector<iString> results);

// Main program
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  if(!ui.WasEntered("FROMLIST") && ui.WasEntered("TOLIST")) {
    std::string msg = "To create a [TOLIST] the [FROMLIST] parameter must be provided.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
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
    std::string msg = "At least one filter must be selected [";
    msg += "NOIGNORE,NOMEASURELESS,NOSINGLEMEASURE,REFERENCE,FIXED,TOLERANCE,";
    msg += "POINTLIST,CUBES,LATLON]";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  if(cubeMeasures || reference) {
    noMeasureless = true;
  }

  // Gets the input parameters
  ControlNet outNet(ui.GetFilename("CNET"));
  FileList inList;
  if(ui.WasEntered("FROMLIST")) {
    inList = ui.GetFilename("FROMLIST");
  }

  // Set up the Serial Number to Filename mapping
  QMap<iString, iString> sn2filename;
  for(int cubeIndex = 0; cubeIndex < (int)inList.size(); cubeIndex ++) {
    iString sn = SerialNumber::Compose(inList[cubeIndex]);
    sn2filename[sn] = inList[cubeIndex];
  }


  Progress progress;
  progress.SetMaximumSteps(outNet.GetNumPoints());
  progress.CheckStatus();

  // Set up vector records of how points/measures are removed
  QVector<iString> ignoredPoints;
  QVector<iString> ignoredMeasures;
  QVector<iString> singleMeasurePoints;
  QVector<iString> measurelessPoints;
  QVector<iString> tolerancePoints;
  QVector<iString> nonReferenceMeasures;
  QVector<iString> nonFixedPoints;
  QVector<iString> nonCubePoints;
  QVector<iString> noCubeMeasures;
  QVector<iString> noMeasurePoints;
  QVector<iString> nonListedPoints;
  QVector<iString> nonLatLonPoints;
  QVector<iString> cannotGenerateLatLonPoints;

  // Set up comparison data
  QVector<iString> serialNumbers;
  if(cubePoints) {
    FileList cubeList(ui.GetFilename("CUBELIST"));
    for(int cubeIndex = 0; cubeIndex < (int)cubeList.size(); cubeIndex ++) {
      iString sn = SerialNumber::Compose(cubeList[cubeIndex]);
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
      outNet.DeletePoint(cp);
      continue;
    }
    if(fixed && !(controlpt->GetType() == ControlPoint::Fixed)) {
      nonFixedPoints.append(controlpt->GetId());
      outNet.DeletePoint(cp);
      continue;
    }

    if(noSingleMeasure) {
      bool invalidPoint = false;
      invalidPoint |= noIgnore && (controlpt->GetNumValidMeasures() < 2);
      invalidPoint |= controlpt->GetNumMeasures() < 2 && (controlpt->GetType() != ControlPoint::Fixed);

      if(invalidPoint) {
        singleMeasurePoints.append(controlpt->GetId());
        outNet.DeletePoint(cp);
        continue;
      }
    }

    // Change the current point into a new point by manipulation of its control measures
    ControlPoint *newPoint = outNet.GetPoint(cp);

    bool shouldDeleteReferenceMeasure = false;
    for(int cm = newPoint->GetNumMeasures() - 1; cm >= 0; cm --) {
      const ControlMeasure *newMeasure = newPoint->GetMeasure(cm);

      if(noIgnore && newMeasure->IsIgnored()) {
        ignoredMeasures.append(newPoint->GetId() + "," + newMeasure->GetCubeSerialNumber());
        //New error with deleting Reference Measures
        if(newPoint->GetRefMeasure() != newMeasure)
          newPoint->Delete(cm);
        else
          shouldDeleteReferenceMeasure = true;
      }
      else if(reference && newPoint->GetRefMeasure() != newMeasure) {
        nonReferenceMeasures.append(newPoint->GetId() + "," + newMeasure->GetCubeSerialNumber());
        newPoint->Delete(cm);
      }
      else if(cubeMeasures) {
        bool hasSerialNumber = false;
        std::string serialNum = newMeasure->GetCubeSerialNumber();
        for(int sn = 0; sn < serialNumbers.size(); sn ++) {
          if(serialNumbers[sn] == serialNum) {
            hasSerialNumber = true;
            break;
          }
        }

        if(!hasSerialNumber) {
          string msg = newPoint->GetId() + "," + newMeasure->GetCubeSerialNumber();
          //Delete Reference Measures not in the list, if retainReference is turned off
          if(newPoint->GetRefMeasure() != newMeasure || 
             (newPoint->GetRefMeasure() == newMeasure && !retainReference))
            newPoint->Delete(cm);
          else {
            shouldDeleteReferenceMeasure = true;
            if(newPoint->GetRefMeasure() == newMeasure && retainReference) {
              msg += ", Reference not in the list but Retained";
            }
          }
          
          noCubeMeasures.append(msg);
        }
      }
    }

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
        outNet.DeletePoint(cp);
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
        outNet.DeletePoint(cp);
        continue;
      }
    }

    // Do not add outPoint if it does not have a cube in CUBELIST as asked
    if(cubePoints && !cubeMeasures) {
      bool hasSerialNumber = false;

      for(int cm = 0; cm < newPoint->GetNumMeasures() && !hasSerialNumber; cm ++) {
        for(int sn = 0; sn < serialNumbers.size() && !hasSerialNumber; sn ++) {
          if(serialNumbers[sn] == newPoint->GetMeasure(cm)->GetCubeSerialNumber())
            hasSerialNumber = true;
        }
      }

      if(!hasSerialNumber) {
        nonCubePoints.append(newPoint->GetId());
        outNet.DeletePoint(cp);
        continue;
      }
    }

    if(noMeasureless && newPoint->GetNumMeasures() == 0) {
      noMeasurePoints.append(newPoint->GetId());
      outNet.DeletePoint(cp);
      continue;
    }
  } //! Finished with simple comparisons


  /**
   * Use another pass to check for Ids
   */
  if(pointsEntered) {
    ExtractPointList(outNet, nonListedPoints);
  }


  /**
   *  Use another pass on outNet, because this is by far the most time consuming
   *  process, and time could be saved by using the reduced size of outNet
   */
  if(latLon) {
    ExtractLatLonRange(outNet, nonLatLonPoints, cannotGenerateLatLonPoints, sn2filename);
  }


  // Write the filenames associated with outNet
  WriteCubeOutList(outNet, sn2filename);

  Progress outProgress;
  outProgress.SetText("Writing Control Network");
  outProgress.SetMaximumSteps(3);
  outProgress.CheckStatus();

  // Write the extracted Control Network
  outNet.Write(ui.GetFilename("ONET"));

  outProgress.CheckStatus();

  // Adds the remove history to the summary and results group
  PvlGroup summary("ResultSummary");
  PvlGroup results("Results");

  if(noIgnore) {
    summary.AddKeyword(PvlKeyword("IgnoredPoints", iString((int)ignoredPoints.size())));
    summary.AddKeyword(PvlKeyword("IgnoredMeasures", iString((int)ignoredMeasures.size())));
  }
  if(noSingleMeasure) {
    summary.AddKeyword(PvlKeyword("SingleMeasurePoints", iString((int)singleMeasurePoints.size())));
  }
  if(noMeasureless) {
    summary.AddKeyword(PvlKeyword("MeasurelessPoints", iString((int)measurelessPoints.size())));
  }
  if(noTolerancePoints) {
    summary.AddKeyword(PvlKeyword("TolerancePoints", iString((int)tolerancePoints.size())));
  }
  if(reference) {
    summary.AddKeyword(PvlKeyword("NonReferenceMeasures", iString((int)nonReferenceMeasures.size())));
  }
  if(fixed) {
    summary.AddKeyword(PvlKeyword("NonFixedPoints", iString((int)nonFixedPoints.size())));
  }
  if(cubePoints) {
    summary.AddKeyword(PvlKeyword("NonCubePoints", iString((int)nonCubePoints.size())));
  }
  if(noMeasurePoints.size() != 0) {
    summary.AddKeyword(PvlKeyword("NoCubeMeasure", iString((int)noMeasurePoints.size())));
  }
  if(cubeMeasures) {
    summary.AddKeyword(PvlKeyword("NoMeasurePoints", iString((int)noCubeMeasures.size())));
  }
  if(pointsEntered) {
    summary.AddKeyword(PvlKeyword("NonListedPoints", iString((int)nonListedPoints.size())));
  }
  if(latLon) {
    summary.AddKeyword(PvlKeyword("LatLonOutOfRange", iString((int)nonLatLonPoints.size())));
    summary.AddKeyword(PvlKeyword("NoLatLonPoints", iString((int)cannotGenerateLatLonPoints.size())));
  }

  outProgress.CheckStatus();

  // Log Control Net results
  Application::Log(summary);

  outProgress.CheckStatus();

  if(ui.WasEntered("PREFIX")) {
    Progress resultsProgress;
    resultsProgress.SetText("Writing Results");
    resultsProgress.SetMaximumSteps(11);
    resultsProgress.CheckStatus();

    std::string prefix = ui.GetString("PREFIX");

    if(noIgnore) {
      iString namecp = Filename(prefix + "IgnoredPoints.txt").Expanded();
      WriteResults(namecp, ignoredPoints);
      iString namecm = Filename(prefix + "IgnoredMeasures.txt").Expanded();
      WriteResults(namecm, ignoredMeasures);
    }

    resultsProgress.CheckStatus();

    if(noSingleMeasure) {
      iString name = Filename(prefix + "SingleMeasurePoints.txt").Expanded();
      WriteResults(name, singleMeasurePoints);
    }

    resultsProgress.CheckStatus();

    if(noMeasureless) {
      iString name = Filename(prefix + "MeasurelessPoints.txt").Expanded();
      WriteResults(name, measurelessPoints);
    }

    resultsProgress.CheckStatus();

    if(noTolerancePoints) {
      iString name = Filename(prefix + "TolerancePoints.txt").Expanded();
      WriteResults(name, tolerancePoints);
    }

    resultsProgress.CheckStatus();

    if(reference) {
      iString name = Filename(prefix + "NonReferenceMeasures.txt").Expanded();
      WriteResults(name, nonReferenceMeasures);
    }

    resultsProgress.CheckStatus();

    if(fixed) {
      iString name = Filename(prefix + "NonFixedPoints.txt").Expanded();
      WriteResults(name, nonFixedPoints);
    }

    resultsProgress.CheckStatus();

    if(cubePoints) {
      iString name = Filename(prefix + "NonCubePoints.txt").Expanded();
      WriteResults(name, nonCubePoints);
    }

    resultsProgress.CheckStatus();

    if(noMeasurePoints.size() != 0) {
      iString name = Filename(prefix + "NoMeasurePoints.txt").Expanded();
      WriteResults(name, noMeasurePoints);
    }

    resultsProgress.CheckStatus();

    if(cubeMeasures) {
      iString name = Filename(prefix + "NonCubeMeasures.txt").Expanded();
      WriteResults(name, noCubeMeasures);
    }

    resultsProgress.CheckStatus();

    if(pointsEntered) {
      iString name = Filename(prefix + "NonListedPoints.txt").Expanded();
      WriteResults(name, nonListedPoints);
    }

    resultsProgress.CheckStatus();

    if(latLon) {
      iString namenon = Filename(prefix + "LatLonOutOfRange.txt").Expanded();
      WriteResults(namenon, nonLatLonPoints);
      iString namegen = Filename(prefix + "NoLatLonPoints.txt").Expanded();
      WriteResults(namegen, cannotGenerateLatLonPoints);
    }

    results.AddComment("Each keyword represents a filter parameter used." \
                       " Check the documentation for specific keyword descriptions.");
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
 */
void ExtractPointList(ControlNet &outNet, QVector<iString> nonListedPoints) {
  UserInterface &ui = Application::GetUserInterface();

  FileList listedPoints(ui.GetFilename("POINTLIST"));

  for(int cp = outNet.GetNumPoints() - 1; cp >= 0; cp --) {
    ControlPoint *controlpt = outNet.GetPoint(cp);
    bool isInList = false;
    for(int pointId = 0; pointId < (int)listedPoints.size()  &&  !isInList; pointId ++) {
      isInList = controlpt->GetId().compare(listedPoints[pointId]) == 0;
    }

    if(!isInList) {
      nonListedPoints.append(controlpt->GetId());
      outNet.DeletePoint(cp);
    }
  }
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
void ExtractLatLonRange(ControlNet &outNet, QVector<iString> nonLatLonPoints,
                        QVector<iString> cannotGenerateLatLonPoints,  QMap<iString, iString> sn2filename) {
  if(outNet.GetNumPoints() == 0) {
    return;
  }

  UserInterface &ui = Application::GetUserInterface();

  // Get the lat/lon and fix the range for the internal 0/360
  Latitude minlat(ui.GetDouble("MINLAT"), Angle::Degrees);
  Latitude maxlat(ui.GetDouble("MAXLAT"), Angle::Degrees);
  Longitude minlon = Longitude(ui.GetDouble("MINLON"), Angle::Degrees).Force360Domain();
  Longitude maxlon = Longitude(ui.GetDouble("MAXLON"), Angle::Degrees).Force360Domain();

  Progress progress;
  progress.SetText("Calculating lat/lon");
  progress.SetMaximumSteps(outNet.GetNumPoints());
  progress.CheckStatus();

  CubeManager manager;
  manager.SetNumOpenCubes(50);   //Should keep memory usage to around 1GB

  for(int cp = outNet.GetNumPoints() - 1; cp >= 0; cp --) {
    progress.CheckStatus();
    const ControlPoint *controlPt = outNet.GetPoint(cp);
    SurfacePoint surfacePt = controlPt->GetBestSurfacePoint();

    // If the Contorl Network takes priority, use it
    //Latitude pointLat(controlPt.UniversalLatitude(),Angle::Degrees);
    //Longitude pointLon(controlPt.UniversalLongitude(),Angle::Degrees);
    //bool hasLatLon = pointLat != Isis::Null && pointLon != Isis::Null;
    if(controlPt->GetType() == Isis::ControlPoint::Fixed || surfacePt.Valid()) {
      if(NotInLatLonRange(surfacePt, minlat, maxlat, minlon, maxlon)) {
        nonLatLonPoints.push_back(controlPt->GetId());
        outNet.DeletePoint(cp);
      }
    }

    /**
     * If the lat/lon cannot be determined from the point, then we need to calculate
     * lat/lon on our own
     */
    else if(ui.WasEntered("FROMLIST")) {

      // Find a cube in the Control Point to get the lat/lon from
      int cm = 0;
      iString sn = "";
      Latitude lat;
      Longitude lon;
      Distance radius;

      // First check the reference Measure
      //if(!sn2filename[controlPt[cm].GetCubeSerialNumber()].length() == 0) {
      if(!sn2filename[controlPt->GetReferenceSN()].length() == 0) {
        sn = controlPt->GetReferenceSN();
      }

      // Search for other Control Measures if needed
      if(sn.empty()) {
        // Find the Serial Number if it exists
        for(int cm = 0; (cm < controlPt->GetNumMeasures()) && sn.empty(); cm ++) {
          if(!sn2filename[controlPt->GetReferenceSN()].length() == 0) {
            sn = controlPt->GetReferenceSN();
          }
        }
      }

      // Connot fine a cube to get the lat/lon from
      if(sn.empty()) {
        cannotGenerateLatLonPoints.push_back(controlPt->GetId());
        outNet.DeletePoint(cp);
      }

      // Calculate the lat/lon and check for validity
      else {
        bool remove = false;

        Cube *cube = manager.OpenCube(sn2filename[sn]);
        Camera *camera = cube->Camera();

        if(camera == NULL) {
          try {
            Projection *projection = ProjectionFactory::Create((*(cube->Label())));

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
          catch(iException &e) {
            remove = true;
            e.Clear();
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
        bool validLatLonRadius = lat.Valid() && lon.Valid() && radius.Valid();
        if(validLatLonRadius) {
          SurfacePoint sfpt(lat, lon, radius);
          notInRange = NotInLatLonRange(sfpt, minlat, maxlat, minlon, maxlon);
        }

        if(remove || notInRange) {
          nonLatLonPoints.push_back(controlPt->GetId());
          outNet.DeletePoint(cp);
        }
        else if(validLatLonRadius) { // Add the reference lat/lon/radius to the Control Point
          outNet.GetPoint(cp)->SetAprioriSurfacePoint(SurfacePoint(lat, lon, radius));
        }
      }
    }
    else {
      cannotGenerateLatLonPoints.push_back(controlPt->GetId());
      outNet.DeletePoint(cp);
    }

  }

  manager.CleanCubes();
}


/**
 * Checks for correct lat/lon range, handling the meridian correctly
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
  bool inRange = true;
  Latitude lat = surfacePtToTest.GetLatitude();
  Longitude lon = surfacePtToTest.GetLongitude();

  // Check latitude range
  if(inRange && minlat > maxlat) {
    inRange &= (lat <= maxlat || lat >= minlat);
  }
  else if(inRange) {
    inRange &= (lat >= minlat && lat <= maxlat);
  }

  // Check longitude range
  if(inRange && minlon > maxlon) {
    inRange &= (lon <= maxlon || lon >= minlon);
  }
  else if(inRange) {
    inRange &= (lon >= minlon && lon <= maxlon);
  }

  return !inRange;
}


/**
 * Finds and writes all input cubes contained within the given Control Network
 * to the output file list
 *
 * @param cnet The Control Network to list the filenames contained within
 * @param sn2file The map for converting the Control Network's serial numbers
 *                to filenames
 */
void WriteCubeOutList(ControlNet cnet, QMap<iString, iString> sn2file) {
  UserInterface &ui = Application::GetUserInterface();

  if(ui.WasEntered("TOLIST")) {

    Progress p;
    p.SetText("Writing Cube List");
    try {
      p.SetMaximumSteps(cnet.GetNumPoints());
      p.CheckStatus();
    }
    catch(iException &e) {
      e.Clear();
      std::string msg = "The provided filters have resulted in an empty Control Network.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    std::set<iString> outputsn;
    for(int cp = 0; cp < cnet.GetNumPoints(); cp ++) {
      for(int cm = 0; cm < cnet.GetPoint(cp)->GetNumMeasures(); cm ++) {
        outputsn.insert(cnet.GetPoint(cp)->GetMeasure(cm)->GetCubeSerialNumber());
      }
      p.CheckStatus();
    }

    std::string toList = ui.GetFilename("TOLIST");
    std::ofstream out_stream;
    out_stream.open(toList.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(std::set<iString>::iterator sn = outputsn.begin(); sn != outputsn.end(); sn ++) {
      if(!sn2file[(*sn)].length() == 0) {
        out_stream << sn2file[(*sn)] << std::endl;
      }
    }

    out_stream.close();
  }
}


/**
 * Places the output
 *
 * @param filename The file to write the vector of results to
 * @param results  A vector of points and/or measures not extracted
 */
void WriteResults(iString filename, QVector<iString> results) {
  if(results.size() == 0) {
    return;
  }

  // Set up the output file for writing
  std::ofstream out_stream;
  out_stream.open(filename.c_str(), std::ios::out);
  out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

  out_stream << results[0];
  for(int index = 1; index < results.size(); index ++) {
    out_stream << std::endl << results[index];
  }

  out_stream.close();
}
