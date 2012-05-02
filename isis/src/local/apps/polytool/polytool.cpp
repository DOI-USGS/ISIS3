#include "Isis.h"

#include "PvlGroup.h"
#include "PvlKeyword.h"

#include <sstream>
#include <ostream>

#include "geos/io/WKTReader.h"

#include "PolygonTools.h"

using namespace std;
using namespace Isis;

geos::geom::Geometry *GetPolygon(std::string name);

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  std::string result;
  if(ui.GetString("PROCESS").compare("DESPIKE") == 0) {
    geos::geom::MultiPolygon *polygon1 = PolygonTools::MakeMultiPolygon(GetPolygon(ui.GetFileName("FROM1")));

    geos::geom::MultiPolygon *outpoly = PolygonTools::Despike(polygon1);

    result = outpoly->toString();
  }
  else if(ui.GetString("PROCESS").compare("DIFFERENCE") == 0) {
    geos::geom::Geometry *polygon1 = GetPolygon(ui.GetFileName("FROM1"));
    geos::geom::Geometry *polygon2 = GetPolygon(ui.GetFileName("FROM2"));

    geos::geom::Geometry *outgeom = PolygonTools::Difference(polygon1, polygon2);

    result = outgeom->toString();
  }
  else if(ui.GetString("PROCESS").compare("EQUAL") == 0) {
    geos::geom::Geometry *geom1 = GetPolygon(ui.GetFileName("FROM1"));
    geos::geom::MultiPolygon *polygon1 = PolygonTools::MakeMultiPolygon(geom1);

    geos::geom::Geometry *geom2 = GetPolygon(ui.GetFileName("FROM2"));
    geos::geom::MultiPolygon *polygon2 = PolygonTools::MakeMultiPolygon(geom2);

    PvlGroup grp("Results");
    if(PolygonTools::Equal(polygon1, polygon2)) {
      grp += PvlKeyword("Equal", "true");
    }
    else {
      grp += PvlKeyword("Equal", "false");
    }

    delete geom1;
    delete geom2;
    delete polygon1;
    delete polygon2;

    Application::Log(grp);
  }
  else if(ui.GetString("PROCESS").compare("INTERSECT") == 0) {
    geos::geom::Geometry *polygon1 = GetPolygon(ui.GetFileName("FROM1"));
    geos::geom::Geometry *polygon2 = GetPolygon(ui.GetFileName("FROM2"));

    geos::geom::Geometry *outgeom = PolygonTools::Intersect(polygon1, polygon2);

    result = outgeom->toString();
  }

  if(!result.empty()) {
    // Output the resultant polygon
    std::string outname = ui.GetFileName("TO");
    std::ofstream outfile;
    outfile.open(outname.c_str());
    outfile << result;
    outfile.close();
    if(outfile.fail()) {
      iString msg = "Unable to write the polygon to [" + outname + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }
}


/**
 * Grabs the first line of a file and attempts to create a Multipolygon to from
 * it.
 *
 * @param name The filename containing the multipolygon in its first line
 *
 * @return geos::geom::MultiPolygon*
 */
geos::geom::Geometry *GetPolygon(std::string name) {
  ifstream is;
  is.open(name.c_str());

  std::string fileData = "";
  while(is.good()) {
    fileData += is.get();
  }
  const std::string fileContents = fileData;

  geos::io::WKTReader geosReader;
  return geosReader.read(fileContents);
}
