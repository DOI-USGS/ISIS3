#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "Filename.h"
#include "iException.h"
#include "Table.h"

using namespace std;
using namespace Isis;

// Globals and prototypes
Cube cube;
LineManager *in;

Table hifix("HiRISE Ancillary");
Table calfix("HiRISE Calibration Ancillary");
Table calimg("HiRISE Calibration Image");

bool flip;
void glob (Buffer &out);

void IsisMain() {
  ProcessByLine p;

  // Initialize the output size
  int samples = 0;
  int lines = 0;
  int bands = 0;

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  string from = ui.GetFilename("FROM");
  cube.Open(from);

  samples = cube.Samples();
  lines = cube.Lines();
  bands = cube.Bands();

  // Get a cube packet to the input file
  Cube *icube = p.SetInputCube("FROM");

  // Get the cube prefix and suffix table
  icube->Read(hifix);

  // Get the calibration prefix and suffix table
  icube->Read(calfix);

  // Get the calibration image table
  icube->Read(calimg);

  // Add the number of buffer pixels and dark pixels to the ouput NS
  samples += hifix[0]["BufferPixels"].Size() + hifix[0]["DarkPixels"].Size();

  // Add the number of lines in the calibration image to the output NL
  lines += calimg.Records();

  // Decide if the calibration and observation data should be flipped.
  flip = false;
  PvlGroup &ins = icube->GetGroup("Instrument");
  int chan = ins["ChannelNumber"];

  iString flipChan = ui.GetString("FLIP");
  if (flipChan.UpCase() != "NONE") {
    if (flipChan.ToInteger() == chan) flip = true;
  }

  // Allocate the output file and make sure things get propogated nicely
  p.PropagateTables(false);
  p.SetOutputCube ("TO",samples,lines,bands);
  p.ClearInputCubes();

  // Create a buffer for reading the input cube
  in = new LineManager(cube);

  // Crop the input cube
  p.StartProcess(glob);

  // Cleanup
  p.EndProcess();
  delete in;
  cube.Close();
}

// Line processing routine
void glob (Buffer &out) {
  double int2ToDouble (int value);

  // Transfer the calibration buffer, image, and dark pixels
  if (out.Line() <= calimg.Records()) {
    int outPos = 0;

    // Buffer pixels
    vector<int> buf = calfix[out.Line()-1]["BufferPixels"];
    for (int i=0; i<(int)buf.size(); i++) {
      out[outPos++] = int2ToDouble(buf[i]);
    }

    // Main calibration pixels
    vector<int> cal = calimg[out.Line()-1]["Calibration"];
    if (flip) {
      for (int i=(int)cal.size()-1; i>=0; i--) {
        out[outPos++] = int2ToDouble(cal[i]);
      }
    }
    else {
      for (int i=0; i<(int)cal.size(); i++) {
        out[outPos++] = int2ToDouble(cal[i]);
      }
    }

    // Dark pixels
    vector<int> dark = calfix[out.Line()-1]["DarkPixels"];
    for (int i=0; i<(int)dark.size(); i++) {
      out[outPos++] = int2ToDouble(dark[i]);
    }

  } // End calibration if

  // We are done with the calibration lines so now
  // transfer the image buffer, observation, and dark pixels
  else {
    int outPos = 0;

    // Buffer pixels
    vector<int> buf = hifix[out.Line()-calimg.Records()-1]["BufferPixels"];
    for (int i=0; i<(int)buf.size(); i++) {
      out[outPos++] = int2ToDouble(buf[i]);
    }

    // Main observation pixels
    in->SetLine(out.Line()-calimg.Records(), 1);
    cube.Read(*in);

    if (flip) {
      for (int i=in->size()-1; i>=0; i--) {
        out[outPos++] = (*in)[i];
      }
    }
    else {
      for (int i=0; i<in->size(); i++) {
        out[outPos++] = (*in)[i];
      }
    }

    // Dark pixels
    vector<int> dark = hifix[out.Line()-calimg.Records()-1]["DarkPixels"];
    for (int i=0; i<(int)dark.size(); i++) {
      out[outPos++] = int2ToDouble(dark[i]);
    }

  } // End main image else
} // End function

double int2ToDouble (int value) {
  if (value == NULL2) return NULL8;
  else if (value == LOW_REPR_SAT2) return LOW_REPR_SAT8;
  else if (value == LOW_INSTR_SAT2) return LOW_INSTR_SAT8;
  else if (value == HIGH_INSTR_SAT2) return HIGH_INSTR_SAT8;
  else if (value == HIGH_REPR_SAT2) return HIGH_REPR_SAT8;
  else return value;

}

