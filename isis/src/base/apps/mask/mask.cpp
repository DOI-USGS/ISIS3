#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

void mask (vector<Buffer *> &in,
           vector<Buffer *> &out);

enum which_special {NONE, NULLP, ALL} spixels;
enum range_preserve {INSIDE,OUTSIDE} preserve;

double minimum,maximum;
bool masked;

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  masked= false;

  // Setup the input and output cubes
  UserInterface &ui = Application::GetUserInterface();
  
  if (!ui.WasEntered("MASK"))
    ui.PutFilename("MASK", ui.GetFilename("FROM"));

  printf("MASK=%s", ui.GetFilename("MASK").c_str());

  p.SetInputCube("FROM");
  p.SetInputCube("MASK",OneBand);
  p.SetOutputCube ("TO");

  
  //  Get min/max info
  minimum = VALID_MIN8;
  maximum = VALID_MAX8;
  if (ui.WasEntered ("MINIMUM")) minimum = ui.GetDouble ("MINIMUM");
  if (ui.WasEntered ("MAXIMUM")) maximum = ui.GetDouble ("MAXIMUM");

  //  Will we preserve inside or outside of min/max range
  preserve = INSIDE;
  string Preserve;
  if (ui.WasEntered ("PRESERVE")) Preserve = ui.GetString ("PRESERVE");
  if (Preserve == "OUTSIDE") preserve = OUTSIDE;

  //  How are special pixels handled?
  spixels = NULLP;
  string Spixels;
  if (ui.WasEntered ("SPIXELS")) Spixels = ui.GetString ("SPIXELS");
  if (Spixels == "NONE") spixels = NONE;
  if (Spixels == "ALL") spixels = ALL;

  // Start the processing
  p.StartProcess(mask);
  p.EndProcess();

  //  Report if no masking occurred
  if (!masked) {
    string message = "No mask was performed-the output file is the same as ";
    message += "the input file.";
    throw iException::Message(iException::User,message,_FILEINFO_);
  }

}

// Line processing routine
  void mask (vector<Buffer *> &in,
                  vector<Buffer *> &out) 
  {	

     Buffer &inp = *in[0];
     Buffer &mask = *in[1];
     Buffer &outp = *out[0];

      // Loop for each pixel in the line. 
     for (int i=0; i<inp.size(); i++) {
       if (IsSpecial (mask[i])) {
         if (spixels == ALL) {
           outp[i] = NULL8;
           masked = true;
         }
         else if (spixels == NULLP && mask[i] == NULL8) {
           outp[i] = NULL8;
           masked = true;
         }
         else {
           outp[i] = inp[i];
         }
       }
            
       else {
         if (preserve == INSIDE && (mask[i] >= minimum && mask[i] <= maximum))
           outp[i] = inp[i];
         else if (preserve == OUTSIDE && (mask[i] < minimum || mask[i] > maximum))
           outp[i] = inp[i];
         else {
           outp[i] = NULL8; 
           masked = true;          
         }         
       }
     }  

   }
