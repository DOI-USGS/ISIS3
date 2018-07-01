#include "ControlNet.h"

#include <string>
#include <iostream>
#include <sstream>
#include <ctime>

#include <QList>
#include <QSet>
#include <QString>
#include <QVector>

#include "ControlCubeGraphNode.h"
#include "ControlMeasure.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "IException.h"
#include "Preference.h"
#include "SurfacePoint.h"
#include "SpecialPixel.h"
#include "TextFile.h"

using namespace std;
using namespace Isis;

/**
 * Unit test for ControlNet class 
 *  
 * @author ????-??-?? Unknown
 * @internal
 *   @history 2016-05-11 Jeannie Backer - Added tests for setTarget methods.
 */
bool lessThan(const ControlMeasure *m1, const ControlMeasure *m2) {
  return m1->GetResidualMagnitude() < m2->GetResidualMagnitude();
}


void printMeasures(QList< ControlMeasure * > measures,
    QMap< ControlMeasure *, QString > measureNames) {

  for (int i = 0; i < measures.size(); i++) {
    cout << "      " << measureNames[measures[i]].toStdString() <<
      " (" << measures[i]->GetCubeSerialNumber() << " -> " <<
      measures[i]->Parent()->GetId() << ", residual = " <<
      measures[i]->GetResidualMagnitude() << ")" << endl;
  }
}


void testConnectivity() {
  ControlNet net;

  ControlPoint *p1 = new ControlPoint("p1");
  ControlPoint *p2 = new ControlPoint("p2");
  ControlPoint *p3 = new ControlPoint("p3");
  ControlPoint *p4 = new ControlPoint("p4");
  ControlPoint *p5 = new ControlPoint("p5");

  QMap< ControlMeasure *, QString > measureNames;

  // m1 will act as a normal "good" edge, one of the first to add to the MST,
  // and the best route to ALPHA
  ControlMeasure *m1 = new ControlMeasure;
  m1->SetCubeSerialNumber("ALPHA");
  m1->SetResidual(1, 1);
  measureNames.insert(m1, "m1");

  // m2 will be a required edge, the only way to get to BETA
  ControlMeasure *m2 = new ControlMeasure;
  m2->SetCubeSerialNumber("BETA");
  m2->SetResidual(2, 2);
  measureNames.insert(m2, "m2");

  // m3 is not the best edge coming off GAMMA, but is part of the shortest
  // sub-path, so it will be added
  ControlMeasure *m3 = new ControlMeasure;
  m3->SetCubeSerialNumber("GAMMA");
  m3->SetResidual(3, 3);
  measureNames.insert(m3, "m3");

  // m4 looks like a good edge to add, but because m5 is so bad, we don't want
  // it in the final MST; it will be pruned out of the MST because m5 will never
  // be added
  ControlMeasure *m4 = new ControlMeasure;
  m4->SetCubeSerialNumber("GAMMA");
  m4->SetResidual(1, 1);
  measureNames.insert(m4, "m4");

  // A classic "bad" edge we definetly don't want in the MST if we can avoid it,
  // and luckily we can
  ControlMeasure *m5 = new ControlMeasure;
  m5->SetCubeSerialNumber("DELTA");
  m5->SetResidual(8, 8);
  measureNames.insert(m5, "m5");

  // This edge is pretty good, forms a path to DELTA from ALPHA that, with m7,
  // is shorter than the path from GAMMA to DELTA by m4 and m5
  ControlMeasure *m6 = new ControlMeasure;
  m6->SetCubeSerialNumber("DELTA");
  m6->SetResidual(3, 3);
  measureNames.insert(m6, "m6");

  // Not a great edge, an example where an edge would not normally be added if
  // alone, but together with another edge forms the best path to a node
  ControlMeasure *m7 = new ControlMeasure;
  m7->SetCubeSerialNumber("ALPHA");
  m7->SetResidual(4, 4);
  measureNames.insert(m7, "m7");

  // A measure living on a point that only connects to one image; this will be
  // pruned
  ControlMeasure *m8 = new ControlMeasure;
  m8->SetCubeSerialNumber("ALPHA");
  m8->SetResidual(1, 1);
  measureNames.insert(m8, "m8");

  // The only measure in the second island, this is used to illustrate how a
  // single node island produces an empty MST
  ControlMeasure *m9 = new ControlMeasure;
  m9->SetCubeSerialNumber("EPSILON");
  m9->SetResidual(1, 1);
  measureNames.insert(m9, "m9");

  p1->Add(m1);
  p1->Add(m2);
  p1->Add(m3);

  p2->Add(m4);
  p2->Add(m5);

  p3->Add(m6);
  p3->Add(m7);

  p4->Add(m8);

  p5->Add(m9);

  net.AddPoint(p1);
  net.AddPoint(p2);
  net.AddPoint(p3);
  net.AddPoint(p4);
  net.AddPoint(p5);

  cout << "\nTesting GetNodeConnections()\n";
  QList< QList< ControlCubeGraphNode * > > islands = net.GetNodeConnections();
  if (islands[0].contains(m9->ControlSN())) islands.swap(0, 1);
  cout << "  " << "Island Count = " << islands.size() << endl;

  cout << "\nTesting MinimumSpanningTree()\n";
  QList< QSet< ControlMeasure * > > spanningTrees;
  int nodeCount = 0;
  for (int i = 0; i < islands.size(); i++) {
    spanningTrees.append(net.MinimumSpanningTree(islands[i], lessThan));
    nodeCount += islands[i].size();
  }

  cout << "  " << "Tree Count = " << spanningTrees.size() << endl;
  cout << "  " << "Graph Node Count = " << nodeCount << endl;

  QList< QMap< QString, ControlMeasure * > > includedMaps;
  QList< QMap< QString, ControlMeasure * > > excludedMaps;
  for (int i = 0; i < spanningTrees.size(); i++) {
    includedMaps.append(QMap< QString, ControlMeasure * >());
    excludedMaps.append(QMap< QString, ControlMeasure * >());
  }

  int measureCount = 0;
  for (int p = 0; p < net.GetNumPoints(); p++) {
    ControlPoint *point = net.GetPoint(p);
    for (int m = 0; m < point->GetNumMeasures(); m++) {
      ControlMeasure *measure = point->GetMeasure(m);
      measureCount++;
      for (int i = 0; i < spanningTrees.size(); i++) {
        if (islands[i].contains(measure->ControlSN())) {
          if (spanningTrees[i].contains(measure))
            includedMaps[i].insert(measureNames[measure], measure);
          else
            excludedMaps[i].insert(measureNames[measure], measure);
        }
      }
    }
  }
  cout << "  " << "Measure Count = " << measureCount << endl;

  int includedMeasures = 0;
  for (int i = 0; i < spanningTrees.size(); i++) {
    QSet< ControlMeasure * > measures = spanningTrees[i];
    includedMeasures += measures.size();
  }

  if (islands.size() != spanningTrees.size()) {
    cout << "  " << "Island Count == " << islands.size() << " != " <<
      spanningTrees.size() << " == MST Count" << endl;
  }
  else {
    cout << "  " << "Island Count == " << islands.size() <<
      " == MST Count" << endl;

    for (int i = 0; i < spanningTrees.size(); i++) {
      cout << "\n  " << "Minimum Spanning Tree " << i << endl;

      cout << "    " << "Nodes = " << islands[i].size() << endl;
      cout << "    " << "Included Measures = " << includedMaps[i].size() << endl;
      printMeasures(includedMaps[i].values(), measureNames);

      cout << "    " << "Excluded Measures = " << excludedMaps[i].size() << endl;
      printMeasures(excludedMaps[i].values(), measureNames);
    }
  }
}


int main() {
  qsrand(42);

  Preference::Preferences(true);
  cout << "UnitTest for ControlNet ...." << endl << endl;

  cout << "******* test cube connection graph ************\n";
  ControlMeasure *p0m0 = new ControlMeasure;
  p0m0->SetCubeSerialNumber("ALPHA");
  ControlMeasure *p0m1 = new ControlMeasure;
  p0m1->SetCubeSerialNumber("BRAVO");
  ControlPoint *p0 = new ControlPoint("p0");
  p0->Add(p0m0);
  p0->Add(p0m1);

  ControlNet net;
  net.AddPoint(p0);

  // test ignoring of measures
  cout << net.CubeGraphToString() << "\n";
  p0m1->SetIgnored(true);
  cout << net.CubeGraphToString() << "\n";
  p0m1->SetIgnored(false);
  cout << net.CubeGraphToString() << "\n";

  // add another point - testing case where measures are added to points which
  // are already added to a net.
  cout << "testing measure addition to point already in network...\n";
  ControlMeasure *p1m0 = new ControlMeasure;
  p1m0->SetCubeSerialNumber("ALPHA");
  ControlPoint *p1 = new ControlPoint("p1");
  p1->Add(p1m0);
  net.AddPoint(p1);
  ControlMeasure *p1m1 = new ControlMeasure;
  p1m1->SetCubeSerialNumber("BRAVO");
  ControlMeasure *p1m2 = new ControlMeasure;
  p1m2->SetCubeSerialNumber("CHARLIE");
  p1->Add(p1m1);
  p1->Add(p1m2);
  cout << net.CubeGraphToString() << "\n";

  // test ignoring of point
  cout << "testing setting point to ignored.......................\n";
  p1->SetIgnored(true);
  cout << net.CubeGraphToString() << "\n";
  p1->SetIgnored(false);
  cout << net.CubeGraphToString() << "\n";

  // test measure deletion
  cout << "testing measure deletion & addition....................\n";
  p0->Delete(p0m1);
  p0m1 = NULL;
  cout << net.CubeGraphToString() << "\n";
  p0m0 = new ControlMeasure;
  p0m0->SetCubeSerialNumber("DELTA");
  p0->Add(p0m0);
  cout << net.CubeGraphToString() << "\n";

  // test point deletion
  cout << "testing point deletion.................................\n";
  net.DeletePoint(p1);
  p1 = NULL;
  cout << net.CubeGraphToString() << "\n";

  cout << "******* Done testing cube graph ***************\n\n\n";

  cout << "testing GetCubeSerials... (NOTE: unittest sorts the results)\n";
  QList< QString > serials = net.GetCubeSerials();
  
  // Let's sort the data since GetCubeSerials relies on a QHash 
  QStringList sortedSerials(serials);
  sortedSerials.sort();

  for (int i = 0; i < sortedSerials.size(); i++)
    cout << "  " << qPrintable(sortedSerials[i]) << "\n";
  cout << "\n";


  ControlNet cn1;

  cout << "testing set target.................................\n";

  cout << "Set target/radii directly. Invalid radii." << endl;
  QVector<Distance> targetRadii;
  try {
    cn1.SetTarget("Fail", targetRadii);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;
  cout << "Set target/radii directly. Valid radii." << endl;
  targetRadii += Distance(Isis::PI, Distance::Meters);
  targetRadii += Distance(Isis::HALFPI, Distance::Meters);
  targetRadii += Distance(Isis::E, Distance::Meters);
  cn1.SetTarget("SomethingStrange", targetRadii);
  vector<Distance> targRad = cn1.GetTargetRadii();
  cout << "        TargetName = " << cn1.GetTarget() << endl;
  cout << "        TargetRadii = (" << targRad[0].meters() << ", " 
                                    << targRad[1].meters() << ", "
                                    << targRad[2].meters() << ")" << endl;
  cout << endl;
  cout << "Set target/radii using empty PVL." << endl;
  Pvl label;
  // no mapping group, (i.e. target name empty, no equatorial radius)
  // catch exception and set radii to nulls
  cn1.SetTarget(label);
  targRad = cn1.GetTargetRadii();
  cout << "        TargetName = " << cn1.GetTarget() << endl;
  cout << "        TargetRadii = (" << targRad[0].meters() << ", " 
                                    << targRad[1].meters() << ", "
                                    << targRad[2].meters() << ")" << endl;
  cout << endl;
  cout << "Set target/radii using PVL with no PolarRadius." << endl;
  label += PvlGroup("Mapping");
  PvlGroup &mapping = label.findGroup("Mapping");
  mapping += PvlKeyword("TargetName", "Mars");
  mapping += PvlKeyword("EquatorialRadius", "");
  cout << label << endl;
  cn1.SetTarget(label);
  targRad = cn1.GetTargetRadii();
  cout << "        TargetName = " << cn1.GetTarget() << endl;
  cout << "        TargetRadii = (" << targRad[0].meters() << ", " 
                                    << targRad[1].meters() << ", "
                                    << targRad[2].meters() << ")" << endl;
  cout << endl;
  cout << "Set target/radii using PVL containing both radii." << endl;
  mapping.addKeyword(PvlKeyword("EquatorialRadius", "6.0"), PvlContainer::Replace);
  mapping.addKeyword(PvlKeyword("PolarRadius", "1.0"), PvlContainer::Replace);
  cout << label << endl;
  cn1.SetTarget(label);
  targRad = cn1.GetTargetRadii();
  cout << "        TargetName = " << cn1.GetTarget() << endl;
  cout << "        TargetRadii = (" << targRad[0].meters() << ", " 
                                    << targRad[1].meters() << ", "
                                    << targRad[2].meters() << ")" << endl;
  cout << endl;
  cout << "Set empty target." << endl;
  cn1.SetTarget("");
  targRad = cn1.GetTargetRadii();
  cout << "        TargetName = " << cn1.GetTarget() << endl;
  cout << "        TargetRadii = (" << targRad[0].meters() << ", " 
                                    << targRad[1].meters() << ", "
                                    << targRad[2].meters() << ")" << endl;
  cout << endl;
  cout << "Set Mars target." << endl;
  cn1.SetTarget("Mars");
  targRad = cn1.GetTargetRadii();
  cout << "        TargetName = " << cn1.GetTarget() << endl;
  cout << "        TargetRadii = (" << targRad[0].meters() << ", " 
                                    << targRad[1].meters() << ", "
                                    << targRad[2].meters() << ")" << endl;
  cout << endl;

  cn1.SetTarget("Mars");
  cn1.SetNetworkId("Test");
  cn1.SetUserName("TSucharski");
  cn1.SetCreatedDate("2010-07-10T12:50:15");
  cn1.SetModifiedDate("2010-07-10T12:50:55");
  cn1.SetDescription("UnitTest of ControlNetwork");

  std::string pointId = "T000";
  std::string id = "id";

  for (int i = 0; i < 5; i++) {
    std::stringstream oss1;
    oss1.flush();
    oss1 << pointId << i;
    ControlPoint *cp = new ControlPoint(oss1.str().c_str());

    if (i == 0) {
      cp->SetType(ControlPoint::Fixed);
      cp->SetAprioriSurfacePointSource(ControlPoint::SurfacePointSource::Basemap);
      cp->SetAprioriSurfacePointSourceFile("/work1/tsucharski/basemap.cub");
      cp->SetAprioriRadiusSource(ControlPoint::RadiusSource::DEM);
      cp->SetAprioriRadiusSourceFile("$base/dems/molaMarsPlanetaryRadius0003.cub");

      SurfacePoint surfacePt(Displacement(-424.024048, Displacement::Meters),
          Displacement(734.4311949, Displacement::Meters),
          Displacement(529.919264, Displacement::Meters),
          Distance(10, Distance::Meters),
          Distance(50, Distance::Meters),
          Distance(20, Distance::Meters));

      cp->SetAdjustedSurfacePoint(surfacePt);
      cp->SetAprioriSurfacePoint(surfacePt);
    }
    else if (i == 1) {
      cp->SetIgnored(true);
    }
    else {
      cp->SetType(ControlPoint::Free);
    }

    for (int k = 0; k < 2; k++) {
      ControlMeasure *cm = new ControlMeasure;
      std::stringstream oss2;
      oss2.flush();
      oss2 << id << k;
      cm->SetCubeSerialNumber(oss2.str().c_str());
      cm->SetType(ControlMeasure::RegisteredSubPixel);
      cm->SetLogData(
        ControlMeasureLogData(ControlMeasureLogData::GoodnessOfFit,
            0.53523 * (k + 1)));
      cm->SetCoordinate(1.0, 2.0);
      cm->SetResidual(-3.0, 4.0);
      cm->SetDiameter(15.0);
      cm->SetAprioriSample(2.0);
      cm->SetAprioriLine(5.0);
      cm->SetSampleSigma(.01);
      cm->SetLineSigma(.21);
      cm->SetChooserName("pointreg");
      cm->SetDateTime("2010-08-27T17:10:06");

      cp->Add(cm);

      if (k == 0) {
        cp->SetRefMeasure(cm);
        cm->SetChooserName("cnetref");
        cm->SetDateTime("2010-08-27T17:10:06");
        cm->SetEditLock(true);
      }
      if (k == 1) {
        cm->SetType(ControlMeasure::Candidate);
        cm->SetIgnored(true);
        cm->SetChooserName("autoseed");
      }

      cm->SetDateTime("2010-08-27T17:10:06");
    }

    cp->SetChooserName("autoseed");
    cp->SetDateTime("2010-08-27T17:10:06");

    if (i == 0)
      cp->SetEditLock(true);

    cn1.AddPoint(cp);
  }

  cout << "Test adding control points with identical id numbers ..." << endl;
  try {
    cn1.AddPoint(cn1.GetPoint(3));
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;

  //  Delete point with invalid point type, first save id for next test
  QString id2 = cn1[2]->GetId();
  cn1.DeletePoint(2);

  cout << "Test deleting nonexistant control point id ..." << endl;
  try {
    cn1.DeletePoint(id2);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;


  cout << "Test deleting nonexistant control point index ..." << endl;
  try {
    cn1.DeletePoint(7);
  }
  catch (IException &e) {
    e.print();
  }
  cout << endl;

  cout << "Writing ControlNet to temp.txt in Pvl format" << endl;
  cn1.Write("temp.txt", true);

  cout << "Reading ControlNet from temp.txt" << endl;
  ControlNet cn2("temp.txt");

  cout << "Writing ControlNet to temp2.txt in Pvl format" << endl;
  cn2.Write("temp2.txt", true);
  cout << "Diffing temp.txt and temp2.txt" << endl;
  if (system("cmp temp.txt temp2.txt")) {
    cout << "ERROR:  Text Files are not the same!" << endl;
  }

  cout << "Test read/write of binary control networks ..." << endl;

  //  Test read/write of binary
  cout << "Writing ControlNet to temp.bin in binary format" << endl;
  cn2.Write("temp.bin");
  ControlNet cn3;

  cout << "Reading ControlNet from temp.bin" << endl;
  cn3.ReadControl("temp.bin");

  cout << "Writing ControlNet to temp.txt in Pvl format" << endl;
  cn3.Write("temp.txt", true);

  cout << "Reading Pvl from temp.txt and then printing" << endl;
  Pvl pvl1("temp.txt");
  cout << endl << pvl1 << endl << endl;

  cout << "Writing ControlNet to temp2.bin in binary format" << endl;
  cn3.Write("temp2.bin");
  cout << "Reading ControlNet from temp2.bin" << endl;
  ControlNet cn4("temp2.bin");

  cout << "Diffing temp.bin and temp2.bin" << endl;
  if (system("cmp temp.bin temp2.bin")) {
    cout << "ERROR:  Binary files are not the same." << endl;
  }
  else {
    cout << "Read/Write of binary files OK." << endl;
  }
  remove("temp.txt");
  remove("temp2.txt");
  remove("temp.bin");
  remove("temp2.bin");

  cout << "Testing GetCubeGraphNodes\n";

  QList< ControlCubeGraphNode * > graphnodes = net.GetCubeGraphNodes();
  // Use this to sort the output
  QList<QString> sortedSNs;
  foreach ( ControlCubeGraphNode * node, graphnodes ) {
    sortedSNs.append(node->getSerialNumber());
  }
  sort(sortedSNs.begin(), sortedSNs.end());
  foreach ( QString sn, sortedSNs ) {
    cout << "    " << sn << "\n";
  }
  
  cout << "\nTesting getGraphNode: "
       << net.getGraphNode("ALPHA")->getSerialNumber() << "\n";
       
  cout << "\nTesting getEdgeCount: " << net.getEdgeCount() << "\n";

  testConnectivity();

  cout << "\nTesting take() functionality to take owernship of the points in a ControlNet:" << endl;

  cout << "Original control net number of points: " << QString::number(net.GetNumPoints()) << endl; 

  QList<ControlPoint*> points = net.take(); 
  
  cout << "Number of points taken out: " << QString::number(points.length()) << endl; 
  
  cout << "Now there should be zero points in the original control net. There are: " 
       << QString::number(net.GetNumPoints()) << endl; 

  cout << "And zero pointIDs in the original control net. There are: " 
       << QString::number(net.GetPointIds().length()) << endl; 

  cout << "And zero cubeGraphNodes in the original control net. There are: " 
       << QString::number(net.GetCubeGraphNodes().length()) << endl; 

  //system("cat unitTest.output | grep -v DateTime > temp.output; mv temp.output unitTest.output");
  //system("cat unitTest.output | sed -r s/`date +%Y-%m-%dT`\\[0-9:\\]\\{8\\}/2010-08-27T17:10:06/g > temp.output; mv temp.output unitTest.output");

  return 0;
#if 0

  // -------------------------------------------------------------------------
  // Testing the google protocol buffer methods added to the ControlNet class
  // SLA 6/30/09
  // -------------------------------------------------------------------------

  cout << "Enter input cnet: ";
  string inNet;
  cin >> inNet;
  string outFile;
  cout << "Enter output file (directory & prefix, no extension): ";
  cin >> outFile;

  ControlNet *cn1 = new ControlNet;
  cout << "Speed Test for ControlNet ...." << endl << endl;
  cout << "\nReading from the ascii file....    " << inNet << endl;
  std::clock_t start = std::clock();
//  cn1.ReadControl("/work1/tsucharski/protobuf/isis/nets/cnet.net");
//  cn1->ReadControl("/work1/tsucharski/protobuf/isis/nets/pntreg2.net");
//  cn1->ReadControl("/work1/tsucharski/protobuf/isis/nets/pntreg_combinedparts.net");
  cn1->ReadControl(inNet);
  std::cout << ((std::clock() - start) / (double)CLOCKS_PER_SEC) << " seconds \n";

  cout << "\nWriting to the binary file...." << endl;
  start = std::clock();
//  cn1.WritePB("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/cnet.bin");
//  cn1->WritePB("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg2.bin");
//  cn1->WritePB("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg_combinedparts.bin");
  cn1->WritePB(outFile + ".bin");
  std::cout << ((std::clock() - start) / (double)CLOCKS_PER_SEC) << " seconds \n";
  delete cn1;

//  ControlNet cn2;
  ControlNet *cn2 = new ControlNet;

  cout << "\nReading from the binary file...." << endl;
  std::clock_t start2 = std::clock();
//  cn2.ReadPBControl("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/cnet.bin");
//  cn2->ReadPBControl("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg2.bin");
//  cn2->ReadPBControl("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg_combinedparts.bin");
  cn2->ReadPBControl(outFile + ".bin");
  std::cout << ((std::clock() - start2) / (double)CLOCKS_PER_SEC) << " seconds \n";

//  apLat = (*cn2)[2].AprioriLatitude();
//  cout<<"binaryNet AprioriLatitude = "<<apLat<<endl;

//cout << "\nConverting the binary to Pvl...." << endl;
//std::clock_t start2 = std::clock();
//cn1.ConvertBinToPvl();
//std::cout<< ( ( std::clock() - start2 ) / (double)CLOCKS_PER_SEC ) <<" seconds \n";



  cout << "\nWriting to the Pvl file...." << endl;
  std::clock_t start3 = std::clock();
//  cn2.Write("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/cnet.pvl");
//  cn2->Write("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg2.pvl");
//  cn2->Write("/work1/tsucharski/protobuf/isis/nets/testMultiMsgs/pntreg_combinedparts.pvl");
  cn2->Write(outFile + ".pvl");
  std::cout << ((std::clock() - start3) / (double)CLOCKS_PER_SEC) << " seconds \n";

#endif
}
