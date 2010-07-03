#include "Isis.h"
#include <iostream>
#include <sstream>
#include <string>
#include "Pvl.h"
#include "Cube.h"
#include "History.h"
#include "ProjectionFactory.h"

using namespace Isis;
using namespace std;

void IsisMain(){
  // Access input parameters (user interface)
  UserInterface &ui = Application::GetUserInterface();
            
  // Open the input cube
  Cube cube;
  cube.Open(ui.GetFilename("FROM"), "rw");

  //Get the map projection file provided by the user
  Pvl userMap;
  userMap.Read(ui.GetFilename("MAP"));
  PvlGroup &mapGrp = userMap.FindGroup("Mapping",Pvl::Traverse);

  // Error checking to ensure the map projection file provided contains
  // information pertaining to a target, body radius, and longitude direction
  if (!mapGrp.HasKeyword("TargetName")) {
    string msg = "The given MAP [" + userMap.Name() +
      "] does not have the TargetName keyword.";
    throw iException::Message( iException::User, msg, _FILEINFO_ );
  }
  else if (!mapGrp.HasKeyword("EquatorialRadius") || 
           !mapGrp.HasKeyword("PolarRadius")) {
    string msg = "The given MAP [" + userMap.Name() +
      "] does not have the EquatorialRadius and PolarRadius keywords.";
    throw iException::Message( iException::User, msg, _FILEINFO_ );
  }
  else if (!mapGrp.HasKeyword("LongitudeDomain")) {
    string msg = "The given MAP [" + userMap.Name() +
      "] does not have the LongitudeDomain keyword.";
    throw iException::Message( iException::User, msg, _FILEINFO_ );
  }

  //Read in line and sample inputs
  double line = ui.GetDouble("LINE");
  double samp = ui.GetDouble("SAMPLE");

  // Get user entered option
  string option = ui.GetString("COORDINATES");

  //Given x,y coordinates
  if(option=="XY") {
    //find values for x and y at the origin (upperleftcorner)
    double x = ui.GetDouble("X");
    double y = ui.GetDouble("Y");
    //Get Resolution and Scale
    double res = 0.0;
    double scale = 0.0;
    Projection *proj = ProjectionFactory::Create(userMap,false);
    if( mapGrp.HasKeyword("PixelResolution") ) {
      double localRadius = proj->LocalRadius( proj->TrueScaleLatitude() );
      res = mapGrp.FindKeyword("PixelResolution");
      scale = (2.0 * Isis::PI * localRadius) / (360.0 * res);
    }
    else if( mapGrp.HasKeyword("Scale") ) {
      double localRadius = proj->LocalRadius( proj->TrueScaleLatitude() );
      scale = mapGrp.FindKeyword("Scale");
      res = (2.0 * Isis::PI * localRadius) / (360.0 * scale);
    }
    else {
      string msg = "The given MAP [" + userMap.Name() +
        "] does not have the PixelResolution or Scale keywords.";
      throw iException::Message( iException::User, msg, _FILEINFO_ );
    }
    x = x - res*(samp - 0.5);
    y = y + res*(line - 0.5);
    //add origen values to Mapping Group
    mapGrp.AddKeyword(PvlKeyword("UpperLeftCornerX",x,"meters"), Pvl::Replace);
    mapGrp.AddKeyword(PvlKeyword("UpperLeftCornerY",y,"meters"), Pvl::Replace);
    if( not mapGrp.HasKeyword("PixelResolution") ) {
      mapGrp.AddKeyword(PvlKeyword("PixelResolution",res,"meters"));
    }
    if( not mapGrp.HasKeyword("Scale") ) {
      mapGrp.AddKeyword(PvlKeyword("Scale",scale,"pixels/degree"));
    }
  }
  //Given latitude,longitude coordinates
  else {
    //get lat and lon from user interface
    double lat = ui.GetDouble("LAT");
    double lon = ui.GetDouble("LONG");
    //create projection using input map
    Projection *proj = ProjectionFactory::Create(userMap,false);
    // feed lat, lon into projection
    proj->SetGround(lat,lon);
    //find origen values for x and y
    double x = proj->XCoord();
    double y = proj->YCoord();
    //Get Resolution and Scale
    double res = 0.0;
    double scale = 0.0;
    if( mapGrp.HasKeyword("PixelResolution") ) {
      double localRadius = proj->LocalRadius( proj->TrueScaleLatitude() );
      res = mapGrp.FindKeyword("PixelResolution");
      scale = (2.0 * Isis::PI * localRadius) / (360.0 * res);
    }
    else if( mapGrp.HasKeyword("Scale") ) {
      double localRadius = proj->LocalRadius( proj->TrueScaleLatitude() );
      scale = mapGrp.FindKeyword("Scale");
      res = (2.0 * Isis::PI * localRadius) / (360.0 * scale);
    }
    else {
      string msg = "The given MAP[" + userMap.Name() +
        "] does not have the PixelResolution or Scale keywords.";
      throw iException::Message( iException::User, msg, _FILEINFO_ );
    }
    x = x - res*(samp - 0.5);
    y = y + res*(line - 0.5);
    //add origen values to Mapping Group
    mapGrp.AddKeyword(PvlKeyword("UpperLeftCornerX",x,"meters"), Pvl::Replace);
    mapGrp.AddKeyword(PvlKeyword("UpperLeftCornerY",y,"meters"), Pvl::Replace);
    if( not mapGrp.HasKeyword("PixelResolution") ) {
      mapGrp.AddKeyword(PvlKeyword("PixelResolution",res,"meters"));
    }
    if( not mapGrp.HasKeyword("Scale") ) {
      mapGrp.AddKeyword(PvlKeyword("Scale",scale,"pixels/degree"));
    }
  }
  // Output the mapping group used to the Gui session log
  Application::GuiLog(userMap);
  // Extract label from cube file
  Pvl *label = cube.Label();
  PvlObject &o = label->FindObject("IsisCube");
  //Add Mapping Group to input cube
  if (o.HasGroup("Mapping")){
    o.DeleteGroup("Mapping");
  }
  o.AddGroup(mapGrp);

  //keep track of change to labels in history
  History hist = History("IsisCube");
  try {
      cube.Read(hist);
  }
  catch(iException &e) {
      e.Clear();
  }
  hist.AddEntry();
  cube.Write(hist); 

  cube.Close();  
}
