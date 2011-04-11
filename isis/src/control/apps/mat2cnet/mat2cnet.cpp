#include "Isis.h"

#include <map>

#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "FileList.h"
#include "ID.h"
#include "iException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "Process.h"
#include "Pvl.h"
#include "SerialNumberList.h"
#include "TextFile.h"

using namespace std;
using namespace Isis;

std::map <int, string> snMap;
//std::map <string,int> fscMap;

void IsisMain() {
  // The following steps can take a significant amount of time, so
  // set up a progress object, incrementing at 1%, to keep the user informed
  Isis::PvlGroup &uip = Isis::Preference::Preferences().FindGroup("UserInterface");
  uip["ProgressBarPercent"] = 1;
  UserInterface &ui = Application::GetUserInterface();
  Progress progress;

  // Prepare the ISIS2 list of file names
  FileList list2(ui.GetFilename("LIST2"));

  // Prepare the ISIS3 SNs, pass the progress object to SerialNumberList
  SerialNumberList snl(ui.GetFilename("LIST3"), true, &progress);
  progress.CheckStatus();

  if (list2.size() != (unsigned)snl.Size()) {
    iString msg = "Invalid input file number of lines. The ISIS 2 file list [";
    msg += ui.GetAsString("LIST2") + "] must contain the same number of lines ";
    msg += "as the ISIS 3 file list [" + ui.GetAsString("LIST3") + "]";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  progress.SetText("Mapping Isis 2 fsc numbers to Isis 3 serial numbers.");
  progress.SetMaximumSteps(list2.size());
  // Setup a map between ISIS2 image number (fsc) and ISIS3 sn
  for (unsigned int f = 0; f < list2.size(); f++) {
    progress.CheckStatus();
    iString currFile(list2[f]);
    Pvl lab(currFile);
    PvlObject qube(lab.FindObject("QUBE"));

    iString fsc;
    if (qube.HasKeyword("IMAGE_NUMBER")) {
      fsc = qube.FindKeyword("IMAGE_NUMBER")[0];
    }
    else if (qube.HasKeyword("IMAGE_ID")) {
      fsc = qube.FindKeyword("IMAGE_ID")[0];
    }
    else {
      throw iException::Message(iException::Pvl,
          "Can not find required keyword IMAGE_NUMBER or IMAGE_ID in [" + currFile + "]",
          _FILEINFO_);
    }

    iString sn(snl.SerialNumber(f));
    snMap.insert(std::pair<int, string>((int)fsc, sn));
  }
  progress.CheckStatus();

  // Create a new control network
  ControlNet cnet;
  cnet.SetNetworkId(ui.GetString("NETWORKID"));
  cnet.SetTarget(ui.GetString("TARGET"));
  cnet.SetUserName(Isis::Application::UserName());
  cnet.SetCreatedDate(Isis::Application::DateTime());
  cnet.SetDescription(ui.GetString("DESCRIPTION"));

  // Open the match point file
  TextFile mpFile(ui.GetFilename("MATCH"));
  iString currLine;
  int inTotalMeas = 0;

  // Read the first line with the number of measurments
  mpFile.GetLine(currLine);
  currLine.Token("=");
  currLine.Token(" ");
  try {
    inTotalMeas = int(currLine);
  }
  catch (Isis::iException &e) {
    throw iException::Message(iException::User, "Invalid match point file header for ["
        + ui.GetAsString("MATCH")
        + "]. First line does not contain number of measurements."
        , _FILEINFO_);
  }

  // Read line 2, the column header line
  mpFile.GetLine(currLine);
  currLine.ConvertWhiteSpace();
  currLine.Compress();
  while (currLine != "") {
    iString label = currLine.Token(" ");
    // this line should contain only text labels,
    double error = 0;
    try {
      error = label;
      // if we are able to convert label to a double, we have an error
      throw iException::Message(iException::User, "Invalid match point file header for ["
          + ui.GetAsString("MATCH")
          + "]. Second line does not contain proper non-numerical column labels."
          , _FILEINFO_);
    }
    catch (Isis::iException e) {
      // if this line does not contain a double, continue
      if (error == 0) {
        e.Clear();// not really an error, this is text value, as expected
        continue;
      }
      // if this line contains numeric data, throw an error
      else {
        throw e;
      }
    }
  }

  // Reset the progress object for feedback about conversion processing
  progress.SetText("Converting match point file");
  progress.SetMaximumSteps(inTotalMeas);

  int line = 2;
  while (mpFile.GetLine(currLine)) {
    line ++;

    // Update the Progress object
    try {
      progress.CheckStatus();
    }
    catch (Isis::iException e) {
      string msg = "\"Matchpoint total\" keyword at the top of the match point file [";
      msg += ui.GetAsString("MATCH") + "] equals [" + iString(inTotalMeas);
      msg += "] and is likely incorrect. Number of measures in match point file exceeds this value at line [";
      msg += iString(line) + "].";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }


    // Declare the Point and Measure
    ControlPoint *cpoint = NULL;
    ControlMeasure *cmeasure = new ControlMeasure;

    // Section the match point line into the important pieces
    currLine.ConvertWhiteSpace();
    currLine.Compress();
    iString pid = "";
    iString fsc = "";
    double lineNum, sampNum, diam;
    string matClass = "";
    try {
      pid = currLine.Token(" ");      // ID of the point
      fsc = currLine.Token(" ");      // FSC of the Isis 2 cube
      lineNum = currLine.Token(" ");  // line number
      sampNum = currLine.Token(" ");  // sample number
      matClass = currLine.Token(" "); // Match Point Class
      diam = currLine.Token(" ");     // Diameter, in case of a crater
    }
    catch (iException &e) {
      iString msg = "Invalid value(s) in match point file [";
      msg += ui.GetAsString("MATCH") + "] at line [" + iString(line);
      msg += "]. Verify line, sample, diameter values are doubles.";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    // Set the coordinate and serial number for this measure
    cmeasure->SetCoordinate(sampNum, lineNum);
    cmeasure->SetCubeSerialNumber(snMap[(int)fsc]);

    if (snMap[(int)fsc].empty()) {
      std::string msg = "None of the images specified in the ISIS 2 file list [";
      msg += ui.GetAsString("LIST2");
      msg += "] have an IMAGE_NUMBER or IMAGE_ID that matches the FSC [" + fsc;
      msg += "], from the match point file [" + ui.GetAsString("MATCH");
      msg += "] at line [" + iString(line) + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    bool isReferenceMeasure = false;

    //Set the Measure Type
    if (iString::Equal(matClass, "U"))
      //Unmeasured - these are ignored in isis2
    {
      cmeasure->SetType(ControlMeasure::Candidate);
      cmeasure->SetIgnored(true);
    }
    else if (iString::Equal(matClass, "T"))
      //Truth
    {
      // Truth type, aka reference measure, is no longer a measure type
      // what this means is it has to be handled by the control point.
      // So, further down the boolean set here will be used.
      isReferenceMeasure = true;
    }
    else if (iString::Equal(matClass, "S"))
      //SubPixel
    {
      cmeasure->SetType(ControlMeasure::RegisteredSubPixel);
    }
    else if (iString::Equal(matClass, "M"))
      //Measured
    {
      cmeasure->SetType(ControlMeasure::RegisteredPixel);
    }
    else if (iString::Equal(matClass, "A"))
      //Approximate
    {
      cmeasure->SetType(ControlMeasure::Candidate);
    }
    else {
      iString msg = "Unknown measurment type [" + matClass + "] ";
      msg += "in match point file [" + ui.GetAsString("MATCH") + "] ";
      msg += "at line [" + iString(line) + "]";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    //Set Diameter
    try {
      //Check to see if the column was, in fact, a double
      if((double)iString(diam) != 0.0)
        cmeasure->SetDiameter(diam);
    }
    catch (iException &e) {
      //If we get here, diam was not a double,
      // but the error is not important otherwise
      e.Clear();
    }

    //Find the point that matches the PointID, create point if it does not exist

    if (cnet.ContainsPoint(pid)) {
      cpoint = cnet.GetPoint((QString) pid);
    }
    else {
      cpoint = new ControlPoint(pid);
      cnet.AddPoint(cpoint);
    }

    //Add the measure
    try {
      cpoint->Add(cmeasure);
      // Equivalent to (iString::Equal(matClass, "T")), as seen above 
      if (isReferenceMeasure) {
        cpoint->SetRefMeasure(cmeasure);
      }
    }
    catch (iException &e) {
      iString msg = "Invalid match point file [" + ui.GetAsString("MATCH") + "]";
      msg += ".  Repeated PointID/FSC combination [" + pid + ", " + fsc;
      msg += "] in match point file at line [" + iString(line) + "].";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }

  // Update the Progress object
  try {
    progress.CheckStatus();
  }
  catch (Isis::iException e) {
    string msg = "\"Matchpoint total\" keyword at the top of the match point file [";
    msg += ui.GetAsString("MATCH") + "] equals [" + iString(inTotalMeas);
    msg += "] and is likely incorrect. Number of measures in match point file exceeds this value at line [";
    msg += iString(line) + "].";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }


  // 10/5/2009 - Jeannie Walldren //////////////////////////////
  // Added RAND PPP file as input     //////////////////////////////

  // Open the RAND PPP file
  if (ui.GetBoolean("INPUTPPP")) {
    int numRandOnly = 0;
    vector<string> randOnlyIDs;
    TextFile randFile(ui.GetFilename("PPP"));
    progress.SetText("Converting RAND PPP file");
    randFile.GetLine(currLine);
    int inTotalLine = (int)(randFile.Size() / (currLine.size()));
    progress.SetMaximumSteps(inTotalLine);
    randFile.Rewind();

    line = 0;
    while (randFile.GetLine(currLine)) {
      line ++;

      // Update the Progress object
      try {
        progress.CheckStatus();
      }
      catch (Isis::iException e) {
        iString msg = "RAND PPP file may not be valid.  Line count calculated [";
        msg += iString(inTotalLine) + "] for RAND PPP file [" + ui.GetAsString("PPP");
        msg += "] appears invalid at line [" + iString(line) + "].";
        throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
      }

      // Declare the Point
      ControlPoint *cpoint = NULL;

      // if end of valid data, break, stop processing
      if (currLine.find("JULIAN") != string::npos) {
        // Since Progress MaximumSteps was approximated using the number of lines in the RAND PPP file,
        // we need to subtract the number of lines left from the Progress steps
        // since the following lines are not going to be processed
        progress.AddSteps(line - inTotalLine); // line < inTotalLine, so this is negative
        break;
      }

      // break columns into substrings since some files have colunms that can't be tokenized easily.
      // This is because some files have columns running into each other without spaces separating them

      // column 1 = latitude, begins the line and is 24 characters
      double lat;
      iString col1 = currLine.substr(0, 24);
      // remove any white space from beginning of string
      //col1.ConvertWhiteSpace();
      //col1.TrimHead(" ");
      try {
        // convert to double
        lat = col1;
      }
      catch (Isis::iException &e) {
        iString msg = "Invalid value(s) in RAND PPP file [";
        msg += ui.GetAsString("PPP") + "] at line [" + iString(line);
        msg += "]. Verify latitude value is a double.";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }

      // column 2 = longitude, begins at 25th char and is 24 characters
      double lon;
      iString col2 = currLine.substr(24, 24);
      // remove any white space from beginning of string
      //col2.TrimHead(" ");
      try {
        // convert to double
        lon = col2;
      }
      catch (Isis::iException &e) {
        iString msg = "Invalid value(s) in RAND PPP file [";
        msg += ui.GetAsString("PPP") + "] at line [" + iString(line);
        msg += "]. Verify longitude value is a double.";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }

      // column 3 = radius, begins at 49th char and is 24 characters
      double rad;
      iString col3 = currLine.substr(48, 24);
      // remove any white space from beginning of string
      //col3.TrimHead(" ");
      try {
        // convert to double and convert km to meters
        rad = col3;
        rad = rad * 1000;
      }
      catch (Isis::iException &e) {
        iString msg = "Invalid value(s) in RAND PPP file [";
        msg += ui.GetAsString("PPP") + "] at line [" + iString(line);
        msg += "]. Verify radius value is a double.";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }

      // column 4 = point id, begins at 73rd char and is 7 characters
      iString pid = currLine.substr(72);
      // remove any white space from beginning of string
      pid.TrimHead(" ");
      if (pid.length() > 7) {
        iString msg = "Invalid value(s) in RAND PPP file [";
        msg += ui.GetAsString("PPP") + "] at line [" + iString(line);
        msg += "]. Point ID [" + pid + "] has more than 7 characters.";
        throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
      }

      //Find the point that matches the PointID, if it does not exist alert the user
      if (!cnet.ContainsPoint(pid)) {
        numRandOnly++;
        randOnlyIDs.push_back(currLine);
      }
      else {
        cpoint = cnet.GetPoint((QString) pid);
        // if the point is already added to the control net, it was found in
        // the match point file also.
        if (ui.GetString("POINTTYPE") == "GROUND") {
          // If the POINTTYPE parameter is set to ground, change point type
          // of points in rand file
          cpoint->SetType(ControlPoint::Ground);
        }
        cpoint->SetAprioriSurfacePointSource(
          ControlPoint::SurfacePointSource::BundleSolution);
        cpoint->SetAprioriSurfacePointSourceFile(ui.GetAsString("PPP"));
        cpoint->SetAprioriRadiusSource(
          ControlPoint::RadiusSource::BundleSolution);
        cpoint->SetAprioriRadiusSourceFile(ui.GetAsString("PPP"));
      }

      if (cpoint != NULL) {
        //Add the lat,lon,rad to point
        try {
          SurfacePoint surfacePt(Latitude(lat, Angle::Degrees),
              Longitude(lon, Angle::Degrees),
              Distance(rad, Distance::Meters));
          cpoint->SetAprioriSurfacePoint(surfacePt);
        }
        catch (iException &e) {
          iString msg = "Unable to set universal ground point to control network from line [";
          msg += iString(line) + "] of RAND PPP file [" + ui.GetAsString("PPP") + "]";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
      }
    }
    // Update the Progress object
    try {
      progress.CheckStatus();
    }
    catch (Isis::iException e) {
      iString msg = "RAND PPP file may not be valid.  Line count calculated [";
      msg += iString(inTotalLine) + "] for RAND PPP file [" + ui.GetAsString("PPP");
      msg += "] appears invalid at line [" + iString(line) + "].";
      throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
    }

    // Write results to Logs
    // Summary group is created with the counts of RAND PPP only points
    PvlGroup summaryGroup = PvlGroup("Summary");
    summaryGroup.AddKeyword(PvlKeyword("RandOnlyPoints", numRandOnly));

    bool log;
    Filename logFile;
    // if a filename was entered, use it to create the log
    if (ui.WasEntered("LOG")) {
      log = true;
      logFile = ui.GetFilename("LOG");
    }
    // if no filename was entered, but there were some RAND PPP only points,
    // create an log named "randOnlyPoints" in the current directory
    else if (numRandOnly > 0) {
      log = true;
      logFile = "randOnlyPoints.log";
    }
    // if all RAND PPP points are found in the MATCH file and no log file is named,
    // only output summary to application log
    else {
      log = false;
    }

    if (log) {
      // Write details in error log
      Pvl results;
      results.SetName("Results");
      results.AddGroup(summaryGroup);
      if (numRandOnly > 0) {
        // if there are any RAND PPP only points,
        // add comment to the summary log to alert user
        summaryGroup.AddComment("Some Point IDs in the RAND PPP file have no measures in the MATCH file.");
        summaryGroup.AddComment("These Point IDs are contained in [" + logFile.Name() + "].");
        TextFile outlog(logFile.Expanded(), "overwrite", randOnlyIDs);
      }
      else {
        // if there are no RAND PPP only points and user wanted to create a log,
        // add comment to the summary log to alert user
        summaryGroup.AddComment("All Point IDs in the RAND PPP file have measures in the MATCH file.");
        summaryGroup.AddComment("No RAND PPP log was created.");
      }
    }
    // Write summary to application log
    Application::Log(summaryGroup);

  }
  // Write the control network out
  cnet.Write(ui.GetFilename("ONET"));


}
