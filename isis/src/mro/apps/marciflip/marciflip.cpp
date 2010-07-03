#include "Isis.h"
#include "ProcessImportPds.h"
#include "Filename.h"
#include "Brick.h"
#include "ProcessByBrick.h"
#include "OriginalLabel.h"
#include "iException.h"

using namespace std;
using namespace Isis;

Isis::Cube *outputCube = NULL;
int currentLine;
int filterHeight = 16;

void flipCube(Isis::Buffer &data);

void IsisMain (){
  ProcessByBrick p;

  Cube *icube = p.SetInputCube("FROM");

  filterHeight = 16 / (int)icube->GetGroup("Instrument")["SummingMode"];
  p.SetBrickSize(icube->Samples(), filterHeight, icube->Bands());
  currentLine = icube->Lines();

  UserInterface &ui = Application::GetUserInterface();
  outputCube = new Isis::Cube();
  outputCube->SetDimensions(icube->Samples(), icube->Lines(), icube->Bands());
  outputCube->Create(ui.GetFilename("TO"));

  if(icube->HasGroup("Instrument")) {
    PvlGroup inst = icube->GetGroup("Instrument");

    // change flipped keyword
    inst["DataFlipped"] = ((int)inst["DataFlipped"] + 1) % 2;

    outputCube->Label()->FindObject("IsisCube").AddGroup(inst);
  }

  if(icube->HasGroup("BandBin")) {
    outputCube->Label()->FindObject("IsisCube").AddGroup(icube->GetGroup("BandBin"));
  }

  if(icube->Label()->HasObject("OriginalLabel")) {
    OriginalLabel origLabel;
    icube->Read(origLabel);
    outputCube->Write(origLabel);
  }

  p.StartProcess(flipCube);
  p.EndProcess();

  outputCube->Close();
  delete outputCube;
}

void flipCube(Isis::Buffer &data) {
  currentLine -= filterHeight;
  Brick outBrick(data.SampleDimension(), data.LineDimension(), data.BandDimension(), data.PixelType());
  outBrick.Copy(data);
  outBrick.SetBasePosition(1, currentLine+1, data.Band());
  outputCube->Write(outBrick);
}
