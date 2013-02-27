#include "Isis.h"

#include <iomanip>
#include <queue>

#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "Kernel.h"
#include "KernelDb.h"
#include "Longitude.h"
#include "Process.h"
#include "PvlTranslationManager.h"
#include "SpiceClient.h"
#include "SpiceClientStarter.h"
#include "Table.h"

using namespace std;
using namespace Isis;

void getUserEnteredKernel(const QString &param, Kernel &kernel);
bool tryKernels(Cube *icube, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik,
                Kernel sclk, Kernel spk,
                Kernel iak, Kernel dem,
                Kernel exk);

void requestSpice(Cube *icube, Pvl &labels, QString missionName);

void IsisMain() {
  // Open the input cube
  Process p;
  UserInterface &ui = Application::GetUserInterface();
  CubeAttributeInput cai;
  Cube *icube = p.SetInputCube(ui.GetFileName("FROM"), cai, ReadWrite);
  // Make sure at least one CK & SPK quality was selected
  if (!ui.GetBoolean("CKPREDICTED") && !ui.GetBoolean("CKRECON") &&
     !ui.GetBoolean("CKSMITHED") && !ui.GetBoolean("CKNADIR")) {
    string msg = "At least one CK quality must be selected";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  if (!ui.GetBoolean("SPKPREDICTED") && !ui.GetBoolean("SPKRECON") &&
     !ui.GetBoolean("SPKSMITHED")) {
    string msg = "At least one SPK quality must be selected";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Make sure it is not projected
  Projection *proj = NULL;
  try {
    proj = icube->projection();
  }
  catch(IException &) {
    proj = NULL;
  }

  if (proj != NULL) {
    string msg = "Can not initialize SPICE for a map projected cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  Pvl lab = *icube->label();

  // if cube has existing polygon delete it
  if (icube->label()->HasObject("Polygon")) {
    icube->label()->DeleteObject("Polygon");
  }

  // Set up for getting the mission name
  // Get the directory where the system missions translation table is.
  QString transFile = p.MissionData("base",
                                    "translations/MissionName2DataDir.trn");

  // Get the mission translation manager ready
  PvlTranslationManager missionXlater(lab, transFile);

  // Get the mission name so we can search the correct DB's for kernels
  QString mission = missionXlater.Translate("MissionName");

  if (ui.GetBoolean("WEB")) {
    requestSpice(icube, *icube->label(), mission);
  }
  else {
    // Get system base kernels
    unsigned int allowed = 0;
    unsigned int allowedCK = 0;
    unsigned int allowedSPK = 0;

    if (ui.GetBoolean("CKPREDICTED"))
      allowedCK |= Kernel::typeEnum("PREDICTED");
    if (ui.GetBoolean("CKRECON"))
      allowedCK |= Kernel::typeEnum("RECONSTRUCTED");
    if (ui.GetBoolean("CKSMITHED"))
      allowedCK |= Kernel::typeEnum("SMITHED");
    if (ui.GetBoolean("CKNADIR"))
      allowedCK |= Kernel::typeEnum("NADIR");
    if (ui.GetBoolean("SPKPREDICTED"))
      allowedSPK |= Kernel::typeEnum("PREDICTED");
    if (ui.GetBoolean("SPKRECON"))
      allowedSPK |= Kernel::typeEnum("RECONSTRUCTED");
    if (ui.GetBoolean("SPKSMITHED"))
      allowedSPK |= Kernel::typeEnum("SMITHED");

    KernelDb baseKernels(allowed);
    KernelDb ckKernels(allowedCK);
    KernelDb spkKernels(allowedSPK);

    baseKernels.loadSystemDb(mission, lab);
    ckKernels.loadSystemDb(mission, lab);
    spkKernels.loadSystemDb(mission, lab);

    Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
    QList< priority_queue<Kernel> > ck;
    lk        = baseKernels.leapSecond(lab);
    pck       = baseKernels.targetAttitudeShape(lab);
    targetSpk = baseKernels.targetPosition(lab);
    ik        = baseKernels.instrument(lab);
    sclk      = baseKernels.spacecraftClock(lab);
    iak       = baseKernels.instrumentAddendum(lab);
    fk        = ckKernels.frame(lab);
    ck        = ckKernels.spacecraftPointing(lab);
    spk       = spkKernels.spacecraftPosition(lab);

    if (ui.GetBoolean("CKNADIR")) {
      // Only add nadir if no spacecraft pointing found, so we will set (priority) type to 0.
      QStringList nadirCk;
      nadirCk.push_back("Nadir");
      // if a priority queue already exists, add Nadir with low priority of 0
      if (ck.size() > 0) {
        ck[0].push(Kernel((Kernel::Type)0, nadirCk));
      }
      // if no queue exists, create a nadir queue
      else {
        priority_queue<Kernel> nadirQueue;
        nadirQueue.push(Kernel((Kernel::Type)0, nadirCk));
        ck.push_back(nadirQueue);
      }
    }

    // Get user defined kernels and override ones already found
    getUserEnteredKernel("LS", lk);
    getUserEnteredKernel("PCK", pck);
    getUserEnteredKernel("TSPK", targetSpk);
    getUserEnteredKernel("FK", fk);
    getUserEnteredKernel("IK", ik);
    getUserEnteredKernel("SCLK", sclk);
    getUserEnteredKernel("SPK", spk);
    getUserEnteredKernel("IAK", iak);
    getUserEnteredKernel("EXTRA", exk);

    // Get shape kernel
    if (ui.GetString("SHAPE") == "USER") {
      getUserEnteredKernel("MODEL", dem);
    }
    else if (ui.GetString("SHAPE") == "SYSTEM") {
      dem = baseKernels.dem(lab);
    }

    bool kernelSuccess = false;

    if ((ck.size() == 0 || ck.at(0).size() == 0) && !ui.WasEntered("CK")) {
      // no ck was found in system and user did not enter ck, throw error
      throw IException(IException::Unknown,
                       "No Camera Kernels found for the image [" + ui.GetFileName("FROM")
                       + "]",
                       _FILEINFO_);
    }
    else if (ui.WasEntered("CK")) {
      // if user entered ck 
      // empty ck queue list found in system
      while (ck.size()) {
        ck.pop_back();
      }
      // create queue with empty kernel so ck[0].size() != 0,
      // this allows us to get into the coming while loop
      priority_queue< Kernel > emptyKernelQueue;
      emptyKernelQueue.push(Kernel());
      ck.push_back(emptyKernelQueue);
    }

    // while the first queue is not empty, loop through it until tryKernels() succeeds
    while (ck.at(0).size() != 0 && !kernelSuccess) {
      // create an empty kernel 
      Kernel realCkKernel;
      QStringList ckKernelList;

      // if the user entered ck kernels, populate the ck kernel list with the
      // user entered files
      if (ui.WasEntered("CK")) {
        vector<QString> userEnteredCks;
        ui.GetAsString("CK", userEnteredCks);
        // convert user entered std vector to QStringList and add to ckKernelList
        ckKernelList = QVector<QString>::fromStdVector(userEnteredCks).toList();
      }
      else {// loop through cks found in the system

        // Add the list of cks from each Kernel object at the top of each
        // priority queue. If multiple priority queues exist, we will not
        // pop of the top priority from any of the queues except for the
        // first one.  So each time tryKernels() fails, the same files
        // will be loaded with the next priority from the first queue.
        for (int i = ck.size() - 1; i >= 0; i--) {
          if (ck.at(i).size() != 0) {
            Kernel topPriority = ck.at(i).top();
            ckKernelList.append(topPriority.kernels());
            // set the type to equal the type of the to priority of the first
            //queue 
            realCkKernel.setType(topPriority.type()); 
          }
        }

      }
      // pop the top priority ck off only the first queue so that the next 
      // iteration will test the next highest priority of the first queue with
      // the top priority of each of the other queues.
      ck[0].pop();

      // Merge SpacecraftPointing and Frame into ck
      for (int i = 0; i < fk.size(); i++) {
        ckKernelList.push_back(fk[i]);
      }

      realCkKernel.setKernels(ckKernelList);

      kernelSuccess = tryKernels(icube, p, lk, pck, targetSpk,
                                 realCkKernel, fk, ik, sclk, spk, iak, dem, exk);
    }

    if (!kernelSuccess)
      throw IException(IException::Unknown,
                       "Unable to initialize camera model",
                       _FILEINFO_);
  }

  p.EndProcess();
}

/**
 * If the user entered the parameter param, then kernel is replaced by the 
 * user's values and quality is reset to 0. Otherwise, the kernels loaded by the
 * KernelDb class will be kept.
 *
 * @param param Name of the kernel input parameter
 *  
 * @param kernel Kernel object to be overwritten if the specified user parameter 
 *               was entered. 
 */
void getUserEnteredKernel(const QString &param, Kernel &kernel) {
  UserInterface &ui = Application::GetUserInterface();
  if (ui.WasEntered(param)) {
    kernel = Kernel();
    // NOTE: This is using GetAsString so that vars like $mgs can be used.
    vector<QString> kernels;
    ui.GetAsString(param, kernels);
    kernel.setKernels(QVector<QString>::fromStdVector(kernels).toList());
  }
}

bool tryKernels(Cube *icube, Process &p,
                Kernel lk, Kernel pck,
                Kernel targetSpk, Kernel ck,
                Kernel fk, Kernel ik, Kernel sclk,
                Kernel spk, Kernel iak,
                Kernel dem, Kernel exk) {
  Pvl lab = *icube->label();

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

  PvlGroup originalKernels = icube->group("Kernels");
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
  spkQuality.AddValue(Kernel::typeEnum(spk.type()));
  currentKernels.AddKeyword(spkQuality, Pvl::Replace);

  PvlKeyword ckQuality("InstrumentPointingQuality");
  ckQuality.AddValue(Kernel::typeEnum(ck.type()));
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
                                         toString(ui.GetDouble("STARTPAD")), "seconds"));
  }

  if (ui.GetDouble("ENDPAD") > DBL_EPSILON) {
    currentKernels.AddKeyword(PvlKeyword("EndPadding",
                                         toString(ui.GetDouble("ENDPAD")), "seconds"));
  }

  currentKernels.AddKeyword(
      PvlKeyword("CameraVersion", toString(CameraFactory::CameraVersion(lab))),
      Pvl::Replace);

  // Add the modified Kernels group to the input cube labels
  icube->putGroup(currentKernels);

  // Create the camera so we can get blobs if necessary
  try {
    Camera *cam;
    try {
      cam = icube->camera();
      Application::Log(currentKernels);
    }
    catch(IException &e) {
      Pvl errPvl = e.toPvl();

      if (errPvl.Groups() > 0) {
        currentKernels += PvlKeyword("Error", errPvl.Group(errPvl.Groups() - 1)["Message"][0]);
      }

      Application::Log(currentKernels);
      icube->putGroup(originalKernels);
      throw;
    }

    if (ui.GetBoolean("ATTACH")) {
      Table ckTable = cam->instrumentRotation()->Cache("InstrumentPointing");
      ckTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      ckTable.Label() += PvlKeyword("Kernels");

      for (int i = 0; i < ckKeyword.Size(); i++)
        ckTable.Label()["Kernels"].AddValue(ckKeyword[i]);

      icube->write(ckTable);

      Table spkTable = cam->instrumentPosition()->Cache("InstrumentPosition");
      spkTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      spkTable.Label() += PvlKeyword("Kernels");
      for (int i = 0; i < spkKeyword.Size(); i++)
        spkTable.Label()["Kernels"].AddValue(spkKeyword[i]);

      icube->write(spkTable);

      Table bodyTable = cam->bodyRotation()->Cache("BodyRotation");
      bodyTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      bodyTable.Label() += PvlKeyword("Kernels");
      for (int i = 0; i < targetSpkKeyword.Size(); i++)
        bodyTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);

      for (int i = 0; i < pckKeyword.Size(); i++)
        bodyTable.Label()["Kernels"].AddValue(pckKeyword[i]);

      bodyTable.Label() += PvlKeyword("SolarLongitude",
          toString(cam->solarLongitude().degrees()));
      icube->write(bodyTable);

      Table sunTable = cam->sunPosition()->Cache("SunPosition");
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

      Pvl *label = icube->label();
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

      *icube->label() += cam->getStoredNaifKeywords();
    }
    //modify Kernels group only
    else {
      Pvl *label = icube->label();
      int i = 0;
      while (i < label->Objects()) {
        PvlObject currObj = label->Object(i);
        if (currObj.IsNamed("Table")) {
          if (currObj["Name"][0] == QString("InstrumentPointing") ||
              currObj["Name"][0] == QString("InstrumentPosition") ||
              currObj["Name"][0] == QString("BodyRotation") ||
              currObj["Name"][0] == QString("SunPosition")) {
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
  catch(IException &) {
    icube->putGroup(originalKernels);
    return false;
  }

  return true;
}

void requestSpice(Cube *icube, Pvl &labels, QString missionName) {
  UserInterface &ui = Application::GetUserInterface();

  QString instrumentId =
      labels.FindGroup("Instrument", Pvl::Traverse)["InstrumentId"][0];

  QString url       = ui.GetString("URL") + "?mission=" + missionName +
                                            "&instrument=" + instrumentId;
  int port          = ui.GetInteger("PORT");
  bool ckSmithed    = ui.GetBoolean("CKSMITHED");
  bool ckRecon      = ui.GetBoolean("CKRECON");
  bool ckPredicted  = ui.GetBoolean("CKPREDICTED");
  bool ckNadir      = ui.GetBoolean("CKNADIR");
  bool spkSmithed   = ui.GetBoolean("SPKSMITHED");
  bool spkRecon     = ui.GetBoolean("SPKRECON");
  bool spkPredicted = ui.GetBoolean("SPKPREDICTED");
  QString shape     = QString(ui.GetString("SHAPE")).toLower();

  if (shape == "user") {
    shape = QString(ui.GetAsString("MODEL"));

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
      IString value = curKeyword[valueIndex];
      if (value == "Table" || value == "Null")
        continue;

      IString fullFileName = FileName(value).expanded();
      QFile testFile(fullFileName.ToQt());

      if (!testFile.exists()) {
        IString msg = "The spice server says you need the kernel [" +
          value + "] which is not present. " +
          "Please update your spice kernels";
        throw iException::Message(iException::Spice, msg, _FILEINFO_);
      }
    }*/
  }

  Application::Log(logGrp);

  icube->putGroup(kernelsGroup);
  icube->label()->AddObject(naifKeywords);

  icube->write(*pointingTable);
  icube->write(*positionTable);
  icube->write(*bodyTable);
  icube->write(*sunPosTable);

  try {
    icube->camera();
  }
  catch (IException &e) {
    throw IException(e, IException::Unknown,
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
