#include "Isis.h"
#include "ProcessImportPds.h"
#include "Filename.h"
#include "Brick.h"
#include "MultivariateStatistics.h"
#include "OriginalLabel.h"
#include "iException.h"

using namespace std;
using namespace Isis;

std::vector<Isis::Cube *> outputCubes;
std::vector<int> currentLine;
int numFilters;
int filterHeight = 16;
int flip = 0;
int cubeHeight = 0;
int colorOffset;
std::vector<int> padding;
Isis::Brick *flipDataBrick1 = NULL;
Isis::Brick *flipDataBrick2 = NULL;

const std::string knownFilters[] = {
    "NIR",
    "RED",
    "ORANGE",
    "GREEN",
    "BLUE",
    "LONG_UV",
    "SHORT_UV"
  };

void writeCubeOutput(Isis::Buffer &data);
void translateMarciLabels(Pvl &pdsLabel, Pvl &cubeLabel);
void writeFlipBricks();
void writeOutputPadding();

void IsisMain () {
  flipDataBrick1 = NULL;
  flipDataBrick2 = NULL;
  ProcessImportPds p;
  
  // Input data for MARCI is unsigned byte
  p.SetPixelType(Isis::UnsignedByte);

  UserInterface &ui = Application::GetUserInterface();
  Filename inFile = ui.GetFilename("FROM");

  //Checks if in file is rdr
  Pvl lab(inFile.Expanded());
  if( lab.HasObject("IMAGE_MAP_PROJECTION") ) {
    string msg = "[" + inFile.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User,msg, _FILEINFO_);
  }

  Pvl pdsLab;
  p.SetPdsFile(inFile.Expanded(), "", pdsLab);

  if((int)pdsLab["SAMPLING_FACTOR"] == 12) {
    throw iException::Message(iException::User, "Summing mode of 12 not supported", _FILEINFO_);
  }

  // We need to know how many filters and their height to import the data properly
  numFilters = pdsLab["FILTER_NAME"].Size();
  currentLine.resize(numFilters);
  filterHeight = 16 / (int)pdsLab["SAMPLING_FACTOR"];

  // For simplicity, we'll keep track of line #'s on each band that we've written so far.
  for(int band = 0; band < numFilters; band ++) {
    currentLine[band] = 1;
  }
  
  int maxPadding = 0;

  padding.resize(numFilters);
  for(int filter = 0; filter < numFilters; filter++) {
    if(ui.GetBoolean("COLOROFFSET") == true) {
      colorOffset = ui.GetInteger("COLOROFFSET_SIZE");

      // find the filter num
      int filtNum = 0;
      int numKnownFilters = sizeof(knownFilters) / sizeof(std::string);
      
      while(filtNum < numKnownFilters && 
            (std::string)pdsLab["FILTER_NAME"][filter] != knownFilters[filtNum]) {
        filtNum ++;
      }
      
      if(filtNum >= numKnownFilters) {
        throw iException::Message(iException::Pvl,
                     "Nothing is known about the [" + pdsLab["FILTER_NAME"][filter] + "] filter. COLOROFFSET not possible.",
                     _FILEINFO_);
      }
      else {
        padding[filter] = (colorOffset * filterHeight) * filtNum;
        maxPadding = max(maxPadding, padding[filter]);
      }
    }
    else {
      colorOffset = 0;
      padding[filter] = 0;
    }
  }

  // Output lines/samps.
  
  int numLines = (int)p.Lines() / numFilters + maxPadding;
  int numSamples = pdsLab.FindKeyword("LINE_SAMPLES", Pvl::Traverse);
  cubeHeight = numLines;

  outputCubes.push_back(new Isis::Cube());
  outputCubes.push_back(new Isis::Cube());

  outputCubes[0]->SetDimensions(numSamples, numLines, numFilters);
  outputCubes[1]->SetDimensions(numSamples, numLines, numFilters);

  Filename outputFile(ui.GetFilename("TO"));
  iString evenFile = outputFile.Path() + "/" + outputFile.Basename() + ".even.cub";
  iString oddFile = outputFile.Path() + "/" + outputFile.Basename() + ".odd.cub";

  outputCubes[0]->Create(evenFile);
  outputCubes[1]->Create(oddFile);
  
  if(ui.GetString("FLIP") == "AUTO") {
    flip = -1; // Flip is unknown, this let's us know we need to figure it out later
    flipDataBrick1 = new Isis::Brick(numSamples, filterHeight, numFilters, Isis::UnsignedByte);     
    flipDataBrick2 = new Isis::Brick(numSamples, filterHeight, numFilters, Isis::UnsignedByte);
  }
  else if(ui.GetString("FLIP") == "YES") {
    flip = 1;
  }
  else {
    flip = 0;
  }
  
  writeOutputPadding();
  p.StartProcess(writeCubeOutput);
  
  // Add original labels
  OriginalLabel origLabel(pdsLab);

  std::vector<iString> framelets;

  framelets.push_back("Even");
  framelets.push_back("Odd");

  // Translate labels to every image and close output cubes before calling EndProcess
  for(unsigned int i = 0; i < outputCubes.size(); i++) {
    translateMarciLabels(pdsLab, *outputCubes[i]->Label());

    PvlObject &isisCube = outputCubes[i]->Label()->FindObject("IsisCube");
    isisCube.FindGroup("Instrument").AddKeyword(PvlKeyword("Framelets", framelets[i]));

    outputCubes[i]->Write(origLabel);
    delete outputCubes[i];
  }
  
  outputCubes.clear();
  
  if(flipDataBrick1 != NULL) {
    delete flipDataBrick1;
    delete flipDataBrick2;
    flipDataBrick1 = NULL;
    flipDataBrick2 = NULL;
  }

  p.EndProcess();
}

void writeCubeOutput(Isis::Buffer &data) {
  // The framelet number is necessary for deciding which cube to put the framelet's data in, EVEN or ODD.
  //   Getting the framelet is just a matter of (line / (height of framelet))
  int framelet =  (data.Line()-1) / (filterHeight * numFilters);
  
  // The filter number is important for telling us which band we're processing. This can be calculated with
  //   (line / (height of filter)), very similar to the framelet calculation.
  int filter = (data.Line()-1) / filterHeight;
  
  // The band number is the filter modulus the number of filters.
  int band = filter % numFilters;
  
  // If flip is -1, then we've got to auto-detect it still. The auto-detect works by first reading in the first two
  //   framelets into two bricks, the size of a framelet in the output. Then a correlation is done between the end of
  //   the first framelet, first band, and the ends of the second framelet, first band. Once the flip has
  //   been determined (yes or no), the two framelets in memory are written into the output cube and the program continues
  //   normal execution from then on.
  if(flip == -1 && framelet < 2) {
    // If we're in the first two framelets, just read into memory.
    if(currentLine[band] <= filterHeight) {
      for(int i = 0; i < data.SampleDimension(); i++) {
        (*flipDataBrick1)[flipDataBrick1->Index(i, currentLine[band] - 1, band)] = data[i];
      }
    }
    else {
      for(int i = 0; i < data.SampleDimension(); i++) {
        (*flipDataBrick2)[flipDataBrick2->Index(i, currentLine[band] - filterHeight - 1, band)] = data[i];
      }
    }
  }
  // Not in the first two framelets, flip is unknown, we should figure it out!
  else if(flip == -1) {
    // We'll do two correlations against baseLine, which is the last line of the first framelet's first band
    Isis::Brick baseLine(flipDataBrick2->SampleDimension(), 1, 1, Isis::Real);
    Isis::Brick firstLine(flipDataBrick2->SampleDimension(), 1, 1, Isis::Real);
    Isis::Brick lastLine(flipDataBrick2->SampleDimension(), 1, 1, Isis::Real);
    
    // Populate our lines that will be correlated
    for(int i = 0; i < flipDataBrick2->SampleDimension(); i++) {
      baseLine[i] = (*flipDataBrick1)[flipDataBrick2->Index(i, flipDataBrick2->LineDimension()-1,0)];
      firstLine[i] = (*flipDataBrick2)[i];
      lastLine[i]  = (*flipDataBrick2)[flipDataBrick2->Index(i, flipDataBrick2->LineDimension()-1,0)];
    }
    
    // The MultivariateStatistics will do our correlation for us. Pass it the first set of data, correlate,
    //   remove the data, pass it the second set and correlate. If the second correlation is better, flip.
    MultivariateStatistics stats;
    stats.AddData(baseLine.DoubleBuffer(), firstLine.DoubleBuffer(), flipDataBrick1->SampleDimension());
    double corr1 = fabs(stats.Correlation());
    stats.RemoveData(flipDataBrick1->DoubleBuffer(), firstLine.DoubleBuffer(), flipDataBrick1->SampleDimension());
    stats.AddData(baseLine.DoubleBuffer(), lastLine.DoubleBuffer(), flipDataBrick1->SampleDimension());
    double corr2 = fabs(stats.Correlation());

    // Failure = No Flip
    if(Isis::IsSpecial(corr1) || Isis::IsSpecial(corr2) || corr1 >= corr2) {
      flip = 0;
    }
    else {
      flip = 1;
    }

    // Write the two bricks in memory to the output
    writeFlipBricks();
  }

  // We're going to write every cube, regardless of if it's the right cube. We'll be populating the extra data with nulls.
  for(unsigned int cube = 0; (flip != -1) && (cube < outputCubes.size()); cube++) {
    // The data will be copied into a brick, and the brick written. We do this so we can set the position to write
    //   the output data.
    Brick output(data.SampleDimension(), data.LineDimension(), 1, Isis::Real);
  
    // currentLine[] is 1-based
    if(flip == 0) {
      output.SetBasePosition(1, currentLine[band] + padding[band], band+1);
    }
    else if(flip == 1) {
      int outLine = outputCubes[cube]->Lines() - filterHeight -
        ((currentLine[band] - 1) / filterHeight) * filterHeight + (currentLine[band]-1) % filterHeight;

      output.SetBasePosition(1, outLine+1 - padding[band], band+1);
    }

    // If the 1-based framelet number mod the output cubes equals the current cube, it's the proper cube to use.
    // Flipped data can end up with even/odd flipped, and indeed probably will, but this isn't a concern.
    if((framelet+1) % outputCubes.size() == cube) {
      for(int i = 0; i < data.size(); i++) {
        output[i] = data[i];
      }
    }
    else {
      for(int i = 0; i < data.size(); i++) {
        output[i] = Isis::Null;
      }
    }
    
    // Data is in our brick, let's write it into the cube.
    outputCubes[cube]->Write(output);
  }

  currentLine[band] ++;
}

void translateMarciLabels(Pvl &pdsLabel, Pvl &cubeLabel) {
  PvlGroup arch("Archive");

  if(pdsLabel.HasKeyword("SAMPLE_BIT_MODE_ID")) {
    arch += PvlKeyword("SampleBitModeId", (string)pdsLabel["SAMPLE_BIT_MODE_ID"]);
  }

  PvlGroup inst("Instrument");
  
  if((string)pdsLabel["SPACECRAFT_NAME"] == "MARS_RECONNAISSANCE_ORBITER") {
    inst += PvlKeyword("SpacecraftName", "MARS RECONNAISSANCE ORBITER");
  }
  else {
    throw iException::Message(iException::User, "The input file does not appear to be a MARCI image", _FILEINFO_);
  }
  
  if((string)pdsLabel["INSTRUMENT_ID"] == "MARCI") {
    inst += PvlKeyword("InstrumentId", "Marci");
  }
  else {
    throw iException::Message(iException::User, "The input file does not appear to be a MARCI image", _FILEINFO_);
  }
  
  inst += PvlKeyword("TargetName", (string)pdsLabel["TARGET_NAME"]);
  inst += PvlKeyword("SummingMode", (string)pdsLabel["SAMPLING_FACTOR"]);
  inst += PvlKeyword("StartTime", (string) pdsLabel["START_TIME"]);
  inst += PvlKeyword("StopTime", (string) pdsLabel["STOP_TIME"]);
  inst += PvlKeyword("SpacecraftClockCount",(string)pdsLabel["SPACECRAFT_CLOCK_START_COUNT"]);
  inst += PvlKeyword("DataFlipped", (flip == 1));
  inst += PvlKeyword("ColorOffset", colorOffset);
  inst += PvlKeyword("InterframeDelay", (iString)((double)pdsLabel["INTERFRAME_DELAY"]), "seconds");
  inst += PvlKeyword("ExposureDuration", (iString)((double)pdsLabel["LINE_EXPOSURE_DURATION"] / 1000.0), "seconds");

  PvlGroup bandBin("BandBin");
  PvlKeyword filterName("FilterName");
  PvlKeyword origBands("OriginalBand");
  for(int filter = 0; filter < pdsLabel["FILTER_NAME"].Size(); filter++) {
    filterName += pdsLabel["FILTER_NAME"][filter];
    origBands += iString(filter+1);
  }

  bandBin += filterName;
  bandBin += origBands;

  PvlObject &isisCube = cubeLabel.FindObject("IsisCube");
  isisCube.AddGroup(inst);
  isisCube.AddGroup(bandBin);
  isisCube.AddGroup(arch);

  // Map VIS/UV to NaifIkCode
  std::map<std::string, int> naifIkCodes;
  naifIkCodes.insert( std::pair<std::string,int>("MRO_MARCI",             -74400) );
  naifIkCodes.insert( std::pair<std::string,int>("MRO_MARCI_VIS",         -74410) );
  naifIkCodes.insert( std::pair<std::string,int>("MRO_MARCI_UV",          -74420) );

  // Map from filter name to VIS/UV
  std::map<std::string, std::string> bandUvVis;
  bandUvVis.insert( std::pair<std::string, std::string>("BLUE",   "MRO_MARCI_VIS") );
  bandUvVis.insert( std::pair<std::string, std::string>("GREEN",  "MRO_MARCI_VIS") );
  bandUvVis.insert( std::pair<std::string, std::string>("ORANGE", "MRO_MARCI_VIS") );
  bandUvVis.insert( std::pair<std::string, std::string>("RED",    "MRO_MARCI_VIS") );
  bandUvVis.insert( std::pair<std::string, std::string>("NIR",    "MRO_MARCI_VIS") );
  bandUvVis.insert( std::pair<std::string, std::string>("LONG_UV",  "MRO_MARCI_UV") );
  bandUvVis.insert( std::pair<std::string, std::string>("SHORT_UV", "MRO_MARCI_UV") );

  PvlGroup kerns("Kernels");
  string uvvis = bandUvVis.find((std::string)bandBin["FilterName"][0])->second;
  int iakCode = naifIkCodes.find(uvvis)->second;
  kerns += PvlKeyword("NaifIkCode", iakCode);

  isisCube.AddGroup(kerns);
}

/**
 * This method will write the two bricks that were read into memory in order to
 *   auto-detect if the image needed flipped.
 */
void writeFlipBricks() {
  Isis::Brick nullBrick(flipDataBrick1->SampleDimension(), flipDataBrick1->LineDimension(), 1, Isis::Real);

  for(int i = 0; i < nullBrick.size(); i++) {
    nullBrick[i] = Isis::Null;
  }

  for(unsigned int cube = 0; cube < outputCubes.size(); cube++) { 
    for(int framelet = 0; framelet < 2; framelet++) {
      for(int band = 0; band < numFilters; band++) {
        Isis::Brick outBrick(flipDataBrick1->SampleDimension(), flipDataBrick1->LineDimension(), 1, Isis::Real);
        
        if((framelet+1) % outputCubes.size() == cube) {
          for(int i = 0; i < outBrick.size(); i++) {
            if(framelet == 0) {
              outBrick[i] = (*flipDataBrick1)[flipDataBrick1->Index(0, 0, band) + i];
            }
            else {
              outBrick[i] = (*flipDataBrick2)[flipDataBrick2->Index(0, 0, band) + i];
            }  
          }
        }
        else {
          outBrick.Copy(nullBrick);
        }

        if(flip == 0) {
          if(framelet == 0) {
            outBrick.SetBasePosition(1, 1 + padding[band], band+1);
          }
          else {
            outBrick.SetBasePosition(1, filterHeight + 1 + padding[band], band+1);
          }
        }
        else {
          int outLine = outputCubes[cube]->Lines() - (filterHeight * (framelet+1)) - padding[band];
          outBrick.SetBasePosition(1, outLine+1, band+1);
        }
        
        outputCubes[cube]->Write(outBrick);
      }
    }
  }
}

// This writes nulls to the bricks where their padding goes
void writeOutputPadding() {
  int paddingHeight = 0;
  
  for(unsigned int pad = 0; pad < padding.size(); pad++) {
    paddingHeight = max(paddingHeight, padding[pad]);
  }
  
  if(paddingHeight == 0) return; // no padding

  for(unsigned int cube = 0; cube < outputCubes.size(); cube++) { 
    Isis::Brick nullBrick(outputCubes[cube]->Samples(), paddingHeight, outputCubes[cube]->Bands(), Isis::Real);
  
    for(int i = 0; i < nullBrick.size(); i++) {
      nullBrick[i] = Isis::Null;
    }

    // Write padding to the beginning & end of all cubes, to ensure it's all set to null
    nullBrick.SetBasePosition(1, 1, 1);
    outputCubes[cube]->Write(nullBrick);
   
    nullBrick.SetBasePosition(1, outputCubes[cube]->Lines() - paddingHeight, 1);
    outputCubes[cube]->Write(nullBrick);
  }
}
