#include "Isis.h"
#include "SpecialPixel.h"
#include "CubeAttribute.h"
#include "Cube.h"
#include "LineManager.h"
#include "Progress.h"
#include "Camera.h"
#include "Constants.h"
#include "Statistics.h"
#include "TextFile.h"
#include "Stretch.h"
#include "iTime.h"

using namespace Isis;
using namespace std;

Stretch stretch;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();

  CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
  Cube icube;

  if(inAtt.Bands().size() != 0) {
    icube.SetVirtualBands(inAtt.Bands());
  }

  icube.Open(Filename(ui.GetFilename("FROM")).Expanded());

  // Make sure it is a Marci cube
  Filename inFilename = ui.GetFilename("FROM");
  try {
    if (icube.GetGroup("Instrument")["InstrumentID"][0] != "Marci") {
      throw iException::Message(iException::User,"",_FILEINFO_);
    }

    if(!icube.GetGroup("Archive").HasKeyword("SampleBitModeId")) {
      throw iException::Message(iException::User,"",_FILEINFO_);
    }
  }
  catch (iException &e) {
    e.Clear();
    string msg = "This program is intended for use on MARCI images only. [";
    msg += inFilename.Expanded() + "] does not appear to be a MARCI image.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  if(icube.GetGroup("Archive")["SampleBitModeId"][0] != "SQROOT") {
    string msg = "Sample bit mode [" + icube.GetGroup("Archive")["SampleBitModeId"][0] + "] is not supported.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Read in calibration coefficients
  Filename calFile("$mro/calibration/marci/marciCoefficients_v???.pvl");
  calFile.HighestVersion();

  vector<double> decimation;

  // Decimation is described in the MRO MARCI Instrument and Calibration document pg. 63
  for(int i = 0; i < 6; i++) {
    // Decimation is 1.0 for bands 1-6
    decimation.push_back(1.0);
  }

  iString startTime = icube.Label()->FindGroup("Instrument", Pvl::Traverse)["StartTime"][0];
  iTime start(startTime);
  iTime changeTime("November 6, 2006 21:30:00 UTC");

  if(start < changeTime) {
    decimation.push_back(1.0);
  }
  else {
    decimation.push_back(0.25);
  }

  // Get the LUT data
  Filename temp("$mro/calibration/marcisqroot_???.lut");
  temp.HighestVersion();
  TextFile stretchPairs(temp.Expanded());

  // Create the stretch pairs
  stretch.ClearPairs();
  for (int i=0; i<stretchPairs.LineCount(); i++) {
    iString line;
    stretchPairs.GetLine(line,true);
    int temp1 = line.Token(" ");
    int temp2 = line.Trim(" ");
    stretch.AddPair(temp1,temp2);
  }

  stretchPairs.Close();

  // This file stores radiance/spectral distance coefficients
  Pvl calibrationData(calFile.Expanded());

  // This will store the radiance coefficient and solar spectral distance coefficients 
  // for each band.
  //   calibrationCoeffs[band].first gives the radiance coefficient
  //   calibrationCoeffs[band].second gives the spectral distance
  vector< pair<double,double> > calibrationCoeffs;

  // Check our coefficient file
  if(calibrationData.Objects() != 7) {
    iString msg = "Calibration file [" + calFile.Expanded() + "] must contain data for 7 filters in ascending order;";
    msg += " only [" + iString(calibrationData.Objects()) + "] objects were found";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }

  // Read it, make sure it's ordered
  for(int obj = 0; obj < calibrationData.Objects(); obj ++) {
    PvlObject &calObj = calibrationData.Object(obj);

    if((int)calObj["FilterNumber"] != obj+1) {
      iString msg = "Calibration file [" + calFile.Expanded() + "] must have the filters in ascending order";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    pair<double,double> calData(calObj["RadianceCoefficient"], calObj["SolarSpectralDistance"]);
    calibrationCoeffs.push_back(calData);
  }

  vector<Cube *> flatcubes;
  vector<LineManager *> fcubeMgrs;
  int summing = icube.GetGroup("Instrument")["SummingMode"][0];

  // Read in the flat files
  for(int band = 0; band < 7; band++) {
    string filePattern = "$mro/calibration/marci/";

    if(band < 5) {
      filePattern += "vis";
    }
    else {
      filePattern += "uv";
    }

    // UV cubes are always summing mode = 8, we can assume this rule will never
    //   be broken
    if(band >= 5 && summing != 8) {
      continue;
    }

    filePattern += "flat_band" + iString(band+1);
    filePattern += "_summing" + iString(summing) + "_v???.cub";

    Filename flatFile(filePattern);
    flatFile.HighestVersion();
    Cube *fcube = new Cube();
    fcube->Open(flatFile.Expanded());
    flatcubes.push_back(fcube);

    LineManager *fcubeMgr = new LineManager(*fcube);
    fcubeMgr->SetLine(1,1);
    fcubeMgrs.push_back(fcubeMgr);
  }

  // Prepare the output cube
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

  // This will store a direct translation from band to filter index
  vector<int> filter;

  // Conversion from filter name to filter index
  map<string, int> filterNameToFilterIndex;
  filterNameToFilterIndex.insert(pair<string,int>("BLUE",     1));
  filterNameToFilterIndex.insert(pair<string,int>("GREEN",    2));
  filterNameToFilterIndex.insert(pair<string,int>("ORANGE",   3));
  filterNameToFilterIndex.insert(pair<string,int>("RED",      4));
  filterNameToFilterIndex.insert(pair<string,int>("NIR",      5));
  filterNameToFilterIndex.insert(pair<string,int>("SHORT_UV", 6));
  filterNameToFilterIndex.insert(pair<string,int>("LONG_UV",  7));

  PvlKeyword &filtNames = icube.Label()->FindGroup("BandBin", Pvl::Traverse)["FilterName"];;
  for(int i = 0; i < filtNames.Size(); i++) {
    if(filterNameToFilterIndex.find(filtNames[i]) != filterNameToFilterIndex.end()) {
      filter.push_back(filterNameToFilterIndex.find(filtNames[i])->second);
    }
    else {
      iString msg = "Unrecognized filter name [" + iString(filtNames[i]) + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

  bool iof = ui.GetBoolean("IOF");
  double exposure = ((double)icube.Label()->FindGroup("Instrument", Pvl::Traverse)["ExposureDuration"]) * 1000.0;
  Camera *cam = NULL;
  double solarDist = Isis::Null;

  if(iof) {
    cam = icube.Camera();
    cam->SetImage(icubeMgr.size() / 2.0, 0.5 + (16 / 2) / summing);
    solarDist = cam->SolarDistance();
  }

  LineManager ocubeMgr(ocube);
  ocubeMgr.SetLine(1,1);

  Progress prog;
  prog.SetText("Calibrating Image");
  prog.SetMaximumSteps(ocube.Lines() * ocube.Bands());
  prog.CheckStatus();

  Statistics stats;

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
        ocubeMgr[i] = stretch.Map(icubeMgr[i]) / (*fcubeMgrs[fcubeIndex])[i];

        ocubeMgr[i] = ocubeMgr[i] / exposure / (summing * decimation[fcubeIndex]) / calibrationCoeffs[fcubeIndex].first;

        // Convert to I/F?
        if(iof) {
          ocubeMgr[i] /= calibrationCoeffs[fcubeIndex].second / Isis::PI / (solarDist * solarDist);
        }
      }
    }

    ocube.Write(ocubeMgr);

    icubeMgr++;
    ocubeMgr++;

    bool newFramelet = false;

    for(int i = 0; i < (int)fcubeMgrs.size(); i++) {
      (*fcubeMgrs[i]) ++;

      if(fcubeMgrs[i]->end()) {
        fcubeMgrs[i]->SetLine(1,1);
        newFramelet = true;
      }
    }

    if(newFramelet && cam != NULL) {
      // center the cameras position on the new framelet to keep the solar distance accurate
      cam->SetBand(icubeMgr.Band());
      cam->SetImage(icubeMgr.size() / 2.0 + 0.5, (icubeMgr.Line() - 0.5) + (16 / 2) / summing);
      solarDist = cam->SolarDistance();
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

  // The cube still owns this
  cam = NULL;

  for(int i = 0; i < (int)flatcubes.size(); i++) {
    delete fcubeMgrs[i];
    delete flatcubes[i];
  }

  fcubeMgrs.clear();
  flatcubes.clear();
}

