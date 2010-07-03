#include "Isis.h"

#include <map>
#include <set>
#include <sstream>

#include "CameraFactory.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "CubeManager.h"
#include "FileList.h"
#include "iException.h"
#include "iString.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "SerialNumber.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void ExtractPointList(ControlNet &outNet, vector<string> & nonListedPoints);
void ExtractLatLonRange(ControlNet &outNet, vector<string> & nonLatLonPoints,
                        vector<string> & cannotGenerateLatLonPoints, map<iString, iString> sn2filename);
bool NotInLatLonRange(double lat, double lon, double minlat,
                      double maxlat, double minlon, double maxlon);
void WriteCubeOutList(ControlNet cnet, map<iString, iString> sn2file);
void WriteResults(string filename, vector<string> results);

// Main program
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  if(!ui.WasEntered("FROMLIST") && ui.WasEntered("TOLIST")) {
    std::string msg = "To create a [TOLIST] the [FROMLIST] parameter must be provided.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  bool noIgnore          = ui.GetBoolean("NOIGNORE");
  bool noHeld            = ui.GetBoolean("NOHELD");
  bool noSingleMeasure   = ui.GetBoolean("NOSINGLEMEASURES");
  bool noMeasureless     = ui.GetBoolean("NOMEASURELESS");
  bool noTolerancePoints = ui.GetBoolean("TOLERANCE");
  bool reference         = ui.GetBoolean("REFERENCE");
  bool ground            = ui.GetBoolean("GROUND");
  bool cubePoints        = ui.GetBoolean("CUBES");
  bool cubeMeasures      = ui.GetBoolean("CUBEMEASURES");
  bool pointsEntered     = ui.WasEntered("POINTLIST");
  bool latLon            = ui.GetBoolean("LATLON");

  if(!(noIgnore || noHeld || noSingleMeasure || noMeasureless || noTolerancePoints ||
       reference || ground || cubePoints || pointsEntered || latLon)) {
    std::string msg = "At least one filter must be selected [";
    msg += "NOIGNORE,NOHELD,NOSINGLEMEASURE,TOLERANCE,REFERENCE,GROUND,";
    msg += "POINTLIST,LATLON]";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  /*else if( cubeMeasures  &&  !cubePoints )
  {
      std::string msg = "When CUBEMEASURES is selected, CUBEPOINTS must be given";
      msg += " a list of cubes.";
      throw iException::Message( iException::User, msg, _FILEINFO_ );
  }*/

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
  map<iString, iString> sn2filename;
  for(int cubeIndex = 0; cubeIndex < (int)inList.size(); cubeIndex ++) {
    iString sn = SerialNumber::Compose(inList[cubeIndex]);
    sn2filename[sn] = inList[cubeIndex];
  }


  Progress progress;
  progress.SetMaximumSteps(outNet.Size());
  progress.CheckStatus();

  // Set up verctor records of how points/measures are removed
  vector<string> ignoredPoints;
  vector<string> ignoredMeasures;
  vector<string> heldPoints;
  vector<string> singleMeasurePoints;
  vector<string> measurelessPoints;
  vector<string> tolerancePoints;
  vector<string> nonReferenceMeasures;
  vector<string> nonGroundPoints;
  vector<string> nonCubePoints;
  vector<string> noCubeMeasures;
  vector<string> noMeasurePoints;
  vector<string> nonListedPoints;
  vector<string> nonLatLonPoints;
  vector<string> cannotGenerateLatLonPoints;

  // Set up comparison data
  vector<iString> serialNumbers;
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


  for(int cp = outNet.Size() - 1; cp >= 0; cp --) {
    progress.CheckStatus();

    // Do preliminary exclusion checks
    if(noIgnore && outNet[cp].Ignore()) {
      ignoredPoints.push_back(outNet[cp].Id());
      outNet.Delete(cp);
      continue;
    }
    if(noHeld && outNet[cp].Held()) {
      heldPoints.push_back(outNet[cp].Id());
      outNet.Delete(cp);
      continue;
    }
    if(ground && !(outNet[cp].Type() == ControlPoint::Ground)) {
      nonGroundPoints.push_back(outNet[cp].Id());
      outNet.Delete(cp);
      continue;
    }

    if(noSingleMeasure) {
      bool invalidPoint = false;
      invalidPoint |= noIgnore && (outNet[cp].NumValidMeasures() < 2);
      invalidPoint |= outNet[cp].Size() < 2 && (outNet[cp].Type() != ControlPoint::Ground);

      if(invalidPoint) {
        singleMeasurePoints.push_back(outNet[cp].Id());
        outNet.Delete(cp);
        continue;
      }
    }

    // Change the current point into a new point by manipulation of its control measures
    ControlPoint &newPoint = outNet[cp];

    for(int cm = newPoint.Size() - 1; cm >= 0; cm --) {
      if(noIgnore && newPoint[cm].Ignore()) {
        ignoredMeasures.push_back(newPoint.Id() + "," + newPoint[cm].CubeSerialNumber());
        newPoint.Delete(cm);
      }
      else if(reference && !newPoint[cm].IsReference()) {
        nonReferenceMeasures.push_back(newPoint.Id() + "," + newPoint[cm].CubeSerialNumber());
        newPoint.Delete(cm);
      }
      else if(cubeMeasures) {
        bool hasSerialNumber = false;

        for(unsigned int sn = 0; sn < serialNumbers.size() && !hasSerialNumber; sn ++) {
          if(serialNumbers[sn] == newPoint[cm].CubeSerialNumber()) hasSerialNumber = true;
        }

        if(!hasSerialNumber) {
          noCubeMeasures.push_back(newPoint.Id() + "," + newPoint[cm].CubeSerialNumber());
          newPoint.Delete(cm);
        }
      }
    }

    // Check for line/sample errors above provided tolerance
    if(noTolerancePoints) {
      bool hasLowTolerance = true;

      for(int cm = 0; cm < newPoint.Size() && hasLowTolerance; cm ++) {
        if(newPoint[cm].SampleError() >= tolerance ||
            newPoint[cm].LineError() >= tolerance) {
          hasLowTolerance = false;
        }
      }

      if(hasLowTolerance) {
        tolerancePoints.push_back(newPoint.Id());
        outNet.Delete(cp);
        continue;
      }
    }

    // Do not add outPoint if it has too few measures
    if(noSingleMeasure) {
      bool invalidPoint = false;
      invalidPoint |= noIgnore && (newPoint.NumValidMeasures() < 2);
      invalidPoint |= newPoint.Size() < 2 && newPoint.Type() != ControlPoint::Ground;

      if(invalidPoint) {
        singleMeasurePoints.push_back(outNet[cp].Id());
        outNet.Delete(cp);
        continue;
      }
    }

    // Do not add outPoint if it does not have a cube in CUBELIST as asked
    if(cubePoints && !cubeMeasures) {
      bool hasSerialNumber = false;

      for(int cm = 0; cm < newPoint.Size() && !hasSerialNumber; cm ++) {
        for(unsigned int sn = 0; sn < serialNumbers.size() && !hasSerialNumber; sn ++) {
          if(serialNumbers[sn] == newPoint[cm].CubeSerialNumber()) hasSerialNumber = true;
        }
      }

      if(!hasSerialNumber) {
        nonCubePoints.push_back(newPoint.Id());
        outNet.Delete(cp);
        continue;
      }
    }

    if(noMeasureless && newPoint.Size() == 0) {
      noMeasurePoints.push_back(newPoint.Id());
      outNet.Delete(cp);
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
  outNet.Write(ui.GetFilename("OUTNET"));

  outProgress.CheckStatus();

  // Adds the remove history to the summary and results group
  PvlGroup summary("ResultSummary");
  PvlGroup results("Results");

  if(noIgnore) {
    summary.AddKeyword(PvlKeyword("IgnoredPoints", iString((int)ignoredPoints.size())));
    summary.AddKeyword(PvlKeyword("IgnoredMeasures", iString((int)ignoredMeasures.size())));
  }
  if(noHeld) {
    summary.AddKeyword(PvlKeyword("HeldPoints", iString((int)heldPoints.size())));
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
  if(ground) {
    summary.AddKeyword(PvlKeyword("NonGroundPoints", iString((int)nonGroundPoints.size())));
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
    resultsProgress.SetMaximumSteps(12);
    resultsProgress.CheckStatus();

    string prefix = ui.GetString("PREFIX");

    if(noIgnore) {
      string namecp(Filename(prefix + "IgnoredPoints.txt").Expanded());
      WriteResults(namecp, ignoredPoints);
      string namecm(Filename(prefix + "IgnoredMeasures.txt").Expanded());
      WriteResults(namecm, ignoredMeasures);
    }

    resultsProgress.CheckStatus();

    if(noHeld) {
      string name(Filename(prefix + "HeldPoints.txt").Expanded());
      WriteResults(name, heldPoints);
    }

    resultsProgress.CheckStatus();

    if(noSingleMeasure) {
      string name(Filename(prefix + "SingleMeasurePoints.txt").Expanded());
      WriteResults(name, singleMeasurePoints);
    }

    resultsProgress.CheckStatus();

    if(noMeasureless) {
      string name(Filename(prefix + "MeasurelessPoints.txt").Expanded());
      WriteResults(name, measurelessPoints);
    }

    resultsProgress.CheckStatus();

    if(noTolerancePoints) {
      string name(Filename(prefix + "TolerancePoints.txt").Expanded());
      WriteResults(name, tolerancePoints);
    }

    resultsProgress.CheckStatus();

    if(reference) {
      string name(Filename(prefix + "NonReferenceMeasures.txt").Expanded());
      WriteResults(name, nonReferenceMeasures);
    }

    resultsProgress.CheckStatus();

    if(ground) {
      string name(Filename(prefix + "NonGroundPoints.txt").Expanded());
      WriteResults(name, nonGroundPoints);
    }

    resultsProgress.CheckStatus();

    if(cubePoints) {
      string name(Filename(prefix + "NonCubePoints.txt").Expanded());
      WriteResults(name, nonCubePoints);
    }

    resultsProgress.CheckStatus();

    if(noMeasurePoints.size() != 0) {
      string name(Filename(prefix + "NoMeasurePoints.txt").Expanded());
      WriteResults(name, noMeasurePoints);
    }

    resultsProgress.CheckStatus();

    if(cubeMeasures) {
      string name(Filename(prefix + "NonCubeMeasures.txt").Expanded());
      WriteResults(name, noCubeMeasures);
    }

    resultsProgress.CheckStatus();

    if(pointsEntered) {
      string name(Filename(prefix + "NonListedPoints.txt").Expanded());
      WriteResults(name, nonListedPoints);
    }

    resultsProgress.CheckStatus();

    if(latLon) {
      string namenon(Filename(prefix + "LatLonOutOfRange.txt").Expanded());
      WriteResults(namenon, nonLatLonPoints);
      string namegen(Filename(prefix + "NoLatLonPoints.txt").Expanded());
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
void ExtractPointList(ControlNet &outNet, vector<string> & nonListedPoints) {
  UserInterface &ui = Application::GetUserInterface();

  FileList listedPoints(ui.GetFilename("POINTLIST"));

  for(int cp = outNet.Size() - 1; cp >= 0; cp --) {
    bool isInList = false;
    for(int pointId = 0; pointId < (int)listedPoints.size()  &&  !isInList; pointId ++) {
      isInList = outNet[cp].Id().compare(listedPoints[pointId]) == 0;
    }

    if(!isInList) {
      nonListedPoints.push_back(outNet[cp].Id());
      outNet.Delete(cp);
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
void ExtractLatLonRange(ControlNet &outNet, vector<string> & nonLatLonPoints,
                        vector<string> & cannotGenerateLatLonPoints,  map<iString, iString> sn2filename) {
  if(outNet.Size() == 0) {
    return;
  }

  UserInterface &ui = Application::GetUserInterface();

  // Get the lat/lon and fix the range for the internal 0/360
  double minlat = ui.GetDouble("MINLAT");
  double maxlat = ui.GetDouble("MAXLAT");
  double minlon = ui.GetDouble("MINLON");
  if(minlon < 0.0) {
    minlon += 360;
  }
  double maxlon = ui.GetDouble("MAXLON");
  if(maxlon < 0.0) {
    minlon += 360;
  }

  Progress progress;
  progress.SetText("Calculating lat/lon");
  progress.SetMaximumSteps(outNet.Size());
  progress.CheckStatus();

  CubeManager manager;
  manager.SetNumOpenCubes(50);   //Should keep memory usage to around 1GB

  for(int cp = outNet.Size() - 1; cp >= 0; cp --) {
    progress.CheckStatus();

    // If the Contorl Network takes priority, use it
    double pointLat = outNet[cp].UniversalLatitude();
    double pointLon = outNet[cp].UniversalLongitude();
    bool hasLatLon = pointLat != Isis::Null && pointLon != Isis::Null;
    if(outNet[cp].Type() == Isis::ControlPoint::Ground || hasLatLon) {
      if(NotInLatLonRange(pointLat, pointLon, minlat, maxlat, minlon, maxlon)) {
        nonLatLonPoints.push_back(outNet[cp].Id());
        outNet.Delete(cp);
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
      double lat = 0.0;
      double lon = 0.0;
      double radius = 0.0;

      // First check the reference Measure
      if(outNet[cp].HasReference()) {
        cm = outNet[cp].ReferenceIndex();
        if(!sn2filename[outNet[cp][cm].CubeSerialNumber()].empty()) {
          sn = outNet[cp][cm].CubeSerialNumber();
        }
      }

      // Search for other Control Measures if needed
      if(sn.empty()) {
        // Find the Serial Number if it exists
        for(int cm = 0; (cm < outNet[cp].Size()) && sn.empty(); cm ++) {
          if(!sn2filename[outNet[cp][cm].CubeSerialNumber()].empty()) {
            sn = outNet[cp][cm].CubeSerialNumber();
          }
        }
      }

      // Connot fine a cube to get the lat/lon from
      if(sn.empty()) {
        //cannotGenerateLatLonPoints += outNet[cp].Id();
        cannotGenerateLatLonPoints.push_back(outNet[cp].Id());
        outNet.Delete(cp);
      }

      // Calculate the lat/lon and check for validity
      else {
        bool remove = false;

        Cube *cube = manager.OpenCube(sn2filename[sn]);
        Camera *camera = cube->Camera();

        if(camera == NULL) {
          try {
            Projection *projection = ProjectionFactory::Create((*(cube->Label())));

            if(!projection->SetCoordinate(outNet[cp][cm].Sample(), outNet[cp][cm].Line())) {
              nonLatLonPoints.push_back(outNet[cp].Id());
              remove = true;
            }

            lat = projection->Latitude();
            lon = projection->Longitude();
            radius = projection->LocalRadius();

            delete projection;
            projection = NULL;
          }
          catch(iException &e) {
            remove = true;
            e.Clear();
          }
        }
        else {
          if(!camera->SetImage(outNet[cp][cm].Sample(), outNet[cp][cm].Line())) {
            nonLatLonPoints.push_back(outNet[cp].Id());
            remove = true;
          }

          lat = camera->UniversalLatitude();
          lon = camera->UniversalLongitude();
          radius = camera->LocalRadius();

          camera = NULL;
        }

        cube = NULL;

        if(remove  ||  NotInLatLonRange(lat, lon, minlat, maxlat, minlon, maxlon)) {
          nonLatLonPoints.push_back(outNet[cp].Id());
          outNet.Delete(cp);
        }
        else { // Add the reference lat/lon/radius to the Control Point
          outNet[cp].SetUniversalGround(lat, lon, radius);
        }
      }
    }
    else {
      cannotGenerateLatLonPoints.push_back(outNet[cp].Id());
      outNet.Delete(cp);
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
bool NotInLatLonRange(double lat, double lon, double minlat,
                      double maxlat, double minlon, double maxlon) {
  bool inRange = true;

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
void WriteCubeOutList(ControlNet cnet, map<iString, iString> sn2file) {
  UserInterface &ui = Application::GetUserInterface();

  if(ui.WasEntered("TOLIST")) {

    Progress p;
    p.SetText("Writing Cube List");
    try {
      p.SetMaximumSteps(cnet.Size());
      p.CheckStatus();
    }
    catch(iException &e) {
      e.Clear();
      string msg = "The provided filters have resulted in an empty Control Network.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    set<iString> outputsn;
    for(int cp = 0; cp < cnet.Size(); cp ++) {
      for(int cm = 0; cm < cnet[cp].Size(); cm ++) {
        outputsn.insert(cnet[cp][cm].CubeSerialNumber());
      }
      p.CheckStatus();
    }

    std::string toList = ui.GetFilename("TOLIST");
    ofstream out_stream;
    out_stream.open(toList.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(set<iString>::iterator sn = outputsn.begin(); sn != outputsn.end(); sn ++) {
      if(!sn2file[(*sn)].empty()) {
        out_stream << sn2file[(*sn)] << endl;
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
void WriteResults(string filename, vector<string> results) {
  if(results.size() == 0) {
    return;
  }

  // Set up the output file for writing
  ofstream out_stream;
  out_stream.open(filename.c_str(), std::ios::out);
  out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

  out_stream << results[0];
  for(unsigned int index = 1; index < results.size(); index ++) {
    out_stream << std::endl << results[index];
  }

  out_stream.close();
}
