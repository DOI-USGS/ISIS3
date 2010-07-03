#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Filename.h"

using namespace std; 
using namespace Isis;

// Global declarations
void isis2ascii (Buffer &in);
ofstream fout;

void IsisMain() {
  // Create a process by line object
  ProcessByLine p;

  // Get the size of the cube
  Cube *icube = p.SetInputCube("FROM");

  //  Open output text file
  UserInterface &ui = Application::GetUserInterface();
  string to = ui.GetFilename("TO","txt");
  fout.open (to.c_str());

  // Print header if needed
  if (ui.GetBoolean("HEADER")) {
    fout << "Input Cube:  " << icube->Filename () << endl;
    fout << "Samples:Lines:Bands:  " << icube->Samples () << ":" <<
            icube->Lines () << ":" << icube->Bands () << endl;
  }
    
  // List the cube
  p.StartProcess(isis2ascii);
  p.EndProcess();

  fout.close ();
}

void isis2ascii (Buffer &in) {
  for (int i=0; i<in.size(); i++) {
    fout.width(13);        //  Width must be set everytime
    if (IsSpecial (in[i])) {
      if (IsNullPixel (in[i])) fout << "NULL";
      if (IsHrsPixel (in[i])) fout << "HRS";
      if (IsHisPixel (in[i])) fout << "HIS";
      if (IsLrsPixel (in[i])) fout << "LRS";
      if (IsLisPixel (in[i])) fout << "LIS";
    }
    else {
      fout << in[i];
    }
  }
  fout << endl;
    
}
