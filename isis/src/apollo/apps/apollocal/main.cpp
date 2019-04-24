#include "Isis.h"

#include "CubeAttribute.h"
#include "ProcessByTile.h"
#include "Pvl.h"
#include "PvlTranslationTable.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void cal(vector<Buffer *> &in, 
         vector<Buffer *> &out);

void IsisMain() {
  // We will be processing by line
  ProcessByTile p;
  p.SetTileSize(128, 128);

  Cube *inCube = p.SetInputCube("FROM");

  PvlGroup &dataDir =
      Preference::Preferences().findGroup("DataDirectory");
  PvlTranslationTable tTable(
      (QString)p.MissionData("base", "translations/MissionName2DataDir.trn"));
  QString missionDir = dataDir[tTable.Translate("MissionName",
      (inCube->group("Instrument")).findKeyword("SpacecraftName")[0])][0];
  QString camera =
      (inCube->group("Instrument")).findKeyword("InstrumentId")[0];

  CubeAttributeInput cai;
  p.SetInputCube(missionDir + "/calibration/" + camera + "_flatfield.cub", cai);

  CubeAttributeOutput cao;
  cao.setPixelType(Real);
  p.SetOutputCube(
      FileName(Application::GetUserInterface().GetAsString("TO")).expanded(),
      cao, inCube->sampleCount(), inCube->lineCount(),
      inCube->bandCount());

  p.StartProcess(cal);
  p.EndProcess();
}


void cal(vector<Buffer *> &in, vector<Buffer *> &out) {
  Buffer &inp = *in[0];      // Input Cube
  Buffer &fff = *in[1];      // Flat Field File
  Buffer &outp = *out[0];    // Output Cube

  
  // Loop for each pixel in the line.
  for (int i=0; i<inp.size(); i++) {
    if (IsSpecial(inp[i])) {
      outp[i] = inp[i];
    }
    else if (IsSpecial(fff[i])) {
      outp[i] = Null;
    }
    else {
      // Log Filter the film negative (and multiply by 2^16/16 to maintain the range of values)
      outp[i] = 65535.0*(1.0 - log(65536 - inp[i])/log(2.0)/16.0);
      outp[i] -=  1300.0;    // subtract dark current
      outp[i] /= fff[i];    // divide flat field to remove vignetting effects
    }
  }
}
