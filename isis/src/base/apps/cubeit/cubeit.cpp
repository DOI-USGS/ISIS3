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

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["helperButtonLog"] = (void *) helperButtonLog;
  return helper;
}

void IsisMain() {
  // Get the list of cubes to stack
  Process p;
  UserInterface &ui = Application::GetUserInterface();
  FileList cubeList(ui.GetFileName("FROMLIST"));

  // Loop through the list
  int nsamps(0), nlines(0), nbands(0);
  PvlGroup outBandBin("BandBin");
  try {
    for(int i = 0; i < cubeList.size(); i++) {
      Cube cube;
      CubeAttributeInput inatt(cubeList[i].toString());
      vector<QString> bands = inatt.bands();
      cube.setVirtualBands(bands);
      cube.open(cubeList[i].toString());
      if(i == 0) {
        nsamps = cube.sampleCount();
        nlines = cube.lineCount();
        nbands = cube.bandCount();
      }
      else {
        // Make sure they are all the same size
        if((nsamps != cube.sampleCount()) || (nlines != cube.lineCount())) {
          QString msg = "Spatial dimensions of cube [" +
                        cubeList[i].toString() + "] does not match other cubes in list";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        // Get the total number of bands
        nbands += cube.bandCount();
      }

      // Build up the band bin group
      PvlObject &isiscube = cube.label()->FindObject("IsisCube");
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
    QString msg = "Invalid cube in list file [" + ui.GetFileName("FROMLIST") + "]";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  // Setup to propagate from the first input cube
  ProcessByLine p2;
  CubeAttributeInput inatt;

  int index = 0;
  if(ui.WasEntered("PROPLAB")) {
    bool match = false;
    QString fname = ui.GetFileName("PROPLAB");
    for(int i = 0; i < cubeList.size(); i++) {
      if(fname == cubeList[i].toString()) {
        index = i;
        match = true;
        break;
      }
    }
    if(!match) {
      QString msg = "FileName [" + ui.GetFileName("PROPLAB") +
                    "] to propagate labels from is not in the list file [" +
                    ui.GetFileName("FROMLIST") + "]";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  p2.SetInputCube(cubeList[index].toString(), inatt);


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
  for(int i = 0; i < cubeList.size(); i++) {
    ProcessMosaic m;
    m.SetBandBinMatch(false);

    Progress *prog = m.Progress();
    prog->SetText("Adding band " + toString((int)i + 1) +
                  " of " + toString(nbands));
    m.SetOutputCube("TO");
    CubeAttributeInput attrib(cubeList[i].toString());
    Cube *icube = m.SetInputCube(cubeList[i].toString(), attrib);
    m.SetImageOverlay(ProcessMosaic::PlaceImagesOnTop);
    m.StartProcess(1, 1, sband);
    sband += icube->bandCount();
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
  QString file(ui.GetFileName("FROMLIST"));
  TextFile text(file);
  QString line;
  for(int i = 0; i < text.LineCount(); i++) {
    text.GetLine(line);
    Application::GuiLog(line);
  }
}
