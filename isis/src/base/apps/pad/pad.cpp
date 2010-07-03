#include "Isis.h"
#include "ProcessMosaic.h"
#include "ProcessByLine.h"
#include "AlphaCube.h"
#include "SpecialPixel.h"
#include "Projection.h"
#include "SubArea.h"

using namespace std; 
using namespace Isis;

void CreateBase (Buffer &buf);

void IsisMain() {
  // We will be use a mosaic technique so get the size of the input file
  ProcessMosaic p;
  Cube *icube = p.SetInputCube ("FROM");
  int ins = icube->Samples();
  int inl = icube->Lines();
  int inb = icube->Bands();

  // Retrieve the padding parameters
  UserInterface &ui = Application::GetUserInterface();
  int leftPad = ui.GetInteger("LEFT");
  int rightPad = ui.GetInteger("RIGHT");
  int topPad = ui.GetInteger("TOP");
  int bottomPad = ui.GetInteger("BOTTOM");

  // Compute the output size
  int ns = ins + leftPad + rightPad;
  int nl = inl + topPad + bottomPad;
  int nb = inb;

  // We need to create the output file
  ProcessByLine bl;
  bl.SetInputCube("FROM");  // Do this to match pixelType
  bl.SetOutputCube("TO",ns,nl,nb);
  bl.ClearInputCubes();     // Now get rid of it
  bl.Progress()->SetText("Creating pad");
  bl.StartProcess(CreateBase);
  bl.EndProcess();  

  // Place the input in the file we just created
  Cube *ocube = p.SetOutputCube ("TO");
  p.Progress()->SetText("Inserting cube");
  p.StartProcess(leftPad+1, topPad+1, 1, input);

  // Construct a label with the results
  PvlGroup results("Results");
  results += PvlKeyword ("InputLines", inl);
  results += PvlKeyword ("InputSamples", ins);
  results += PvlKeyword ("LeftPad", leftPad);
  results += PvlKeyword ("RightPad", rightPad);
  results += PvlKeyword ("TopPad", topPad);
  results += PvlKeyword ("BottomPad", bottomPad);
  results += PvlKeyword ("OutputLines", nl);
  results += PvlKeyword ("OutputSamples", ns);

  // Update the Mapping, Instrument, and AlphaCube groups in the
  // output cube label
  SubArea s;
  s.SetSubArea(inl,ins,1-topPad,1-leftPad,inl+bottomPad,ins+rightPad,1.0,1.0);
  s.UpdateLabel(icube,ocube,results);

  p.EndProcess();

  // Write the results to the log
  Application::Log(results);
}

void CreateBase (Buffer &buf) {
  for (int i=0; i<buf.size(); i++) {
    buf[i] = NULL8;
  }
}
