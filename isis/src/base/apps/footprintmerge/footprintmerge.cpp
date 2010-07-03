#include "Isis.h"

#include <iostream>

#include "FileList.h"
#include "Progress.h"
#include "iException.h"
#include "SerialNumber.h"
#include "PolygonTools.h"
#include "ImagePolygon.h"
#include "Process.h"
#include "Pvl.h"

#include "geos/geom/Geometry.h"
#include "geos/geom/MultiPolygon.h"

using namespace std; 
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  FileList imageList;
  imageList.Read(ui.GetFilename ("FROMLIST"));
  if (imageList.size() < 1) {
    std::string msg = "The list file [" + ui.GetFilename("FROMLIST") +
                 "] does not contain any data";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  Progress prog;
  prog.SetText ("Merging footprints");
  prog.SetMaximumSteps(imageList.size());
  prog.CheckStatus ();

  //  For this first loop union all image polygons
  vector<geos::geom::Geometry *> allPolys;
  vector<string> files;
  bool conv360 = false;

  for (unsigned int img=0; img<imageList.size(); img++) {

    Cube cube;
    cube.Open(imageList[img]);

    // Make sure cube has been run through spiceinit
    try {
      cube.Camera();
    } catch ( iException &e ) {
      string msg = "Spiceinit must be run prior to running footprintmerge";
      msg += " for cube [" + imageList[img] + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    //  Make sure cube has been run through footprintinit
    ImagePolygon *poly = new ImagePolygon;
    try {
      cube.Read(*poly);
      cube.Close();
    } catch ( iException &e ) {
      string msg = "Footprintinit must be run prior to running footprintmerge";
      msg += " for cube [" + imageList[img] + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    //  If more than 1 poly, set conv360 for later conversion to 180 and merging
    if (poly->Polys()->getNumGeometries() > 1) conv360 = true;

    allPolys.push_back(PolygonTools::CopyMultiPolygon(poly->Polys()));

    files.push_back(imageList[img]);

    prog.CheckStatus ();

  }

  //  If any islands are on the 0/360 boundary convert to -180/180 and merge
  //  any polys that were split on the boundary.
  if (conv360) {
    for (unsigned int i=0; i<allPolys.size(); i++) {
      geos::geom::MultiPolygon *m = (geos::geom::MultiPolygon *)allPolys[i];
      if (m->getNumGeometries() > 1 ||
          m->getCoordinates()->minCoordinate()->x > 180.) {
        geos::geom::MultiPolygon *poly =
                       PolygonTools::To180((geos::geom::MultiPolygon *)allPolys[i]);
        allPolys[i] = poly;
      }
    }
  }
  
  //  Create union poly
  geos::geom::GeometryCollection *polyCollection = 
                  Isis::globalFactory.createGeometryCollection(allPolys);
  geos::geom::Geometry *unionPoly = polyCollection->buffer(0);


  //  How many polygons are in unionPoly
  vector<geos::geom::Geometry *> islandPolys;

  if (unionPoly->getGeometryTypeId() == geos::geom::GEOS_POLYGON) {
    //  There are no islands, all cubes are in a single cluster
    std::cout<<"NO ISLANDS, ALL CUBES OVERLAP"<<std::endl;
    return;
  }
  else if (unionPoly->getGeometryTypeId() == geos::geom::GEOS_MULTIPOLYGON) {
    geos::geom::MultiPolygon *multi = (geos::geom::MultiPolygon *) unionPoly;
    for (unsigned int i=0; i<multi->getNumGeometries(); ++i) {
      islandPolys.push_back(multi->getGeometryN(i)->clone());
    }
  }
  
  // Intersect each input poly (the first of each cube) with each poly
  // from the unionPoly, and keep track of which images are in each
  // unionPoly.

  prog.SetText ("Intersecting footprints");
  prog.SetMaximumSteps(allPolys.size());
  prog.CheckStatus ();

  typedef std::vector<std::string> islandFiles;
  std::vector< islandFiles > islands;
  islands.resize(islandPolys.size());
  for (unsigned int i=0; i<allPolys.size(); i++) {
    for (unsigned int p=0; p<islandPolys.size(); p++) {
      if (allPolys[i]->intersects(islandPolys[p])) {
        islands[p].push_back(files[i]);
      }
    }
    prog.CheckStatus ();
  }
  
  
  string mode = ui.GetString("MODE");

  //  print out island statistics
  // Brief
  if (mode=="BRIEF") {
    PvlGroup results("Results");
    results += PvlKeyword("NumberOfIslands",(int)islandPolys.size());
    Application::Log(results);
  }
  else if (mode=="FULL") {
    string out = ui.GetFilename("TO");
    PvlObject results("Results");
    for (unsigned int p=0; p<islandPolys.size(); p++) {
      int numFiles = islands[p].size();
      string isle = "FootprintIsland_" + iString((int)p+1);
      PvlGroup island(isle);
      island += PvlKeyword ("NumberFiles",numFiles);
      PvlKeyword files("Files");
      for (int f=0; f<numFiles; f++) {
        files.AddValue(islands[p][f]);
      }
      island.AddKeyword(files);
      results.AddGroup(island);
    }
    Pvl temp;
    temp.AddObject(results);
    if (Filename(out).Exists()) {
      temp.Append(out);
    }
    else {
      temp.Write(out);
    }
    
  }

}
