#include "Isis.h"

#include <string>
#include <iostream>

#include "OriginalLabel.h"
#include "Process.h"
#include "Cube.h"
#include "Pvl.h"
#include "Cube.h"
#include "Application.h"
#include "ImagePolygon.h"

using namespace std;
void IsisMain() {
  Isis::Preference::Preferences(true);

  cout << "Testing Isis::Process Class ... " << endl;
  Isis::Process p;

  cout << "Testing GetUserInterface ... " << endl;
  Isis::UserInterface &ui = Isis::Application::GetUserInterface();
  cout << ui.GetAsString("FROM") << endl;
  cout << endl;

  cout << "Testing SetOutputCube without an input cube ..." << endl;
  try {
    p.SetOutputCube("TO");
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetOutputCube with bogus samples ..." << endl;
  try {
    p.SetOutputCube("TO",0,1,1);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetOutputCube with bogus lines ..." << endl;
  try {
    p.SetOutputCube("TO",1,0,1);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetOutputCube with bogus bands ..." << endl;
  try {
    p.SetOutputCube("TO",1,1,0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetInputCube/SetInputWorkCube ... " << endl;
  Isis::Cube *icube = p.SetInputCube("FROM");
  cout << "Samples:  " << icube->Samples() << endl;
  cout << "Lines:  " << icube->Lines() << endl;
  cout << "Bands:  " << icube->Bands() << endl;
  cout << endl;

  cout << "Testing SetInputWorkCube OneBand Error ..." << endl;
  try {
    p.SetInputCube("FROM",Isis::OneBand);
  }
  catch (Isis::iException &e) {
    cout << "An exception was thrown!" << endl;
    e.Clear();
  }
  cout << endl;
  
  cout << "Testing SetInputWorkCube SizeMatch error for bands ..." << endl;
  try {
    Isis::CubeAttributeInput att("+1");
    p.SetInputCube("unitTest.cub",
                   att,Isis::SizeMatch);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
  
  cout << "Testing SetInputWorkCube SizeMatch error for lines ..." << endl;
  Isis::Cube cube;
  cube.SetDimensions(126,100,2);
  cube.Create("/tmp/isisprocess_01");
  cube.Close();
  try {
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    p2.SetInputCube("/tmp/isisprocess_01",att);
    p2.SetInputCube("unitTest.cub",att,
                    Isis::SizeMatch);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetInputWorkCube SizeMatch error for samples ..." << endl;
  cube.SetDimensions(100,126,2);
  cube.Create("/tmp/isisprocess_02");
  cube.Close();
  try {
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    p2.SetInputCube("/tmp/isisprocess_02",att);
    p2.SetInputCube("unitTest.cub",att,
                        Isis::SizeMatch);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;
 
  cout << "Testing SetInputWorkCube SpatialMatch error for lines ..." << endl;
  try {
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    p2.SetInputCube("/tmp/isisprocess_01",att);
    p2.SetInputCube("unitTest.cub",att,
                        Isis::SpatialMatch);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetInputWorkCube SpatialMatch error for samples ..." << endl;
  try {
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    p2.SetInputCube("/tmp/isisprocess_02",att);
    p2.SetInputCube("unitTest.cub",att,
                        Isis::SpatialMatch);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetInputWorkCube BandMatchOrOne error ..." << endl;
  cube.SetDimensions(126,126,3);
  cube.Create("/tmp/isisprocess_03");
  cube.Close();
  try {
    Isis::Process p2;
    Isis::CubeAttributeInput att;
    p2.SetInputCube("/tmp/isisprocess_03",att);
    p2.SetInputCube("unitTest.cub",att,
                        Isis::BandMatchOrOne);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing SetInputWorkCube AllMatchOrOne error ..." << endl;
   cube.SetDimensions(126,126,3);
   cube.Create("/tmp/isisprocess_03");
   cube.Close();
   try {
     Isis::Process p2;
     Isis::CubeAttributeInput att;
     p2.SetInputCube("/tmp/isisprocess_03",att);
     p2.SetInputCube("unitTest.cub",att,
                         Isis::AllMatchOrOne);
   }
   catch (Isis::iException &e) {
     e.Report(false);
   }
   cout << endl;

  cout << "Testing Logging ..." << endl;
  Isis::PvlGroup results("Results");
  results += Isis::PvlKeyword("Test","Me");
  results += Isis::PvlKeyword("No","Way");
  Isis::Application::Log(results);
  cout << endl;

  cout << "Testing label propagation (on) ..." << endl;
  Isis::Process p3;
  p3.SetInputCube("FROM");
  Isis::Cube *ocube = p3.SetOutputCube("TO");
  Isis::PvlGroup lab = ocube->GetGroup("Test");
  cout << lab["Keyword"] << endl; 
  cout << endl;

  cout << "Testing label propagation (off) ..." << endl;
  p3.PropagateLabels(false);
  Isis::Cube *ocube4 = p3.SetOutputCube("TO4",126,126,1);
  if (!ocube4->HasGroup("Test")) {
    cout << "Group Test does not exist" << endl;
  }
  p3.EndProcess();
  cout << endl;

  cout << "Testing OriginalLabel propagation (on) ..." << endl;
  Isis::Process p4;
  p4.SetInputCube("FROM");
  Isis::OriginalLabel ol(ui.GetAsString("FROM"));
  Isis::Pvl labels = ol.ReturnLabels();
  cout << labels << endl;
  cout << endl;

  cout << "Testing OriginalLabel propagation (off) ..." << endl;
  p4.PropagateOriginalLabel(false);
  Isis::Cube *ocube5 = p4.SetOutputCube("TO4",126,126,1);
  if (!ocube5->HasGroup("OriginalLabel")) {
    cout << "Group OriginalLabel does not exist" << endl;
  }
  p4.EndProcess();
  cout << endl;

  cout << "Testing Table propagation (on) ..." << endl;
  Isis::Process p5;
  p5.SetInputCube("FROM");
  Isis::Cube *ocube6 = p5.SetOutputCube("TO");
  Isis::Table table("Table");
  ocube6->Read(table);
  
  cout << "Number of record = " << table.Records() << endl;
  cout << "Record Size = " << table.RecordSize() << endl;
  cout << endl;

  cout << "Testing Table propagation (off) ..." << endl;
  p4.PropagateTables(false);
  Isis::Cube *ocube7 = p5.SetOutputCube("TO4",126,126,1);
  if (!ocube7->HasGroup("Table")) {
    cout << "Group Table does not exist" << endl;
  }
  p5.EndProcess();
  cout << endl;

  cout << "Testing Polygon propagation (on) ..." << endl;
  Isis::Process p6;
  p6.SetInputCube("FROM");
  Isis::Cube *ocube8 = p6.SetOutputCube("TO");

  Isis::Pvl *inlab1 = ocube8->Label();
  for (int i=0; i<inlab1->Objects(); i++) {
    if (inlab1->Object(i).IsNamed("Polygon")) {
      cout << "Image Polygon does exist" << endl;
      cout << "Size: " << (int)inlab1->Object(i)["Bytes"] << endl;
      // We cannot instantiate without spice data, so we won't try.
    }
  }

  cout << "Testing Polygon propagation (off) ..." << endl;
  p6.PropagatePolygons(false);
  bool exists = false;
  Isis::Cube *ocube9 = p6.SetOutputCube("TO4",126,126,1);
  Isis::Pvl *inlab2 = ocube9->Label();
  for (int i=0; i<inlab2->Objects(); i++) {
    if (inlab2->Object(i).IsNamed("Polygon")) {
      cout << "Image Polygon does exist" << endl;
      exists = true;
    }
  }
  if(!exists) {
    cout << "Image Polygon does not exist" << endl;
  }

  p6.EndProcess();
  cout << endl;

  cube.Open("/tmp/isisprocess_01");
  cube.Close(true);
  cube.Open("/tmp/isisprocess_02");
  cube.Close(true);
  cube.Open("/tmp/isisprocess_03");
  cube.Close(true);
  cube.Open("/tmp/isisprocess_04");
  cube.Close(true);
}
