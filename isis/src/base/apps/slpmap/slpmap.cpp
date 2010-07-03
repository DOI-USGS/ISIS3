#include <cmath>
#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "Camera.h"
#include "Projection.h"

using namespace std; 
using namespace Isis;

bool degrees;

double resolution[2];

void createSlpCube (Buffer &in, double &v);
void createAspectCube (Buffer &in, double &v);

void IsisMain() {
  ProcessByBoxcar p;

  UserInterface &ui = Application::GetUserInterface();

  degrees = ui.GetBoolean("DEGREES");

  // as ProcessByBoxcar only allows one input cube and one
  // output cube, two seperate process must be done
  Cube *icube = p.SetInputCube ("FROM");
  p.SetBoxcarSize( 3, 3 );
  p.SetOutputCube ("TO");

  if(ui.GetString("OUTPUT") == "SLOPE" ) {
    // For slope we need a resolution
    // Try to use the camera first
    try {
      Camera *cam = icube->Camera();

      // Really we should be doing this at every point in the image... but for now,
      //   the center will work.
      if(!cam->SetImage(icube->Samples()/2,icube->Lines()/2)) {
        // Get into the catch(...)
        throw iException::Message(iException::Programmer,"",_FILEINFO_);
      }

      // Convert resolution to the DN value's unit (easier)
      resolution[0] = cam->SampleResolution() / ui.GetDouble("CONVERSION");;
      resolution[1] = cam->LineResolution()   / ui.GetDouble("CONVERSION");;
    }
    catch (iException &e) {
      // Failed at getting the camera, reset our exception and try again with the projection
      e.Clear();

      Projection *proj = icube->Projection();

      if(!proj->SetWorld(icube->Samples()/2,icube->Lines()/2)) {
        iString message = "Failed to SetWorld at the center of the image";
        throw iException::Message(iException::Programmer,message,_FILEINFO_);
      }

      // Convert resolution to the DN value's unit (easier)
      resolution[0] = proj->Resolution() / ui.GetDouble("CONVERSION");
      resolution[1] = proj->Resolution() / ui.GetDouble("CONVERSION");
    }

    p.StartProcess (createSlpCube);
  } 
  else {
    p.StartProcess (createAspectCube);
  }

  p.EndProcess();
}


/**
 * http://webhelp.esri.com/arcgisdesktop/9.3/index.cfm?TopicName=How%20Slope%20(3D%20Analyst)%20works 
 *  
 *  "Conceptually, the Slope function fits a plane to the z-values of a 3 x 3
 *  cell neighborhood around the processing or center cell. The slope value of
 *  this plane is calculated using the average maximum technique (see
 *  References). The direction the plane faces is the aspect for the processing
 *  cell. The lower the slope value, the flatter the terrain; the higher the
 *  slope value, the steeper the terrain."
 * 
 * @param in Input cube data (3x3 matrix)
 * @param v Output value
 */
void createSlpCube( Buffer &in, double &v ) {
  const double &a = in[0];
  const double &b = in[1];
  const double &c = in[2];
  const double &d = in[3];
  //const double &e = in[4];
  const double &f = in[5];
  const double &g = in[6];
  const double &h = in[7];
  const double &i = in[8];

  // If anything we're actually calculating with is special, fail
  if(Isis::IsSpecial(a) || 
     Isis::IsSpecial(b) || 
     Isis::IsSpecial(c) || 
     Isis::IsSpecial(f) || 
     Isis::IsSpecial(g) || 
     Isis::IsSpecial(h) || 
     Isis::IsSpecial(i)) {
    v = Isis::Null;
    return;
  }

  // [dz/dx] = ((c + 2f + i) - (a + 2d + g)) / (8 * x_cell_size)
  double changeInX = ((c + 2*f + i) - (a + 2*d + g)) / (8 * resolution[0]);

  // [dz/dy] = ((g + 2h + i) - (a + 2b + c)) / (8 * y_cell_size)
  double changeInY = ((g + 2*h + i) - (a + 2*b + c)) / (8 * resolution[0]);

  double changeMag = sqrt(changeInX*changeInX + changeInY*changeInY);

  double slopeRadians = atan(changeMag);
  
  if(!degrees) {
    v = slopeRadians;
  }
  else {
    v = slopeRadians * 180.0 / Isis::PI;
  }
}


/**
 * http://webhelp.esri.com/arcgisdesktop/9.3/index.cfm?TopicName=How%20Aspect%20(3D%20Analyst)%20works
 *  
 *  "Conceptually, the Aspect function fits a plane to the z-values of a 3 x 3
 *  cell neighborhood around the processing or center cell. The direction the
 *  plane faces is the aspect for the processing cell."
 *  
 * @param in Input cube data (3x3 matrix)
 * @param v Output value
 */
void createAspectCube( Buffer &in, double &v ) {
  const double &a = in[0];
  const double &b = in[1];
  const double &c = in[2];
  const double &d = in[3];
  //const double &e = in[4];
  const double &f = in[5];
  const double &g = in[6];
  const double &h = in[7];
  const double &i = in[8];

  // If anything we're actually calculating with is special, fail
  if(Isis::IsSpecial(a) || 
     Isis::IsSpecial(b) || 
     Isis::IsSpecial(c) || 
     Isis::IsSpecial(f) || 
     Isis::IsSpecial(g) || 
     Isis::IsSpecial(h) || 
     Isis::IsSpecial(i)) {
    v = Isis::Null;
    return;
  }

  // [dz/dx] = ((c + 2f + i) - (a + 2d + g)) / 8
  double changeInX = ((c + 2*f + i) - (a + 2*d + g)) / 8;

  // [dz/dy] = ((g + 2h + i) - (a + 2b + c)) / 8
  double changeInY = ((g + 2*h + i) - (a + 2*b + c)) / 8;

  // aspect = 57.29578 * atan2 ([dz/dy], -[dz/dx]) = in degrees
  double aspectRadians = atan2(changeInY, -changeInX);


  /*
    The aspect value is then converted to compass direction values (0-360 degrees), according to the following rule:
    if aspect < 0
      cell = 90.0 - aspect
    else if aspect > 90.0
      cell = 360.0 - aspect + 90.0
    else
      cell = 90.0 - aspect 
  */

  if(aspectRadians < 0) {
    v = Isis::PI / 2.0 - aspectRadians;
  }
  else if(aspectRadians > Isis::PI / 2.0) {
    v = Isis::PI * 2.0 - aspectRadians + Isis::PI / 2.0;
  }
  else {
    v = Isis::PI / 2.0 - aspectRadians;
  }
  
  if(!degrees) {
    v = aspectRadians;
  }
  else {
    v = aspectRadians * 180.0 / Isis::PI;
  }

}
