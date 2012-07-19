#include "Isis.h"
#include "ProcessMosaic.h"
#include "AlphaCube.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessMosaic p;
  p.SetBandBinMatch(false);

  // Set the input cube for the process object
  p.SetInputCube("FROM");

  // Set up the mosaic priority
  UserInterface &ui = Application::GetUserInterface();
  string combineMethod = ui.GetString("COMBINE");
  ProcessMosaic::ImageOverlay priority;
  if(combineMethod == "PARENT") {
    priority = ProcessMosaic::PlaceImagesBeneath;
  }
  else {
    priority = ProcessMosaic::PlaceImagesOnTop;
  }

  // Get the extraction label from the input file
  Pvl lab;
  lab.Read(ui.GetFileName("FROM"));
  AlphaCube acube(lab);
  int outSample = (int)(acube.AlphaSample(0.5) + 0.5);
  int outLine = (int)(acube.AlphaLine(0.5) + 0.5);
  int outBand = 1;

  p.SetOutputCube("PARENT");
  p.SetImageOverlay(priority);
  p.StartProcess(outSample, outLine, outBand);
  p.EndProcess();
}

