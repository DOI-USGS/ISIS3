#include <cmath>

#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void robertGradient(Buffer &in, double &v);
void robertGradientApprox(Buffer &in, double &v);
void sobelGradient(Buffer &in, double &v);
void sobelGradientApprox(Buffer &in, double &v);

void IsisMain() {

  ProcessByBoxcar p;

  UserInterface &ui = Application::GetUserInterface();

  //  Open the input cube
  p.SetInputCube("FROM");

  //  Allocate the output cube
  p.SetOutputCube("TO");

  //  Which computation?
  QString method = ui.GetString("METHOD");

  //  Which gradient?
  QString gradType = ui.GetString("GRADTYPE");

  //  Set boxcar size depending on the gradient type
  if (gradType == "SOBEL") {
    p.SetBoxcarSize(3, 3);
    if (method == "EXACT") {
      p.StartProcess(sobelGradient);
    }
    else { // APPROXIMATE
      p.StartProcess(sobelGradientApprox);
    }
  }

  else { // ROBERTS
    p.SetBoxcarSize(2, 2);
    if (method == "EXACT") {
      p.StartProcess(robertGradient);
    }
    else { // APPROXIMATE
      p.StartProcess(robertGradientApprox);
    }
  }

  p.EndProcess();

}


//  Sobel gradient filter
void sobelGradient(Buffer &in, double &v) {

  bool specials = false;
  for(int i = 0; i < in.size(); ++i) {
    if(IsSpecial(in[i])) {
      specials = true;
    }
  }
  if(specials) {
    v = Isis::Null;
    return;
  }
  v = sqrt(
        pow( (in[2] + 2 * in[5] + in[8]) - (in[0] + 2 * in[3] + in[6]), 2 ) +
        pow( (in[0] + 2 * in[1] + in[2]) - (in[6] + 2 * in[7] + in[8]), 2 )
      );

}


//  Sobel approximate gradient filter
void sobelGradientApprox(Buffer &in, double &v) {

  bool specials = false;
  for(int i = 0; i < in.size(); ++i) {
    if(IsSpecial(in[i])) {
      specials = true;
    }
  }
  if(specials) {
    v = Isis::Null;
    return;
  }
  v = abs((in[2] + 2 * in[5] + in[8]) - (in[0] + 2 * in[3] + in[6])) +
      abs((in[0] + 2 * in[1] + in[2]) - (in[6] + 2 * in[7] + in[8])); 
 
}


//   Roberts gradient filter
void robertGradient(Buffer &in, double &v) {

  bool specials = false;
  for(int i = 0; i < in.size(); ++i) {
    if(IsSpecial(in[i])) {
      specials = true;
    }
  }
  if(specials) {
    v = Isis::Null;
    return;
  }
  v = sqrt( pow((in[0] - in[3]), 2) + pow((in[1] - in[2]), 2) );

}


//  Roberts approximate gradient filter
void robertGradientApprox(Buffer &in, double &v) {

  bool specials = false;
  for(int i = 0; i < in.size(); ++i) {
    if(IsSpecial(in[i])) {
      specials = true;
    }
  }
  if(specials) {
    v = Isis::Null;
    return;
  }
  v = abs(in[0] - in[3]) + abs(in[1] - in[2]);

}
