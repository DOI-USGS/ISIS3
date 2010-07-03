#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void Deriv (Buffer &in, double &v);

void IsisMain() {
 
  ProcessByBoxcar p;

  UserInterface &ui = Application::GetUserInterface();

  //  Open the input cube
  p.SetInputCube ("FROM");

  //  Allocate the output cube
  p.SetOutputCube ("TO");

  //  Which deriv (horizontal or vetical)?
  string derivDir = ui.GetString ("DIRECTION");
  //  Set boxcar size depending on the derivative direction
  if (derivDir == "HORZ") {
    p.SetBoxcarSize (2,1);
  }
  else if (derivDir == "VERT") {
    p.SetBoxcarSize (1,2);
  }

  p.StartProcess (Deriv);
  p.EndProcess ();

}

//  Derivative process
void Deriv (Buffer &in,double &v) {
  if (IsSpecial(in[0]) || IsSpecial(in[1])) {
    v = Isis::Null;
    return;
  }
  v = in[0] - in[1];

}
