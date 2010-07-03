#include "Isis.h"
#include "Process.h"
#include "BundleAdjust.h"
#include "Table.h"
#include "iException.h"
#include "Sensor.h"
#include "History.h"

using namespace std;
using namespace Isis;

double GetRadius(std::string filename, double lat, double lon);

void IsisMain() {
  // Create a serial number list
  UserInterface &ui = Application::GetUserInterface();
  string filename = ui.GetFilename("FROM");
  SerialNumberList serialNumberList;
  serialNumberList.Add(filename);

  // Get the coordinate for updating the camera pointing
  // We will want to make the camera pointing match the lat/lon at this
  // line sample
  double samp1 = ui.GetDouble("SAMP1");
  double line1 = ui.GetDouble("LINE1");
  double lat1 = ui.GetDouble("LAT1");
  double lon1 = ui.GetDouble("LON1");
  double rad1;
  if (ui.WasEntered("RAD1")) {
    rad1 = ui.GetDouble("RAD1");
  }
  else {
    rad1 = GetRadius(ui.GetFilename("FROM"),lat1,lon1);
  }

  // In order to use the bundle adjustment class we will need a control
  // network
  ControlMeasure m;
  m.SetCubeSerialNumber(serialNumberList.SerialNumber(0));
  m.SetCoordinate(samp1,line1);
  m.SetType(ControlMeasure::Manual);

  ControlPoint p;
  p.SetUniversalGround(lat1,lon1,rad1);
  p.SetId("Point1");
  p.SetType(ControlPoint::Ground);
  p.Add(m);

  ControlNet cnet;
  cnet.SetType(ControlNet::ImageToGround);
  cnet.Add(p);

  // See if they wanted to solve for twist
  if (ui.GetBoolean("TWIST")) {
    double samp2 = ui.GetDouble("SAMP2");
    double line2 = ui.GetDouble("LINE2");
    double lat2 = ui.GetDouble("LAT2");
    double lon2 = ui.GetDouble("LON2");
    double rad2;
    if (ui.WasEntered("RAD2")) {
      rad2 = ui.GetDouble("RAD2");
    }
    else {
      rad2 = GetRadius(ui.GetFilename("FROM"),lat2,lon2);
    }

    ControlMeasure m;
    m.SetCubeSerialNumber(serialNumberList.SerialNumber(0));
    m.SetCoordinate(samp2,line2);
    m.SetType(ControlMeasure::Manual);

    ControlPoint p;
    p.SetUniversalGround(lat2,lon2,rad2);
    p.SetId("Point2");
    p.SetType(ControlPoint::Ground);
    p.Add(m);

    cnet.Add(p);
  }

  // Bundle adjust to solve for new pointing
  try {
    BundleAdjust b(cnet,serialNumberList);
    b.SetSolveTwist(ui.GetBoolean("TWIST"));
    double tol = ui.GetDouble("TOL");
    int maxIterations = ui.GetInteger("MAXITS");
    b.Solve(tol,maxIterations);

    Cube c;
    c.Open(filename,"rw");

    //check for existing polygon, if exists delete it
    if (c.Label()->HasObject("Polygon")){
      c.Label()->DeleteObject("Polygon");
    }

    Table cmatrix = b.Cmatrix(0);

    // Write out a description in the spice table
    //PvlKeyword description("Description");
    //description = "Camera pointing updated via deltack application";
    //cmatrix.Label().FindObject("Table",Pvl::Traverse).AddKeyword(description);

    // Update the cube history
    c.Write(cmatrix);
    History h("IsisCube");
    c.Read(h);
    h.AddEntry();
    c.Write(h);
    c.Close();
    PvlGroup gp( "DeltackResults" );
    gp += PvlKeyword("Status","Camera pointing updated");
    Application::Log( gp );
  }
  catch (iException &e) {
    string msg = "Unable to update camera pointing for [" + filename + "]";
    throw iException::Message(Isis::iException::Camera,msg,_FILEINFO_);
  }

}

// Compute the radius at the lat/lon
double GetRadius(std::string filename, double lat, double lon) {
  Pvl lab(filename);
  Sensor sensor(lab);
  sensor.SetUniversalGround(lat,lon);
  double radius = sensor.LocalRadius();
  if (IsSpecial(radius)) {
    string msg = "Could not determine radius from DEM at lat/lon [";
    msg += iString(lat) + "," + iString(lon) + "]";
    throw iException::Message(Isis::iException::Camera,msg,_FILEINFO_);
  }
  return radius;
}

