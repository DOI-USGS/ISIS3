/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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

  filterHeight = 16 / (int)icube->group("Instrument")["SummingMode"];
  p.SetBrickSize(icube->sampleCount(), filterHeight, icube->bandCount());
  currentLine = icube->lineCount();

  UserInterface &ui = Application::GetUserInterface();
  outputCube = new Isis::Cube();
  outputCube->setDimensions(icube->sampleCount(), icube->lineCount(), icube->bandCount());
  outputCube->create(ui.GetFileName("TO"));

  if(icube->hasGroup("Instrument")) {
    PvlGroup inst = icube->group("Instrument");

    // change flipped keyword
    inst["DataFlipped"] = toString(((int)inst["DataFlipped"] + 1) % 2);

    outputCube->label()->findObject("IsisCube").addGroup(inst);
  }

  if(icube->hasGroup("BandBin")) {
    outputCube->label()->findObject("IsisCube").addGroup(
        icube->group("BandBin"));
  }

  if(icube->label()->hasObject("OriginalLabel")) {
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
