#include "Isis.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "CameraDistortionMap.h"
#include "Cube.h"
#include "Progress.h"
#include "iException.h"
#include "ControlNet.h"
#include "SerialNumber.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "Brick.h"

using namespace std;
using namespace Isis;

struct Coordinate {
  double line;         //!< Old line
  double samp;         //!< Old sample
  double errLine;      //!< Coorected line
  double errSamp;      //!< Corrected sample
  double olddetX;      //!< Old detector sample coordinate
  double olddetY;      //!< Old detector line coordinate
  double newdetX;      //!< Correct detector sample coordinate
  double newdetY;      //!< Correct detector line coordinate
  double gof;          //!< Goodness of fit
  double latitude;     //!< Latitude of the point
  double longitude;    //!< Longitude of the point
};

inline double distance(const double &x1, const double &y1, 
                       const double &x2, const double &y2) {
  return (sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
}


void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Open the FROM cube. It must have a camera model associated with it
  Cube from;
  CubeAttributeInput &attFrom = ui.GetInputAttribute("FROM");
  vector<string> bandFrom = attFrom.Bands();
  from.SetVirtualBands(bandFrom);
  from.Open(ui.GetFilename("FROM"), "r");
  Camera *fcamera = from.Camera();
  CameraDistortionMap *dmap = fcamera->DistortionMap();


  // Initialize control point network
  ControlNet cn(ui.GetFilename("CNETFILE"));
  bool checkForNulls = ui.GetBoolean("CHECKDN");

  double dtol = ui.GetDouble("TOLERANCE");
  double pp = fcamera->PixelPitch();
  double mmTol = fabs(pp * dtol);

  Progress prog;
  prog.SetMaximumSteps(cn.Size());
  prog.CheckStatus();

  // Loop through grid of points and get statistics to compute
  // translation values
  std::vector<Coordinate> coords;
  Brick pixel(from, 1, 1, 1);
  BigInt badPoint(0), nulls(0), oldNotInImage(0), newNotInImage(0), badTol(0);
  for(int p = 0; p < cn.Size(); p++) {
    ControlPoint pnt = cn[p];
    if((!pnt.Ignore()) && (pnt.Size() == 2)) {
      Coordinate c;
      if (pnt[0].Type() == ControlMeasure::Reference) {
        c.samp = pnt[0].Sample();
        c.line = pnt[0].Line();
        c.errSamp = pnt[1].Sample();
        c.errLine = pnt[1].Line();
        c.gof = 0;//pnt[1].GoodnessOfFit();
      }
      else {
        c.samp = pnt[1].Sample();
        c.line = pnt[1].Line();
        c.errSamp = pnt[0].Sample();
        c.errLine = pnt[0].Line();
        c.gof = 0;//pnt[0].GoodnessOfFit();

      }

      //  Check for valid point if requested
      bool isGood(true);
      if (checkForNulls) {
        pixel.SetBasePosition((int) c.samp, (int) c.line, 1);
        from.Read(pixel);
        isGood = !IsSpecial(pixel[0]);
      }

      if (isGood) {
        if (fcamera->SetImage(c.samp, c.line)) {
          c.latitude = fcamera->UniversalLatitude();
          c.longitude = fcamera->UniversalLongitude();
          double o_ux = dmap->UndistortedFocalPlaneX();
          double o_uy = dmap->UndistortedFocalPlaneY();
          double o_dx = dmap->FocalPlaneX();
          double o_dy = dmap->FocalPlaneY();
          if (fcamera->SetImage(c.errSamp, c.errLine)) {
            double c_ux = dmap->UndistortedFocalPlaneX();
            double c_uy = dmap->UndistortedFocalPlaneY();
            double c_dx = dmap->FocalPlaneX();
            double c_dy = dmap->FocalPlaneY();

            double ddist = distance(o_dx, o_dy, c_dx, c_dy);
            double udist = distance(o_ux, o_uy, c_ux, c_uy);

            if ((ddist <= mmTol) && (udist <= mmTol)) {
              c.olddetX = o_dx;
              c.olddetY = o_dy;
              c.newdetX = c_ux;
              c.newdetY = c_uy;
              coords.push_back(c);
            }
            else badTol++;
          }
          else newNotInImage++;
        }
        else oldNotInImage++;
      }
      else nulls++;
    }
    else {
       badPoint++; 
    }
    prog.CheckStatus();
  }

  PvlGroup results("Results");
  results += PvlKeyword("PixelPitch", pp, "millimeters");  
  results += PvlKeyword("TotalPoints", cn.Size());  
  results += PvlKeyword("ValidPoints", (BigInt) coords.size());
  results += PvlKeyword("InvalidPoints", badPoint);
  if (checkForNulls) results += PvlKeyword("NullDNs", nulls);
  results += PvlKeyword("OldPointNotInImage", oldNotInImage);
  results += PvlKeyword("NewPointNotInImage", newNotInImage);
  results += PvlKeyword("ToleranceExceeded", badTol);

  // Log it
  Application::Log(results);

  // Don't need the cubes opened anymore
  from.Close();

  // If flatfile was entered, create the flatfile
  // The flatfile is comma seperated and can be imported into an excel
  // spreadsheet
  if(ui.WasEntered("TO")) {
    string fFile = Filename(ui.GetFilename("TO")).Expanded();
    ofstream os;
    os.open(fFile.c_str(), ios::out);
    os << "OldSample,OldLine,NewSample,NewLine," <<
       "X,Y,XC,YC,"<<
       "GoodnessOfFit,Latitude,Longitude" << endl;
    for(unsigned int i = 0; i < coords.size(); i++) {
      Coordinate &c = coords[i];
      os << c.samp << "," << c.line << "," << c.errSamp << "," << c.errLine << ","
         << c.olddetX << "," << c.olddetY << "," << c.newdetX << "," << c.newdetY << "," 
         << c.gof << "," << c.latitude << "," << c.longitude << endl;

    }
  }

  return;
}

