#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace std; 
using namespace Isis;

void SumLineSample (Buffer &in, Buffer &out);
void LineNumber (Buffer &in, Buffer &out);
void SampleNumber (Buffer &in, Buffer &out);
void CheckerBoard (Buffer &in, Buffer &out);

int size;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  UserInterface &ui= Application::GetUserInterface();

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube ("TO");
  
  // Start the processing
  string option = ui.GetString("OPTION");
  if (option == "GRADIENT") {
    p.StartProcess(SumLineSample);
  }
  if (option == "LINEWEDGE") {
    p.StartProcess(LineNumber);
  }
  if (option == "SAMPLEWEDGE") {
    p.StartProcess(SampleNumber);
  }
  if (option == "CHECKERBOARD") {
    if(ui.WasEntered("SIZE")){
      size = ui.GetInteger("SIZE");
    }
    else {
      size = 5;
    }
    p.StartProcess(CheckerBoard);
  }

  // End the processing
  p.EndProcess();
}

// Line processing routines
void SumLineSample (Buffer &in, Buffer &out) {
  for (int i=0; i<in.size(); i++) {
    int sum = in.Sample(i)+in.Line();
    out[i]=sum;
  }
}
void LineNumber (Buffer &in, Buffer &out) { 
  for (int i=0; i<in.size(); i++) {
    out[i]=in.Line();
  }
}
void SampleNumber (Buffer &in, Buffer &out) {
  for (int i=0; i<in.size(); i++) {
    out[i]=in.Sample(i);
  }
}
void CheckerBoard (Buffer &in, Buffer &out) {
   for (int i=0; i<in.size(); i++) {
    if ((i%(2*size)>=size && in.Line()%(2*size)>=size) ||
         (i%(2*size)<size && in.Line()%(2*size)<size)) {
      out[i]=0;
    }
    else {
      out[i]=255;
    }
  }
}
