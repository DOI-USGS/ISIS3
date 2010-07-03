#define GUIHELPERS

#include "Isis.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "iException.h"
#include "Spice.h"
#include "FileList.h"
#include "ProjectionFactory.h"
#include "Cube.h"
#include "iString.h"
#include "SpecialPixel.h"
#include "Camera.h"
#include <map>
#include <cctype>

using namespace std;
using namespace Isis;


//functions in the code
void addProject(PvlGroup &mapping);
void addTarget(PvlGroup &mapping);
void addRange(PvlGroup &mapping);
void calcRange(double &minLat, double &maxLat,
               double &minLon, double &maxLon);
void addResolution(PvlGroup &mapping);
double calcResolution();

//helper button functins in the code
void helperButtonLogMap();
void helperButtonLoadMap ();
void helperButtonLogTargDef();
void helperButtonLoadTargDef();
void helperButtonLogRadius();
void helperButtonCalcRange ();
double helperButtonCalcResolution ();

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["helperButtonLogMap"] = (void*) helperButtonLogMap;
  helper ["helperButtonLoadMap"] = (void*) helperButtonLoadMap;
  helper ["helperButtonLogTargDef"] = (void*) helperButtonLogTargDef;
  helper ["helperButtonLoadTargDef"] = (void*) helperButtonLoadTargDef;
  helper ["helperButtonLogRadius"] = (void*) helperButtonLogRadius;
  helper ["helperButtonCalcRange"] = (void*) helperButtonCalcRange;
  helper ["helperButtonCalcResolution"] = (void*) helperButtonCalcResolution;
  return helper;
}

void IsisMain() {
// Make the mapping pvl group and add info to it.
  PvlGroup mapping("Mapping");
  addProject(mapping);
  addTarget(mapping);
  addRange(mapping);
  addResolution(mapping);

  // Get map file name form GUI and write the mapping group pvl 
  // to the output file, add .map extension if the
  // user didnt enter an extension
  UserInterface &ui = Application::GetUserInterface();
  Filename out = ui.GetFilename("MAP");
  string output = ui.GetFilename("MAP");
  if (out.Extension() == "") {
    output += ".map"; 
  }
  Pvl p;
  p.AddGroup(mapping);
  p.Write(output);
}

//Helper function to output map file to log.
void helperButtonLogMap () {
  UserInterface &ui = Application::GetUserInterface();
  string mapFile(ui.GetFilename("MAP"));
  Pvl p;
  p.Read(mapFile);
  PvlGroup t = p.FindGroup("mapping",Pvl::Traverse);
  string Ostring = "***** Output of [" + mapFile + "] *****";
  Application::GuiLog(Ostring);
  Application::GuiLog(t);
}
//...........end of helper function LogMap ........

//Helper 2 function to update GUI with map file values.
void helperButtonLoadMap () {
  UserInterface &ui = Application::GetUserInterface();
  string mapFile(ui.GetFilename("MAP"));
  Pvl p;
  p.Read(mapFile);
  PvlGroup t = p.FindGroup("mapping",Pvl::Traverse);
//Projection Stuff
  ui.Clear("CLON");
  ui.Clear("CLAT");
  ui.Clear("SCALEFACTOR");
  ui.Clear("PAR1");
  ui.Clear("PAR2");
  ui.Clear("PLAT");
  ui.Clear("PLON");
  ui.Clear("PROT");

  if (t.HasKeyword("ProjectionName")) {
    iString projIn = (string)t["ProjectionName"];
    projIn.UpCase();
    ui.Clear("PROJECTION");
    ui.PutAsString("PROJECTION",projIn);
  }
  if (t.HasKeyword("CenterLongitude")) {
    double clonIn = t["CenterLongitude"];
    ui.Clear("CLON");
    ui.PutDouble("CLON",clonIn);
  }
  if (t.HasKeyword("CenterLatitude")) {
    double clatIn = t["CenterLatitude"];
    ui.Clear("CLAT");
    ui.PutDouble("CLAT",clatIn);
  }
  if (t.HasKeyword("ScaleFactor")) {
    double scaleFactorIn = t["ScaleFactor"];
    ui.Clear("SCALEFACTOR");
    ui.PutDouble("SCALEFACTOR",scaleFactorIn);
  }
  if (t.HasKeyword("FirstStandardParallel")) {
    double par1In = t["FirstStandardParallel"];
    ui.Clear("PAR1");
    ui.PutDouble("PAR1",par1In);
  }
  if (t.HasKeyword("SecondStandardParallel")) {
    double par2In = t["SecondStandardParallel"];
    ui.Clear("PAR2");
    ui.PutDouble("PAR2",par2In);
  }
  if (t.HasKeyword("PoleLatitude")) {
    double pLatIn = t["PoleLatitude"];
    ui.Clear("PLAT");
    ui.PutDouble("PLAT",pLatIn);
  }
  if (t.HasKeyword("PoleLongitude")) {
    double pLonIn = t["PoleLongitude"];
    ui.Clear("PLON");
    ui.PutDouble("PLON",pLonIn);
  }
  if (t.HasKeyword("PoleRotation")) {
    double pRotIn = t["PoleRotation"];
    ui.Clear("PROT");
    ui.PutDouble("PROT",pRotIn);
  }

//Target Parameters stuff
  string use = "IMAGE";
  ui.Clear("TARGOPT");
  ui.PutAsString("TARGOPT",use);
  ui.Clear("TARGDEF");
  ui.Clear("TARGETNAME");
  ui.Clear("LATTYPE");
  ui.Clear("LONDIR");
  ui.Clear("EQRADIUS");
  ui.Clear("POLRADIUS");

  if (t.HasKeyword("TargetName")) {
    string use = "USER";
    ui.Clear("TARGOPT");
    ui.PutAsString("TARGOPT",use);
    string tNameIn = t["TargetName"];
    ui.Clear("TARGETNAME");
    ui.PutAsString("TARGETNAME",tNameIn);

    iString LTIn = (string)t["LatitudeType"];
    LTIn.UpCase();
    ui.Clear("LATTYPE");
    ui.PutAsString("LATTYPE",LTIn);

    iString LDIn = (string)t["LongitudeDirection"];
    LDIn.UpCase();
    ui.Clear("LONDIR");
    ui.PutAsString("LONDIR",LDIn);

    string LDomIn = t["LongitudeDomain"];
    ui.Clear("LONDOM");
    ui.PutAsString("LONDOM",LDomIn);

    string EQIn = t["EquatorialRadius"];
    ui.Clear("EQRADIUS");
    ui.PutAsString("EQRADIUS",EQIn);
    string PRIn = t["PolarRadius"];
    ui.Clear("POLRADIUS");
    ui.PutAsString("POLRADIUS",PRIn);
  }
//Ground Range Parameter stuff
  ui.Clear("MINLAT");
  ui.Clear("MAXLAT");
  ui.Clear("MINLON");
  ui.Clear("MAXLON");
  string useR = "IMAGE";
  ui.Clear("RNGOPT");
  ui.PutAsString("RNGOPT",useR);

  if (t.HasKeyword("MinimumLatitude")) {
    double minlatIn = t["MinimumLatitude"];
    string useR = "USER";
    ui.Clear("RNGOPT");
    ui.PutAsString("RNGOPT",useR);
    ui.Clear("MINLAT");
    ui.PutDouble("MINLAT",minlatIn);
    double maxlatIn = t["MaximumLatitude"];
    ui.Clear("MAXLAT");
    ui.PutDouble("MAXLAT",maxlatIn);
    double minlonIn = t["MinimumLongitude"];
    ui.Clear("MINLON");
    ui.PutDouble("MINLON",minlonIn);
    double maxlonIn = t["MaximumLongitude"];
    ui.Clear("MAXLON");
    ui.PutDouble("MAXLON",maxlonIn);
  }
//Resolution Parameter stuff
  if (t.HasKeyword("PixelResolution")) {
    string useM = "MPP";
    ui.Clear("RESOPT");
    ui.PutAsString("RESOPT",useM);
    double pixresIn = t["PixelResolution"];
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION",pixresIn);
  }
  if (t.HasKeyword("Scale")) {
    string useM = "PPD";
    ui.Clear("RESOPT");
    ui.PutAsString("RESOPT",useM);
    double Mscale = t["Scale"];
    ui.Clear("RESOLUTION");
    ui.PutDouble("RESOLUTION",Mscale);
  }
}
//----------End Helper function to load map file into GUI -----------

//helper function to output targdef to log
void helperButtonLogTargDef (){
  UserInterface &ui = Application::GetUserInterface();
  string targetFile(ui.GetFilename("TARGDEF"));  
  Pvl p;
  p.Read(targetFile);
  PvlGroup t = p.FindGroup("mapping",Pvl::Traverse);
  string Ostring = "***** Output of [" + targetFile + "] *****";
  Application::GuiLog(Ostring);
  Application::GuiLog(t);
}
//------------End Helper function to display targdef info to log -----

//helper function to load target def. to GUI
void helperButtonLoadTargDef() {
  UserInterface &ui = Application::GetUserInterface();
  string targetFile(ui.GetFilename("TARGDEF"));

// test if targdef was entered
  Pvl p;
  p.Read(targetFile);
  PvlGroup t = p.FindGroup("mapping",Pvl::Traverse);
//Load the targdef values into the GUI
  string tOpt = "USER";
  ui.Clear("TARGOPT");
  ui.PutAsString("TARGOPT",tOpt);
  if (t.HasKeyword("TargetName")) {
    string tNameIn = t["TargetName"];
    ui.Clear("TARGETNAME");
    ui.PutAsString("TARGETNAME",tNameIn);
  }
  if (t.HasKeyword("LatitudeType")) {
    iString LTIn = (string)t["LatitudeType"];
    LTIn.UpCase();
    ui.Clear("LATTYPE");
    ui.PutAsString("LATTYPE",LTIn);
  }
  if (t.HasKeyword("LongitudeDirection")) {
    iString LDIn = (string)t["LongitudeDirection"];
    LDIn.UpCase();
    ui.Clear("LONDIR");
    ui.PutAsString("LONDIR",LDIn);
  }
  if (t.HasKeyword("LongitudeDomain")) {
    string LDomIn = t["LongitudeDomain"];
    ui.Clear("LONDOM");
    ui.PutAsString("LONDOM",LDomIn);
  }
  if (t.HasKeyword("EquatorialRadius")) {
    string EQIn = t["EquatorialRadius"];
    ui.Clear("EQRADIUS");
    ui.PutAsString("EQRADIUS",EQIn);
  }
  if (t.HasKeyword("PolarRadius")) {
    string PRIn = t["PolarRadius"];
    ui.Clear("POLRADIUS");
    ui.PutAsString("POLRADIUS",PRIn);
  }
}
//-----------end Helper Function to Load target def into GUI --------

//Helper function to show system radius in log.
void helperButtonLogRadius() {
  UserInterface &ui = Application::GetUserInterface();
  iString targetName = ui.GetString("TARGETNAME");
  Pvl tMap;
//call function to get system radius
  PvlGroup tGrp = Projection::TargetRadii(targetName);
  tMap.AddGroup(tGrp);
  string Ostring = "***** System radii for " + targetName + "*****";
//writh to log string(Ostring and mapping group
  Application::GuiLog(Ostring);
  Application::GuiLog(tMap);
}
//-----------end ot log radius helper function---------

//Helper function to run calcRange function.
void helperButtonCalcRange () {
  UserInterface &ui = Application::GetUserInterface();
  double minLat;
  double maxLat;
  double minLon;
  double maxLon;
// Run the function calcRange of calculate range info
  calcRange(minLat,maxLat,minLon,maxLon);
// Write ranges to the GUI
  string use = "USER";
  ui.Clear("RNGOPT");
  ui.PutAsString("RNGOPT",use);
  ui.Clear("MINLAT");
  ui.PutDouble("MINLAT",minLat);
  ui.Clear("MAXLAT");
  ui.PutDouble("MAXLAT",maxLat);
  ui.Clear("MINLON");
  ui.PutDouble("MINLON",minLon);
  ui.Clear("MAXLON");
  ui.PutDouble("MAXLON",maxLon);
}
//--------------End Helper function to run calcRange ----------------

//Helper function to run calcResolution function.
double helperButtonCalcResolution(){
  UserInterface &ui = Application::GetUserInterface();
// Call calcResolution which will return a double
  double Res = calcResolution();
//update the GUI with the new resolution
  string resUse = "MPP";
  ui.Clear("RESOPT");
  ui.PutAsString("RESOPT",resUse);
  ui.Clear("RESOLUTION");
  ui.PutDouble("RESOLUTION",Res);
  return Res;
}
//..............End Helper function to run calcResolution

// Function to Add the projection information to the Mapping PVL
void addProject(PvlGroup &mapping) {
  UserInterface &ui = Application::GetUserInterface();
  iString projName = ui.GetString("PROJECTION");
//setup a look up table for projection names
  map <iString,iString> projLUT;
  projLUT ["SINUSOIDAL"] = "Sinusoidal";
  projLUT ["MERCATOR"] = "Mercator";
  projLUT ["TRANSVERSEMERCATOR"] = "TransverseMercator";
  projLUT ["ORTHOGRAPHIC"] = "Orthographic";
  projLUT ["POLARSTEREOGRAPHIC"] = "PolarStereographic";
  projLUT ["SIMPLECYLINDRICAL"] = "SimpleCylindrical";
  projLUT ["EQUIRECTANGULAR"] = "Equirectangular";
  projLUT ["LAMBERTCONFORMAL"] = "LambertConformal";
  projLUT ["OBLIQUECYLINDRICAL"] = "ObliqueCylindrical";

// Add Projection keywords to the mappping PVL
  mapping += PvlKeyword("ProjectionName",projLUT[projName]);
  if (ui.WasEntered("CLON")) {
    double clonOut = ui.GetDouble("CLON");
    mapping += PvlKeyword("CenterLongitude",clonOut);
  }
  if (ui.WasEntered("CLAT")) {
    double clatOut = ui.GetDouble("CLAT");
    mapping += PvlKeyword("CenterLatitude",clatOut);
  }
  if (ui.WasEntered("SCALEFACTOR")) {
    double scaleFactorOut = ui.GetDouble("SCALEFACTOR");
    mapping += PvlKeyword("ScaleFactor",scaleFactorOut);
  }
  if (ui.WasEntered("PAR1")) {
    double par1 = ui.GetDouble("PAR1");
    mapping += PvlKeyword("FirstStandardParallel",par1);
  }
  if (ui.WasEntered("PAR2")) {
    double par2 = ui.GetDouble("PAR2");
    mapping += PvlKeyword("SecondStandardParallel",par2);
  }
  if (ui.WasEntered("PLAT")) {
    double plat = ui.GetDouble("PLAT");
    mapping += PvlKeyword("PoleLatitude",plat);
  }
  if (ui.WasEntered("PLON")) {
    double plon = ui.GetDouble("PLON");
    mapping += PvlKeyword("PoleLongitude",plon);
  }
  if (ui.WasEntered("PROT")) {
    double prot = ui.GetDouble("PROT");
    mapping += PvlKeyword("PoleRotation",prot);
  }
}

// Function to Add the target information to the Mapping PVL
void addTarget(PvlGroup &mapping) {
  UserInterface &ui = Application::GetUserInterface();
  if (ui.GetString("TARGOPT") == "SYSTEM") {
    string targetFile(ui.GetFilename("TARGDEF"));
    Pvl p;
    p.Read(targetFile);
    PvlGroup t = p.FindGroup("mapping");
    if (t.HasKeyword("TargetName")) {
      mapping += t["TargetName"];
    }
    if (t.HasKeyword("EquatorialRadius")) {
      mapping += t["EquatorialRadius"];
    }
    if (t.HasKeyword("PolarRadius")) {
      mapping += t["PolarRadius"];
    }
    if (t.HasKeyword("LatitudeType")) {
      mapping += t["LatitudeType"];
    }
    if (t.HasKeyword("LongitudeDirection")) {
      mapping += t["LongitudeDirection"];
    }
  }
// if TARGOPT is user and no radii enter the run TargetRadii to get
// the system radii for target name.
  else if (ui.GetString("TARGOPT") == "USER") {
    iString targetName = ui.GetString("TARGETNAME");
    PvlGroup grp = Projection::TargetRadii(targetName);
    double equatorialRad = grp["EquatorialRadius"];
    double polarRad = grp["PolarRadius"];
// if radii were entered in GUI then set radii to entered value
    if (ui.WasEntered("EQRADIUS")) {
      equatorialRad = ui.GetDouble("EQRADIUS");
    }
    if (ui.WasEntered("POLRADIUS")) {
      polarRad = ui.GetDouble("POLRADIUS");
    }
    iString latType = ui.GetString("LATTYPE");
    if (latType == "PLANETOCENTRIC") {
      latType = "Planetocentric";
    }
    else if (latType == "PLANETOGRAPHIC") {
      latType = "Planetographic";
    }
    iString lonDir = ui.GetString("LONDIR");
    if (lonDir == "POSITIVEEAST") {
      lonDir = "PositiveEast";
    }
    else if (lonDir == "POSITIVEWEST") {
      lonDir = "PositiveWest";
    }
    iString lonDom = ui.GetString("LONDOM");
//Add targdef values to the mapping pvl
    mapping += PvlKeyword("TargetName",targetName);
    mapping += PvlKeyword("EquatorialRadius",equatorialRad, "meters");
    mapping += PvlKeyword("PolarRadius",polarRad, "meters");
    mapping += PvlKeyword("LatitudeType",latType);
    mapping += PvlKeyword("LongitudeDirection",lonDir);
    mapping += PvlKeyword("LongitudeDomain",lonDom);
  }
}

// Function to add the range information to the mapping PVL
void addRange(PvlGroup &mapping) {
  UserInterface &ui = Application::GetUserInterface();
//Use the values that have been entered in the GUI
  if (ui.GetString("RNGOPT") == "USER") {
    double minLat = ui.GetDouble("MINLAT");
    mapping += PvlKeyword("MinimumLatitude",minLat);
    double maxLat = ui.GetDouble("MAXLAT");
    mapping += PvlKeyword("MaximumLatitude",maxLat);
    double minLon = ui.GetDouble("MINLON");
    mapping += PvlKeyword("MinimumLongitude",minLon);
    double maxLon = ui.GetDouble("MAXLON");
    mapping += PvlKeyword("MaximumLongitude",maxLon);
  }
  else if (ui.GetString("RNGOPT") == "CALC") {
// calculate range values using function calcRange and fromlist
    double minLat;
    double maxLat;
    double minLon;
    double maxLon;
//Call calcRange to calculate min and max ground range values
    calcRange(minLat,maxLat,minLon,maxLon);
    mapping += PvlKeyword("MinimumLatitude",minLat);
    mapping += PvlKeyword("MaximumLatitude",maxLat);
    mapping += PvlKeyword("MinimumLongitude",minLon);
    mapping += PvlKeyword("MaximumLongitude",maxLon);
  }
}

// Function to add the resolution information to the mapping PVL
void addResolution(PvlGroup &mapping) {
// Use values that have been entered in GUI
  UserInterface &ui = Application::GetUserInterface();
  if (ui.GetString("RESOPT") == "PPD") {
    double res = ui.GetDouble("RESOLUTION");
    mapping += PvlKeyword("Scale",iString(res),"pixels/degree");
  }
  else if (ui.GetString("RESOPT") == "MPP") {
    double res = ui.GetDouble("RESOLUTION");
    mapping += PvlKeyword("PixelResolution",iString(res),"meters/pixel");
  }
  else if (ui.GetString("RESOPT") == "CALC") {
// run the function to calculate the resolution
    double Res = calcResolution();
    mapping += PvlKeyword("PixelResolution",Res,"meters/pixel");
  }
}

// Function to calculate the ground range from multiple inputs (list of images)
void calcRange(double &minLat, double &maxLat, 
               double &minLon, double &maxLon) {
  UserInterface &ui = Application::GetUserInterface();
  FileList flist(ui.GetFilename("FROMLIST"));
  minLat = DBL_MAX;
  maxLat = -DBL_MAX;
  minLon = DBL_MAX;
  maxLon = -DBL_MAX;
  //set up PVL of mapping info from GUI:: need to do this so we use the 
  // most current info from GUI to calculate the range
  Pvl userMap;
  PvlGroup userGrp("Mapping");
  if (ui.GetString("TARGOPT") == "SYSTEM") {
    userMap.Read(ui.GetFilename("TARGDEF"));
  }
  else if (ui.GetString("TARGOPT") == "USER") {
    userGrp += PvlKeyword("TargetName", ui.GetString("TARGETNAME"));
    iString targetName = ui.GetString("TARGETNAME");
    PvlGroup grp = Projection::TargetRadii(targetName);
    double equatorialRad = grp["EquatorialRadius"];
    double polarRad = grp["PolarRadius"];

// if radii were entered in GUI then set radii to entered value
    if (ui.WasEntered("EQRADIUS")) {
      equatorialRad = ui.GetDouble("EQRADIUS");
    }
    if (ui.WasEntered("POLRADIUS")) {
      polarRad = ui.GetDouble("POLRADIUS");
    }
    if (ui.GetString("LATTYPE") == "PLANETOCENTRIC") {
      string latType = "Planetocentric";
      userGrp += PvlKeyword("LatitudeType", latType);
    }
    else if (ui.GetString("LATTYPE") == "PLANETOGRAPHIC") {
      string latType = "Planetographic";
      userGrp += PvlKeyword("LatitudeType", latType);
    }
    if (ui.GetString("LONDIR") == "POSITIVEEAST") {
      string lonD = "PositiveEast";
      userGrp += PvlKeyword("LongitudeDirection", lonD);
    }
    else if (ui.GetString("LONDIR") == "POSITIVEWEST") {
      string lonD = "PositiveWest";
      userGrp += PvlKeyword("LongitudeDirection", lonD);
    }
    userGrp += PvlKeyword("LongitudeDomain", ui.GetString("LONDOM"));
    userGrp += PvlKeyword("EquatorialRadius", equatorialRad);
    userGrp += PvlKeyword("PolarRadius", polarRad);
    userMap.AddGroup(userGrp);
  }

  for (int i=0; i<(int)flist.size(); i++) {
// Get the mapping group from the camera
    double camMinLat;
    double camMaxLat;
    double camMinLon;
    double camMaxLon;
    Cube c;
    c.Open(flist[i]);
    Camera *cam=c.Camera();
    Pvl defaultMap;
    cam->BasicMapping(defaultMap);
    PvlGroup &defaultGrp = defaultMap.FindGroup("Mapping");
//Move any defaults that are not in the user map
    for (int k=0; k<defaultGrp.Keywords(); k++) {
      if (!userGrp.HasKeyword(defaultGrp[k].Name())) {
        userGrp += defaultGrp[k];
      }
    }
    userMap.AddGroup(userGrp);
//get the camera ground range min and max and solve for range
    cam->GroundRange(camMinLat,camMaxLat,camMinLon,camMaxLon,userMap);
    if (camMinLat < minLat) {
      minLat = camMinLat;
    }
    if (camMaxLat > maxLat) {
      maxLat = camMaxLat;
    }
    if (camMinLon < minLon) {
      minLon = camMinLon;
    }
    if (camMaxLon > maxLon) {
      maxLon = camMaxLon;
    }
  }
}

// Function to calculate the resolution for images in FROMLIST
//  value will be in meters
double calcResolution() {
  UserInterface &ui = Application::GetUserInterface();
  FileList flist(ui.GetFilename("FROMLIST"));
  double sumRes = 0.0;
  double highRes = DBL_MAX;
  double lowRes = -DBL_MAX;
// Loop through the from list at get high and low camera resolution
  for (int i=0; i<(int)flist.size(); i++) {
    Cube c;
    c.Open(flist[i]);
    Camera *cam = c.Camera();
    double camLowRes = cam->LowestImageResolution();
    double camHighRes = cam->HighestImageResolution();
    if (camLowRes > lowRes) lowRes = camLowRes;
    if (camHighRes < highRes) highRes = camHighRes;
    sumRes = sumRes + (camLowRes + camHighRes) / 2.0;
  }
//user input (RESCALCOPT) at GUI will determain what is output
// highest, lowest, or avg
  if (ui.GetString("RESCALCOPT") == "HIGH") return highRes;
  if (ui.GetString("RESCALCOPT") == "LOW") return lowRes;
  return(sumRes / flist.size());
}


