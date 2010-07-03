#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

// Line processing routine
void trim (Buffer &in, Buffer &out);

int top,bottom,lleft,rright;
 
void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Setup the input and output cubes
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube ("TO");

  // Override the defaults if the user entered a value
  UserInterface &ui = Application::GetUserInterface();
  top = ui.GetInteger ("TOP");
  bottom = ui.GetInteger ("BOTTOM");
  lleft = ui.GetInteger ("LEFT");
  rright = ui.GetInteger ("RIGHT");

  //  Will anything be trimmed from the cube?
  bool notrim = false;
  if (top == 0 && bottom == 0 && lleft == 0 && rright == 0) {
    notrim = true;
  }

  //  Adjust bottom and right
  bottom = icube->Lines() - bottom;
  rright = icube->Samples() - rright;

  // Start the processing
  p.StartProcess(trim);
  p.EndProcess();

  //The user didn't trim anything
  if (notrim == true) {
    string message = "No trimming was done-output equals input file";
    throw iException::Message(iException::User,message,_FILEINFO_);
  }
	   
}


//  Line processing routine
  void trim (Buffer &in,Buffer &out) 
  {	

  //  Test for line trim and NULL full line
  if (in.Line () <= top || in.Line () > bottom) {
    for (int i=0; i<in.size (); i++) {
      out[i] = NULL8;
    }
  }
  else {
    //  Loop and test for sample trim    
    for (int i=0; i<in.size (); i++) {
      if (in.Sample (i) <= lleft || in.Sample (i) > rright) {
        out[i] = NULL8;
      }
      else {
        out[i] = in[i];
      }
    }
  }
}


