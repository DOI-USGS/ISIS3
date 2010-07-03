#define GUIHELPERS
#include "Isis.h"
#include "Pvl.h"

#include "Process.h"

#include "Blob.h"
#include "History.h"
#include "CameraDetectorMap.h"
#include "Application.h"
#include "CameraFocalPlaneMap.h"
#include "PvlObject.h"
#include "AlphaCube.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Isis;
using namespace std;

void LoadMatchSummingMode ();
void LoadInputSummingMode ();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["LoadMatchSummingMode"] = (void*) LoadMatchSummingMode;
  helper ["LoadInputSummingMode"] = (void*) LoadInputSummingMode;
  return helper;
}

void IsisMain(){
            
  // Create a process so we can output the noproj'd labels without overwriting
  Process p;

  // Open the user interface and get the input file and the ideal specs file
  UserInterface &ui = Application::GetUserInterface();
  Cube *mcube,*icube;

  // If a MATCH cube is entered, make sure to SetInputCube it first to get the SPICE blobs
  // from it propagated to the TO labels

  // Until polygon blobs are detached without "/" don't propagate them
  p.PropagatePolygons ( false );

  if ((ui.WasEntered ("MATCH"))) {
    mcube = p.SetInputCube("MATCH");
    icube = p.SetInputCube("FROM");
  }
  else{
    mcube = icube = p.SetInputCube("FROM");
  }

  Camera *incam = mcube->Camera();

  // Extract Instrument groups from input labels for the output match and noproj'd cubes
  PvlGroup inst = mcube->GetGroup("Instrument");
  PvlGroup fromInst = icube->GetGroup("Instrument");
  std::string groupName = (string) inst["SpacecraftName"] + "/";
  groupName += (string) inst.FindKeyword("InstrumentId");

  // Get Ideal camera specifications
  Filename specs;
  if ((ui.WasEntered ("SPECS"))) {
    specs = ui.GetFilename("SPECS");
  }
  else {
    specs = "$base/applications/noprojInstruments???.pvl";
    specs.HighestVersion();
  }
  Pvl idealSpecs( specs.Expanded() );
  PvlObject obSpecs = idealSpecs.FindObject("IdealInstrumentsSpecifications");

  PvlGroup idealGp = obSpecs.FindGroup(groupName);
  double transx,transy,transl,transs;
  transx = transy = transl = transs = 0.;
  if (idealGp.HasKeyword("TransX")) transx = idealGp["TransX"];
  if (idealGp.HasKeyword("TransY")) transy = idealGp["TransY"];
  if (idealGp.HasKeyword("ItransL")) transl = idealGp["ItransL"];
  if (idealGp.HasKeyword("ItransS")) transs = idealGp["ItransS"];
  int detectorSamples = mcube->Samples();
  if (idealGp.HasKeyword("DetectorSamples")) detectorSamples = idealGp["DetectorSamples"];
  int numberLines = mcube->Lines();
  int numberBands = mcube->Bands();

  if (idealGp.HasKeyword("DetectorLines")) numberLines = idealGp["DetectorLines"];

  int xDepend = incam->FocalPlaneMap()->FocalPlaneXDependency();

  // Get output summing mode
  if (ui.GetString("SOURCE") == "FROMMATCH") {
    LoadMatchSummingMode();
  }
  else if (ui.GetString("SOURCE") == "FROMINPUT") {
    LoadInputSummingMode();
  }

  double pixPitch = incam->PixelPitch()*ui.GetDouble("SUMMINGMODE");
  detectorSamples /= (int) (ui.GetDouble("SUMMINGMODE"));
  // Get the user options
  int sampleExpansion = int((ui.GetDouble("SAMPEXP")/100.)*detectorSamples + .5);
  int lineExpansion = int((ui.GetDouble("LINEEXP")/100.)*numberLines + .5);
  string instType;
  double exposure;

  // Adjust translations for summing mode
  transl /= ui.GetDouble("SUMMINGMODE");
  transs /= ui.GetDouble("SUMMINGMODE");

  detectorSamples += sampleExpansion;
  numberLines += lineExpansion;

  // Determine whether this ideal camera is a line scan or framing camera and
  // set the instrument id and exposure
  int detectorLines;
  int expandFlag;

  if (incam->DetectorMap()->LineRate() != 0.0) {
    instType= "LINESCAN";
    // Isis3 line rate is always in seconds so convert to milliseconds for the 
    // Ideal instrument
    exposure = incam->DetectorMap()->LineRate()*1000.;
    detectorLines = 1;
    expandFlag = 1;
  }
  else {
    instType = "FRAMING";
    detectorLines = numberLines;
    expandFlag = 0;
    // Framing cameras don't need exposure time
  }

  // Adjust focal plane translations with line expansion for scanners since
  // the CCD is only 1 line
  if (expandFlag) {
    transl += lineExpansion/2;

    if (xDepend == CameraFocalPlaneMap::Line) {
      transx -= lineExpansion/2.*pixPitch*expandFlag;
    }
    else {
      transy -= lineExpansion/2.*pixPitch*expandFlag;
    }
  }

  // Get the start time for parent line 1
  AlphaCube alpha(*(icube->Label()));
  double sample = alpha.BetaSample(.5);
  double line = alpha.BetaLine(.5);
  incam->SetImage(sample,line);
  double et = incam->EphemerisTime();

  // Get the output file name and set its attributes
  CubeAttributeOutput cao;

  // Can we do a regular label? Didn't work on 12-15-2006
  cao.Label(Isis::DetachedLabel);
  cao.Set("PropagateRange");
  cao.Set("PropagatePixelType");

  // Determine the output image size from
  //   1) the idealInstrument pvl if there or
  //   2) the input size expanded by user specified percentage
  Cube *ocube = p.SetOutputCube("match.cub",cao,1, 1, 1);

  // Extract the times and the target from the instrument group
  string startTime = inst["StartTime"];
  string stopTime;
  if (inst.HasKeyword("StopTime")) stopTime = (string) inst["StopTime"];

  string target = inst["TargetName"];

  // rename the instrument groups
  inst.SetName("OriginalInstrument");
  fromInst.SetName("OriginalInstrument");

  // add it back to the IsisCube object under a new group name
  ocube->PutGroup(inst);

  // and remove the version from the IsisCube Object
  ocube->DeleteGroup("Instrument");

  // Now rename the group back to the Instrument group and clear out old keywords
  inst.SetName("Instrument");
  inst.Clear();

  // Add keywords for the "Ideal" instrument
  Isis::PvlKeyword key("SpacecraftName", "IdealSpacecraft" );
  inst.AddKeyword( key);   

  key.SetName("InstrumentId");
  key.SetValue( "IdealCamera" );
  inst.AddKeyword( key);   

  key.SetName("TargetName");
  key.SetValue( target );
  inst.AddKeyword( key );

  key.SetName("SampleDetectors");
  key.SetValue( Isis::iString( detectorSamples ));
  inst.AddKeyword( key);   

  key.SetName("LineDetectors");
  key.SetValue( Isis::iString( detectorLines ));
  inst.AddKeyword( key);   

  key.SetName("InstrumentType" );
  key.SetValue( instType );
  inst.AddKeyword( key);   

  key.SetName("FocalLength");
  key.SetValue( Isis::iString( incam->FocalLength()), "millimeters" );
  inst.AddKeyword( key );

  key.SetName("PixelPitch");
  key.SetValue( Isis::iString( incam->PixelPitch()*
                              ui.GetDouble("SUMMINGMODE")), "millimeters" );
  inst.AddKeyword( key );

  key.SetName("EphemerisTime"); 
  key.SetValue( Isis::iString( et ), "seconds" );
  inst.AddKeyword( key );

  key.SetName("StartTime");
  key.SetValue( startTime );
  inst.AddKeyword( key );

  if (stopTime != "") {
    key.SetName("StopTime");
    key.SetValue( stopTime );
    inst.AddKeyword( key );
  }

  key.SetName("FocalPlaneXDependency");
  key.SetValue( incam->FocalPlaneMap()->FocalPlaneXDependency() );
  inst.AddKeyword( key );

  if (transx != 0) {
    key.SetName("TransX0");
    key.SetValue( transx );
    inst.AddKeyword( key );
  }

  if (transy != 0) {
    key.SetName("TransY0");
    key.SetValue( transy );
    inst.AddKeyword( key );
  }

  if (transs != 0) {
    key.SetName("TransS0");
    key.SetValue( transs );
    inst.AddKeyword( key );
  }

  if (transl != 0) {
     key.SetName("TransL0");
     key.SetValue( transl );
     inst.AddKeyword( key );
  }

  key.SetName("TransX");
  key.SetValue( incam->FocalPlaneMap()->SignMostSigX() );
  inst.AddKeyword( key );

  key.SetName("TransY");
  key.SetValue( incam->FocalPlaneMap()->SignMostSigY() );
  inst.AddKeyword( key );

  if (instType == "LINESCAN") {
    key.SetName("ExposureDuration");
    key.SetValue( Isis::iString( incam->DetectorMap()->LineRate()*1000.), "milliseconds" );
    inst.AddKeyword( key );
  }

  key.SetName("MatchedCube");
  key.SetValue( mcube->Filename() ); 
  inst.AddKeyword( key );

  ocube->PutGroup(inst);

  p.EndProcess();

// Now adjust the label to fake the true size of the image to match without
// taking all the space it would require for the image data
  Pvl label;
  label.Read("match.lbl");
  PvlGroup &dims = label.FindGroup("Dimensions",Pvl::Traverse);
  dims["Lines"] = numberLines;
  dims["Samples"] = detectorSamples;
  dims["Bands"] = numberBands;
  label.Write("match.lbl");
  
// And run cam2cam to apply the transformation
  string parameters;
  parameters += " FROM= " + ui.GetFilename("FROM");
  parameters += " MATCH= " + string("match.cub");
  parameters += " TO= " + ui.GetFilename("TO");
  parameters += " INTERP=" + ui.GetString("INTERP");
  Isis::iApp ->Exec("cam2cam",parameters);

//  Cleanup by deleting the match files
  remove("match.History.IsisCube");
  remove("match.lbl");
  remove("match.cub");
  remove("match.OriginalLabel.IsisCube");
  remove("match.Table.BodyRotation");
  remove("match.Table.HiRISE Ancillary");
  remove("match.Table.HiRISE Calibration Ancillary");
  remove("match.Table.HiRISE Calibration Image");
  remove("match.Table.InstrumentPointing");
  remove("match.Table.InstrumentPosition");
  remove("match.Table.SunPosition");

// Finally finish by adding the OriginalInstrument group to the TO cube
  Cube toCube;
  toCube.Open(ui.GetFilename("TO"), "rw");
// Extract label and create cube object
  Pvl *toLabel = toCube.Label();
  PvlObject &o = toLabel->FindObject("IsisCube");
  o.DeleteGroup( "OriginalInstrument" );
  o.AddGroup(fromInst);
  toCube.Close();
}

 // Helper function to get output summing mode from cube to MATCH 
void LoadMatchSummingMode () {
  string file;
  UserInterface &ui = Application::GetUserInterface();

  // Get camera from cube to match
  if ((ui.GetString ("SOURCE") =="FROMMATCH") && (ui.WasEntered("MATCH"))) {
    file = ui.GetFilename("MATCH");
  }
  else {
    file = ui.GetFilename("FROM");
  }

 // Open the input cube and get the camera object
  Cube c;
  c.Open(file);
  Camera *cam = c.Camera();

  ui.Clear("SUMMINGMODE");
  ui.PutDouble("SUMMINGMODE",cam->DetectorMap()->SampleScaleFactor());

  ui.Clear("SOURCE");
  ui.PutAsString("SOURCE", "FROMUSER");
}


// Helper function to get output summing mode from input cube (FROM)
void LoadInputSummingMode () {
  UserInterface &ui = Application::GetUserInterface();

 // Get camera from cube to match
  string file = ui.GetFilename("FROM");
 // Open the input cube and get the camera object
  Cube c;
  c.Open(file);
  Camera *cam = c.Camera();

  ui.Clear("SUMMINGMODE");
  ui.PutDouble("SUMMINGMODE",cam->DetectorMap()->SampleScaleFactor());

  ui.Clear("SOURCE");
  ui.PutAsString("SOURCE", "FROMUSER");
}
