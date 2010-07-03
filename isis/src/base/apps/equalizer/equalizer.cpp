#include "Isis.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "FileList.h"
#include "Filename.h"
#include "OverlapStatistics.h"
#include "LineManager.h"
#include "MultivariateStatistics.h"
#include "iString.h"
#include "OverlapNormalization.h"
#include "CubeAttribute.h"
#include "tnt/tnt_array2d.h"
#include "jama/jama_svd.h"
#include <cmath>

using namespace Isis;

Isis::Statistics GatherStatistics(Cube &icube, const int band, 
  double sampPercent, std::string maxCubeStr);
void ApplyViaObject(Buffer &in, Buffer &out);
void ApplyViaFile(Buffer &in, Buffer &out);

std::vector<OverlapStatistics> g_overlapList;
std::vector<OverlapNormalization*> g_oNormList;
std::vector<int> normIndices;
std::vector<double> gains, offsets, avgs;
int g_imageIndex, g_maxCube, g_maxBand;

void IsisMain() {
  // Get the list of cubes to mosaic
  FileList imageList;
  UserInterface &ui = Application::GetUserInterface();
  imageList.Read(ui.GetFilename("FROMLIST"));
  if (imageList.size() < 1) {
    std::string msg = "The list file [" + ui.GetFilename("FROMLIST") +
                 "] does not contain any data";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // Make sure the user enters a "OUTSTATS" file if the CALCULATE option 
  // is selected
  std::string processOpt = ui.GetString("PROCESS");
  if (processOpt == "CALCULATE") {
    if (!ui.WasEntered("OUTSTATS")) {
      std::string msg = "If the CALCULATE option is selected, you must enter";
      msg += " an OUTSTATS file";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
  }

  // Make sure number of bands and projection parameters match for all cubes
  for (unsigned int i=0; i<imageList.size(); i++) {
    Cube cube1;
    cube1.Open(imageList[i]);
    g_maxBand = cube1.Bands();

    for (unsigned int j=(i+1); j<imageList.size(); j++) {
      Cube cube2;
      cube2.Open(imageList[j]);

      // Make sure number of bands match
      if (g_maxBand != cube2.Bands()) {
        string msg = "Number of bands do not match between cubes [" +
                     imageList[i] + "] and [" + imageList[j] + "]";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }

      //Create projection from each cube
      Projection *proj1 = cube1.Projection();
      Projection *proj2 = cube2.Projection();

      // Test to make sure projection parameters match
      if (*proj1 != *proj2) {
        string msg = "Mapping groups do not match between cubes [" +
                     imageList[i] + "] and [" + imageList[j] + "]";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }
    }
  }

  // Read hold list if one was entered
  std::vector<int> hold;
  if (ui.WasEntered("HOLD")) {
    FileList holdList;
    holdList.Read(ui.GetFilename("HOLD"));

    // Make sure each file in the holdlist matches a file in the fromlist
    for (int i=0; i<(int)holdList.size(); i++) {
      bool matched = false;
      for (int j=0; j<(int)imageList.size(); j++) {
        if (holdList[i] == imageList[j]) {
          matched = true;
          hold.push_back(j);
          break;
        }
      }
      if (!matched) {
        std::string msg = "The hold list file [" + holdList[i] +
                     "] does not match a file in the from list";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }
    }
  }

  // Read to list if one was entered
  FileList outList;
  if (ui.WasEntered("TOLIST")) {
    outList.Read(ui.GetFilename("TOLIST"));

    // Make sure each file in the tolist matches a file in the fromlist
    if (outList.size() != imageList.size()) {
      std::string msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding output file in the TO LIST.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // Make sure that all output files do not have the same names as their
    // corresponding input files
    for (unsigned i = 0; i < outList.size(); i++) {
      if (outList[i].compare(imageList[i]) == 0) {
        std::string msg = "The to list file [" + outList[i] +
                     "] has the same name as its corresponding from list file.";
        throw iException::Message(iException::User,msg,_FILEINFO_);
      }
    }
  }
  
  // Test to ensure sampling percent in bound
  double sampPercent = ui.GetDouble("PERCENT"); 
  if (sampPercent <= 0.0 || sampPercent > 100.0) {
    string msg = "The sampling percent must be a decimal (0.0, 100.0]";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  int mincnt = ui.GetInteger("MINCOUNT");
  bool wtopt = ui.GetBoolean("WEIGHT");
  if (processOpt != "APPLY") {
    // Loop through all the input cubes, calculating statistics for each cube to use later   
    iString maxCubeStr ((int)imageList.size());
    for (int band=1; band<=g_maxBand; band++) {
      std::vector<Statistics> statsList;
      for (int img=0; img<(int)imageList.size(); img++) {
	Process p;
	const CubeAttributeInput att;
	const std::string inp = imageList[img];
	Cube *icube = p.SetInputCube(inp, att);
	
	// Add a Statistics object to the list for every band of every input cube
	g_imageIndex = img;
	Statistics stats = GatherStatistics(*icube, band, sampPercent, maxCubeStr);
	statsList.push_back(stats);
	p.EndProcess();
      }
  
      // Create a separate OverlapNormalization object for every band
      OverlapNormalization *oNorm = new OverlapNormalization (statsList);
      for (int h=0; h<(int)hold.size(); h++) oNorm->AddHold(hold[h]);
      g_oNormList.push_back(oNorm);
    }

    // A list for keeping track of which input cubes are known to overlap another
    std::vector<bool> doesOverlapList;
    for (unsigned int i=0; i<imageList.size(); i++) doesOverlapList.push_back(false);

    // Find overlapping areas and add them to the set of known overlaps for each
    // band shared amongst cubes
    for (unsigned int i=0; i<imageList.size(); i++){
      Cube cube1;
      cube1.Open(imageList[i]);
  
      for (unsigned int j=(i+1); j<imageList.size(); j++) {
	Cube cube2;
	cube2.Open(imageList[j]);
	iString cubeStr1 ((int)(i+1));
	iString cubeStr2 ((int)(j+1));
	string statMsg = "Gathering Overlap Statisitcs for Cube " +
	  cubeStr1 + " vs " + cubeStr2 + " of " + maxCubeStr;
  
	// Get overlap statistics for cubes
	OverlapStatistics oStats(cube1, cube2, statMsg, sampPercent);
  
	// Only push the stats onto the oList vector if there is an overlap in at
	// least one of the bands
	if (oStats.HasOverlap()) {        
	  oStats.SetMincount(mincnt);
	  g_overlapList.push_back(oStats);
	  for (int band=1; band<=g_maxBand; band++) {
	    // Fill wt vector with 1's if the overlaps are not to be weighted, or
	    // fill the vector with the number of valid pixels in each overlap          
	    int weight = 1;
	    if (wtopt) weight = oStats.GetMStats(band).ValidPixels();
  
	    // Make sure overlap has at least MINCOUNT pixels and add          
	    if (oStats.GetMStats(band).ValidPixels() >= mincnt) {
	      g_oNormList[band-1]->AddOverlap(oStats.GetMStats(band).X(), i,
			       oStats.GetMStats(band).Y(), j, weight);
	      doesOverlapList[i] = true;
	      doesOverlapList[j] = true;
	    }
	  }
	}
      }
    }
  
    // Print an error if one or more of the images does not overlap another
    {
      std::string badFiles = "";
      for (unsigned int img=0; img<imageList.size(); img++) {
	// Print the name of each input cube without an overlap
	if (!doesOverlapList[img]) {
	   badFiles += "[" + imageList[img] + "] ";
	}
      }
      if (badFiles != "") {
	std::string msg = "File(s) " + badFiles;
	msg += " do(es) not overlap any other input images with enough valid pixels";
	throw iException::Message(iException::User,msg,_FILEINFO_);
      }
    }
  
    // Determine whether to calculate gains or offsets
    std::string adjust = ui.GetString("ADJUST");
    OverlapNormalization::SolutionType sType = OverlapNormalization::Both;  
    if (adjust == "CONTRAST")   sType = OverlapNormalization::Gains;
    if (adjust == "BRIGHTNESS") sType = OverlapNormalization::Offsets;
  
    // Loop through each band making all necessary calculations
    for (int band=0; band<g_maxBand; band++) {
      g_oNormList[band]->Solve(sType);
    }
  }

  // Print gathered statistics to the gui and the print file
  int validCnt = 0;
  int invalidCnt = 0;
  if (processOpt != "APPLY") {
    PvlGroup results("Results");

    // Compute the number valid and invalid overlaps
    for (unsigned int o=0; o<g_overlapList.size(); o++) {
      for (int band=1; band<=g_maxBand; band++) {
	if (g_overlapList[o].IsValid(band)) validCnt++;
	else invalidCnt++;
      }
    }

    results += PvlKeyword("TotalOverlaps", validCnt+invalidCnt);
    results += PvlKeyword("ValidOverlaps", validCnt);
    results += PvlKeyword("InvalidOverlaps", invalidCnt);
    std::string weightStr = "false";
    if (wtopt) weightStr = "true";
    results += PvlKeyword("Weighted", weightStr);
    results += PvlKeyword("MinCount", mincnt);

    // Name and band modifiers for each image
    for (unsigned int img=0; img<imageList.size(); img++) {
      results += PvlKeyword("FileName", imageList[img]);
  
      // Band by band statistics
      for (int band=1; band<=g_maxBand; band++) {
	iString mult (g_oNormList[band-1]->Gain(img));
	iString base (g_oNormList[band-1]->Offset(img));        
	iString avg (g_oNormList[band-1]->Average(img));
	iString bandNum (band);
	std::string bandStr = "Band" + bandNum;
	PvlKeyword bandStats(bandStr);
	bandStats += mult;
	bandStats += base;
	bandStats += avg;
	results += bandStats;
      }
    }

    // Write the results to the log
    Application::Log(results);
  }

  // Setup the output text file if the user requested one
  if (ui.WasEntered("OUTSTATS")) {
    PvlObject equ("EqualizationInformation");
    PvlGroup gen("General");
    gen += PvlKeyword("TotalOverlaps", validCnt+invalidCnt);
    gen += PvlKeyword("ValidOverlaps", validCnt);
    gen += PvlKeyword("InvalidOverlaps", invalidCnt);
    std::string weightStr = "false";
    if (wtopt) weightStr = "true";
    gen += PvlKeyword("Weighted", weightStr);
    gen += PvlKeyword("MinCount", mincnt);
    equ.AddGroup(gen);
    for (unsigned int img=0; img<imageList.size(); img++) {
      // Format and name information
      PvlGroup norm("Normalization");
      norm.AddComment("Formula: newDN = (oldDN - AVERAGE) * GAIN + AVERAGE + OFFSET");
      norm.AddComment("BandN = (GAIN, OFFSET, AVERAGE)");
      norm += PvlKeyword("FileName", imageList[img]);
      
      // Band by band statistics
      for (int band=1; band<=g_maxBand; band++) {
        iString mult (g_oNormList[band-1]->Gain(img));
        iString base (g_oNormList[band-1]->Offset(img));        
        iString avg (g_oNormList[band-1]->Average(img));
        iString bandNum (band);
        std::string bandStr = "Band" + bandNum;
        PvlKeyword bandStats(bandStr);
        bandStats += mult;
        bandStats += base;
        bandStats += avg;
        norm += bandStats;
      }
      equ.AddGroup(norm);
    }

    // Write the equalization and overlap statistics to the file
    std::string out = Filename(ui.GetFilename("OUTSTATS")).Expanded();
    std::ofstream os;
    os.open(out.c_str(),std::ios::app);    
    Pvl p;
    p.SetTerminator("");
    p.AddObject(equ);
    os << p << std::endl;
    for (unsigned int i=0; i<g_overlapList.size(); i++) {
      os << g_overlapList[i];
      if (i != g_overlapList.size()-1) os << std::endl;
    }
    os << "End";
  }

  // Check for errors with the input statistics
  if (processOpt == "APPLY") {
    Pvl inStats (ui.GetFilename("INSTATS"));
    PvlObject &equalInfo = inStats.FindObject("EqualizationInformation");

    // Make sure each file in the instats matches a file in the fromlist
    if (imageList.size() > (unsigned)equalInfo.Groups()-1) {
      std::string msg = "Each input file in the FROM LIST must have a ";
      msg += "corresponding input file in the INPUT STATISTICS.";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // Check that each file in the FROM LIST is present in the INPUT STATISTICS
    for (unsigned i = 0; i < imageList.size(); i++) {
      std::string fromFile = imageList[i];
      bool foundFile = false;
      for (int j = 1; j < equalInfo.Groups(); j++) {
	PvlGroup &normalization = equalInfo.Group(j);
	std::string normFile  = normalization["Filename"][0];
	if (fromFile == normFile) {

	  // Store the index in INPUT STATISTICS file corresponding to the
	  // current FROM LIST file
	  normIndices.push_back(j);
	  foundFile = true;
	}
      }
      if (!foundFile) {
	std::string msg = "The from list file [" + fromFile +
		 "] does not have any corresponding file in the stats list.";
	throw iException::Message(iException::User,msg,_FILEINFO_);
      }
    }
  }

  // Apply the correction to the images if the user wants this done
  if (processOpt != "CALCULATE") {
    iString maxCubeStr ((int)imageList.size());
    for (int img=0; img<(int)imageList.size(); img++) {
      // Set up for progress bar
      ProcessByLine p;
      iString curCubeStr (img+1);
      p.Progress()->SetText("Equalizing Cube " + curCubeStr + " of " + maxCubeStr);

      // Open input cube
      CubeAttributeInput att;
      const std::string inp = imageList[img];
      Cube *icube = p.SetInputCube(inp, att);

      // Establish the output file depending upon whether or not a to list
      // was entered
      std::string out;
      if (ui.WasEntered("TOLIST")) {
	out = outList[img];
      }
      else {
	Filename file = imageList[img];
	out = file.Path() + "/" + file.Basename() + ".equ." + file.Extension();
      }

      // Allocate output cube
      CubeAttributeOutput outAtt;
      p.SetOutputCube(out,outAtt,icube->Samples(),icube->Lines(),icube->Bands());

      // Apply gain/offset to the image
      g_imageIndex = img;
      if (processOpt == "APPLY") {

	// Apply correction based on pre-determined statistics information
	Pvl inStats (ui.GetFilename("INSTATS"));
	PvlObject &equalInfo = inStats.FindObject("EqualizationInformation");
	PvlGroup &normalization = equalInfo.Group(normIndices[g_imageIndex]);
	gains.clear();
	offsets.clear();
	avgs.clear();

	// Get and store the modifiers for each band
	for (int band = 1; band < normalization.Keywords(); band++) {
	  gains.push_back(normalization[band][0]);
	  offsets.push_back(normalization[band][1]);
	  avgs.push_back(normalization[band][2]);
	}
	p.StartProcess(ApplyViaFile); 
      }
      else {

	// Apply correction based on the statistics gathered in this run
	p.StartProcess(ApplyViaObject);
      }
      p.EndProcess();
    }
  }
  
  // Clean-up for batch list runs
  for (unsigned int o=0; o<g_oNormList.size(); o++) delete g_oNormList[o];
  g_oNormList.clear();
  g_overlapList.clear();
  normIndices.clear();
  gains.clear();
  offsets.clear();
  avgs.clear();
}

// Gather general statistics on a particular band of a cube
Isis::Statistics GatherStatistics(Cube &icube, const int band, 
  double sampPercent, std::string maxCubeStr) {
  // Create our progress message
  iString curCubeStr (g_imageIndex+1);
  std::string statMsg = "";
  if (icube.Bands() == 1) {
    statMsg = "Calculating Statistics for Band 1 in Cube " + curCubeStr +
      " of " + maxCubeStr;
  }
  else {
    iString curBandStr (band);
    iString maxBandStr (icube.Bands());
    statMsg = "Calculating Statistics for Band " + curBandStr + " of " +
      maxBandStr + " in Cube " + curCubeStr + " of " + maxCubeStr;
  }
  
  int linc = (int) (100.0 / sampPercent + 0.5); // Calculate our line incrementer
  
  // Make sure band is valid
  if ((band <= 0) || (band > icube.Bands())) {
    string msg = "Invalid band in method [GatherStatistics]";
    throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
  }
  
  // Construct a line buffer manager and a statistics object
  Isis::LineManager line (icube);
  
  
  Isis::Progress progress;
  progress.SetText(statMsg);
  
  // Calculate the number of steps for the Progress object, and add an extra
  // step if the total lines and incrementer do not divide evenly
  int maxSteps = icube.Lines() / linc;
  if (icube.Lines() % linc != 0) maxSteps += 1;
  progress.SetMaximumSteps(maxSteps);
  progress.CheckStatus();
  
  // Add data to Statistics object by line
  Isis::Statistics stats;
  int i=1;
  while (i<=icube.Lines()) {
    line.SetLine(i,band);
    icube.Read(line);
    stats.AddData (line.DoubleBuffer(), line.size());
    
    // Make sure we consider the last line
    if (i+linc > icube.Lines() && i != icube.Lines()) {
      i = icube.Lines();
      progress.AddSteps(1);
    }
    else i += linc; // Increment the current line by our incrementer  
    
    progress.CheckStatus();
  }
  
  return stats;
}

// Apply the equalization to the cubes with Overlap Normalization
void ApplyViaObject(Buffer &in, Buffer &out) {
  int band = in.Band();
  for (int i=0; i<in.size(); i++) {
    out[i] = g_oNormList[band-1]->Evaluate(in[i],g_imageIndex);
  }
}

// Apply the equalization to the cubes with INPUT STATISTICS
void ApplyViaFile(Buffer &in, Buffer &out) {
  int band = in.Band();
  double gain = gains[band-1];
  double offset = offsets[band-1];
  double avg = avgs[band-1];
  for (int i=0; i<in.size(); i++) {
    if (Isis::IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = (in[i] - avg) * gain + offset + avg; 
    }
  }
}
