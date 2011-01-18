#include "Isis.h"

#include <sstream>
#include <set>

#include <QStack>
#include <QString>
#include <QVector>

#include "Camera.h"
#include "CameraFactory.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "CubeManager.h"
#include "FileList.h"
#include "Filename.h"
#include "iString.h"
#include "ProjectionFactory.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "UserInterface.h"

using namespace Isis;

QMap< iString, std::set<iString> > constructPointSets(
  std::set<iString> &index, ControlNet innet);
QVector< std::set<iString> > findIslands(
  std::set<iString> &index,
  QMap < iString, std::set<iString> > adjCubes);
void WriteOutput(SerialNumberList num2cube,
                 std::string filename,
                 std::set<iString> sns,
                 QMap< iString, std::set<iString> > cps);

// Main program
void IsisMain() {

  Progress progress;
  UserInterface &ui = Application::GetUserInterface();
  // This constructor was removed in the Control redesign
  //ControlNet innet(ui.GetFilename("CNET"), NULL, true);
  ControlNet innet(ui.GetFilename("CNET"));
  iString prefix(ui.GetString("PREFIX"));
  bool ignore = ui.GetBoolean("IGNORE");

  // Sets up the list of serial numbers for
  FileList inlist(ui.GetFilename("FROMLIST"));
  std::set<iString> inListNums;
  QMap<iString, int> netSerialNumCount;
  QVector<iString> listedSerialNumbers;
  SerialNumberList num2cube;

  if(inlist.size() > 0) {
    progress.SetText("Initializing");
    progress.SetMaximumSteps(inlist.size());
    progress.CheckStatus();
  }

  for(int index = 0; (unsigned)index < inlist.size(); index ++) {
    num2cube.Add(inlist[index]);
    iString st = num2cube.SerialNumber(inlist[index]);
    inListNums.insert(st);
    listedSerialNumbers.push_back(st);   // Used with nonListedSerialNumbers
    progress.CheckStatus();
  }

  QVector<iString> nonListedSerialNumbers;

  std::set<iString> singleMeasureSerialNumbers;
  QMap< iString, std::set<iString> > singleMeasureControlPoints;

  std::set<iString> duplicateSerialNumbers;
  QMap< iString, std::set<iString> > duplicateControlPoints;

  std::set<iString> noLatLonSerialNumbers;
  QMap< iString, std::set<iString> > noLatLonControlPoints;
  std::set<iString> latlonchecked;

  QMap< iString, int > cubeMeasureCount;

  // Set calculating progress
  if(innet.GetNumPoints() > 0) {
    progress.SetText("Calculating");
    progress.SetMaximumSteps(innet.GetNumPoints());
    progress.CheckStatus();
  }

  // Manage cubes used in NOLATLON
  CubeManager cbman;
  cbman.SetNumOpenCubes(50);

  // Loop through all control points in control net
  for(int cp = 0; cp < innet.GetNumPoints(); cp ++) {
    if(ignore && innet.GetPoint(cp)->IsIgnored()) continue;
    ControlPoint *controlpt = innet.GetPoint(cp);

    // Checks for lat/Lon production
    if(ui.GetBoolean("NOLATLON")) {

      // Loop through all control measures in control points
      for(int cm = 0; cm < controlpt->GetNumMeasures(); cm ++) {
        ControlMeasure *controlms = controlpt->GetMeasure(cm);

        // If we have the cube, check it out
        if(num2cube.HasSerialNumber(controlms->GetCubeSerialNumber())) {
          Cube *cube = cbman.OpenCube(num2cube.Filename(
                                        controlms->GetCubeSerialNumber()));
          Camera *cam = NULL;
          bool createFail = false;
          bool setPassed = true;

          // Try to create
          try {
            cam = cube->Camera();
          }
          catch(iException &e) {
            createFail = true;
            e.Clear();
          }

          // Check the exact measure location
          if(!createFail) {
            setPassed = cam->SetImage(controlms->GetSample(), controlms->GetLine());
          }

          // Record it if it failed at anything
          if(createFail || !setPassed) {
            noLatLonSerialNumbers.insert(controlms->GetCubeSerialNumber());
            noLatLonControlPoints[controlms->GetCubeSerialNumber()].insert(controlpt->GetId());
          }
        }
      }
    }
    // Checks of the ControlPoint has only 1 Measure
    if(controlpt->GetNumValidMeasures() == 1) {
      iString sn = controlpt->GetMeasure(0)->GetCubeSerialNumber();
      singleMeasureSerialNumbers.insert(sn);
      singleMeasureControlPoints[sn].insert(controlpt->GetId());

      // Records how many times a cube is in the ControlNet
      cubeMeasureCount[sn] ++;
    }
    else {
      // Checks for duplicate Measures for the same SerialNumber
      QVector<ControlMeasure *> controlMeasures;
      for(int cm = 0; cm < controlpt->GetNumMeasures(); cm ++) {
        ControlMeasure *controlms = controlpt->GetMeasure(cm);

        if(ignore  &&  controlms->IsIgnored()) continue;

        controlMeasures.append(controlms);
        iString currentsn = controlms->GetCubeSerialNumber();

        // Records how many times a cube is in the ControlNet
        cubeMeasureCount[currentsn] ++;

        // Compares previous ControlMeasure SerialNumbers with the current
        for(int pre_cm = controlMeasures.size() - 1 - 1; pre_cm >= 0; pre_cm --) {
          if(controlMeasures[pre_cm]->GetCubeSerialNumber() == currentsn) {
            duplicateSerialNumbers.insert(currentsn);   //serial number duplication
            duplicateControlPoints[currentsn].insert(controlpt->GetId());
          }
        }

        // Removes from the serial number list, cubes that are included in the cnet
        inListNums.erase(currentsn);
        netSerialNumCount[currentsn] ++;

        // Records if the currentsnum is not in the input cube list
        bool contains = false;
        for(int sn = 0; sn < (int)listedSerialNumbers.size()  &&  !contains; sn ++) {
          if(currentsn == listedSerialNumbers[sn]) {
            contains = true;
          }
        }
        // Check if already added
        for(int sn = 0; sn < (int)nonListedSerialNumbers.size()  &&  !contains; sn ++) {
          if(currentsn == nonListedSerialNumbers[sn]) {
            contains = true;
          }
        }

        if(!contains) {
          nonListedSerialNumbers.push_back(currentsn);
        }
      }
    }

    progress.CheckStatus();
  }


  // Checks/detects islands
  std::set<iString> index;
  QMap< iString, std::set<iString> > adjCubes = constructPointSets(index, innet);
  QVector< std::set<iString> > islands = findIslands(index, adjCubes);

  // Output islands in the file-by-file format
  //  Islands that have no cubes listed in the input list will
  //  not be shown.
  for(int i = 0; i < (int)islands.size(); i++) {
    iString name(Filename(prefix + "Island." + iString(i + 1)).Expanded());
    std::ofstream out_stream;
    out_stream.open(name.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    bool hasList = false;
    for(std::set<iString>::iterator island = islands[i].begin();
        island != islands[i].end(); island++) {
      if(num2cube.HasSerialNumber(*island)) {
        out_stream << (num2cube.HasSerialNumber(*island) ? Filename(num2cube.Filename(*island)).Name() : "") << " " << *island;
        out_stream << "\n";

        hasList = true;
      }
    }

    out_stream.close();

    if(!hasList) {
      remove(name.c_str());
    }
  }


  // Output the results to screen and files accordingly

  PvlGroup results("Results");

  std::stringstream ss(std::stringstream::in | std::stringstream::out);

  results.AddKeyword(PvlKeyword("Islands", iString((BigInt)islands.size())));
  ss << std::endl << "----------------------------------------" \
     "----------------------------------------" << std::endl;
  if(islands.size() == 1) {
    ss << "The cubes are fully connected by the Control Network." << std::endl;
  }
  else if(islands.size() == 0) {
    ss << "There are no control points in the provided Control Network [";
    ss << Filename(ui.GetFilename("CNET")).Name() << "]" << std::endl;
  }
  else {
    ss << "The cubes are NOT fully connected by the Control Network." << std::endl;
    ss << "There are " << islands.size() << " disjoint sets of cubes." << std::endl;
  }

  if(ui.GetBoolean("SINGLEMEASURE")  &&  singleMeasureSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("SingleMeasure", iString((BigInt)singleMeasureSerialNumbers.size())));

    iString name(Filename(prefix + "SinglePointCubes.txt").Expanded());
    WriteOutput(num2cube, name,
                singleMeasureSerialNumbers, singleMeasureControlPoints);

    int serials = singleMeasureSerialNumbers.size();
    ss << "----------------------------------------" \
       "----------------------------------------" << std::endl;
    ss << "There " << ((serials == 1) ? "is " : "are ") << singleMeasureSerialNumbers.size();
    ss << ((serials == 1) ? " cube" : " cubes") << " in Control Points with only a single";
    ss << " Control Measure." << std::endl;
    ss << "The serial numbers of these measures are listed in [";
    ss <<  Filename(name).Name() + "]" << std::endl;
  }

  if(ui.GetBoolean("DUPLICATE")  &&  duplicateSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("DuplicateMeasures", iString((BigInt)duplicateSerialNumbers.size())));

    iString name(Filename(prefix + "DuplicateMeasures.txt").Expanded());
    WriteOutput(num2cube, name,
                duplicateSerialNumbers, duplicateControlPoints);

    ss << "----------------------------------------" \
       "----------------------------------------" << std::endl;
    ss << "There are " << duplicateSerialNumbers.size();
    ss << " duplicate Control Measures in the";
    ss << " Control Net." << std::endl;
    ss << "The serial numbers of these duplicate Control Measures";
    ss << " are listed in [" + Filename(name).Name() + "]" << std::endl;
  }

  if(ui.GetBoolean("NOLATLON")  &&  noLatLonSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("NoLatLonCubes", iString((BigInt)noLatLonSerialNumbers.size())));

    iString name(Filename(prefix + "NoLatLon.txt").Expanded());
    WriteOutput(num2cube, name,
                noLatLonSerialNumbers, noLatLonControlPoints);

    ss << "----------------------------------------" \
       "----------------------------------------" << std::endl;
    ss << "There are " << noLatLonSerialNumbers.size();
    ss << " serial numbers in the Control Network which are listed in the";
    ss << " input list and cannot compute latitude and longitudes." << std::endl;
    ss << "These serial numbers, filenames, and control points are listed in [";
    ss << Filename(name).Name() + "]" << std::endl;
  }

  // At this point, inListNums is the list of cubes NOT included in the
  //  ControlNet, and inListNums are their those cube's serial numbers.
  if(ui.GetBoolean("NOCONTROL") && !inListNums.empty()) {
    results.AddKeyword(PvlKeyword("NoControl", iString((BigInt)inListNums.size())));

    iString name(Filename(prefix + "NoControl.txt").Expanded());
    std::ofstream out_stream;
    out_stream.open(name.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(std::set<iString>::iterator sn = inListNums.begin();
        sn != inListNums.end();
        sn ++) {
      out_stream << *sn;
      out_stream << "\t" << (num2cube.HasSerialNumber(*sn) ? Filename(num2cube.Filename(*sn)).Name() : "");
      out_stream << "\n";
    }
    out_stream.close();

    ss << "----------------------------------------" \
       "----------------------------------------" << std::endl;
    ss << "There are " << inListNums.size();
    ss << " cubes in the input list [" << Filename(ui.GetFilename("FROMLIST")).Name();
    ss << "] which do not exist or are ignored in the Control Network [";
    ss << Filename(ui.GetFilename("CNET")).Name() << "]" << std::endl;
    ss << "These cubes are listed in [" + Filename(name).Name() + "]" << std::endl;
  }

  // In addition, nonListedSerialNumbers should be the SerialNumbers of
  //  ControlMeasures in the ControlNet that do not have a correlating
  //  cube in the input list.
  if(ui.GetBoolean("NOCUBE")  &&  nonListedSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("NoCube", iString((BigInt)nonListedSerialNumbers.size())));

    iString name(Filename(prefix + "NoCube.txt").Expanded());
    std::fstream out_stream;
    out_stream.open(name.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(int sn = 0; sn < (int)nonListedSerialNumbers.size(); sn++) {
      out_stream << nonListedSerialNumbers[sn];
      out_stream << "\n";
    }

    out_stream.close();

    ss << "----------------------------------------" \
       "----------------------------------------" << std::endl;
    ss << "There are " << nonListedSerialNumbers.size();
    ss << " serial numbers in the Control Net [";
    ss << Filename(ui.GetFilename("CNET")).Basename();
    ss << "] which do not exist in the input list [";
    ss << Filename(ui.GetFilename("FROMLIST")).Name() << "]" << std::endl;
    ss << "These serial numbers are listed in [";
    ss << Filename(name).Name() + "]" << std::endl;
  }

  // At this point cubeMeasureCount should be equal to the number of
  //  ControlMeasures associated with each serial number.
  if(ui.GetBoolean("SINGLECUBE")) {
    std::set<iString> singleMeasureCubes;
    for(QMap<iString, int>::iterator cube = cubeMeasureCount.begin();
        cube != cubeMeasureCount.end();
        cube ++) {
      if(cube.value() == 1) {
        singleMeasureCubes.insert(cube.key());
      }
    }

    if(singleMeasureCubes.size() > 0) {
      results.AddKeyword(
        PvlKeyword("SingleCube", iString((BigInt)singleMeasureCubes.size())));

      iString name(Filename(prefix + "SingleCube.txt").Expanded());
      std::ofstream out_stream;
      out_stream.open(name.c_str(), std::ios::out);
      out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

      for(std::set<iString>::iterator sn = singleMeasureCubes.begin();
          sn != singleMeasureCubes.end();
          sn ++) {
        out_stream << *sn;
        out_stream << "\t" << (num2cube.HasSerialNumber(*sn) ? Filename(num2cube.Filename(*sn)).Name() : "");
        out_stream << "\n";
      }

      out_stream.close();

      ss << "----------------------------------------" \
         "----------------------------------------" << std::endl;
      ss << "There are " << singleMeasureCubes.size();
      ss << " serial numbers in the Control Net [";
      ss << Filename(ui.GetFilename("CNET")).Basename();
      ss << "] which only exist in one Control Measure." << std::endl;
      ss << "These serial numbers are listed in [";
      ss << Filename(name).Name() + "]" << std::endl;
    }
  }

  ss << "----------------------------------------" \
     "----------------------------------------" << std::endl << std::endl;
  std::string log = ss.str();
  Application::Log(results);

  if(ui.IsInteractive()) {
    Application::GuiLog(log);
  }
  else {
    std::cout << ss.str();
  }

}


// Links cubes to other cubes it shares control points with
QMap< iString, std::set<iString> > constructPointSets(std::set<iString> & index,
    ControlNet innet) {
  QMap< iString, std::set<iString > > adjPoints;

  bool ignore = Application::GetUserInterface().GetBoolean("IGNORE");
  for(int cp = 0; cp < innet.GetNumPoints(); cp++) {
    ControlPoint *controlpt = innet.GetPoint(cp);

    if(ignore && controlpt->IsIgnored()) continue;

    if(controlpt->GetNumValidMeasures() < 2) continue;

    // Map SerialNumbers together based on ControlMeasures
    for(int cm1 = 0; cm1 < controlpt->GetNumMeasures(); cm1++) {
      if(ignore && controlpt->IsIgnored()) continue;

      iString sn = controlpt->GetMeasure(cm1)->GetCubeSerialNumber();
      index.insert(sn);
      for(int cm2 = 0; cm2 < controlpt->GetNumMeasures(); cm2++) {
        if(ignore && controlpt->GetMeasure(cm2)->IsIgnored()) continue;

        if(cm1 != cm2) {
          adjPoints[ sn ].insert(controlpt->GetMeasure(cm2)->GetCubeSerialNumber());
        }
      }
    }

  }

  return adjPoints;
}


// Uses a depth-first search to construct the islands
QVector< std::set<iString> > findIslands(std::set<iString> & index,
    QMap< iString, std::set<iString> > adjCubes) {
  QVector< std::set<iString> > islands;

  while(index.size() != 0) {
    std::set<iString> connectedSet;

    QStack<iString> str_stack;
    std::set<iString>::iterator first = index.begin();
    str_stack.push(*first);

    // Depth search
    while(true) {
      index.erase(str_stack.top());
      connectedSet.insert(str_stack.top());

      // Find the first connected unvisited node
      iString nextNode = "";
      std::set<iString> neighbors = adjCubes[str_stack.top()];
      for(std::set<iString>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
        if(index.count(*i) == 1) {
          nextNode = *i;
          break;
        }
      }

      if(nextNode != "") {
        // Push the unvisited node
        str_stack.push(nextNode);
      }
      else {
        // Pop the visited node
        str_stack.pop();

        if(str_stack.size() == 0) break;
      }
    }

    islands.push_back(connectedSet);
  }

  return islands;
}


// Writes the list of cubes [ SerialNumber, Filename, ControlPoints ] to the output file
void WriteOutput(SerialNumberList num2cube, std::string filename,
                 std::set<iString> sns, QMap< iString, std::set<iString> > cps) {

  UserInterface &ui = Application::GetUserInterface();

  // Set the character to separate the entries
  iString delimit;
  if(ui.GetString("DELIMIT") == "TAB") {
    delimit = "\t";
  }
  else if(ui.GetString("DELIMIT") == "COMMA") {
    delimit = ",";
  }
  else if(ui.GetString("DELIMIT") == "SPACE") {
    delimit = " ";
  }
  else {
    delimit = ui.GetString("CUSTOM");
  }

  // Set up the output file for writing
  std::ofstream out_stream;
  out_stream.open(filename.c_str(), std::ios::out);
  out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

  for(std::set<iString>::iterator sn = sns.begin();
      sn != sns.end(); sn++) {
    // Serial Number of cube
    out_stream << *sn;

    // Filename of cube if given
    if(num2cube.HasSerialNumber(*sn)) {
      out_stream << delimit << Filename(num2cube.Filename(*sn)).Name();
    }
    else {
      out_stream << delimit << "UnknownFilename";
    }

    // Control Points where the cube was found to have the issue
    for(std::set<iString>::iterator cp = cps[*sn].begin();
        cp != cps[*sn].end(); cp++) {
      out_stream << "\t" << *cp;
    }

    out_stream << "\n";
  }

  out_stream.close();
}
