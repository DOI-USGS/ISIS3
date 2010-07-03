#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void robertGradient (Buffer &in, double &v);
void sobelGradient (Buffer &in, double &v);

void IsisMain() {
 
  ProcessByBoxcar p;

  UserInterface &ui = Application::GetUserInterface();

  //  Open the input cube
  p.SetInputCube ("FROM");

  //  Allocate the output cube
  p.SetOutputCube ("TO");

  //  Which gradient?
  string gradType = ui.GetString ("GRADTYPE");
  //  Set boxcar size depending on the gradient type
  if (gradType == "SOBEL") {
    p.SetBoxcarSize (3,3);
    p.StartProcess (sobelGradient);
  }
  else if (gradType == "ROBERTS") {
    p.SetBoxcarSize (2,2);
    p.StartProcess (robertGradient);
  }

  p.EndProcess ();

}

//  Sobel gradient filter
void sobelGradient (Buffer &in,double &v) {

  bool specials = false;
  for (int i=0; i<in.size(); ++i){
    if (IsSpecial(in[i])){
      specials = true;
    }
  } 
  if(specials){
    v = Isis::Null;
    return;
  }
  v = abs( (in[0]+2*in[1]+in[2]) - (in[6]+2*in[7]+in[8]) ) +
    abs( (in[2]+2*in[5]+in[8]) - (in[0]+2*in[3]+in[6]) );

}


//   Roberts gradient filter
void robertGradient (Buffer &in,double &v) {
  bool specials = false;
  for (int i=0; i<in.size(); ++i){
    if (IsSpecial(in[i])){
      specials = true;
    }
  } 
  if(specials){
    v = Isis::Null;
    return;
  }
  v = abs (in[0] - in[3]) + abs (in[1] - in[2]);

}
