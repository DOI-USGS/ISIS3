#include "Isis.h"
#include "ProcessByLine.h"
#include "Pixel.h"
#include "iException.h"
#include "TextFile.h"
#include "Table.h"
#include <cmath>

using namespace std; 
using namespace Isis;

const double digitalOffset = 8.30690;
const double globalBias = 2.15547;
const double globalDarkCoefficient = 0.730;
const double vConstant = -0.954194;

double gainFactor;
double absoluteCoefficient;
double saturationThreshold = 255.0;
double cryonorm = 10000.0; // This will almost always be 10,000
double cryocoolerDuration;
double optimalExposureDuration;

int numCoefficients = -1; // This will almost always be 1 or 2
int offsetModeID = 0;

bool convertToNull = false; 
bool doThermalCorrection = false;

std::vector<double> thermBgCoefficients;

void NirCal(std::vector< Isis::Buffer* > &in, std::vector< Isis::Buffer* > &out); 

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;
  UserInterface &ui = Application::GetUserInterface();

  // Basic settings
  convertToNull = ui.GetBoolean("BPFLAG");
  doThermalCorrection = ui.GetBoolean("THCOR");
  absoluteCoefficient = ui.GetDouble("ABSCOEF");
  saturationThreshold = ui.GetDouble("HISAT");

  // Do files (filter + product ID matters)
  Cube *icube = p.SetInputCube("FROM");
  Cube *ocube =p.SetOutputCube("TO");
  Cube *ffcube, *ofcube, *afcube, *dccube, *biascube, *bpcube;  

  iString filter = (string)(icube->GetGroup("BandBin"))["FilterName"];
  filter = filter.DownCase();
  iString productID = (string)(icube->GetGroup("Archive"))["ProductID"];
  iString orbit = productID.substr(productID.find('.')+1, productID.length()-1);

  // If hemisphere code greater than 'I' set to 'n' else set to 's'
  char hemisphereCode = (productID[productID.find('.')-1] > 'I') ? 'n' : 's';
  iString compressionType = (string)(icube->GetGroup("Instrument"))["EncodingFormat"];
  offsetModeID = (icube->GetGroup("Instrument"))["OffsetModeID"];
  int gainModeID = (icube->GetGroup("Instrument"))["GainModeID"];
  iString gainModeIDStr = gainModeID;
  double exposureDuration = (double)(icube->GetGroup("Instrument"))["ExposureDuration"];
  optimalExposureDuration = (exposureDuration * 0.984675) + 0.233398;
  cryocoolerDuration = (icube->GetGroup("Instrument"))["CryocoolerDuration"];

  if (ui.WasEntered("FFFILE")) {
    ffcube = p.SetInputCube("FFFILE");
  } else {
    string fffileLoc = "$clementine1/calibration/nir/";
    fffileLoc += "newnir_flat_" + filter + ".cub";
    CubeAttributeInput cubeAtt;
    ffcube = p.SetInputCube(fffileLoc, cubeAtt);
  }

  if (ui.WasEntered("OFFILE")) {
    ofcube = p.SetInputCube("OFFILE");
  } else {
    string fffileLoc = "$clementine1/calibration/nir/nirorbitflats/";
    fffileLoc += "nir_orbflat_" + orbit + "_" + filter + ".cub";
    CubeAttributeInput cubeAtt;
    ofcube = p.SetInputCube(fffileLoc, cubeAtt);
  }

  string afFileTableLoc = "";
  if (ui.WasEntered("AFFILE")) {
    afcube = p.SetInputCube("AFFILE");
  } else {
    string affileLoc;
    afFileTableLoc = "$clementine1/calibration/nir/nir.addflats.dat";

    TextFile aFFileTable(afFileTableLoc);
    int numLines = aFFileTable.LineCount();
    for (int i=0; i<numLines; i++) {
      iString line;
      aFFileTable.GetLine(line, true);
      line = line.Compress();
      if (orbit.compare(line.Token(" ")) == 0 &&
          filter.compare(line.Token(" ")) == 0 &&
          gainModeID == (int)line.Token(" ") &&
          offsetModeID == (int)line.Token(" ") &&
          (int)exposureDuration == (int)line.Token(" ") &&
          hemisphereCode == line.Token(" ")[0] ) {
        line.Token(" "); // strip unused data
        affileLoc = line.Token(" ");
        break;
      }
    }

    if (affileLoc.compare("") == 0){
      affileLoc = "zeros.cub";
    }

    iString gainFactorDef = "$clementine1/calibration/nir/";
    gainFactorDef += "clemnircal.def";
    Pvl gainFactorData(gainFactorDef);
    iString group = "GainModeID";
    group += (iString)gainModeID;

    if(!gainFactorData.HasGroup(group)) {
      iString err = "The Gain Factor for Gain Mode ID [";
      err += gainModeID;
      err += "] could not be found in clemnircal.def";
      throw iException::Message(iException::Programmer, err, _FILEINFO_);
    }

    gainFactor = (gainFactorData.FindGroup(group))["GAIN"];

    if(abs(gainFactor) < DBL_EPSILON) {
      iString err = "The Gain Factor for Gain Mode ID [";
      err += gainModeID;
      err += "] can not be zero.";
      throw iException::Message(iException::Programmer, err, _FILEINFO_);
    }

    affileLoc = "$clementine1/calibration/nir/nirmodeflats/" + affileLoc;
    CubeAttributeInput cubeAtt;
    afcube = p.SetInputCube(affileLoc, cubeAtt);
  }

  if (ui.WasEntered("DCFILE")) {
    dccube = p.SetInputCube("DCFILE");
  } else {
    string dcfileLoc = "$clementine1/calibration/nir/";

    if (compressionType.compare("CLEM-JPEG-0") == 0) {
      dcfileLoc += "dark_nir_cmp0.cub";
    } else {
      dcfileLoc += "dark_nir.cub";
    }

    CubeAttributeInput cubeAtt;
    dccube = p.SetInputCube(dcfileLoc, cubeAtt);
  }

  if (ui.WasEntered("BIASFILE")) {
    biascube = p.SetInputCube("BIASFILE");
  } else {
    string biasfileLoc = "$clementine1/calibration/nir/";

    if (compressionType.compare("CLEM-JPEG-0") == 0) {
      biasfileLoc += "bias_nir_cmp0.cub";
    } else {
      biasfileLoc += "bias_nir.cub";
    }

    CubeAttributeInput cubeAtt;
    biascube = p.SetInputCube(biasfileLoc, cubeAtt);
  }

  if (ui.WasEntered("BPFILE")) {
    bpcube = p.SetInputCube("BPFILE");
  } else {
    string bpfileLoc = "$clementine1/calibration/nir/";
    if (compressionType.compare("CLEM-JPEG-0") == 0) {
      bpfileLoc += "badpix_nir_cmp0.v3.cub";
    } else {
      bpfileLoc += "badpix_nir.v3.cub";
    }

    CubeAttributeInput cubeAtt;
    bpcube = p.SetInputCube(bpfileLoc, cubeAtt);
  }

  // We need thermal data
  iString thermTbl = "$clementine1/calibration/nir/";
  thermTbl += "nir" + filter + ".therm.dat";

  TextFile thermTable(thermTbl);
  int numLines = thermTable.LineCount();
  iString line;
  for (int i=0; i<numLines; i++) {
    thermTable.GetLine(line);

    // In some lines there's a beginning space, and in others there isn't. To make it consitent,
    // I'm adding a space to every line and using Compress() to remove any extras. Compress() is
    // necessary in order to ensure data integrity from line to line also.
    line = " " + line;
    line = line.Compress();
    line.Token(" "); // There's an initial space that needs removed

    if((int)orbit == (int)line.Token(" ")) { // if orbits match, get data
      cryonorm = (double)line.Token(" ");
      numCoefficients = (int)line.Token(" ");

      // Read in coefficients
      thermBgCoefficients.push_back((double)line.Token(" "));

      for(int iCoeff = 0; iCoeff < numCoefficients; iCoeff ++) {
        thermBgCoefficients.push_back((double)line.Token(" "));
      }
      break;
    }
  }

  if(numCoefficients == 0) {
    iString err = "The orbit [" + orbit + "] could not be located in the thermal corrections table [" + thermTbl + "].";
    throw iException::Message(iException::Programmer, err, _FILEINFO_);                 
  }  

  // Start the processing
  p.StartProcess(NirCal);

  // Add the radiometry group
  PvlGroup calgrp("Radiometry");
  calgrp += PvlKeyword("FlatFieldFile",ffcube->Filename());
  calgrp += PvlKeyword("OrbitFlatFieldFile",ofcube->Filename());
  calgrp += PvlKeyword("AdditiveFile",afcube->Filename());
  calgrp += PvlKeyword("DarkCurrentFile",dccube->Filename());
  calgrp += PvlKeyword("BiasFile",biascube->Filename());
  calgrp += PvlKeyword("BadPixelFile",bpcube->Filename());

  //Table files
  calgrp += PvlKeyword("ThermalCorrectionTable",thermTbl);
  calgrp += PvlKeyword("AdditiveFileTable",afFileTableLoc);

  calgrp += PvlKeyword("DigitalOffset",digitalOffset);
  calgrp += PvlKeyword("GlobalBias",globalBias);
  calgrp += PvlKeyword("GlobalDarkCoefficient",globalDarkCoefficient);
  calgrp += PvlKeyword("V",vConstant);
  //Calculated in processing routine
  calgrp += PvlKeyword("GainFactor",gainFactor);
  calgrp += PvlKeyword("AbsoluteCoefficient",absoluteCoefficient);
  calgrp += PvlKeyword("CryoNorm",cryonorm);
  calgrp += PvlKeyword("OptimalExposureDuration",optimalExposureDuration);

  ocube->PutGroup(calgrp);
  p.EndProcess();
}

void NirCal(std::vector< Isis::Buffer* > &in, std::vector< Isis::Buffer* > &out) {
  Isis::Buffer &incube = *in[0];
  Isis::Buffer &ffcube = *in[1];
  Isis::Buffer &ofcube = *in[2];
  Isis::Buffer &afcube = *in[3];
  Isis::Buffer &dccube = *in[4];
  Isis::Buffer &biascube = *in[5];
  Isis::Buffer &badpixelcube = *in[6];
  Isis::Buffer &outcube = *out[0];

  //Compute the thermal background correction
  double backgroundCorr = 0.0;

  for(int iCoefficient = 0; iCoefficient <= numCoefficients; iCoefficient ++) {
    backgroundCorr += thermBgCoefficients[iCoefficient] * pow(cryocoolerDuration / cryonorm, iCoefficient);
  }

  //No correction for thermal shape is required
  double thermalShapeCorrection = 0.0;

  for(int iSample = 0; iSample < incube.SampleDimension(); iSample ++) {
    // If the bad pixel cube has 0.0 as a value, then this is a known bad pixel
    if((badpixelcube[iSample] == 0.0)) {
      outcube[iSample] = Isis::Null;
      continue;
    }
    else if(Pixel::IsSpecial(incube[iSample])) {
      outcube[iSample] = incube[iSample];
      continue;
    }
    else if(incube[iSample] >= saturationThreshold) {
      outcube[iSample] = Isis::Hrs;
      continue;
    }
    else if(Pixel::IsSpecial(ffcube[iSample]) || ffcube[iSample] == 0.0) {
      // In ISIS2 code the flat field cube was not being checked for
      //   high saturations, and math was still performed on the
      //   files. So, either the result was near zero (because it's
      //   a dividend) or there are no high saturations in this cube
      //   ever.
      outcube[iSample] = Isis::Null;
      continue;
    }
    // Can't do math on special pixels...
    else if(Pixel::IsSpecial(dccube[iSample]) ||
            Pixel::IsSpecial(afcube[iSample]) ||
            Pixel::IsSpecial(ofcube[iSample]) ||
            Pixel::IsSpecial(biascube[iSample]) ||
            Pixel::IsSpecial(badpixelcube[iSample])) {
      outcube[iSample] = Isis::Null;
      continue;
    }

    // Gain factor already checked for zero values
    double term1 = (incube[iSample] - digitalOffset) / gainFactor;

    double term2 = term1 - globalBias - biascube[iSample] - (offsetModeID * vConstant);

    double term3 = term2 / optimalExposureDuration;

    double term4 = term3 - globalDarkCoefficient - dccube[iSample];

    double term5 = term4 - backgroundCorr - thermalShapeCorrection;

    double term6 = term5 / ffcube[iSample];

    double term7 = term6 / ofcube[iSample];

    double term8 = term7 - afcube[iSample];


    outcube[iSample] = term8 * absoluteCoefficient;
  }
}
