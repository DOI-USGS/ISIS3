#include "Isis.h"

#include <iomanip>

#include "Camera.h"
#include "CameraFactory.h"
#include "Filename.h"
#include "iException.h"
#include "KernelDb.h"
#include "Longitude.h"
#include "Process.h"
#include "PvlTranslationManager.h"
#include "SpiceClient.h"
#include "SpiceClientStarter.h"
#include "Table.h"

using namespace std;
using namespace Isis;

bool TryKernels(Cube *icube, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik,
                Kernel sclk, Kernel spk,
                Kernel iak, Kernel dem,
                Kernel exk);

void RequestSpice(Cube *icube, Pvl &labels, iString missionName);
void GetUserEnteredKernel(const string &param, Kernel &kernel);

void IsisMain() {
  // Open the input cube
  Process p;
  UserInterface &ui = Application::GetUserInterface();
  CubeAttributeInput cai;
  Cube *icube = p.SetInputCube(ui.GetFilename("FROM"), cai, ReadWrite);

  // Make sure at least one CK & SPK quality was selected
  if (!ui.GetBoolean("CKPREDICTED") && !ui.GetBoolean("CKRECON") &&
     !ui.GetBoolean("CKSMITHED") && !ui.GetBoolean("CKNADIR")) {
    string msg = "At least one CK quality must be selected";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  if (!ui.GetBoolean("SPKPREDICTED") && !ui.GetBoolean("SPKRECON") &&
     !ui.GetBoolean("SPKSMITHED")) {
    string msg = "At least one SPK quality must be selected";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  // Make sure it is not projected
  Projection *proj = NULL;
  try {
    proj = icube->getProjection();
  }
  catch(iException &e) {
    proj = NULL;
    e.Clear();
  }

  if (proj != NULL) {
    string msg = "Can not initialize SPICE for a map projected cube";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  Pvl lab = *icube->getLabel();

  // if cube has existing polygon delete it
  if (icube->getLabel()->HasObject("Polygon")) {
    icube->getLabel()->DeleteObject("Polygon");
  }

  // Set up for getting the mission name
  // Get the directory where the system missions translation table is.
  string transFile = p.MissionData("base",
                                   "translations/MissionName2DataDir.trn");

  // Get the mission translation manager ready
  PvlTranslationManager missionXlater(lab, transFile);

  // Get the mission name so we can search the correct DB's for kernels
  string mission = missionXlater.Translate("MissionName");

  if (ui.GetBoolean("WEB")) {
    RequestSpice(icube, *icube->getLabel(), mission);
  }
  else {
    // Get system base kernels
    unsigned int allowed = 0;
    unsigned int allowedCK = 0;
    unsigned int allowedSPK = 0;

    if (ui.GetBoolean("CKPREDICTED"))
      allowedCK |= spiceInit::kernelTypeEnum("PREDICTED");
    if (ui.GetBoolean("CKRECON"))
      allowedCK |= spiceInit::kernelTypeEnum("RECONSTRUCTED");
    if (ui.GetBoolean("CKSMITHED"))
      allowedCK |= spiceInit::kernelTypeEnum("SMITHED");
    if (ui.GetBoolean("CKNADIR"))
      allowedCK |= spiceInit::kernelTypeEnum("NADIR");
    if (ui.GetBoolean("SPKPREDICTED"))
      allowedSPK |= spiceInit::kernelTypeEnum("PREDICTED");
    if (ui.GetBoolean("SPKRECON"))
      allowedSPK |= spiceInit::kernelTypeEnum("RECONSTRUCTED");
    if (ui.GetBoolean("SPKSMITHED"))
      allowedSPK |= spiceInit::kernelTypeEnum("SMITHED");

    KernelDb baseKernels(allowed);
    KernelDb ckKernels(allowedCK);
    KernelDb spkKernels(allowedSPK);

    baseKernels.LoadSystemDb(mission);
    ckKernels.LoadSystemDb(mission);
    spkKernels.LoadSystemDb(mission);

    Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
    priority_queue< Kernel > ck;
    lk        = baseKernels.LeapSecond(lab);
    pck       = baseKernels.TargetAttitudeShape(lab);
    targetSpk = baseKernels.TargetPosition(lab);
    ik        = baseKernels.Instrument(lab);
    sclk      = baseKernels.SpacecraftClock(lab);
    iak       = baseKernels.InstrumentAddendum(lab);
    fk        = ckKernels.Frame(lab);
    ck        = ckKernels.SpacecraftPointing(lab);
    spk       = spkKernels.SpacecraftPosition(lab);

    if (ui.GetBoolean("CKNADIR")) {
      // Only add nadir if no spacecraft pointing found
      vector<string> kernels;
      kernels.push_back("Nadir");
      ck.push(Kernel((spiceInit::kernelTypes)0, kernels));
    }

    // Get user defined kernels and override ones already found
    GetUserEnteredKernel("LS", lk);
    GetUserEnteredKernel("PCK", pck);
    GetUserEnteredKernel("TSPK", targetSpk);
    GetUserEnteredKernel("FK", fk);
    GetUserEnteredKernel("IK", ik);
    GetUserEnteredKernel("SCLK", sclk);
    GetUserEnteredKernel("SPK", spk);
    GetUserEnteredKernel("IAK", iak);
    GetUserEnteredKernel("EXTRA", exk);

    // Get shape kernel
    if (ui.GetString("SHAPE") == "USER")
      GetUserEnteredKernel("MODEL", dem);
    else if (ui.GetString("SHAPE") == "SYSTEM")
      dem = baseKernels.Dem(lab);

    bool kernelSuccess = false;

    if (ck.size() == 0 && !ui.WasEntered("CK")) {
      throw iException::Message(iException::Camera,
                                "No Camera Kernel found for the image [" + ui.GetFilename("FROM")
                                + "]",
                                _FILEINFO_);
    }
    else if (ui.WasEntered("CK")) {
      // ck needs to be array size 1 and empty kernel objects
      while (ck.size()) ck.pop();
      ck.push(Kernel());
    }

    while (ck.size() != 0 && !kernelSuccess) {
      Kernel realCkKernel = ck.top();
      ck.pop();

      if (ui.WasEntered("CK"))
        ui.GetAsString("CK", realCkKernel.kernels);

      // Merge SpacecraftPointing and Frame into ck
      for (int i = 0; i < fk.size(); i++)
        realCkKernel.push_back(fk[i]);

      kernelSuccess = TryKernels(icube, p, lk, pck, targetSpk,
                                 realCkKernel, fk, ik, sclk, spk, iak, dem, exk);
    }

    if (!kernelSuccess)
      throw iException::Message(iException::Camera,
                                "Unable to initialize camera model",
                                _FILEINFO_);
  }

  p.EndProcess();
}

/**
 * If the user entered the parameter param, then
 * kernel is replaced by the user's values and
 * quality is reset to 0.
 *
 * @param param
 * @param kernel
 */
void GetUserEnteredKernel(const string &param, Kernel &kernel) {
  UserInterface &ui = Application::GetUserInterface();

  if (ui.WasEntered(param)) {
    kernel = Kernel();
    // NOTE: This is using GetAsString so that vars like $mgs can be used.
    ui.GetAsString(param, kernel.kernels);
  }
}

bool TryKernels(Cube *icube, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik, Kernel sclk,
                Kernel spk, Kernel iak,
                Kernel dem, Kernel exk) {
  Pvl lab = *icube->getLabel();

  // Add the new kernel files to the existing kernels group
  PvlKeyword lkKeyword("LeapSecond");
  PvlKeyword pckKeyword("TargetAttitudeShape");
  PvlKeyword targetSpkKeyword("TargetPosition");
  PvlKeyword ckKeyword("InstrumentPointing");
  PvlKeyword ikKeyword("Instrument");
  PvlKeyword sclkKeyword("SpacecraftClock");
  PvlKeyword spkKeyword("InstrumentPosition");
  PvlKeyword iakKeyword("InstrumentAddendum");
  PvlKeyword demKeyword("ShapeModel");
  PvlKeyword exkKeyword("Extra");

  for (int i = 0; i < lk.size(); i++) {
    lkKeyword.AddValue(lk[i]);
  }
  for (int i = 0; i < pck.size(); i++) {
    pckKeyword.AddValue(pck[i]);
  }
  for (int i = 0; i < targetSpk.size(); i++) {
    targetSpkKeyword.AddValue(targetSpk[i]);
  }
  for (int i = 0; i < ck.size(); i++) {
    ckKeyword.AddValue(ck[i]);
  }
  for (int i = 0; i < ik.size(); i++) {
    ikKeyword.AddValue(ik[i]);
  }
  for (int i = 0; i < sclk.size(); i++) {
    sclkKeyword.AddValue(sclk[i]);
  }
  for (int i = 0; i < spk.size(); i++) {
    spkKeyword.AddValue(spk[i]);
  }
  for (int i = 0; i < iak.size(); i++) {
    iakKeyword.AddValue(iak[i]);
  }
  for (int i = 0; i < dem.size(); i++) {
    demKeyword.AddValue(dem[i]);
  }
  for (int i = 0; i < exk.size(); i++) {
    exkKeyword.AddValue(exk[i]);
  }

  PvlGroup originalKernels = icube->getGroup("Kernels");
  PvlGroup currentKernels = originalKernels;
  currentKernels.AddKeyword(lkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(pckKeyword, Pvl::Replace);
  currentKernels.AddKeyword(targetSpkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(ckKeyword, Pvl::Replace);
  currentKernels.AddKeyword(ikKeyword, Pvl::Replace);
  currentKernels.AddKeyword(sclkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(spkKeyword, Pvl::Replace);
  currentKernels.AddKeyword(iakKeyword, Pvl::Replace);
  currentKernels.AddKeyword(demKeyword, Pvl::Replace);

  // report qualities
  PvlKeyword spkQuality("InstrumentPositionQuality");
  spkQuality.AddValue(spiceInit::kernelTypeEnum(spk.kernelType));
  currentKernels.AddKeyword(spkQuality, Pvl::Replace);

  PvlKeyword ckQuality("InstrumentPointingQuality");
  ckQuality.AddValue(spiceInit::kernelTypeEnum(ck.kernelType));
  currentKernels.AddKeyword(ckQuality, Pvl::Replace);

  if (!exkKeyword.IsNull()) {
    currentKernels.AddKeyword(exkKeyword, Pvl::Replace);
  }
  else if (currentKernels.HasKeyword("EXTRA")) {
    currentKernels.DeleteKeyword("EXTRA");
  }

  // Get rid of old keywords from previously inited cubes
  if (currentKernels.HasKeyword("SpacecraftPointing"))
    currentKernels.DeleteKeyword("SpacecraftPointing");

  if (currentKernels.HasKeyword("SpacecraftPosition"))
    currentKernels.DeleteKeyword("SpacecraftPosition");

  if (currentKernels.HasKeyword("ElevationModel"))
    currentKernels.DeleteKeyword("ElevationModel");

  if (currentKernels.HasKeyword("Frame"))
    currentKernels.DeleteKeyword("Frame");

  if (currentKernels.HasKeyword("StartPadding"))
    currentKernels.DeleteKeyword("StartPadding");

  if (currentKernels.HasKeyword("EndPadding"))
    currentKernels.DeleteKeyword("EndPadding");

  UserInterface &ui = Application::GetUserInterface();
  // Add any time padding the user specified to the spice group
  if (ui.GetDouble("STARTPAD") > DBL_EPSILON) {
    currentKernels.AddKeyword(PvlKeyword("StartPadding",
                                         ui.GetDouble("STARTPAD"), "seconds"));
  }

  if (ui.GetDouble("ENDPAD") > DBL_EPSILON) {
    currentKernels.AddKeyword(PvlKeyword("EndPadding",
                                         ui.GetDouble("ENDPAD"), "seconds"));
  }

  currentKernels.AddKeyword(
      PvlKeyword("CameraVersion", CameraFactory::CameraVersion(lab)),
      Pvl::Replace);

  // Add the modified Kernels group to the input cube labels
  icube->putGroup(currentKernels);

  // Create the camera so we can get blobs if necessary
  try {
    Camera *cam;
    try {
      cam = icube->getCamera();
      Application::Log(currentKernels);
    }
    catch(iException &e) {
      Pvl errPvl = e.PvlErrors();

      if (errPvl.Groups() > 0) {
        currentKernels += PvlKeyword("Error", errPvl.Group(errPvl.Groups() - 1)["Message"][0]);
      }

      Application::Log(currentKernels);
      icube->putGroup(originalKernels);
      throw e;
    }

    if (ui.GetBoolean("ATTACH")) {
      Table ckTable = cam->InstrumentRotation()->Cache("InstrumentPointing");
      ckTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      ckTable.Label() += PvlKeyword("Kernels");

      for (int i = 0; i < ckKeyword.Size(); i++)
        ckTable.Label()["Kernels"].AddValue(ckKeyword[i]);

      icube->write(ckTable);

      Table spkTable = cam->InstrumentPosition()->Cache("InstrumentPosition");
      spkTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      spkTable.Label() += PvlKeyword("Kernels");
      for (int i = 0; i < spkKeyword.Size(); i++)
        spkTable.Label()["Kernels"].AddValue(spkKeyword[i]);

      icube->write(spkTable);

      Table bodyTable = cam->BodyRotation()->Cache("BodyRotation");
      bodyTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      bodyTable.Label() += PvlKeyword("Kernels");
      for (int i = 0; i < targetSpkKeyword.Size(); i++)
        bodyTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

      for (int i = 0; i < pckKeyword.Size(); i++)
        bodyTable.Label()["Kernels"].AddValue(pckKeyword[i]);

      bodyTable.Label() += PvlKeyword("SolarLongitude",
          cam->SolarLongitude().degrees());
      icube->write(bodyTable);

      Table sunTable = cam->SunPosition()->Cache("SunPosition");
      sunTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      sunTable.Label() += PvlKeyword("Kernels");
      for (int i = 0; i < targetSpkKeyword.Size(); i++)
        sunTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

      icube->write(sunTable);

      //  Save original kernels in keyword before changing to Table
      PvlKeyword origCk = currentKernels["InstrumentPointing"];
      PvlKeyword origSpk = currentKernels["InstrumentPosition"];
      PvlKeyword origTargPos = currentKernels["TargetPosition"];

      currentKernels["InstrumentPointing"] = "Table";
      for (int i = 0; i < origCk.Size(); i++)
        currentKernels["InstrumentPointing"].AddValue(origCk[i]);

      currentKernels["InstrumentPosition"] = "Table";
      for (int i = 0; i < origSpk.Size(); i++)
        currentKernels["InstrumentPosition"].AddValue(origSpk[i]);

      currentKernels["TargetPosition"] = "Table";
      for (int i = 0; i < origTargPos.Size(); i++)
        currentKernels["TargetPosition"].AddValue(origTargPos[i]);

      icube->putGroup(currentKernels);

      Pvl *label = icube->getLabel();
      int i = 0;
      while (i < label->Objects()) {
        PvlObject currObj = label->Object(i);
        if (currObj.IsNamed("NaifKeywords")) {
          label->DeleteObject(i);
        }
        else {
          i ++;
        }
      }

      *icube->getLabel() += cam->getStoredNaifKeywords();
    }
    //modify Kernels group only
    else {
      Pvl *label = icube->getLabel();
      int i = 0;
      while (i < label->Objects()) {
        PvlObject currObj = label->Object(i);
        if (currObj.IsNamed("Table")) {
          if (currObj["Name"][0] == iString("InstrumentPointing") ||
              currObj["Name"][0] == iString("InstrumentPosition") ||
              currObj["Name"][0] == iString("BodyRotation") ||
              currObj["Name"][0] == iString("SunPosition")) {
            label->DeleteObject(i);
          }
          else {
            i++;
          }
        }
        else if (currObj.IsNamed("NaifKeywords")) {
          label->DeleteObject(i);
        }
        else {
          i++;
        }
      }
    }

    p.WriteHistory(*icube);
  }
  catch(iException &e) {
    e.Clear();
    icube->putGroup(originalKernels);
    return false;
  }

  return true;
}

void RequestSpice(Cube *icube, Pvl &labels, iString missionName) {
  UserInterface &ui = Application::GetUserInterface();

  iString instrumentId =
      labels.FindGroup("Instrument", Pvl::Traverse)["InstrumentId"][0];

  iString url       = ui.GetString("URL") + "?mission=" + missionName +
                                            "&instrument=" + instrumentId;
  int port          = ui.GetInteger("PORT");
  bool ckSmithed    = ui.GetBoolean("CKSMITHED");
  bool ckRecon      = ui.GetBoolean("CKRECON");
  bool ckPredicted  = ui.GetBoolean("CKPREDICTED");
  bool ckNadir      = ui.GetBoolean("CKNADIR");
  bool spkSmithed   = ui.GetBoolean("SPKSMITHED");
  bool spkRecon     = ui.GetBoolean("SPKRECON");
  bool spkPredicted = ui.GetBoolean("SPKPREDICTED");
  iString shape     = iString(ui.GetString("SHAPE")).DownCase();

  if (shape == "user") {
    shape = iString(ui.GetAsString("MODEL"));

    // Test for valid labels with mapping group at least
    Pvl shapeTest(shape);
    shapeTest.FindGroup("Mapping", Pvl::Traverse);
  }

  double startPad = ui.GetDouble("STARTPAD");
  double endPad   = ui.GetDouble("ENDPAD");

  SpiceClient client(url, port, labels,
                     ckSmithed, ckRecon, ckPredicted, ckNadir,
                     spkSmithed, spkRecon, spkPredicted,
                     shape, startPad, endPad);

  Progress connectionProgress;
  connectionProgress.SetText("Requesting Spice Data");
  connectionProgress.SetMaximumSteps(1);
  connectionProgress.CheckStatus();
  SpiceClientStarter starter(client);
  starter.start();
  client.blockUntilComplete();
  connectionProgress.CheckStatus();

  PvlGroup kernelsGroup = client.kernelsGroup();
  PvlGroup logGrp = client.applicationLog();
  PvlObject naifKeywords = client.naifKeywordsObject();
  Table *pointingTable = client.pointingTable();
  Table *positionTable = client.positionTable();
  Table *bodyTable = client.bodyRotationTable();
  Table *sunPosTable = client.sunPositionTable();

  // Verify everything in the kernels group exists, if not then our kernels are
  //   out of date.
  for (int keywordIndex = 0; 
      keywordIndex < kernelsGroup.Keywords();
      keywordIndex++) {
    PvlKeyword &curKeyword = kernelsGroup[keywordIndex];

    if (curKeyword.Name() == "NaifFrameCode" || 
        curKeyword.Name() == "InstrumentPointingQuality" ||
        curKeyword.Name() == "InstrumentPositionQuality" ||
        curKeyword.Name() == "CameraVersion" ||
        curKeyword.Name() == "TargetPosition" ||
        curKeyword.Name() == "InstrumentPointing" ||
        curKeyword.Name() == "InstrumentPosition" ||
        curKeyword.Name() == "TargetAttitudeShape") {
      continue;
    }

    /*for (int valueIndex = 0; valueIndex < curKeyword.Size(); valueIndex ++) {
      iString value = curKeyword[valueIndex];
      if (value == "Table" || value == "Null")
        continue;

      iString fullFilename = Filename(value).Expanded();
      QFile testFile(fullFilename.ToQt());

      if (!testFile.exists()) {
        iString msg = "The spice server says you need the kernel [" +
          value + "] which is not present. " +
          "Please update your spice kernels";
        throw iException::Message(iException::Spice, msg, _FILEINFO_);
      }
    }*/
  }

  Application::Log(logGrp);

  icube->putGroup(kernelsGroup);
  icube->getLabel()->AddObject(naifKeywords);

  icube->write(*pointingTable);
  icube->write(*positionTable);
  icube->write(*bodyTable);
  icube->write(*sunPosTable);

  try {
    icube->getCamera();
  }
  catch (iException &e) {
    throw iException::Message(iException::Spice,
       "The SPICE server returned incompatible SPICE data",
        _FILEINFO_);
  }

  delete pointingTable;
  pointingTable = NULL;

  delete positionTable;
  positionTable = NULL;

  delete bodyTable;
  bodyTable = NULL;

  delete sunPosTable;
  sunPosTable = NULL;
}
