#include <cmath>
#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "Constants.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

double pixelResolution;
double saz;
double zenith;

void shade (Buffer &in, double &v);

void IsisMain() {
 
  const double DEG2RAD = PI/180.0;
  ProcessByBoxcar p;

  UserInterface &ui = Application::GetUserInterface();

  // Open the input cube
  Cube *inCube = p.SetInputCube ("FROM");

  // Allocate the output cube
  p.SetOutputCube ("TO");

  p.SetBoxcarSize(3,3);

  // Read user parameters
  // This parameter as used by the algorithm is 0-360 with 0 at 3 o'clock, 
  // increasing in the clockwise direction. The value taken in from the user is
  // 0-360 with 0 at 12 o'clock increasing in the clockwise direction.
  saz = ui.GetDouble ("AZIMUTH");
  saz += 270;
  if (saz > 360) saz -= 360;
  saz *= DEG2RAD;
  
  // This parameter as used by the algorithm is 0-90 as the angle to the light source,
  // this means 90 is at the horizon and 0 is directly overhead.  
  zenith = ui.GetDouble ("ZENITH");
  zenith *= DEG2RAD;

  // Get from labels or from user
  if (ui.WasEntered("PIXELRESOL")) {
    pixelResolution = ui.GetDouble ("PIXELRESOL");
  }
  else {
    if (inCube->Label()->FindObject("IsisCube").HasGroup("Mapping")) {
      pixelResolution = inCube->Label()->FindObject("IsisCube").FindGroup("Mapping")["PixelResolution"];
    }
    else {
      string msg = "The file [" + ui.GetFilename("FROM") + "] does not have a mapping group,"
                   + " you must enter a Pixel Resolution";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  p.StartProcess (shade);
  p.EndProcess ();

}

// Shade processing routine
void shade (Buffer &in,double &v) {

  // first we just check to make sure that we don't have any special pixels 
  bool specials = false;
  for (int i=0; i<in.size(); ++i){
      if (IsSpecial(in[i])){
          specials = true;
      }
  }
 
  // if we have any special pixels we bail out (well ok just set to null)
  if( specials ) {
      v = Isis::Null;
  } else {
      /* if we have a 3x3 section that doesn't have an special pixels we hit that 3x3
      / against two gradients

        [-1 0 1 ]   [-1 -1 -1 ]
        [-1 0 1 ]   [ 0  0  0 ]
        [-1 0 1 ]   [ 1  1  1 ]

        These two gradients are not special in any way aside from that they are 
        are orthogonal. they can be replaced if someone has reason as long as they
        stay orthogonal to eachoher. (for those that don't know orthoginal means
        for matrix A: A * transpose(A) = I where I is the identity matrix).

      */

      double p = ( (-1 )*in[0]+( 0 )*in[1]+( 1 )*in[2] 
                  +(-1 )*in[3]+( 0 )*in[4]+( 1 )*in[5]
                  +(-1 )*in[6]+( 0 )*in[7]+( 1 )*in[8]) / ( 3.0 * pixelResolution );

      double q = ( (-1 )*in[0]+(-1 )*in[1]+(-1 )*in[2] 
                  +( 0 )*in[3]+( 0 )*in[4]+( 0 )*in[5]
                  +( 1 )*in[6]+( 1 )*in[7]+( 1 )*in[8]) / ( 3.0 * pixelResolution);
      
      /* after we hit them by the gradients the formula in order to make the shade is

                            1 + p0*p + q0*q
      shade =  -------------------------------------------------
                 sqrt(1 + p*p + Q*q) + sqrt(1 + p0*p0 + q0*q0)

      where p0 = -cos( sun azimuth ) * tan( solar elevation ) and
            q0 = -sin( sun azimuth ) * tan( solar elevation )

      and p and q are the two orthogonal gradients of the data (calculated above)

      this equation comes from 
      Horn, B.K.P. (1982). "Hill shading and the reflectance map". Geo-processing, v. 2, no. 1, p. 65-146.

      It is avaliable online at http://people.csail.mit.edu/bkph/papers/Hill-Shading.pdf
      */
      double p0 = -cos(saz) * tan(zenith);
      double q0 = -sin(saz) * tan(zenith);

      double numerator = 1.0 + p0*p + q0*q;

      double denominator = sqrt(1 + p*p + q*q) + sqrt(1 + p0*p0 + q0*q0);

      v = numerator/denominator;

  }
}
