#include "Isis.h"

#include <string>

#include "ProcessMapMosaic.h"
#include "Application.h"
#include "CubeAttribute.h"
#include "FileList.h"
#include "ProcessMosaic.h"
#include "ProgramLauncher.h"
#include "LineManager.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  Preference::Preferences(true);

  cout << "Testing Isis::ProcessMapMosaic Class ... " << endl;

  // Create the temp parent cube
  FileList cubes;
  cubes.Read("unitTest.lis");

  cout << "Testing Mosaic 1" << endl;
  ProcessMapMosaic m1;
  CubeAttributeOutput oAtt;
  ProcessMapMosaic::MosaicPriority priority = ProcessMapMosaic::input;
  m1.SetBandBinMatch(false);
  m1.SetOutputCube(cubes, oAtt, "./unitTest.cub");

  //set priority
  m1.SetPriority(priority);

  for(unsigned int i = 0; i < cubes.size(); i++) {
    if(m1.StartProcess(cubes[i])) {
      cout << cubes[i] << " is inside the mosaic" << endl;
    }
    else {
      cout << cubes[i] << " is outside the mosaic" << endl;
    }
  }

  m1.EndProcess();
  cout << "Mosaic label: " << endl;
  ProgramLauncher::RunIsisProgram("catlab", "FROM=./unitTest.cub");

  remove("./unitTest.cub");

  cout << "Testing Mosaic 2" << endl;
  ProcessMapMosaic m2;
  m2.SetBandBinMatch(false);
  m2.SetOutputCube(cubes, -6, -4, 29, 31, oAtt, "./unitTest.cub");

  //set priority
  m2.SetPriority(priority);

  for(unsigned int i = 0; i < cubes.size(); i++) {
    if(m2.StartProcess(cubes[i])) {
      cout << cubes[i] << " is inside the mosaic" << endl;
    }
    else {
      cout << cubes[i] << " is outside the mosaic" << endl;
    }
  }

  m2.EndProcess();
  cout << "Mosaic label: " << endl;

  ProgramLauncher::RunIsisProgram("catlab", "FROM=./unitTest.cub");


  Cube tmp;
  tmp.Open("./unitTest.cub");
  LineManager lm(tmp);
  lm.SetLine(1, 1);

  while(!lm.end()) {
    tmp.Read(lm);
    cout << "Mosaic Data: " << lm[lm.SampleDimension()/4] << '\t' <<
              lm[lm.SampleDimension()/2] << '\t' <<
              lm[(3*lm.SampleDimension())/4] << endl;
    lm++;
  }

  tmp.Close();
  remove("./unitTest.cub");
}



