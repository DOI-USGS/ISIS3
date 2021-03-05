#define GUIHELPERS
#include "Isis.h"

#include <iostream>
#include <sstream>
#include <QString>

#include "AlphaCube.h"
#include "Application.h"
#include "Blob.h"
#include "CameraDetectorMap.h"
#include "CameraFocalPlaneMap.h"
#include "History.h"
#include "iTime.h"
#include "Process.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "PvlObject.h"


using namespace Isis;
using namespace std;

void LoadMatchSummingMode();
void LoadInputSummingMode();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["LoadMatchSummingMode"] = (void *) LoadMatchSummingMode;
  helper ["LoadInputSummingMode"] = (void *) LoadInputSummingMode;
  return helper;
}

void storeSpice(PvlGroup *instrumentGroup, PvlObject *naifKeywordsObject,
                QString oldName, QString spiceName,
                double constantCoeff, double multiplierCoeff, bool putMultiplierInX);

void IsisMain() {

  // Create a process so we can output the noproj'd labels without overwriting
  Process p;

  // Open the user interface and get the input file and the ideal specs file
  UserInterface &ui = Application::GetUserInterface();
  Cube *mcube, *icube;

  // If a MATCH cube is entered, make sure to SetInputCube it first to get the SPICE blobs
  // from it propagated to the TO labels

  // Until polygon blobs are detached without "/" don't propagate them
  p.PropagatePolygons(false);

  if((ui.WasEntered("MATCH"))) {
    mcube = p.SetInputCube("MATCH");
    icube = p.SetInputCube("FROM");
  }
  else {
    mcube = icube = p.SetInputCube("FROM");
  }

  Camera *incam = mcube->camera();

  // Extract Instrument groups from input labels for the output match and noproj'd cubes
  PvlGroup inst = mcube->group("Instrument");
  PvlGroup fromInst = icube->group("Instrument");
  QString groupName = (QString) inst["SpacecraftName"] + "/";
  groupName += (QString) inst.findKeyword("InstrumentId");

  // Get Ideal camera specifications
  FileName specs;
  if((ui.WasEntered("SPECS"))) {
    specs = ui.GetFileName("SPECS");
  }
  else {
    specs = "$ISISROOT/appdata/templates/noproj/noprojInstruments.pvl";
  }
  Pvl idealSpecs(specs.expanded());
  PvlObject obSpecs = idealSpecs.findObject("IdealInstrumentsSpecifications");

  PvlGroup idealGp = obSpecs.findGroup(groupName);
  double transx, transy, transl, transs;
  transx = transy = transl = transs = 0.;
  if(idealGp.hasKeyword("TransX")) transx = idealGp["TransX"];
  if(idealGp.hasKeyword("TransY")) transy = idealGp["TransY"];
  if(idealGp.hasKeyword("ItransL")) transl = idealGp["ItransL"];
  if(idealGp.hasKeyword("ItransS")) transs = idealGp["ItransS"];
  int detectorSamples = mcube->sampleCount();
  if(idealGp.hasKeyword("DetectorSamples")) detectorSamples = idealGp["DetectorSamples"];
  int numberLines = mcube->lineCount();
  int numberBands = mcube->bandCount();

  if(idealGp.hasKeyword("DetectorLines")) numberLines = idealGp["DetectorLines"];

  int xDepend = incam->FocalPlaneMap()->FocalPlaneXDependency();

  // Get output summing mode
  if(ui.GetString("SOURCE") == "FROMMATCH") {
    LoadMatchSummingMode();
  }
  else if(ui.GetString("SOURCE") == "FROMINPUT") {
    LoadInputSummingMode();
  }

  double pixPitch = incam->PixelPitch() * ui.GetDouble("SUMMINGMODE");
  detectorSamples /= (int)(ui.GetDouble("SUMMINGMODE"));
  // Get the user options
  int sampleExpansion = int((ui.GetDouble("SAMPEXP") / 100.) * detectorSamples + .5);
  int lineExpansion = int((ui.GetDouble("LINEEXP") / 100.) * numberLines + .5);
  QString instType;

  // Adjust translations for summing mode
  transl /= ui.GetDouble("SUMMINGMODE");
  transs /= ui.GetDouble("SUMMINGMODE");

  detectorSamples += sampleExpansion;
  numberLines += lineExpansion;

  // Determine whether this ideal camera is a line scan or framing camera and
  // set the instrument id and exposure
  int detectorLines;
  int expandFlag;

  if(incam->DetectorMap()->LineRate() != 0.0) {
    instType = "LINESCAN";
    // ISIS line rate is always in seconds so convert to milliseconds for the
    // Ideal instrument
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
  if(expandFlag) {
    transl += lineExpansion / 2;

    if(xDepend == CameraFocalPlaneMap::Line) {
      transx -= lineExpansion / 2.*pixPitch * expandFlag;
    }
    else {
      transy -= lineExpansion / 2.*pixPitch * expandFlag;
    }
  }

  // Get the start time for parent line 1
  AlphaCube alpha(*icube);
  double sample = alpha.BetaSample(.5);
  double line = alpha.BetaLine(.5);
  incam->SetImage(sample, line);
  double et = incam->time().Et();

  // Get the output file name and set its attributes
  CubeAttributeOutput cao;

  // Can we do a regular label? Didn't work on 12-15-2006
  cao.setLabelAttachment(Isis::DetachedLabel);

  // Determine the output image size from
  //   1) the idealInstrument pvl if there or
  //   2) the input size expanded by user specified percentage
  Cube *ocube = p.SetOutputCube("match.cub", cao, 1, 1, 1);

  // Extract the times and the target from the instrument group
  QString startTime = inst["StartTime"];
  QString stopTime;
  if(inst.hasKeyword("StopTime")) stopTime = (QString) inst["StopTime"];

  QString target = inst["TargetName"];

  // rename the instrument groups
  inst.setName("OriginalInstrument");
  fromInst.setName("OriginalInstrument");

  // add it back to the IsisCube object under a new group name
  ocube->putGroup(inst);

  // and remove the version from the IsisCube Object
  ocube->deleteGroup("Instrument");

  // Now rename the group back to the Instrument group and clear out old keywords
  inst.setName("Instrument");
  inst.clear();

  // Add keywords for the "Ideal" instrument
  Isis::PvlKeyword key("SpacecraftName", "IdealSpacecraft");
  inst.addKeyword(key);

  key.setName("InstrumentId");
  key.setValue("IdealCamera");
  inst.addKeyword(key);

  key.setName("TargetName");
  key.setValue(target);
  inst.addKeyword(key);

  key.setName("SampleDetectors");
  key.setValue(Isis::toString(detectorSamples));
  inst.addKeyword(key);

  key.setName("LineDetectors");
  key.setValue(Isis::toString(detectorLines));
  inst.addKeyword(key);

  key.setName("InstrumentType");
  key.setValue(instType);
  inst.addKeyword(key);

  Pvl &ocubeLabel = *ocube->label();
  PvlObject *naifKeywordsObject = NULL;

  if (ocubeLabel.hasObject("NaifKeywords")) {
    naifKeywordsObject = &ocubeLabel.findObject("NaifKeywords");

    // Clean up the naif keywords object... delete everything that isn't a radii
    for (int keyIndex = naifKeywordsObject->keywords() - 1; keyIndex >= 0; keyIndex--) {
      QString keyName = (*naifKeywordsObject)[keyIndex].name();

      if (!keyName.contains("RADII")) {
        naifKeywordsObject->deleteKeyword(keyIndex);
      }
    }

    // Clean up the kernels group... delete everything that isn't internalized or the orig frame
    //   code
    PvlGroup &kernelsGroup = ocube->group("Kernels");
    for (int keyIndex = kernelsGroup.keywords() - 1; keyIndex >= 0; keyIndex--) {
      PvlKeyword &kernelsKeyword = kernelsGroup[keyIndex];

      bool isTable = false;
      bool isFrameCode = kernelsKeyword.isNamed("NaifFrameCode") ||
                         kernelsKeyword.isNamed("NaifIkCode");
      bool isShapeModel = kernelsKeyword.isNamed("ShapeModel");

      for (int keyValueIndex = 0; keyValueIndex < kernelsKeyword.size(); keyValueIndex++) {
        if (kernelsKeyword[keyValueIndex] == "Table") {
          isTable = true;
        }
      }

      if (!isTable && !isFrameCode && !isShapeModel) {
        kernelsGroup.deleteKeyword(keyIndex);
      }
    }
  }

  if (naifKeywordsObject) {
    naifKeywordsObject->addKeyword(PvlKeyword("IDEAL_FOCAL_LENGTH", toString(incam->FocalLength())),
                                   Pvl::Replace);
  }
  else {
    inst.addKeyword(PvlKeyword("FocalLength", toString(incam->FocalLength()), "millimeters"));
  }

  double newPixelPitch = incam->PixelPitch() * ui.GetDouble("SUMMINGMODE");
  if (naifKeywordsObject) {
    naifKeywordsObject->addKeyword(PvlKeyword("IDEAL_PIXEL_PITCH", toString(newPixelPitch)),
                                   Pvl::Replace);
  }
  else {
    inst.addKeyword(PvlKeyword("PixelPitch", toString(newPixelPitch), "millimeters"));
  }

  key.setName("EphemerisTime");
  key.setValue(Isis::toString(et), "seconds");
  inst.addKeyword(key);

  key.setName("StartTime");
  key.setValue(startTime);
  inst.addKeyword(key);

  if(stopTime != "") {
    key.setName("StopTime");
    key.setValue(stopTime);
    inst.addKeyword(key);
  }

  key.setName("FocalPlaneXDependency");
  key.setValue(toString((int)incam->FocalPlaneMap()->FocalPlaneXDependency()));
  inst.addKeyword(key);

  int xDependency = incam->FocalPlaneMap()->FocalPlaneXDependency();

  double newInstrumentTransX = incam->FocalPlaneMap()->SignMostSigX();
  inst.addKeyword(PvlKeyword("TransX", toString(newInstrumentTransX)));

  double newInstrumentTransY = incam->FocalPlaneMap()->SignMostSigY();
  inst.addKeyword(PvlKeyword("TransY", toString(newInstrumentTransY)));

  storeSpice(&inst, naifKeywordsObject, "TransX0", "IDEAL_TRANSX", transx,
             newPixelPitch * newInstrumentTransX, (xDependency == CameraFocalPlaneMap::Sample));

  storeSpice(&inst, naifKeywordsObject, "TransY0", "IDEAL_TRANSY", transy,
             newPixelPitch * newInstrumentTransY, (xDependency == CameraFocalPlaneMap::Line));

  double transSXCoefficient = 1.0 / newPixelPitch * newInstrumentTransX;
  double transLXCoefficient = 1.0 / newPixelPitch * newInstrumentTransY;

  if (xDependency == CameraFocalPlaneMap::Line) {
    swap(transSXCoefficient, transLXCoefficient);
  }

  storeSpice(&inst, naifKeywordsObject, "TransS0", "IDEAL_TRANSS",
             transs, transSXCoefficient, (xDependency == CameraFocalPlaneMap::Sample));
  storeSpice(&inst, naifKeywordsObject, "TransL0", "IDEAL_TRANSL",
             transl, transLXCoefficient, (xDependency == CameraFocalPlaneMap::Line));

  if(instType == "LINESCAN") {
    key.setName("ExposureDuration");
    key.setValue(Isis::toString(incam->DetectorMap()->LineRate() * 1000.), "milliseconds");
    inst.addKeyword(key);
  }

  key.setName("MatchedCube");
  key.setValue(mcube->fileName());
  inst.addKeyword(key);

  ocube->putGroup(inst);

  p.EndProcess();

// Now adjust the label to fake the true size of the image to match without
// taking all the space it would require for the image data
  Pvl label;
  label.read("match.lbl");
  PvlGroup &dims = label.findGroup("Dimensions", Pvl::Traverse);
  dims["Lines"] = toString(numberLines);
  dims["Samples"] = toString(detectorSamples);
  dims["Bands"] = toString(numberBands);
  label.write("match.lbl");

// And run cam2cam to apply the transformation
  QString parameters;
  parameters += " FROM= " + ui.GetFileName("FROM");
  parameters += " MATCH= " + QString("match.cub");
  parameters += " TO= " + ui.GetFileName("TO");
  parameters += " INTERP=" + ui.GetString("INTERP");
  ProgramLauncher::RunIsisProgram("cam2cam", parameters);

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
  toCube.open(ui.GetFileName("TO"), "rw");
// Extract label and create cube object
  Pvl *toLabel = toCube.label();
  PvlObject &o = toLabel->findObject("IsisCube");
  o.deleteGroup("OriginalInstrument");
  o.addGroup(fromInst);

  // Remove AlphaCube in output cube since noproj changes image geometries
  // (can't undo a noproj or uncrop a noproj'd image etc)
  if (o.hasGroup("AlphaCube")) {
    o.deleteGroup("AlphaCube");
  }
  toCube.close();
}

// Helper function to get output summing mode from cube to MATCH
void LoadMatchSummingMode() {
  QString file;
  UserInterface &ui = Application::GetUserInterface();

  // Get camera from cube to match
  if((ui.GetString("SOURCE") == "FROMMATCH") && (ui.WasEntered("MATCH"))) {
    file = ui.GetFileName("MATCH");
  }
  else {
    file = ui.GetFileName("FROM");
  }

// Open the input cube and get the camera object
  Cube c;
  c.open(file);
  Camera *cam = c.camera();

  ui.Clear("SUMMINGMODE");
  ui.PutDouble("SUMMINGMODE", cam->DetectorMap()->SampleScaleFactor());

  ui.Clear("SOURCE");
  ui.PutAsString("SOURCE", "FROMUSER");
}


void storeSpice(PvlGroup *instrumentGroup, PvlObject *naifKeywordsObject,
                QString oldName, QString spiceName,
                double constantCoeff, double multiplierCoeff, bool putMultiplierInX) {
  if(constantCoeff != 0 && !naifKeywordsObject && instrumentGroup) {
    instrumentGroup->addKeyword(PvlKeyword(oldName, toString(constantCoeff)));
  }
  else if (naifKeywordsObject) {
    PvlKeyword spiceKeyword(spiceName);
    spiceKeyword += toString(constantCoeff);

    if (putMultiplierInX) {
      spiceKeyword += toString(multiplierCoeff);
      spiceKeyword += toString(0.0);
    }
    else {
      spiceKeyword += toString(0.0);
      spiceKeyword += toString(multiplierCoeff);
    }

    naifKeywordsObject->addKeyword(spiceKeyword, Pvl::Replace);
  }
}


// Helper function to get output summing mode from input cube (FROM)
void LoadInputSummingMode() {
  UserInterface &ui = Application::GetUserInterface();

  // Get camera from cube to match
  QString file = ui.GetFileName("FROM");
  // Open the input cube and get the camera object
  Cube c;
  c.open(file);
  Camera *cam = c.camera();

  ui.Clear("SUMMINGMODE");
  ui.PutDouble("SUMMINGMODE", cam->DetectorMap()->SampleScaleFactor());

  ui.Clear("SOURCE");
  ui.PutAsString("SOURCE", "FROMUSER");
}
