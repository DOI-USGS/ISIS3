#include "Isis.h"
#include "ProcessMosaic.h"
#include "AlphaCube.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessMosaic p;
  p.SetBandBinMatch(false);

  // Set the input cube for the process object
  // ihumphrey -- SetInputCube explicitly uses parameters for ProcessMosaic
  Cube *icube = p.SetInputCube("FROM", 1, 1, 1, -1, -1, -1);

  // Set up the mosaic priority
  UserInterface &ui = Application::GetUserInterface();
  QString combineMethod = ui.GetString("COMBINE");
  ProcessMosaic::ImageOverlay priority;
  if(combineMethod == "PARENT") {
    priority = ProcessMosaic::PlaceImagesBeneath;
  }
  else {
    priority = ProcessMosaic::PlaceImagesOnTop;
  }

  // Get the extraction label from the input file
  AlphaCube acube(*icube);
  int outSample = (int)(acube.AlphaSample(0.5) + 0.5);
  int outLine = (int)(acube.AlphaLine(0.5) + 0.5);
  int outBand = 1;

  p.SetOutputCube("PARENT");
  p.SetImageOverlay(priority);
  p.StartProcess(outSample, outLine, outBand);
  p.EndProcess();
}

