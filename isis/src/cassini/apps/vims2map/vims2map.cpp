#define GUIHELPERS

#include "Isis.h"
#include "Camera.h"
#include "ProcessByBrick.h"
#include "ProcessGroundPolygons.h"
#include "FileList.h"
#include "ProjectionFactory.h"
#include "ProcessRubberSheet.h"

#include "iException.h"
#include "vims2map.h"

using namespace std;
using namespace Isis;

void PrintMap ();
void rasterizeVims(Isis::Buffer &in);
std::vector<double> vimsValues;

map <string,void*> GuiHelpers(){
  map <string,void*> helper;
  helper ["PrintMap"] = (void*) PrintMap;
  return helper;
}

// Global variables
ProcessGroundPolygons g_pgp;
UniversalGroundMap *g_groundMap;
int g_bands;

void IsisMain() {

  ProcessRubberSheet p;
  Camera *incam = NULL;
  Cube *icube;

  // Get the map projection file provided by the user
  UserInterface &ui = Application::GetUserInterface();
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  FileList list;
  list.Read(ui.GetFilename("FROMLIST"));
  if (list.size() < 1) {
    string msg = "The list file [" + ui.GetFilename("FROMLIST") +
                 "does not contain any data";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  double newminlat, newmaxlat, newminlon, newmaxlon;
  double minlat = 90; 
  double maxlat = -90;
  double minlon = 360.0;
  double maxlon = 0;
  PvlGroup camGrp;
  PvlGroup bandBinGrp;
  string lastBandString;

  //Loop thru each file in the FROMLIST 
  for(unsigned int i = 0; i<list.size(); i++) {
    // Open the input cube and get the camera
    CubeAttributeInput atts0(list[i]);
    icube = p.SetInputCube (list[i], atts0);
    g_bands = icube->Bands();
    incam = icube->Camera();
  
    // Make sure it is not the sky
    if (incam->IsSky()) {
      string msg = "The image [" + list[i] +
                   "] is targeting the sky, use skymap instead.";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }

    // Make sure all the bands for all the files match
    if( i>1 && atts0.BandsStr() != lastBandString) {
      string msg = "The Band numbers for all the files do not match.";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    } else {
      lastBandString = atts0.BandsStr();
    }
  
    // Get the mapping group and the BandBin group
    Pvl camMap;
    incam->BasicMapping(camMap);
    camGrp = camMap.FindGroup("Mapping");
    if(icube->HasGroup("BandBin")) {
      bandBinGrp = icube->GetGroup("BandBin");
      
    }
      
    incam->GroundRange(newminlat,newmaxlat,newminlon,newmaxlon,userMap);
    //set min lat/lon
    if(newminlat < minlat) {
      minlat = newminlat;
    }
    if(newminlon < minlon) {
      minlon = newminlon;
    }
  
    //set max lat/lon
    if(newmaxlat > maxlat) {
      maxlat = newmaxlat;
    }
    if(newmaxlon > maxlon) {
      maxlon = newmaxlon;
    }
  } //end for list.size
 
  camGrp.AddKeyword(PvlKeyword("MinimumLatitude",minlat),Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MaximumLatitude",maxlat),Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MinimumLongitude",minlon),Pvl::Replace);
  camGrp.AddKeyword(PvlKeyword("MaximumLongitude",maxlon),Pvl::Replace);


  // We want to delete the keywords we just added if the user wants the range
  // out of the mapfile, otherwise they will replace any keywords not in the
  // mapfile
  if (ui.GetString("DEFAULTRANGE") == "MAP") {
    camGrp.DeleteKeyword("MinimumLatitude");
    camGrp.DeleteKeyword("MaximumLatitude");
    camGrp.DeleteKeyword("MinimumLongitude");
    camGrp.DeleteKeyword("MaximumLongitude");
  }
  // Otherwise, remove the keywords from the map file so the camera keywords
  // will be propogated correctly
  else {
    while (userGrp.HasKeyword("MinimumLatitude")) {
      userGrp.DeleteKeyword("MinimumLatitude");
    }
    while (userGrp.HasKeyword("MinimumLongitude")) {
      userGrp.DeleteKeyword("MinimumLongitude");
    }
    while (userGrp.HasKeyword("MaximumLatitude")) {
      userGrp.DeleteKeyword("MaximumLatitude");
    }
    while (userGrp.HasKeyword("MaximumLongitude")) {
      userGrp.DeleteKeyword("MaximumLongitude");
    }
  }

  // If the user decided to enter a ground range then override
  if (ui.WasEntered("MINLON")) {
    userGrp.AddKeyword(PvlKeyword("MinimumLongitude",
                                      ui.GetDouble("MINLON")),Pvl::Replace);
  }

  if (ui.WasEntered("MAXLON")) {
    userGrp.AddKeyword(PvlKeyword("MaximumLongitude",
                                      ui.GetDouble("MAXLON")),Pvl::Replace);
  }

  if (ui.WasEntered("MINLAT")) {
    userGrp.AddKeyword(PvlKeyword("MinimumLatitude",
                                      ui.GetDouble("MINLAT")),Pvl::Replace);
  }

  if (ui.WasEntered("MAXLAT")) {
    userGrp.AddKeyword(PvlKeyword("MaximumLatitude",
                                      ui.GetDouble("MAXLAT")),Pvl::Replace);
  }

  // If they want the res. from the mapfile, delete it from the camera so
  // nothing gets overriden
  if (ui.GetString("PIXRES") == "MAP") {
    camGrp.DeleteKeyword("PixelResolution");
  }
  // Otherwise, delete any resolution keywords from the mapfile so the camera
  // info is propogated over
  else if (ui.GetString("PIXRES") == "CAMERA") {
    if (userGrp.HasKeyword("Scale")) {
      userGrp.DeleteKeyword("Scale");
    }
    if (userGrp.HasKeyword("PixelResolution")) {
      userGrp.DeleteKeyword("PixelResolution");
    }
  }

  // Copy any defaults that are not in the user map from the camera map file
  for (int k=0; k<camGrp.Keywords(); k++) {
    if (!userGrp.HasKeyword(camGrp[k].Name())) {
      userGrp += camGrp[k];
    }
  }

  // If the user decided to enter a resolution then override
  if (ui.GetString("PIXRES") == "MPP") {
    userGrp.AddKeyword(PvlKeyword("PixelResolution",
                                      ui.GetDouble("RESOLUTION")),
                                      Pvl::Replace);
    if (userGrp.HasKeyword("Scale")) {
      userGrp.DeleteKeyword("Scale");
    }
  }
  else if (ui.GetString("PIXRES") == "PPD") {
    userGrp.AddKeyword(PvlKeyword("Scale",
                                      ui.GetDouble("RESOLUTION")),
                                      Pvl::Replace);
    if (userGrp.HasKeyword("PixelResolution")) {
      userGrp.DeleteKeyword("PixelResolution");
    }
  }

  // See if the user want us to handle the longitude seam
  if (ui.GetString("DEFAULTRANGE") == "CAMERA" || ui.GetString("DEFAULTRANGE") == "MINIMIZE") {
    if (incam->IntersectsLongitudeDomain(userMap)) {
      if (ui.GetString("LONSEAM") == "AUTO") {
        if ((int) userGrp["LongitudeDomain"] == 360) {
          userGrp.AddKeyword(PvlKeyword("LongitudeDomain",180),
                                            Pvl::Replace);
          if (incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.AddKeyword(PvlKeyword("LongitudeDomain",360),
                                            Pvl::Replace);
          }
        }
        else {
          userGrp.AddKeyword(PvlKeyword("LongitudeDomain",360),
                                            Pvl::Replace);
          if (incam->IntersectsLongitudeDomain(userMap)) {
            // Its looks like a global image so switch back to the
            // users preference
            userGrp.AddKeyword(PvlKeyword("LongitudeDomain",180),
                                              Pvl::Replace);
          }
        }
        // Make the target info match the new longitude domain
        double minlat,maxlat,minlon,maxlon;
        incam->GroundRange(minlat,maxlat,minlon,maxlon,userMap);
        userGrp.AddKeyword(PvlKeyword("MinimumLatitude",minlat),Pvl::Replace);
        userGrp.AddKeyword(PvlKeyword("MaximumLatitude",maxlat),Pvl::Replace);
        userGrp.AddKeyword(PvlKeyword("MinimumLongitude",minlon),Pvl::Replace);
        userGrp.AddKeyword(PvlKeyword("MaximumLongitude",maxlon),Pvl::Replace);
      }

      else if (ui.GetString("LONSEAM") == "ERROR") {
        string msg = "The image [" + ui.GetFilename("FROM") + "] crosses the " +
                                                              "longitude seam";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    }
  }

  
  Pvl pvl;
  pvl.AddGroup(userGrp);
  pvl.AddGroup(bandBinGrp);
  g_pgp.SetOutputCube("TO", pvl, g_bands);
 
  for (unsigned int f = 0; f<list.size(); f++) {

    Pvl vimsCubePvl(list[f]);
    g_groundMap = new UniversalGroundMap(vimsCubePvl);

    // Loop through the input cube and get the all pixels values for all bands
    ProcessByBrick pbb;
    pbb.Progress()->SetText("Working on file:  " + list[f]);
    pbb.SetBrickSize(1,1,g_bands);
    CubeAttributeInput atts0(list[f]);
    pbb.SetInputCube(list[f], atts0, 0);
    pbb.StartProcess(rasterizeVims);
    pbb.EndProcess();
  }
  g_pgp.EndProcess();


}

// Helper function to print out mapfile to session log
void PrintMap() {
  UserInterface &ui = Application::GetUserInterface();

  // Get mapping group from map file
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &userGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  //Write map file out to the log
  Isis::Application::GuiLog(userGrp);
}

void rasterizeVims (Isis::Buffer &in) {

  std::vector<double>lat, lon;
  std::vector<double>vimsValues;

  for(int i = 0; i<in.size(); i++) {
    vimsValues.push_back(in[i]);
  }

  int l = in.Line();
  int s = in.Sample();

  std::vector<double> vimsSamps, vimsLines;
  vimsSamps.push_back(s-0.5);
  vimsSamps.push_back(s+0.5);
  vimsSamps.push_back(s+0.5);
  vimsSamps.push_back(s-0.5);

  vimsLines.push_back(l-0.5);
  vimsLines.push_back(l-0.5);
  vimsLines.push_back(l+0.5);
  vimsLines.push_back(l+0.5);

  //Now need to convert all samps and lines to lat lon.
  for (unsigned int j = 0; j<vimsSamps.size(); j++) {

    if (g_groundMap->SetImage(vimsSamps[j], vimsLines[j])) {
      double latitude, longitude;

      latitude = g_groundMap->UniversalLatitude();
      longitude = g_groundMap->UniversalLongitude();
      lat.push_back(latitude);
      lon.push_back(longitude);
    }

  }

  if (lat.size() > 3) {
    g_pgp.Rasterize(lat, lon, vimsValues);
  }

  lat.clear();
  lon.clear();
  vimsSamps.clear();
  vimsLines.clear();
}
