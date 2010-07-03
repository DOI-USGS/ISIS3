#include "Isis.h"
#include "SpecialPixel.h"
#include "CubeAttribute.h"
#include "Cube.h"
#include "LineManager.h"
#include "Progress.h"

using namespace Isis;
using namespace std;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();

  CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
  Cube icube;

  if(inAtt.Bands().size() != 0) {
    icube.SetVirtualBands(inAtt.Bands());
  }

  icube.Open(Filename(ui.GetFilename("FROM")).Expanded());

  // Make sure it is a Themis EDR/RDR
  Filename inFilename = ui.GetFilename("FROM");
  try {
    if (icube.GetGroup("Instrument")["InstrumentID"][0] != "THEMIS_VIS") {
      string msg = "This program is intended for use on THEMIS VIS images only. [";
      msg += inFilename.Expanded() + "] does not appear to be a THEMIS VIS image.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }
  catch (iException &e) {
      string msg = "This program is intended for use on THEMIS VIS images only. [";
      msg += inFilename.Expanded() + "] does not appear to be a THEMIS VIS image.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  vector<Cube *> flatcubes;
  vector<LineManager *> fcubeMgrs;
  int summing = icube.GetGroup("Instrument")["SpatialSumming"][0];

  for(int filt = 0; filt < 5; filt++) {
    string filePattern = "$odyssey/calibration/flat_filter_";
    filePattern += iString(filt+1) + "_summing_";
    filePattern += iString(summing) + "_v????.cub";
    Filename flatFile(filePattern);
    flatFile.HighestVersion();
    Cube *fcube = new Cube();
    fcube->Open(flatFile.Expanded());
    flatcubes.push_back(fcube);

    LineManager *fcubeMgr = new LineManager(*fcube);
    fcubeMgr->SetLine(1,1);
    fcubeMgrs.push_back(fcubeMgr);
  }

  Cube ocube;

  CubeAttributeOutput outAtt = ui.GetOutputAttribute("TO");
  ocube.SetDimensions(icube.Samples(), icube.Lines(), icube.Bands());
  ocube.SetByteOrder(outAtt.ByteOrder());
  ocube.SetCubeFormat(outAtt.FileFormat());
  if(outAtt.DetachedLabel()) ocube.SetDetached();
  if(outAtt.AttachedLabel()) ocube.SetAttached();
  ocube.SetPixelType(outAtt.PixelType());

  ocube.Create(Filename(ui.GetFilename("TO")).Expanded());

  LineManager icubeMgr(icube);
  vector<int> filter;

  PvlKeyword &filtNums = icube.Label()->FindGroup("BandBin", Pvl::Traverse)["FilterNumber"];;
  for(int i = 0; i < filtNums.Size(); i++) {
    filter.push_back(filtNums[i]);
  }

  LineManager ocubeMgr(ocube);
  ocubeMgr.SetLine(1,1);

  Progress prog;
  prog.SetText("Applying Flat-Field Correction");
  prog.SetMaximumSteps(ocube.Lines() * ocube.Bands());
  prog.CheckStatus();

  do {
    icube.Read(icubeMgr);
    ocube.Read(ocubeMgr);

    int fcubeIndex = filter[ocubeMgr.Band()-1] - 1;
    flatcubes[fcubeIndex]->Read((*fcubeMgrs[fcubeIndex]));

    for(int i = 0; i < ocubeMgr.size(); i++) {
      if(IsSpecial((*fcubeMgrs[fcubeIndex])[i]) || (*fcubeMgrs[fcubeIndex])[i] == 0.0) {
        ocubeMgr[i] = Isis::Null;
      }
      else if(IsSpecial(icubeMgr[i])) {
        ocubeMgr[i] = icubeMgr[i];
      }
      else {
        ocubeMgr[i] = icubeMgr[i] / (*fcubeMgrs[fcubeIndex])[i];
      }
    }

    ocube.Write(ocubeMgr);

    icubeMgr++;
    ocubeMgr++;

    for(int i = 0; i < (int)fcubeMgrs.size(); i++) {
      (*fcubeMgrs[i]) ++;

      if(fcubeMgrs[i]->end()) {
        fcubeMgrs[i]->SetLine(1,1);
      }
    }

    prog.CheckStatus();
  }
  while(!ocubeMgr.end());

  // Propagate labels and objects (in case of spice data)
  PvlObject &inCubeObj = icube.Label()->FindObject("IsisCube");
  PvlObject &outCubeObj = ocube.Label()->FindObject("IsisCube");

  for(int g = 0; g < inCubeObj.Groups(); g++) {
    outCubeObj.AddGroup(inCubeObj.Group(g));
  }

  for(int o = 0; o < icube.Label()->Objects(); o++) {
    if(icube.Label()->Object(o).IsNamed("Table")) {
      Blob t(icube.Label()->Object(o)["Name"], icube.Label()->Object(o).Name());
      icube.Read(t);
      ocube.Write(t);
    }
  }

  icube.Close();
  ocube.Close();

  for(int i = 0; i < (int)flatcubes.size(); i++) {
    delete fcubeMgrs[i];
    delete flatcubes[i];
  }

  fcubeMgrs.clear();
  flatcubes.clear();
}

