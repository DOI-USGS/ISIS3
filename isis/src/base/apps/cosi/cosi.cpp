#include <cmath>

#include "Isis.h"
#include "ProcessByLine.h"
#include "Camera.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

Camera *cam;
double maxinc;
void divide(Buffer &in, Buffer &out);

void IsisMain () {

  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  cam = icube->Camera();
  maxinc = Application::GetUserInterface().GetDouble("MAXINC");
  p.SetOutputCube("TO");
  p.StartProcess(divide);
  p.EndProcess();
}

void divide(Buffer &in, Buffer &out) {
  for (int i=0; i<in.size(); i++) {
    if (cam->SetImage(i+1,in.Line())) {
      if (IsSpecial(in[i])) {
        out[i] = in[i];
      }
      else {
        double incidence = cam->IncidenceAngle();
        if (abs(incidence) >= maxinc) {
          out[i] = Isis::Null;
        }
        else {
          out[i] = in[i] / cos(incidence*Isis::PI/180.0);
        }
      }
    }
    else {
      out[i] = Isis::Null;
    }
  }
}
