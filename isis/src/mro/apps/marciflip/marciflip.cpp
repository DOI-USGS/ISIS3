#include "Isis.h"
#include "ProcessImportPds.h"
#include "FileName.h"
#include "Brick.h"
#include "ProcessByBrick.h"
#include "OriginalLabel.h"
#include "IException.h"

using namespace std;
using namespace Isis;

Isis::Cube *outputCube = NULL;
int currentLine;
int filterHeight = 16;

void flipCube(Isis::Buffer &data);

void IsisMain() {
  ProcessByBrick p;

  Cube *icube = p.SetInputCube("FROM");

  filterHeight = 16 / (int)icube->getGroup("Instrument")["SummingMode"];
  p.SetBrickSize(icube->getSampleCount(), filterHeight, icube->getBandCount());
  currentLine = icube->getLineCount();

  UserInterface &ui = Application::GetUserInterface();
  outputCube = new Isis::Cube();
  outputCube->setDimensions(icube->getSampleCount(), icube->getLineCount(), icube->getBandCount());
  outputCube->create(ui.GetFileName("TO"));

  if(icube->hasGroup("Instrument")) {
    PvlGroup inst = icube->getGroup("Instrument");

    // change flipped keyword
    inst["DataFlipped"] = ((int)inst["DataFlipped"] + 1) % 2;

    outputCube->getLabel()->FindObject("IsisCube").AddGroup(inst);
  }

  if(icube->hasGroup("BandBin")) {
    outputCube->getLabel()->FindObject("IsisCube").AddGroup(
        icube->getGroup("BandBin"));
  }

  if(icube->getLabel()->HasObject("OriginalLabel")) {
    OriginalLabel origLabel;
    icube->read(origLabel);
    outputCube->write(origLabel);
  }

  p.StartProcess(flipCube);
  p.EndProcess();

  outputCube->close();
  delete outputCube;
}

void flipCube(Isis::Buffer &data) {
  currentLine -= filterHeight;
  Brick outBrick(data.SampleDimension(), data.LineDimension(), data.BandDimension(), data.PixelType());
  outBrick.Copy(data);
  outBrick.SetBasePosition(1, currentLine + 1, data.Band());
  outputCube->write(outBrick);
}
