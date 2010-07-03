#include "Isis.h"
#include "Cube.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "Filename.h"
#include "iException.h"
#include "Projection.h"
#include "AlphaCube.h"
#include "Table.h"
#include "SubArea.h"

using namespace std; 
using namespace Isis;

// Globals and prototypes
int ss,sl,sb;
int ns,nl,nb;
int sinc,linc;
Cube cube;
LineManager *in = NULL;

void crop (Buffer &out);

void IsisMain() {
  ProcessByLine p;

  // Open the input cube
  UserInterface &ui = Application::GetUserInterface();
  string from = ui.GetAsString("FROM");
  CubeAttributeInput inAtt(from);
  cube.SetVirtualBands(inAtt.Bands());
  from = ui.GetFilename("FROM");
  cube.Open(from);

  // Determine the sub-area to extract
  ss = ui.GetInteger("SAMPLE");
  sl = ui.GetInteger("LINE");
  sb = 1;

  int origns = cube.Samples();
  int orignl = cube.Lines();
  int es = cube.Samples();
  if (ui.WasEntered("NSAMPLES")) es = ui.GetInteger("NSAMPLES") + ss - 1;
  int el = cube.Lines();
  if (ui.WasEntered("NLINES")) el = ui.GetInteger("NLINES") + sl - 1;
  int eb = cube.Bands();

  sinc = ui.GetInteger("SINC");
  linc = ui.GetInteger("LINC");

  // Make sure starting positions fall within the cube
  if (ss > cube.Samples()) {
    cube.Close();
    string msg = "[SAMPLE] exceeds number of samples in the [FROM] cube";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  if (sl > cube.Lines()) {
    cube.Close();
    string msg = "[LINE] exceeds number of lines in the [FROM] cube";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Make sure the number of elements do not fall outside the cube
  if (es > cube.Samples()) {
    cube.Close();
    string msg = "[SAMPLE+NSAMPLES-1] exceeds number of ";
    msg += "samples in the [FROM] cube";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  if (el > cube.Lines()) {
    cube.Close();
    string msg = "[LINE+NLINES-1] exceeds number of ";
    msg += "lines in the [FROM] cube";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Determine the size of the output cube and then update the image size
  ns = (es - ss + 1) / sinc;
  nl = (el - sl + 1) / linc;
  nb = eb;
  if (ns == 0) ns = 1;
  if (nl == 0) nl = 1;
  es = ss + (ns - 1) * sinc;
  el = sl + (nl - 1) * linc;

  // Allocate the output file and make sure things get propogated nicely
  p.SetInputCube("FROM");
  p.PropagateTables(false);
  Cube *ocube = p.SetOutputCube ("TO",ns,nl,nb);
  p.ClearInputCubes();

  // propagate tables manually
  Pvl &inLabels = *cube.Label();

  // Loop through the labels looking for object = Table
  for(int labelObj = 0; labelObj < inLabels.Objects(); labelObj++) {
    PvlObject &obj = inLabels.Object(labelObj);

    if(obj.Name() != "Table") continue;

    // If we're not propagating spice data, dont propagate the following tables...
    if(!ui.GetBoolean("PROPSPICE")) {
      if((iString)obj["Name"][0] == "InstrumentPointing") continue;
      if((iString)obj["Name"][0] == "InstrumentPosition") continue;
      if((iString)obj["Name"][0] == "BodyRotation") continue;
      if((iString)obj["Name"][0] == "SunPosition") continue;
    }

    // Read the table into a table object
    Table table(obj["Name"], from);

    // We are not going to bother with line/sample associations; they apply
    //   only to the alpha cube at this time. I'm leaving this code here for the
    //   equations in case we try our hand at modifying these tables at a later date.

    /* Deal with associations, sample first
    if(table.IsSampleAssociated()) {
      int numDeleted = 0;
      for(int samp = 0; samp < cube.Samples(); samp++) {
        // This tests checks to see if we would include this sample. 
        //   samp - (ss-1)) / sinc must be a whole number less than ns.
        if((samp - (ss-1)) % sinc != 0 || (samp - (ss-1)) / sinc >= ns || (samp - (ss-1)) < 0) {
          table.Delete(samp-numDeleted);
          numDeleted ++;
        }
      }
    }

    // Deal with line association
    if(table.IsLineAssociated()) {
      int numDeleted = 0;
      for(int line = 0; line < cube.Lines(); line++) {
        // This tests checks to see if we would include this line. 
        //   line - (sl-1)) / linc must be a whole number less than nl.
        if((line - (sl-1)) % linc != 0 || (line - (sl-1)) / linc >= nl || (line - (sl-1)) < 0) {
          table.Delete(line-numDeleted);
          numDeleted ++;
        }
      }
    }*/

    // Write the table
    ocube->Write(table);
  }

  Pvl &outLabels = *ocube->Label();
  if(!ui.GetBoolean("PROPSPICE") && outLabels.FindObject("IsisCube").HasGroup("Kernels")) {
    PvlGroup &kerns = outLabels.FindObject("IsisCube").FindGroup("Kernels");

    string tryKey = "NaifIkCode";
    if(kerns.HasKeyword("NaifFrameCode")) {
      tryKey = "NaifFrameCode";
    }

    if(kerns.HasKeyword(tryKey)) {
      PvlKeyword ikCode = kerns[tryKey];
      kerns = PvlGroup("Kernels");
      kerns += ikCode;
    }
  }

  // Create a buffer for reading the input cube
  in = new LineManager(cube);

  // Crop the input cube
  p.StartProcess(crop);

  delete in;
  in = NULL;

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword ("InputLines", orignl);
  results += PvlKeyword ("InputSamples", origns);
  results += PvlKeyword ("StartingLine", sl);
  results += PvlKeyword ("StartingSample", ss);
  results += PvlKeyword ("EndingLine", el);
  results += PvlKeyword ("EndingSample", es);
  results += PvlKeyword ("LineIncrement", linc);
  results += PvlKeyword ("SampleIncrement", sinc);
  results += PvlKeyword ("OutputLines", nl);
  results += PvlKeyword ("OutputSamples", ns);

  // Update the Mapping, Instrument, and AlphaCube groups in the output
  // cube label 
  SubArea s;
  s.SetSubArea(orignl,origns,sl,ss,el,es,linc,sinc);
  s.UpdateLabel(&cube,ocube,results);

  // Cleanup
  p.EndProcess();
  cube.Close();
 
  // Write the results to the log
  Application::Log(results);
}

// Line processing routine
void crop (Buffer &out) {
  // Read the input line
  int iline = sl + (out.Line() - 1) * linc;
  in->SetLine(iline,sb);
  cube.Read(*in);

  // Loop and move appropriate samples
  for (int i=0; i<out.size(); i++) {
    out[i] = (*in)[(ss - 1) + i * sinc];
  }

  if (out.Line() == nl) sb++;
}
