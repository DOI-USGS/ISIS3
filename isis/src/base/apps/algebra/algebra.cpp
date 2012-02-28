#include "Isis.h"
#include "ProcessByBrick.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"

using namespace std;
using namespace Isis;

void add(vector<Buffer *> &in,
         vector<Buffer *> &out);
void sub(vector<Buffer *> &in,
         vector<Buffer *> &out);
void mult(vector<Buffer *> &in,
          vector<Buffer *> &out);
void divide(vector<Buffer *> &in,
         vector<Buffer *> &out);
void unaryFunction(Buffer &in, Buffer &out);

double Isisa, Isisb, Isisc, Isisd, Isise;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output files
  UserInterface &ui = Application::GetUserInterface();
  // Setup the input and output cubes
  p.SetInputCube("FROM");
  if(ui.WasEntered("FROM2")) p.SetInputCube("FROM2");
  p.SetOutputCube("TO");

  // Get the coefficients
  Isisa = ui.GetDouble("A");
  Isisb = ui.GetDouble("B");
  Isisc = ui.GetDouble("C");
  Isisd = ui.GetDouble("D");
  Isise = ui.GetDouble("E");

  // Start the processing based on the operator
  string op = ui.GetString("OPERATOR");
  if(op == "ADD") p.ProcessCubes(&add);
  if(op == "SUBTRACT") p.ProcessCubes(&sub);
  if(op == "MULTIPLY") p.ProcessCubes(&mult);
  if(op == "DIVIDE") p.ProcessCubes(&divide);
  if(op == "UNARY") p.ProcessCube(&unaryFunction);
}

// Add routine
void add(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inp1 = *in[0];
  Buffer &inp2 = *in[1];
  Buffer &outp = *out[0];
  // Loop for each pixel in the line.
  for(int i = 0; i < inp1.size(); i++) {
    if(IsSpecial(inp1[i])) {
      outp[i] = inp1[i];
    }
    else if(IsSpecial(inp2[i])) {
      outp[i] = NULL8;
    }
    else {
      outp[i] = ((inp1[i] - Isisd) * Isisa) + ((inp2[i] - Isise) * Isisb) + Isisc;
    }
  }
}

// Sub routine
void sub(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inp1 = *in[0];
  Buffer &inp2 = *in[1];
  Buffer &outp = *out[0];

  // Loop for each pixel in the line.
  for(int i = 0; i < inp1.size(); i++) {
    if(IsSpecial(inp1[i])) {
      outp[i] = inp1[i];
    }
    else if(IsSpecial(inp2[i])) {
      outp[i] = NULL8;
    }
    else {
      outp[i] = ((inp1[i] - Isisd) * Isisa) - ((inp2[i] - Isise) * Isisb) + Isisc;
    }
  }
}

// Sub routine
void mult(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inp1 = *in[0];
  Buffer &inp2 = *in[1];
  Buffer &outp = *out[0];

  // Loop for each pixel in the line.
  for(int i = 0; i < inp1.size(); i++) {
    if(IsSpecial(inp1[i])) {
      outp[i] = inp1[i];
    }
    else if(IsSpecial(inp2[i])) {
      outp[i] = NULL8;
    }
    else {
      outp[i] = ((inp1[i] - Isisd) * Isisa) * ((inp2[i] - Isise) * Isisb) + Isisc;
    }
  }
}

// Div routine
void divide(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inp1 = *in[0];
  Buffer &inp2 = *in[1];
  Buffer &outp = *out[0];

  // Loop for each pixel in the line.
  for(int i = 0; i < inp1.size(); i++) {
    if(IsSpecial(inp1[i])) {
      outp[i] = inp1[i];
    }
    else if(IsSpecial(inp2[i])) {
      outp[i] = NULL8;
    }
    else {
      if((inp2[i] - Isise) * Isisb  == 0.0) {
        outp[i] = NULL8;
      }
      else {
        outp[i] = ((inp1[i] - Isisd) * Isisa) / ((inp2[i] - Isise) * Isisb) + Isisc;
      }
    }
  }
}

// Unary routine
void unaryFunction(Buffer &in, Buffer &out) {
  // Loop for each pixel in the line.
  for(int i = 0; i < in.size(); i++) {
    if(IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = in[i] * Isisa + Isisc;
    }
  }
}
