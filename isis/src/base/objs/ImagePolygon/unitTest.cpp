#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "geos/geom/MultiPolygon.h"

using namespace std;
using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);

  /**
   * @brief Test ImagePolygon object for accuracy and correct behavior.
   *
   * @author 2005-11-22 Tracie Sucharski
   *
   * @history 2007-01-19  Tracie Sucharski, Removed ToGround method (for now)
   *          because of round off problems going back and forth between
   *          lat/lon,line/samp.
   * @history 2007-01-31  Tracie Sucharski,  Added WKT method to return polygon
   *                           in string as WKT.
   * @history 2007-11-09  Tracie Sucharski,  Remove WKT method, geos now has
   *                            a method to return a WKT string.
   * @history 2007-11-20  Tracie Sucharski,  Added test for sub-polys
  */

  //   simple MOC image
  string inFile = "$mgs/testData/ab102401.cub";

  // Open the cube
  Cube cube;
  Cube cube1;
  cube.Open(inFile, "r");

  ImagePolygon poly;
  try {
    poly.Create(cube);
  }
  catch(iException &e) {
    std::string msg = "Cannot create polygon for [" + cube.Filename() + "]";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }


  //  write poly as WKT
  ProgramLauncher::RunSystemCommand("echo \"" + poly.Polys()->toString() 
    + "\" | sed "
      "'s/\\([0-9][0-9]*\\.[0-9][0-9][0-9][0-9][0-9]\\)\\([0-9]*\\)/\\1/g'"
    );

  //  Test sub-poly option
  try {
    poly.Create(cube, 12, 1, 384, 640, 385);
  }
  catch(iException &e) {
    std::string msg = "Cannot create sub-polygon for [" + cube.Filename() + "]";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }

  //  write poly as WKT
  ProgramLauncher::RunSystemCommand("echo \"" + poly.Polys()->toString() 
    + "\" | sed "
      "'s/\\([0-9][0-9]*\\.[0-9][0-9][0-9][0-9][0-9][0-9]\\)\\([0-9]*\\)/\\1/g'"
    );


  //  Test lower quality option
  try {
    poly.Create(cube, 10, 12, 1, 384, 640, 385);
  }
  catch(iException &e) {
    std::string msg = "Cannot create lower quality polygon for [" +
        cube.Filename() + "]";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }
  //  write poly as WKT
  ProgramLauncher::RunSystemCommand("echo \"" + poly.Polys()->toString() 
    + "\" | sed "
      "'s/\\([0-9][0-9]*\\.[0-9][0-9][0-9][0-9][0-9][0-9]\\)\\([0-9]*\\)/\\1/g'"
    );

  cube.Close();
}

