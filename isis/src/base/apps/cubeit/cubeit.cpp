#define GUIHELPERS

#include "Isis.h"
#include "ProcessByLine.h"
#include "ProcessMosaic.h"
#include "IException.h"
#include "FileList.h"
#include "TextFile.h"
#include "SpecialPixel.h"

using namespace std;
using namespace Isis;

void NullBand(Buffer &out);

//helper button function in the code
void helperButtonLog();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["helperButtonLog"] = (void *) helperButtonLog;
  return helper;
}

void IsisMain() {
  // Get the list of cubes to stack
  Process p;
  UserInterface &ui = Application::GetUserInterface();
  FileList cubeList(ui.GetFilename("FROMLIST"));

  // Loop through the list
  int nsamps(0), nlines(0), nbands(0);
  PvlGroup outBandBin("BandBin");
  try {
    for(unsigned int i = 0; i < cubeList.size(); i++) {
      Cube cube;
      CubeAttributeInput inatt(cubeList[i]);
      vector<string> bands = inatt.Bands();
      cube.setVirtualBands(bands);
      cube.open(cubeList[i]);
      if(i == 0) {
        nsamps = cube.getSampleCount();
        nlines = cube.getLineCount();
        nbands = cube.getBandCount();
      }
      else {
        // Make sure they are all the same size
        if((nsamps != cube.getSampleCount()) || (nlines != cube.getLineCount())) {
          string msg = "Spatial dimensions of cube [" +
                       cubeList[i] + "] does not match other cubes in list";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        // Get the total number of bands
        nbands += cube.getBandCount();
      }

      // Build up the band bin group
      PvlObject &isiscube = cube.getLabel()->FindObject("IsisCube");
      if(isiscube.HasGroup("BandBin")) {
        PvlGroup &inBandBin = isiscube.FindGroup("BandBin");
        for(int key = 0; key < inBandBin.Keywords(); key++) {
          PvlKeyword &inKey = inBandBin[key];
          if(!outBandBin.HasKeyword(inKey.Name())) {
            outBandBin += inKey;
          }
          else {
            PvlKeyword &outKey = outBandBin[inKey.Name()];
            for(int index = 0; index < (int)inKey.Size(); index++) {
              outKey.AddValue(inKey[index], inKey.Unit(index));
            }
          }
        }
      }
      cube.close();
    }
  }
  catch(IException &e) {
    string msg = "Invalid cube in list file [" + ui.GetFilename("FROMLIST") + "]";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  // Setup to propagate from the first input cube
  ProcessByLine p2;
  CubeAttributeInput inatt;

  int index = 0;
  if(ui.WasEntered("PROPLAB")) {
    bool match = false;
    string fname = (iString)ui.GetFilename("PROPLAB");
    for(int i = 0; i < (int)cubeList.size(); i++) {
      if(fname == Filename(cubeList[i]).Expanded()) {
        index = i;
        match = true;
        break;
      }
    }
    if(!match) {
      string msg = "Filename [" + ui.GetFilename("PROPLAB") +
                   "] to propagate labels from is not in the list file [" +
                   ui.GetFilename("FROMLIST") + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  p2.SetInputCube(cubeList[index], inatt);


  // Create the output cube
  Cube *ocube = p2.SetOutputCube("TO", nsamps, nlines, nbands);
  p2.ClearInputCubes();

  p2.Progress()->SetText("Allocating cube");
  p2.StartProcess(NullBand);

  // Add the band bin group if necessary
  if(outBandBin.Keywords() > 0) {
    ocube->putGroup(outBandBin);
  }
  p2.EndProcess();

  // Now loop and mosaic in each cube
  int sband = 1;
  for(unsigned int i = 0; i < cubeList.size(); i++) {
    ProcessMosaic m;
    m.SetBandBinMatch(false);

    Progress *prog = m.Progress();
    prog->SetText("Adding band " + iString((int)i + 1) +
                  " of " + iString(nbands));
    m.SetOutputCube("TO");
    CubeAttributeInput attrib(cubeList[i]);
    Cube *icube = m.SetInputCube(cubeList[i], attrib);
    m.SetPriority(ProcessMosaic::input);
    m.StartProcess(1, 1, sband);
    sband += icube->getBandCount();
    m.EndProcess();
  }
}

// Line processing routine
void NullBand(Buffer &out) {
  for(int i = 0; i < out.size(); i++) {
    out[i] = NULL8;
  }
}

//Helper function to output the input file to log.
void helperButtonLog() {
  UserInterface &ui = Application::GetUserInterface();
  string file(ui.GetFilename("FROMLIST"));
  TextFile text(file);
  string line;
  for(int i = 0; i < text.LineCount(); i++) {
    text.GetLine(line);
    Application::GuiLog(line);
  }
}
