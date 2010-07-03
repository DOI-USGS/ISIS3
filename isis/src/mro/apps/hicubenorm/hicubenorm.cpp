#include "Isis.h"

// system include files go first
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

// Isis specific include files go next
#include "ProcessByTile.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "iException.h"
#include "Pvl.h"
#include "Statistics.h"
#include "VecFilter.h"

using namespace std;
using namespace Isis;

// These global vectors are used to keep track of info for columns 
// of image data.  For example, a 100 sample x 200 line x 2 band cube will
// have a vectors of 200 columns (100 samples x 2 bands).
vector<double> stddev;
vector<int> validpixels;
vector<double> minimum;
vector<double> maximum;
vector<int> band;
vector<int> element;
vector<double> median;
vector<double> average;
vector<double> normalizer;

// Size of the cube
int totalLines;
int totalSamples;

enum Mode {SUBTRACT,DIVIDE};

// function prototypes
void getStats(Buffer &in);
void multiply(Buffer &in, Buffer &out);
void subtract(Buffer &in, Buffer &out);
void pvlOut(const string &pv);
void tableOut(const string &pv);
void PVLIn(const Isis::Filename & filename);
void tableIn(const Isis::Filename & filename);
void keepSame(int &totalBands, int &rowcol, Mode mode);
void filterStats(vector<double> &filter,int &filtsize, bool &pause_crop,
  int &channel);

// Main Program
void IsisMain() {
  vector<double> filter;
  int rowcol;                    // how many rows or cols per band
  bool normalizeUsingAverage;    // mult/sub using average or median?
  int totalBands;
  // Used for filtering the initial cubenorm averages and median values
  int filtsize;
  bool pause_crop;
  int channel;
  // ERROR CHECK:  The user must specify at least the TO or STATS
  // parameters.
  UserInterface &ui = Application::GetUserInterface();
  if (!(ui.WasEntered("TO")) && !(ui.WasEntered("STATS"))) {
    string msg = "User must specify a TO and/or STATS file.";
    throw iException::Message(iException::User,msg,_FILEINFO_);
  }

  // We will be processing by tile.
  ProcessByTile p;

  // Setup the input cube;
  // Obtain information from the input file
  Cube *icube = p.SetInputCube("FROM");
  totalSamples = icube->Samples();
  totalLines   = icube->Lines();
  totalBands   = icube->Bands();
  channel = icube->GetGroup("Instrument")["ChannelNumber"];

  // Setup the tile size for columnar processing
  p.SetTileSize(1, totalLines);
  rowcol = totalSamples;

  // Now get the statistics for each column
  normalizeUsingAverage = ui.GetString("NORMALIZER") == "AVERAGE";

  //gather statistics
  if (ui.GetString("STATSOURCE") == "CUBE"){
    p.StartProcess(getStats);
  }
  else if (ui.GetString("STATSOURCE") == "TABLE"){
    tableIn(ui.GetFilename("FROMSTATS"));
  }
  else{
    PVLIn(ui.GetFilename("FROMSTATS"));
  }
 
  //check to make sure the first vector has as many elements as the last
  // vector, and that there is a vector element for each row/col
  if (band.size() != (unsigned int)(rowcol*totalBands)) { 
    string message = "You have entered an invalid input file " + 
      ui.GetFilename("FROMSTATS");
    throw  iException::Message(Isis::iException::Io, message,
    	 _FILEINFO_);
  }

  //get information needed to filter the statistics
  filtsize = ui.GetInteger("FILTER");
  pause_crop = ui.GetBoolean("PAUSECROP");

  //filter the column averages
  filter = average;
  filterStats(filter,filtsize,pause_crop,channel);
  average = filter;

  //filter the column medians
  filter = median;
  filterStats(filter,filtsize,pause_crop,channel);
  median = filter;

  //If a STATS file was specified then create statistics file
  if (ui.WasEntered("STATS")) {
     string op = ui.GetString("FORMAT");
     if (op == "PVL")    pvlOut(ui.GetFilename("STATS"));
     if (op == "TABLE")  tableOut(ui.GetFilename("STATS"));
  }

  // Update the statistics vectors before creating the output
  // file
  if (normalizeUsingAverage) {
    normalizer = average;
  } else {
    normalizer = median;
  }

  // If an output file was specified then normalize the cube
  if (ui.WasEntered ("TO")) {
    // Before creating a normalized cube check to see if there
    // are any column averages less than or equal to zero.
    if (ui.GetString("MODE") == "MULTIPLY") {
      for (unsigned int i=0; i<band.size(); i++) {
        if (IsValidPixel(normalizer[i]) && normalizer[i]<=0.0) {
          string msg = "Cube file can not be normalized with [MULTIPLY] ";
          msg += "option, some column averages <= 0.0";
          throw iException::Message(iException::User,msg,_FILEINFO_);
        }
      }
    }

    // Setup the output file and apply the coefficients by either
    // subtracting or multipling them
    p.SetOutputCube("TO");

    // Should we preserve the average/median of the input image???
    if (ui.GetBoolean("PRESERVE")) {
      if (ui.GetString("MODE") == "SUBTRACT") {
        keepSame(totalBands,rowcol,SUBTRACT);
      } else {
        keepSame(totalBands,rowcol,DIVIDE);
      }
    }

    // Process based on the mode
    if (ui.GetString("MODE") == "SUBTRACT") {
      p.StartProcess(subtract);
    }
    else {
      p.StartProcess(multiply);
    }
  }

  // Cleanup
  p.EndProcess();
  stddev.clear();
  validpixels.clear();
  minimum.clear();
  maximum.clear();
  band.clear();
  element.clear();
  median.clear();
  average.clear();
  normalizer.clear();
  filter.clear();
}

//**********************************************************
// DOUSER - Get statistics on a column or row of pixels
//**********************************************************
void getStats(Buffer &in) {
  Statistics stats;
  stats.AddData(in.DoubleBuffer(),in.size());
  
  band.push_back(in.Band());
  element.push_back(in.Sample());

  // Sort the input buffer
  vector<double> pixels;
  for (int i=0; i<in.size(); i++) {
    if (IsValidPixel(in[i])) pixels.push_back(in[i]);
  }
  sort(pixels.begin(),pixels.end());

  // Now obtain the median value and store in the median vector
  int size = pixels.size();
  if (size != 0) {
    int med = size/2;
    if (size%2 == 0) {
      median.push_back((pixels[med-1]+pixels[med])/2.0);
    }
    else {
      median.push_back(pixels[med]);
    }
  }
  else {
    median.push_back(Isis::Null);
  }

  // Store the statistics in the appropriate vectors
  average.push_back(stats.Average());
  stddev.push_back(stats.StandardDeviation());
  validpixels.push_back(stats.ValidPixels());
  minimum.push_back(stats.Minimum());
  maximum.push_back(stats.Maximum());
}

//********************************************************
// Create PVL output of statistics
//*******************************************************
void pvlOut(const string &StatFile) {
  PvlGroup results ("Results");
  for (unsigned int i=0; i <band.size(); i++) {
    results += PvlKeyword("Band",band[i]);
    results += PvlKeyword("RowCol",element[i]);
    results += PvlKeyword("ValidPixels",validpixels[i]);
    if (validpixels[i] > 0) {
      results += PvlKeyword("Mean",average[i]);
      results += PvlKeyword("Median",median[i]);
      results += PvlKeyword("Std",stddev[i]);
      results += PvlKeyword("Minimum",minimum[i]);
      results += PvlKeyword("Maximum",maximum[i]);
    }
    else {
      results += PvlKeyword("Mean",0.0);
      results += PvlKeyword("Median",0.0);
      results += PvlKeyword("Std",0.0);
      results += PvlKeyword("Minimum",0.0);
      results += PvlKeyword("Maximum",0.0);
    }
  }

  Pvl t;
  t.AddGroup(results);
  t.Write(StatFile);
}

//********************************************************
// Create Tabular output of statistics
//*******************************************************
void tableOut(const string &StatFile) {
  // Open output file
  // TODO check status and throw error
  ofstream out;
  out.open(StatFile.c_str(),std::ios::out);

  // Output a header
  out << std::setw(8)  << "Band";
  out << std::setw(8)  << "RowCol";
  out << std::setw(15) << "ValidPoints";
  out << std::setw(15) << "Average";
  out << std::setw(15) << "Median";
  out << std::setw(15) << "StdDev";
  out << std::setw(15) << "Minimum";
  out << std::setw(15) << "Maximum";
  out << endl;

  // Print out the table results
  for (unsigned int i=0; i<band.size(); i++) {
    out << std::setw(8)  << band[i];
    out << std::setw(8)  << element[i];
    out << std::setw(15) << validpixels[i];
    if (validpixels[i] > 0) {
      out << std::setw(15) << average[i];
      out << std::setw(15) << median[i];
      //Make sure the table's SD is 0 for RowCols with 1 or less valid pixels
      if( validpixels[i] > 1 ) {
        out << std::setw(15) << stddev[i];
      }
      else {
        out << std::setw(15) << 0;
      }
      out << std::setw(15) << minimum[i];
      out << std::setw(15) << maximum[i];
    }
    else {
      out << std::setw(15) << 0;
      out << std::setw(15) << 0;
      out << std::setw(15) << 0;
      out << std::setw(15) << 0;
      out << std::setw(15) << 0;
    }
    out << endl;
  }
  out.close();
}

//********************************************************
// Gather statistics from a PVL input file
//*******************************************************
void PVLIn(const Isis::Filename &filename){
  Pvl pvlFileIn;
  pvlFileIn.Read(filename.Name());
  PvlGroup results = pvlFileIn.FindGroup("Results");
  PvlObject::PvlKeywordIterator itr = results.Begin();
  
  while (itr != results.End()){
    band.push_back((*itr)[0]);
    itr++;
    element.push_back((*itr)[0]);
    itr++;
    validpixels.push_back((*itr)[0]);
    itr++;
    average.push_back((*itr)[0]);
    itr++;
    median.push_back((*itr)[0]);
    itr++;
    stddev.push_back((*itr)[0]);
    itr++;
    minimum.push_back((*itr)[0]);
    itr++;
    maximum.push_back((*itr)[0]);
    itr++;
  }
}

//********************************************************
// Gather statistics from a table input file
//*******************************************************
void tableIn(const Isis::Filename & filename){
  ifstream in;
  string expanded(filename.Expanded());
  in.open(expanded.c_str(),std::ios::in);
  
  
  if (!in){
    string message = "Error opening " + filename.Expanded();
    throw  iException::Message(Isis::iException::Io, message,
    	 _FILEINFO_);
  }
  
  //skip the header (106 bytes)
  in.seekg(106);
  

  //read it
  iString inString;
  while (in >> inString){
    band.push_back(inString);
    in>> inString;
    element.push_back(inString);
    in>> inString;
    validpixels.push_back(inString);
    in >> inString;
    average.push_back(inString);
    in >> inString;
    median.push_back(inString);
    in >> inString;
    stddev.push_back(inString);
    in >> inString;
    minimum.push_back(inString);
    in >> inString;
    maximum.push_back(inString);
    //Make sure Standard Deviation is not < 0 when reading in from a table
    vector<double>::iterator p;
    p = stddev.end() - 1;
    if( *p < 0 ) {
      *p = 0;
    }
  }
  in.close();
}

//********************************************************
// Compute coefficients such that when we subtract/divide 
// using the coefficient the average or median of the 
// output image stays the same
void keepSame(int &totalBands,int &rowcol,Mode mode) {
  // Loop for each band
  for (int iband=1; iband<=totalBands; iband++) {
    double sumAverage = 0.0;
    double sumValidPixels = 0;
    for (int i=0; i<rowcol; i++) {
      int index = (iband - 1) * rowcol + i;
      if (IsValidPixel(normalizer[index])) {
        sumAverage += normalizer[index] * validpixels[index];
        sumValidPixels += validpixels[index];
      }
    }

    // Neither sumValidPixels nor totalAverage will be zero
    // because of a test done earlier in IsisMain
    double totalAverage = sumAverage/sumValidPixels;
    for (int i=0; i<rowcol; i++) {
      int index = (iband - 1) * rowcol + i;
      if (IsValidPixel(normalizer[index])) {
        if (mode == SUBTRACT) {
          normalizer[index] = normalizer[index] - totalAverage;
        } else {
          normalizer[index] = normalizer[index] / totalAverage;
	}
      }
    }
  }
}

// Apply coefficients multiplicatively
void multiply(Buffer &in, Buffer &out) {
  // Compute the index into the normalizer array
  // We have to tweak the index based on the shape of the buffer
  // either a column or line
  int index;
  if (in.SampleDimension() == 1) {
    index = (in.Band() - 1) * totalSamples;   // Get to the proper band
    index += in.Sample() - 1;                 // Get to the proper column
  }
  else {
    index = (in.Band() - 1) * totalLines;     // Get to the proper band
    index += in.Line() - 1;                   // Get to the proper row
  }
  double coeff = normalizer[index];

  // Now loop and apply the coefficents
  for (int i=0; i<in.size(); i++) {
    if (IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = Null;
      if (coeff != 0.0 && IsValidPixel(coeff)) {
        out[i] = in[i]/coeff;
      }
    }
  }
}

// Apply coefficients subtractively
void subtract(Buffer &in, Buffer &out) {
  // Compute the index into the normalizer array
  // We have to tweak the index based on the shape of the buffer
  // either a column or line
  int index;
  if (in.SampleDimension() == 1) {
    index = (in.Band() - 1) * totalSamples;   // Get to the proper band
    index += in.Sample() - 1;                 // Get to the proper column
  }
  else {
    index = (in.Band() - 1) * totalLines;     // Get to the proper band
    index += in.Line() - 1;                   // Get to the proper row
  }
  double coeff = normalizer[index];

  // Now loop and apply the coefficents
  for (int i=0; i<in.size(); i++) {
    if (IsSpecial(in[i])) {
      out[i] = in[i];
    }
    else {
      out[i] = Null;
      if (IsValidPixel(coeff)) {
        out[i] = in[i] - coeff;
      }
    }
  }
}

// Perform lowpass and highpass filters on statistics
void filterStats(vector<double> &filter, int &filtsize, bool &pause_crop,
      int &channel) {
  int fsize = (int)filter.size();
  const int left_cut = 4;
  const int right_cut = 4;
  const int ch_pause_cnt = 3;
  const int ch_pause[2][ch_pause_cnt] = {{252,515,778},{247,510,773}};
  const int ch_width[2][ch_pause_cnt] = {{11,11,11},{11,11,11}};
  const string ch_direc[2] = {"RIGHT","LEFT"};
  const int iterations = 10;
  vector<double> filtin;
  vector<double> filtout;
  vector<double> filtorig;
  VecFilter vfilter;

  filtorig = filter;

  // To avoid filter ringing, cut out those areas in the data that
  // are especially problematic such as the left and right edges and
  // at the pause points
  for (int i=0; i<=left_cut-1; i++) {
    filter[i] = 0.0;
  }
  for (int i=fsize-1; i>=fsize-right_cut; i--) {
    filter[i] = 0.0;
  }

  // Zero out the pause point pixels if requested and the input
  // image file has a bin mode of 1
  if (pause_crop && fsize == 1024) {
    for (int i=0; i<ch_pause_cnt; i++) {
      int i1;
      int i2;
      if (ch_direc[channel] == "LEFT") {
        i1 = ch_pause[channel][i] - ch_width[channel][i];
	i2 = ch_pause[channel][i] - 1;
      } else {
        i1 = ch_pause[channel][i] - 1;
	i2 = ch_pause[channel][i] + ch_width[channel][i] - 2;
      }
      if (i1 < 0) i1 = 0;
      if (i2 > fsize-1) i2 = fsize - 1;
      for (int j=i1; j<=i2; j++) {
        filter[j] = 0.0;
      }
    }
  }

  // Here is the boxfilter - the outer most loop is for the number
  // of filter iterations
  filtin = filter;

  for (int pass=1; pass<=3; pass++) {
    for (int it=1; it<=iterations; it++) {
      filtout = vfilter.LowPass(filter,filtsize);
      filter = filtout;
    }

    // Zero out any columns that are different from the average by more
    // than a specific percent
    if (pass < 3) {
      double frac = .25;
      if (pass == 2) frac = .125;
      for (int k=0; k<fsize; k++) {
        if (filter[k] != 0.0 && filtorig[k] != 0.0) {
	  if (fabs(filtorig[k] - filter[k])/filter[k] > frac) {
	    filtin[k] = 0.0;
	  }
	}
      }
      filter = filtin;
    }
  }

  // Perform the highpass by differencing the original from the lowpass
  filter = vfilter.HighPass(filtorig,filtout);

  filtin.clear();
  filtout.clear();
  filtorig.clear();
}
