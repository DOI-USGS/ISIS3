#include "Isis.h"

#include "IsisDebug.h"

#include <iostream>
#include <sstream>
#include <set>

#include <QStack>
#include <QString>
#include <QVector>

#include <geos_c.h>
#include <geos/algorithm/ConvexHull.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Polygon.h>

#include "Camera.h"
#include "CameraFactory.h"
#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlPointList.h"
#include "CubeManager.h"
#include "FileList.h"
#include "FileName.h"
#include "iString.h"
#include "ProjectionFactory.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SerialNumber.h"
#include "SerialNumberList.h"
#include "UserInterface.h"

using namespace Isis;
using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::set;
using std::string;
using std::stringstream;

QMap< iString, set<iString> > constructPointSets(
  set<iString> &index, ControlNet innet);
QVector< set<iString> > findIslands(
  set<iString> &index,
  QMap < iString, set<iString> > adjCubes);

QList< ControlCubeGraphNode * > checkSerialList(
    SerialNumberList * serialNumbers, ControlNet * controlNet);

void WriteOutput(SerialNumberList num2cube,
                 string filename,
                 set<iString> sns,
                 QMap< iString, set<iString> > cps);

double getControlFitness(const ControlCubeGraphNode * node, double tolerance, Cube * cube);
void noLatLonCheck(ControlNet &cnet, CubeManager &manager, Progress &progress,
    bool ignore, SerialNumberList &num2cube,
    set<iString> &noLatLonSerialNumbers,
    QMap< iString, set<iString> > &noLatLonControlPoints);

string buildRow(SerialNumberList &serials, string sn);
string buildRow(SerialNumberList &serials, string sn, set<iString> &cps);
string buildRow(SerialNumberList &serials, string sn, double value);
void outputRow(ofstream &outStream, string rowText);


iString delimiter;


// Main program
void IsisMain() {

  Progress progress;
  UserInterface &ui = Application::GetUserInterface();

  ControlNet innet(ui.GetFileName("CNET"));
  iString prefix(ui.GetString("PREFIX"));
  bool ignore = ui.GetBoolean("IGNORE");

  // Set the character to separate the entries
  if(ui.GetString("DELIMIT") == "TAB") {
    delimiter = "\t";
  }
  else if(ui.GetString("DELIMIT") == "COMMA") {
    delimiter = ",";
  }
  else if(ui.GetString("DELIMIT") == "SPACE") {
    delimiter = " ";
  }
  else {
    delimiter = ui.GetString("CUSTOM");
  }

  // Sets up the list of serial numbers for
  FileList inlist(ui.GetFileName("FROMLIST"));
  set<iString> inListNums;
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

  set<iString> singleMeasureSerialNumbers;
  QMap< iString, set<iString> > singleMeasureControlPoints;

  QMap< iString, set<iString> > duplicateControlPoints;

  set<iString> noLatLonSerialNumbers;
  QMap< iString, set<iString> > noLatLonControlPoints;
  set<iString> latlonchecked;

  QMap< iString, int > cubeMeasureCount;

  // Manage cubes used in NOLATLON
  CubeManager cbman;
  cbman.SetNumOpenCubes(50);

  if (ui.GetBoolean("NOLATLON")) {
    noLatLonCheck(innet, cbman, progress, ignore, num2cube,
        noLatLonSerialNumbers, noLatLonControlPoints);
  }

  // Set calculating progress
  if(innet.GetNumPoints() > 0) {
    progress.SetText("Calculating");
    progress.SetMaximumSteps(innet.GetNumPoints());
    progress.CheckStatus();
  }

  // Loop through all control points in control net
  for(int cp = 0; cp < innet.GetNumPoints(); cp ++) {
    if(ignore && innet.GetPoint(cp)->IsIgnored()) continue;
    ControlPoint *controlpt = innet.GetPoint(cp);

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
  set<iString> index;
  QMap< iString, set<iString> > adjCubes = constructPointSets(index, innet);
  QVector< set<iString> > islands = findIslands(index, adjCubes);

  // Output islands in the file-by-file format
  //  Islands that have no cubes listed in the input list will
  //  not be shown.
  for(int i = 0; i < (int)islands.size(); i++) {
    iString name(FileName(prefix + "Island." + iString(i + 1)).expanded());
    ofstream out_stream;
    out_stream.open(name.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    bool hasList = false;
    for(set<iString>::iterator island = islands[i].begin();
        island != islands[i].end(); island++) {
      if(num2cube.HasSerialNumber(*island)) {
        outputRow(out_stream, buildRow(num2cube, *island));
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

  stringstream ss(stringstream::in | stringstream::out);

  results.AddKeyword(PvlKeyword("Islands", iString((BigInt)islands.size())));
  ss << endl << "----------------------------------------" \
     "----------------------------------------" << endl;
  if(islands.size() == 1) {
    ss << "The cubes are fully connected by the Control Network." << endl;
  }
  else if(islands.size() == 0) {
    ss << "There are no control points in the provided Control Network [";
    ss << FileName(ui.GetFileName("CNET")).name() << "]" << endl;
  }
  else {
    ss << "The cubes are NOT fully connected by the Control Network." << endl;
    ss << "There are " << islands.size() << " disjoint sets of cubes." << endl;
  }

  if(ui.GetBoolean("SINGLEMEASURE")  &&  singleMeasureSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("SingleMeasure", iString((BigInt)singleMeasureSerialNumbers.size())));

    iString name(FileName(prefix + "SinglePointCubes.txt").expanded());
    WriteOutput(num2cube, name,
                singleMeasureSerialNumbers, singleMeasureControlPoints);

    int serials = singleMeasureSerialNumbers.size();
    ss << "----------------------------------------" \
       "----------------------------------------" << endl;
    ss << "There " << ((serials == 1) ? "is " : "are ") << singleMeasureSerialNumbers.size();
    ss << ((serials == 1) ? " cube" : " cubes") << " in Control Points with only a single";
    ss << " Control Measure." << endl;
    ss << "The serial numbers of these measures are listed in [";
    ss <<  FileName(name).name() + "]" << endl;
  }

  if(ui.GetBoolean("NOLATLON")  &&  noLatLonSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("NoLatLonCubes", iString((BigInt)noLatLonSerialNumbers.size())));

    iString name(FileName(prefix + "NoLatLon.txt").expanded());
    WriteOutput(num2cube, name,
                noLatLonSerialNumbers, noLatLonControlPoints);

    ss << "----------------------------------------" \
       "----------------------------------------" << endl;
    ss << "There are " << noLatLonSerialNumbers.size();
    ss << " serial numbers in the Control Network which are listed in the";
    ss << " input list and cannot compute latitude and longitudes." << endl;
    ss << "These serial numbers, filenames, and control points are listed in [";
    ss << FileName(name).name() + "]" << endl;
  }

  // Perform Low Coverage check if it was selected
  iString coverageOp = "LowCoverage";
  int failedCoverageCheck = 0;
  if (ui.GetBoolean(iString(coverageOp).UpCase())) {
    QList< ControlCubeGraphNode * > nodes = innet.GetCubeGraphNodes();

    if (nodes.size() > 0) {
      iString name(FileName(prefix + coverageOp + ".txt").expanded());
      ofstream out_stream;
      out_stream.open(name.c_str(), std::ios::out);
      out_stream.seekp(0, std::ios::beg); // Start writing from file beginning

      double tolerance = ui.GetDouble("TOLERANCE");
      foreach (ControlCubeGraphNode * node, nodes) {
        iString sn = node->getSerialNumber();

        if (num2cube.HasSerialNumber(sn)) {
          // Create a convex hull
          Cube *cube = cbman.OpenCube(num2cube.FileName(sn));
          double controlFitness = getControlFitness(node, tolerance, cube);

          if (controlFitness < tolerance) {
            outputRow(out_stream, buildRow(num2cube, sn, controlFitness));
            failedCoverageCheck++;
          }
        }
      }

      out_stream.close();

      // Represent the user-specified tolerance as a percentage value
      double tolerancePercent = tolerance * 100.0;
      ss << "----------------------------------------"
        "----------------------------------------" << endl;
      ss << "There are " << failedCoverageCheck << " images in both the "
        "input list and Control Network whose convex hulls cover less than " <<
        tolerancePercent << "\% of the image" << endl;
      ss << "The names of these images, along with the failing convex hull "
        "coverages, are listed in [" << FileName(name).name() << "]" << endl;

      results.AddKeyword(
          PvlKeyword(coverageOp, iString((BigInt) failedCoverageCheck)));
    }
  }

  // At this point, inListNums is the list of cubes NOT included in the
  //  ControlNet, and inListNums are their those cube's serial numbers.
  if(ui.GetBoolean("NOCONTROL") && !inListNums.empty()) {
    results.AddKeyword(PvlKeyword("NoControl", iString((BigInt)inListNums.size())));

    iString name(FileName(prefix + "NoControl.txt").expanded());
    ofstream out_stream;
    out_stream.open(name.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(set<iString>::iterator sn = inListNums.begin();
        sn != inListNums.end();
        sn ++) {
      outputRow(out_stream, buildRow(num2cube, *sn));
    }
    out_stream.close();

    ss << "----------------------------------------" \
       "----------------------------------------" << endl;
    ss << "There are " << inListNums.size();
    ss << " cubes in the input list [" << FileName(ui.GetFileName("FROMLIST")).name();
    ss << "] which do not exist or are ignored in the Control Network [";
    ss << FileName(ui.GetFileName("CNET")).name() << "]" << endl;
    ss << "These cubes are listed in [" + FileName(name).name() + "]" << endl;
  }

  // In addition, nonListedSerialNumbers should be the SerialNumbers of
  //  ControlMeasures in the ControlNet that do not have a correlating
  //  cube in the input list.
  if(ui.GetBoolean("NOCUBE")  &&  nonListedSerialNumbers.size() > 0) {
    results.AddKeyword(
      PvlKeyword("NoCube", iString((BigInt)nonListedSerialNumbers.size())));

    iString name(FileName(prefix + "NoCube.txt").expanded());
    ofstream out_stream;
    out_stream.open(name.c_str(), std::ios::out);
    out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

    for(int sn = 0; sn < (int)nonListedSerialNumbers.size(); sn++) {
      int validMeasureCount = innet.getGraphNode(
          nonListedSerialNumbers[sn])->getValidMeasures().size();
      string rowText = nonListedSerialNumbers[sn] + " (Valid Measures: " +
          iString(validMeasureCount) + ")";
      outputRow(out_stream, rowText);
    }

    out_stream.close();

    ss << "----------------------------------------" \
       "----------------------------------------" << endl;
    ss << "There are " << nonListedSerialNumbers.size();
    ss << " serial numbers in the Control Net [";
    ss << FileName(ui.GetFileName("CNET")).baseName();
    ss << "] \nwhich do not exist in the  input list [";
    ss << FileName(ui.GetFileName("FROMLIST")).name() << "]" << endl;
    ss << "These serial numbers are listed in [";
    ss << FileName(name).name() + "]" << endl;
  }

  // At this point cubeMeasureCount should be equal to the number of
  //  ControlMeasures associated with each serial number.
  if(ui.GetBoolean("SINGLECUBE")) {
    set<iString> singleMeasureCubes;
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

      iString name(FileName(prefix + "SingleCube.txt").expanded());
      ofstream out_stream;
      out_stream.open(name.c_str(), std::ios::out);
      out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

      for(set<iString>::iterator sn = singleMeasureCubes.begin();
          sn != singleMeasureCubes.end();
          sn ++) {
        outputRow(out_stream, buildRow(num2cube, *sn));
      }

      out_stream.close();

      ss << "----------------------------------------" \
         "----------------------------------------" << endl;
      ss << "There are " << singleMeasureCubes.size();
      ss << " serial numbers in the Control Net [";
      ss << FileName(ui.GetFileName("CNET")).baseName();
      ss << "] which only exist in one Control Measure." << endl;
      ss << "These serial numbers are listed in [";
      ss << FileName(name).name() + "]" << endl;
    }
  }

  ss << "----------------------------------------" \
     "----------------------------------------" << endl << endl;
  string log = ss.str();
  Application::Log(results);

  if(ui.IsInteractive()) {
    Application::GuiLog(log);
  }
  else {
    std::cout << ss.str();
  }

}


// Links cubes to other cubes it shares control points with
QMap< iString, set<iString> > constructPointSets(set<iString> & index,
    ControlNet innet) {
  QMap< iString, set<iString > > adjPoints;

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
QVector< set<iString> > findIslands(set<iString> & index,
    QMap< iString, set<iString> > adjCubes) {
  QVector< set<iString> > islands;

  while(index.size() != 0) {
    set<iString> connectedSet;

    QStack<iString> str_stack;
    set<iString>::iterator first = index.begin();
    str_stack.push(*first);

    // Depth search
    while(true) {
      index.erase(str_stack.top());
      connectedSet.insert(str_stack.top());

      // Find the first connected unvisited node
      iString nextNode = "";
      set<iString> neighbors = adjCubes[str_stack.top()];
      for(set<iString>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
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


// Writes the list of cubes [ SerialNumber, FileName, ControlPoints ] to the output file
void WriteOutput(SerialNumberList num2cube, string filename,
                 set<iString> sns, QMap< iString, set<iString> > cps) {

  // Set up the output file for writing
  ofstream out_stream;
  out_stream.open(filename.c_str(), std::ios::out);
  out_stream.seekp(0, std::ios::beg);   //Start writing from beginning of file

  for(set<iString>::iterator sn = sns.begin();
      sn != sns.end(); sn++) {
    outputRow(out_stream, buildRow(num2cube, *sn, cps[*sn]));
  }

  out_stream.close();
}


double getControlFitness(const ControlCubeGraphNode * node, double tolerance, Cube * cube) {
  double controlFitness = 0;

  static  geos::geom::GeometryFactory geosFactory;
  geos::geom::CoordinateSequence * pts = new geos::geom::CoordinateArraySequence();
  QList< ControlMeasure * > measures = node->getMeasures();

  // Populate pts with a list of control points
  foreach (ControlMeasure * measure, measures) {
    pts->add(geos::geom::Coordinate(measure->GetSample(), measure->GetLine()));
  }
  pts->add(geos::geom::Coordinate(measures[0]->GetSample(), measures[0]->GetLine()));

  if (pts->size() >= 4) {

    // Calculate the convex hull
    geos::geom::Geometry * convexHull = geosFactory.createPolygon(
        geosFactory.createLinearRing(pts), 0)->convexHull();

    // Calculate the area of the convex hull
    double convexArea = convexHull->getArea();
    iString sn = node->getSerialNumber();
    double cubeArea = cube->getSampleCount() * cube->getLineCount();

    controlFitness = convexArea / cubeArea;

    if (pts) {
      delete pts;
      pts = NULL;
    }
  }

  return controlFitness;

}


void noLatLonCheck(ControlNet &cnet, CubeManager &manager, Progress &progress,
    bool ignore, SerialNumberList &num2cube,
    set<iString> &noLatLonSerialNumbers,
    QMap< iString, set<iString> > &noLatLonControlPoints) {

  // Set calculating progress
  QList<ControlCubeGraphNode *> graphNodes = cnet.GetCubeGraphNodes();
  if (graphNodes.size() > 0) {
    progress.SetText("Checking for No Lat/Lon");
    progress.SetMaximumSteps(graphNodes.size());
    progress.CheckStatus();
  }

  for (int sn = 0; sn < graphNodes.size(); sn++) {
    ControlCubeGraphNode *graphNode = graphNodes[sn];
    iString serialNumber = graphNode->getSerialNumber();

    if (num2cube.HasSerialNumber(serialNumber)) {
      Cube *cube = manager.OpenCube(num2cube.FileName(serialNumber));

      // Try to create
      Camera *cam = NULL;
      bool createdCamera = true;
      try {
        cam = cube->getCamera();
      }
      catch(IException &) {
        createdCamera = false;
      }

      QList<ControlMeasure *> measures = graphNode->getMeasures();
      for (int cm = 0; cm < measures.size(); cm++) {
        ControlMeasure *measure = measures[cm];
        ControlPoint *point = measure->Parent();

        if (ignore && point->IsIgnored()) continue;

        // Check the exact measure location
        bool setCamera = false;
        if (createdCamera) {
          setCamera = cam->SetImage(measure->GetSample(), measure->GetLine());
        }

        // Record it if it failed at anything
        if (!createdCamera || !setCamera) {
          noLatLonSerialNumbers.insert(serialNumber);
          noLatLonControlPoints[serialNumber].insert(point->GetId());
        }
      }
    }

    progress.CheckStatus();
  }
}


string buildRow(SerialNumberList &serials, string sn) {
  string cubeName = serials.HasSerialNumber(sn) ?
      FileName(serials.FileName(sn)).expanded() : "UnknownFilename";
  return cubeName + delimiter + sn;
}


string buildRow(SerialNumberList &serials, string sn, set<iString> &cps) {
  string rowText = buildRow(serials, sn);

  // Control Points where the cube was found to have the issue
  for (set<iString>::iterator cp = cps.begin(); cp != cps.end(); cp++) {
    rowText += delimiter + *cp;
  }

  return rowText;
}


string buildRow(SerialNumberList &serials, string sn, double value) {
  return buildRow(serials, sn) + delimiter + iString(value);
}


void outputRow(ofstream &outStream, string rowText) {
  outStream << rowText << "\n";
}

