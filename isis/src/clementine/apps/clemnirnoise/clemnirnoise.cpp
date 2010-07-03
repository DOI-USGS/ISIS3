#include "Isis.h"

#include <cmath>

#include "ProcessByBrick.h"
#include "Pixel.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

void NoiseFilter(Buffer &in, Buffer &out); 

void IsisMain() {
  // We will be processing by brick
  ProcessByBrick p;
 // UserInterface &ui = Application::GetUserInterface();

  // Setup the input and output cubes
  Cube *icube = p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  // Start the processing
  p.SetBrickSize(icube->Samples(), icube->Lines(), 1);
  p.StartProcess(NoiseFilter);
  p.EndProcess();
}

void NoiseFilter(Buffer &in, Buffer &out) {
  double TOL1 = 9.0;
  double TOL2 = 3.0;
  int NPOS = 100;
  double diff;
  double diffSum;
  double diffAvg;
  int dCount;
  int LCOUNT = 0;

  for(int il = 0; il < in.LineDimension();il++){
    for(int is = 0; is < in.SampleDimension();is++){
      int index = in.SampleDimension() * il + is;
      out[index] = in[index];
    }
    // Determine if a noise pattern exists samples 2,6,10,14,...
    diffSum = 0.0;
    dCount = 0;
    for(int is = 1; is < in.SampleDimension()-4;is+=4){
      int index = in.SampleDimension() * il + is;
      if(!Pixel::IsSpecial(in[index]) &&
         !Pixel::IsSpecial(in[index]) &&
         !Pixel::IsSpecial(in[index])){
        diff = abs((in[index]-in[index-1]) + (in[index]-in[index+1]));
        diffSum += diff;
        dCount += 2;
      }
    }
    diffAvg = 0.0;
    if (dCount > 1) {
      diffAvg = diffSum / dCount;
    }
    if (diffAvg > TOL1){
      LCOUNT++;
      for(int is = 1; is < in.SampleDimension()-4;is+=4){
        int index = in.SampleDimension() * il + is;
        out[index] = Isis::Null;
      }
    }

    // Determine if a noise pattern exists samples 4,8,12,16,...
    diffSum = 0.0;
    dCount = 0;
    for(int is = 4; is < in.SampleDimension()-4;is+=4){
      int index = in.SampleDimension() * il + is;
      if(!Pixel::IsSpecial(in[index]) &&
         !Pixel::IsSpecial(in[index]) &&
         !Pixel::IsSpecial(in[index])){
        diff = (in[index]-in[index-1]) + (in[index]-in[index+1]);
        if(diff < 0.0){
          diffSum += diff;
          dCount += 2;
        }
      }
    }
    diffAvg = 0.0;
    if (dCount > 1) {
      diffAvg = diffSum / dCount;
    }
    if (diffAvg > TOL2 && dCount > NPOS){
      LCOUNT++;
      for(int is = 4; is < in.SampleDimension()-4;is+=4){
        int index = in.SampleDimension() * il + is;
        out[index] = Isis::Null;
      }
    }
  }
}
