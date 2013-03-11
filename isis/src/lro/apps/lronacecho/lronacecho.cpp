#include "Isis.h"

#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void correct(Buffer &in, Buffer &out);

double g_delta;

bool g_isLeft, g_isSummed;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output files
  UserInterface &ui = Application::GetUserInterface();
  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  // Get the coefficients
  g_delta = ui.GetDouble("DELTA");

  Pvl lab(ui.GetFileName("FROM"));
  PvlGroup &inst = lab.FindGroup("Instrument", Pvl::Traverse);

  // Check if it is a NAC image
  QString instId = inst["InstrumentId"];
  if (instId != "NACL" && instId != "NACR") {
    string msg = "This is not a NAC image.  lrocnaccal requires a NAC image.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (instId == "NACL")
    g_isLeft = true;
  else
    g_isLeft = false;

  if ((int) inst["SpatialSumming"] == 1)
    g_isSummed = false;
  else
    g_isSummed = true;

  p.ProcessCube(correct, false);
}

// Unary routine
void correct(Buffer &in, Buffer &out) {
  // Loop for each pixel in the line
  int step = 2;
  if (g_isSummed)
    step = 1;

  if (g_isLeft) {
    out[0] = in[0];
    out[1] = in[1];
    for(int i = step; i < in.size(); i++) {
      if(IsSpecial(in[i]) || IsSpecial(in[i-step])) {
        out[i] = in[i];
      }
      else {
        out[i] = in[i] - g_delta * out[i-step];
      }
    }
  }
  else {
    out[in.size()-1] = in[in.size()-1];
    out[in.size()-2] = in[in.size()-2];
    for(int i = in.size() - 1 - step; i >= 0; i--) {
      if(IsSpecial(in[i]) || IsSpecial(in[i+step])) {
        out[i] = in[i];
      }
      else {
        out[i] = in[i] - g_delta * out[i+step];
      }
    }
  }
  //we then normalize the row
  for(int i = 0; i < out.size(); i++)
      out[i] = (out[i] * (1 + g_delta));
}
