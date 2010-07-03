#include "Isis.h"
#include "ProcessByLine.h"
#include "HiLab.h"
#include "MultivariateStatistics.h"
#include "PvlGroup.h"

#include <sstream>

using namespace std; 
using namespace Isis;

int channel, bin;
double correlation;
struct furrow {
  int startSample;
  int endSample;
  int increment;
  vector<MultivariateStatistics> mvstats;
};
vector<furrow> furrows;
  
void furrowCheck(Buffer &in, Buffer &out);
void getStats(Buffer &in);

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;
  
  // Setup the input and output cubes
  Cube* icube = p.SetInputCube("FROM");
  
  UserInterface &ui = Application::GetUserInterface();
  correlation = ui.GetDouble("CORRELATION");
  HiLab hiInfo(icube);
  channel = hiInfo.getChannel();
  bin = hiInfo.getBin();

  // Bin 1 images have up to four furrows; others have only one
  if(bin == 1) {
    // Numbers derived from TRA_000827_0985
    furrows.resize(4);
    furrows[0].startSample = 5;
    furrows[0].endSample = 1;
    furrows[0].increment = -1;
    
    furrows[1].startSample = 255;
    furrows[1].endSample = 251;
    furrows[1].increment = -1;
    
    furrows[2].startSample = 518;
    furrows[2].endSample = 514;
    furrows[2].increment = -1;
    
    furrows[3].startSample = 781;
    furrows[3].endSample = 777;
    furrows[3].increment = -1;
  }
  else {
    furrows.resize(1);
    if(channel == 0) {
      furrows[0].startSample = 5;
      furrows[0].endSample = 1;
      furrows[0].increment = -1;
    }
    else if(channel == 1) {
      furrows[0].startSample = icube->Samples() - 5 + 1;
      furrows[0].endSample = icube->Samples();
      furrows[0].increment = 1;
    }
    else {
      string msg = "Cannot process merged images.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }
  
  for(int i = 0; i < (int)furrows.size(); i++) {
    int n = abs(furrows[i].startSample - furrows[i].endSample);
    furrows[i].mvstats.resize(n);
  }
  
  p.StartProcess(getStats);
  PvlGroup stats("Correlations");
  p.SetOutputCube ("TO");
    
  // Add correlation data to cube label  
  for(int i = 0; i < (int)furrows.size(); i++) {
    for(int j = 0; j < (int)furrows[i].mvstats.size(); j++) {
      string label, begin, finish;
      stringstream ss; 

      if(channel == 0) {
        ss << furrows[i].startSample-j;
        ss >> begin;  
        ss.clear();
        ss << furrows[i].startSample-j-1;
        ss >> finish;
      }
      else if(channel == 1) {
        ss << furrows[i].startSample+j;
        ss >> begin; 
        ss.clear();
        ss << furrows[i].startSample+j+1;
        ss >> finish;
      }
      label += "Column" + begin + "to" + finish;
      stats += PvlKeyword(label, furrows[i].mvstats[j].Correlation());
    }
  }
  Application::Log(stats);
  
  p.StartProcess(furrowCheck);
  p.EndProcess();
}

// Populate mvstats vector with column data (first or last five columns)
void getStats(Buffer &in) {
  for(int i = 0; i < (int)furrows.size(); i++) {
    for(int j = 0; j < (int)furrows[i].mvstats.size(); j++) {
      int index = furrows[i].startSample + j * furrows[i].increment - 1;
      furrows[i].mvstats[j].AddData(&in[index], 
                                    &in[index+furrows[i].increment], 1);
    }
  }
}

void furrowCheck(Buffer &in, Buffer &out) {
  bool bFurrows = false;
  // Copy first, then correct furrows
  for(int i = 0; i < in.size(); i++) {
    out[i] = in[i];
  }

  for(int i = 0; i < (int)furrows.size(); i++) {
    for(int j = (int)furrows[i].mvstats.size(); j > 0; j--) {
      if(furrows[i].mvstats[j-1].Correlation() < correlation || bFurrows == true) {
        bFurrows = true;
        if(channel == 0) {
          out[furrows[i].startSample-j-1] = Isis::Null;
        }
        else {
          out[furrows[i].startSample+j-1] = Isis::Null;
        }
      }
    }
  }
}
