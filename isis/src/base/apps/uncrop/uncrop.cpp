#include "Isis.h"
#include "ProcessMosaic.h"
#include "AlphaCube.h"

using namespace std; 
using namespace Isis;

void IsisMain() {
  ProcessMosaic p;
  p.SetBandBinMatch(false);

  // Set the input cube for the process object
  p.SetInputCube ("FROM");

  // Set up the mosaic priority
  UserInterface &ui = Application::GetUserInterface();
  string combineMethod = ui.GetString ("COMBINE");
  MosaicPriority priority;
  if (combineMethod == "PARENT") {
    priority = mosaic;
  }
  else {
    priority = input;
  }

  // Get the extraction label from the input file
  Pvl lab;
  lab.Read(ui.GetFilename("FROM"));
  AlphaCube acube(lab);
  int outSample = (int) (acube.AlphaSample(0.5) + 0.5);
  int outLine = (int) (acube.AlphaLine(0.5) + 0.5);
  int outBand = 1; 

  p.SetOutputCube ("PARENT");

  p.StartProcess(outSample, outLine, outBand, priority);
  p.EndProcess();
}

