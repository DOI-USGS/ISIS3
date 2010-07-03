#include "Isis.h"

#include <iomanip>

#include "Process.h"
#include "Filename.h"
#include "PvlTranslationManager.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "iException.h"
#include "KernelDb.h"
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

void GetUserEnteredKernel(const string &param, Kernel &kernel);

void IsisMain() {
  // Open the input cube
  Process p;
  UserInterface &ui = Application::GetUserInterface();
  CubeAttributeInput cai;
  Cube *icube = p.SetInputCube(ui.GetFilename("FROM"), cai, ReadWrite);

  // Make sure at least one CK & SPK quality was selected
  if (!ui.GetBoolean("CKPREDICTED") && !ui.GetBoolean("CKRECON") && !ui.GetBoolean("CKSMITHED") && !ui.GetBoolean("CKNADIR")) {
    string msg = "At least one CK quality must be selected";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }
  if (!ui.GetBoolean("SPKPREDICTED") && !ui.GetBoolean("SPKRECON") && !ui.GetBoolean("SPKSMITHED")) {
    string msg = "At least one SPK quality must be selected";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Make sure it is not projected
  Projection *proj = NULL;
  try {
    proj = icube->Projection();
  } catch (iException &e) {
    proj = NULL;
    e.Clear();
  }

  if (proj != NULL) {
    string msg = "Can not initialize SPICE for a map projected cube";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  Pvl lab = *icube->Label();

  // if cube has existing polygon delete it
  if (icube->Label()->HasObject("Polygon")) {
    icube->Label()->DeleteObject("Polygon");
  }

  // Set up for getting the mission name
  // Get the directory where the system missions translation table is.
  string transFile = p.MissionData("base", "translations/MissionName2DataDir.trn");

  // Get the mission translation manager ready
  PvlTranslationManager missionXlater (lab, transFile);

  // Get the mission name so we can search the correct DB's for kernels
  string mission = missionXlater.Translate ("MissionName");

  // Get system base kernels
  unsigned int allowed = 0;
  unsigned int allowedCK = 0;
  unsigned int allowedSPK = 0;
  if (ui.GetBoolean("CKPREDICTED"))  allowedCK |= spiceInit::kernelTypeEnum("PREDICTED");
  if (ui.GetBoolean("CKRECON"))      allowedCK |= spiceInit::kernelTypeEnum("RECONSTRUCTED");
  if (ui.GetBoolean("CKSMITHED"))    allowedCK |= spiceInit::kernelTypeEnum("SMITHED");
  if (ui.GetBoolean("CKNADIR"))      allowedCK |= spiceInit::kernelTypeEnum("NADIR");
  if (ui.GetBoolean("SPKPREDICTED")) allowedSPK |= spiceInit::kernelTypeEnum("PREDICTED");
  if (ui.GetBoolean("SPKRECON"))     allowedSPK |= spiceInit::kernelTypeEnum("RECONSTRUCTED");
  if (ui.GetBoolean("SPKSMITHED"))   allowedSPK |= spiceInit::kernelTypeEnum("SMITHED");
  KernelDb baseKernels (allowed);
  KernelDb ckKernels (allowedCK);
  KernelDb spkKernels (allowedSPK);

  baseKernels.LoadSystemDb(mission);
  ckKernels.LoadSystemDb(mission);
  spkKernels.LoadSystemDb(mission);

  Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
  std::priority_queue< Kernel > ck;
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
    std::vector<std::string> kernels;
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
  if (ui.GetString ("SHAPE") == "USER") {
    GetUserEnteredKernel("MODEL", dem);
  } else if (ui.GetString("SHAPE") == "SYSTEM") {
    dem = baseKernels.Dem(lab);
  }

  bool kernelSuccess = false;

  if (ck.size() == 0 && !ui.WasEntered("CK")) {
    throw iException::Message(iException::Camera, 
                              "No Camera Kernel found for the image ["+ui.GetFilename("FROM")
                              +"]", 
                              _FILEINFO_);
  }
  else if(ui.WasEntered("CK")) {
    // ck needs to be array size 1 and empty kernel objects
    while(ck.size()) ck.pop();
    ck.push(Kernel());
  }

  while(ck.size() != 0 && !kernelSuccess) {
    Kernel realCkKernel = ck.top();
    ck.pop();

    if (ui.WasEntered("CK")) {
      ui.GetAsString("CK", realCkKernel.kernels);
    }

    // Merge SpacecraftPointing and Frame into ck
    for (int i = 0; i < fk.size(); i++) {
      realCkKernel.push_back(fk[i]);
    }

    kernelSuccess = TryKernels(icube, p, lk, pck, targetSpk,
                   realCkKernel, fk, ik, sclk, spk, iak, dem, exk);
  }

  if(!kernelSuccess) {
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

  if (ui.WasEntered(param))  {
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
  Pvl lab = *icube->Label();

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

  for (int i=0; i<lk.size(); i++) {
    lkKeyword.AddValue(lk[i]);
  }
  for (int i=0; i<pck.size(); i++) {
    pckKeyword.AddValue(pck[i]);
  }
  for (int i=0; i<targetSpk.size(); i++) {
    targetSpkKeyword.AddValue(targetSpk[i]);
  }
  for (int i=0; i<ck.size(); i++) {
    ckKeyword.AddValue(ck[i]);
  }
  for (int i=0; i<ik.size(); i++) {
    ikKeyword.AddValue(ik[i]);
  }
  for (int i=0; i<sclk.size(); i++) {
    sclkKeyword.AddValue(sclk[i]);
  }
  for (int i=0; i<spk.size(); i++) {
    spkKeyword.AddValue(spk[i]);
  }
  for (int i=0; i<iak.size(); i++) {
    iakKeyword.AddValue(iak[i]);
  }
  for (int i=0; i<dem.size(); i++) {
    demKeyword.AddValue(dem[i]);
  }
  for (int i=0; i<exk.size(); i++) {
    exkKeyword.AddValue(exk[i]);
  }

  PvlGroup originalKernels = icube->GetGroup("Kernels");
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
  else if( currentKernels.HasKeyword("EXTRA") ) {
    currentKernels.DeleteKeyword( "EXTRA" );
  }

  // Get rid of old keywords from previously inited cubes
  if (currentKernels.HasKeyword("SpacecraftPointing")) {
    currentKernels.DeleteKeyword("SpacecraftPointing");
  }
  if (currentKernels.HasKeyword("SpacecraftPosition")) {
    currentKernels.DeleteKeyword("SpacecraftPosition");
  }
  if (currentKernels.HasKeyword("ElevationModel")) {
    currentKernels.DeleteKeyword("ElevationModel");
  }
  if (currentKernels.HasKeyword("Frame")) {
    currentKernels.DeleteKeyword("Frame");
  }
  if (currentKernels.HasKeyword("StartPadding")) {
    currentKernels.DeleteKeyword("StartPadding");
  }
  if (currentKernels.HasKeyword("EndPadding")) {
    currentKernels.DeleteKeyword("EndPadding");
  }

  UserInterface &ui = Application::GetUserInterface();
  // Add any time padding the user specified to the spice group
  if(ui.GetDouble("STARTPAD") > DBL_EPSILON) {
     currentKernels.AddKeyword(PvlKeyword("StartPadding", ui.GetDouble("STARTPAD"), "seconds"));
  }

  if(ui.GetDouble("ENDPAD") > DBL_EPSILON) {
     currentKernels.AddKeyword(PvlKeyword("EndPadding", ui.GetDouble("ENDPAD"), "seconds"));
  }

  currentKernels.AddKeyword(PvlKeyword("CameraVersion",CameraFactory::CameraVersion(lab)), Pvl::Replace);

  // Add the modified Kernels group to the input cube labels
  icube->PutGroup(currentKernels);

  // Create the camera so we can get blobs if necessary
  try {
    Camera *cam;
    try {
      cam = icube->Camera();
      Application::Log(currentKernels);
    } catch (iException &e) {
      Pvl errPvl = e.PvlErrors();

      if(errPvl.Groups() > 0) {
        currentKernels += PvlKeyword("Error", errPvl.Group(errPvl.Groups()-1)["Message"][0]); 
      }
      
      Application::Log(currentKernels);
      icube->PutGroup(originalKernels);
      throw e;
    }
    if (ui.GetBoolean("ATTACH")) {
      Table ckTable = cam->InstrumentRotation()->Cache("InstrumentPointing");
      ckTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      ckTable.Label() += PvlKeyword("Kernels");

      for (int i=0; i<ckKeyword.Size(); i++) {
        ckTable.Label()["Kernels"].AddValue(ckKeyword[i]);
      }
      icube->Write(ckTable);

      Table spkTable = cam->InstrumentPosition()->Cache("InstrumentPosition");
      spkTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      spkTable.Label() += PvlKeyword("Kernels");
      for (int i=0; i<spkKeyword.Size(); i++) {
        spkTable.Label()["Kernels"].AddValue(spkKeyword[i]);
      }
      icube->Write(spkTable);

      Table bodyTable = cam->BodyRotation()->Cache("BodyRotation");
      bodyTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      bodyTable.Label() += PvlKeyword("Kernels");
      for (int i=0; i<targetSpkKeyword.Size(); i++) {
        bodyTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);
      }
      for (int i=0; i<pckKeyword.Size(); i++) {
        bodyTable.Label()["Kernels"].AddValue(pckKeyword[i]);
      }
      bodyTable.Label() += PvlKeyword("SolarLongitude", cam->SolarLongitude());
      icube->Write(bodyTable);

      Table sunTable = cam->SunPosition()->Cache("SunPosition");
      sunTable.Label() += PvlKeyword("Description", "Created by spiceinit");
      sunTable.Label() += PvlKeyword("Kernels");
      for (int i=0; i<targetSpkKeyword.Size(); i++) {
        sunTable.Label()["Kernels"].AddValue(targetSpkKeyword[i]);
      }
      icube->Write(sunTable);

      //  Save original kernels in keyword before changing to Table
      PvlKeyword origCk = currentKernels["InstrumentPointing"];
      PvlKeyword origSpk = currentKernels["InstrumentPosition"];
      PvlKeyword origTargPos = currentKernels["TargetPosition"];

      currentKernels["InstrumentPointing"] = "Table";
      for (int i=0; i<origCk.Size(); i++) {
        currentKernels["InstrumentPointing"].AddValue(origCk[i]);
      }
      currentKernels["InstrumentPosition"] = "Table";
      for (int i=0; i<origSpk.Size(); i++) {
        currentKernels["InstrumentPosition"].AddValue(origSpk[i]);
      }
      currentKernels["TargetPosition"] = "Table";
      for (int i=0; i<origTargPos.Size(); i++) {
        currentKernels["TargetPosition"].AddValue(origTargPos[i]);
      }
      icube->PutGroup(currentKernels);
    }
    //modify Kernels group only
    else {
      Pvl *label = icube->Label();
      int i=0;
      while (i < label->Objects()) {
        PvlObject currObj = label->Object(i);
        if (currObj.IsNamed("Table")) {
          if (currObj["Name"][0] == iString("InstrumentPointing")) {
            label->DeleteObject(i);
          } else if (currObj["Name"][0] == iString("InstrumentPosition")) {
            label->DeleteObject(i);
          } else if (currObj["Name"][0] == iString("BodyRotation")) {
            label->DeleteObject(i);
          } else if (currObj["Name"][0] == iString("SunPosition")) {
            label->DeleteObject(i);
          } else {
            i++;
          }
        } else {
          i++;
        }
      }
    }

    p.WriteHistory(*icube);
  } catch (iException &e) {
    e.Clear();
    icube->PutGroup(originalKernels);
    return false;
  }

  return true;
}
