#include "Isis.h"
#include "Cube.h"
#include "Table.h"
#include "Camera.h"
#include "IException.h"
#include "CameraDetectorMap.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  try {
    // Open the cube
    Cube cube;
    cube.open(ui.GetFileName("FROM"), "rw");

    //check for existing polygon, if exists delete it
    if(cube.label()->HasObject("Polygon")) {
      cube.label()->DeleteObject("Polygon");
    }

    // Get the camera, interpolate to a parabola
    Camera *cam = cube.camera();
    if(cam->DetectorMap()->LineRate() == 0.0) {
      QString msg = "[" + ui.GetFileName("FROM") + "] is not a line scan camera";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    cam->instrumentRotation()->SetPolynomial();

    // Get the instrument pointing keyword from the kernels group and update
    // its value to table.
    Isis::PvlGroup kernels =
      cube.label()->FindGroup("Kernels", Isis::Pvl::Traverse);

    // Save original kernels in keyword before changing to "Table" in the kernels group
    PvlKeyword origCk = kernels["InstrumentPointing"];

    // Write out the "Table" label to the tabled kernels in the kernels group
    kernels["InstrumentPointing"] = "Table";

    // And finally write out the original kernels after Table
    for (int i = 0;  i < origCk.Size();  i++) {
      kernels["InstrumentPointing"].AddValue(origCk[i]);
    }

    cube.putGroup(kernels);

    // Pull out the pointing cache as a table and write it
    Table cmatrix = cam->instrumentRotation()->Cache("InstrumentPointing");
    cmatrix.Label().AddComment("Smoothed using spicefit");
    cube.write(cmatrix);
    cube.close();
  }
  catch(IException &e) {
    QString msg = "Unable to fit pointing for [" + ui.GetFileName("FROM") + "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }
}
