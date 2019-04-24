#include "Isis.h"

#include <QFile>

#include "Application.h"
#include "ProcessByLine.h"
#include "ProgramLauncher.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void difference(vector<Buffer *> &input, vector<Buffer *> &output);

void IsisMain() {
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  UserInterface &ui = Application::GetUserInterface();
  int highLines, highSamples, lowLines, lowSamples;

  // Get the boxcar sizes to be used by the low and highpass filters
  // All numbers have to be odd.  If nothing is entered into the UI,
  // NS and/or NL are used.
  if(ui.GetString("MODE") == "VERTICAL") {
    if(ui.WasEntered("VHNS")) {
      highSamples = ui.GetInteger("VHNS");
    }
    else {
      highSamples = icube->sampleCount();
      if(highSamples % 2 == 0) highSamples -= 1;
    }

    if(ui.WasEntered("VLNL")) {
      lowLines = ui.GetInteger("VLNL");
    }
    else {
      lowLines = icube->lineCount();
      if(lowLines % 2 == 0) lowLines -= 1;
    }

    lowSamples = ui.GetInteger("VLNS");
    highLines = ui.GetInteger("VHNL");
  }
  else {
    if(ui.WasEntered("HHNL")) {
      highLines = ui.GetInteger("HHNL");
    }
    else {
      highLines = icube->lineCount();
      if(highLines % 2 == 0) highLines -= 1;
    }

    if(ui.WasEntered("HLNS")) {
      lowSamples = ui.GetInteger("HLNS");
    }
    else {
      lowSamples = icube->sampleCount();
      if(lowSamples % 2 == 0) lowSamples -= 1;
    }

    highSamples = ui.GetInteger("HHNS");
    lowLines = ui.GetInteger("HLNL");
  }

  // Algorithm: lowpass(from, temp) -> hipass(temp, noise) -> to = from-noise

  // Run lowpass filter on input
  QString tempFileName = FileName::createTempFile("$TEMPORARY/dstripe.temporary.cub").expanded();
  QString lowParams = "";
  lowParams += "from= " + ui.GetFileName("FROM");
  lowParams += " to= " + tempFileName + " ";
  lowParams += " samples= " + toString(lowSamples);
  lowParams += " lines= " + toString(lowLines);

  ProgramLauncher::RunIsisProgram("lowpass", lowParams);
  
  // Make a copy of the lowpass filter results if the user wants it
  if(!ui.GetBoolean("DELETENOISE")) {
    QString lowParams = "";
    lowParams += "from= " + ui.GetFileName("FROM");
    lowParams += " to= " + ui.GetFileName("LPFNOISE");
    lowParams += " samples= " + toString(lowSamples);
    lowParams += " lines= " + toString(lowLines);
    ProgramLauncher::RunIsisProgram("lowpass", lowParams);
  }

  // Run highpass filter after lowpass is done, i.e. highpass(lowpass(input))
  QString tempNoiseFileName = FileName::createTempFile("$TEMPORARY/dstripe.noise.temporary.cub").expanded();
  QString highParams = "";
  highParams += " from= "+tempFileName + " ";
  highParams += " to= " + tempNoiseFileName + " ";
  highParams += " samples= " + toString(highSamples);
  highParams += " lines= " + toString(highLines);

  ProgramLauncher::RunIsisProgram("highpass", highParams);
  QFile::remove(tempFileName);

  // Take the difference (FROM-NOISE) and write it to output
  CubeAttributeInput att;
  p.SetInputCube(tempNoiseFileName, att);
  p.SetOutputCube("TO");
  p.StartProcess(difference);
  p.EndProcess();
  if(ui.GetBoolean("DELETENOISE")) {
    QFile::remove(tempNoiseFileName);
  }
}

// Subtracts noise from the input buffer, resulting in a cleaner output image
void difference(vector<Buffer *> &input, vector<Buffer *> &output) {
  Buffer &from = *input[0];
  Buffer &noise = *input[1];
  Buffer &to = *output[0];

  for(int i = 0; i < from.size(); i++) {
    if(IsSpecial(from[i]) || IsSpecial(noise[i])) {
      to[i] = from[i];
    }
    else {
      to[i] = from[i] - noise[i];
    }
  }
}
