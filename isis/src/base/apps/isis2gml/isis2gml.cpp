#include "Isis.h"

#include <iostream>

#include "geos/geom/MultiPolygon.h"
#include "UserInterface.h"
#include "FileName.h"
#include "Cube.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"

using namespace std;
using namespace Isis;

void IsisMain() {


  UserInterface &ui = Application::GetUserInterface();

  Cube cube;
  cube.open(ui.GetFileName("FROM"));
  ImagePolygon poly;
  poly.Create(cube);

  geos::geom::MultiPolygon *mPolygon = poly.Polys();

  std::string polyString;

  if(ui.WasEntered("LABEL")) {
    string fid = ui.GetString("LABEL");
    polyString = PolygonTools::ToGML(mPolygon, fid);
  }
  else {
    polyString = PolygonTools::ToGML(mPolygon);
  }

  string outfile;
  ofstream fout;
  if(ui.WasEntered("TO")) {
    outfile = ui.GetFileName("TO");
  }
  else {
    FileName inputFile = ui.GetFileName("FROM");
    inputFile.removeExtension();
    inputFile.addExtension("gml");
    outfile = inputFile.name();
  }
  fout.open(outfile.c_str());

  fout << polyString << endl;

  //      fout.open(FileName(outfile).expanded().c_str(),ios::out);
  fout.close();



}



