#include "Isis.h"

#include <iostream>

#include "geos/geom/MultiPolygon.h"
#include "UserInterface.h"
#include "Filename.h"
#include "Cube.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"

using namespace std;
using namespace Isis;

void IsisMain(){


  UserInterface &ui = Application::GetUserInterface();

  Cube cube;
  cube.Open(ui.GetFilename("FROM"));
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
  if (ui.WasEntered("TO")) {
    outfile = ui.GetFilename("TO");
  }
  else {
    Filename inputFile = ui.GetFilename("FROM");
    inputFile.RemoveExtension();
    inputFile.AddExtension("gml");
    outfile = inputFile.Name();
  }
  fout.open (outfile.c_str());

  fout << polyString << endl;

  //      fout.open(Filename(outfile).Expanded().c_str(),ios::out);
  fout.close();



}



