#include "Isis.h"

#include <string>

#include "ProcessMapMosaic.h"
#include "Application.h"
#include "CubeAttribute.h"
#include "FileList.h"
#include "ProcessMosaic.h"
#include "LineManager.h"

using namespace std;

void IsisMain() {

  Isis::Preference::Preferences(true);

  cout << "Testing Isis::ProcessMapMosaic Class ... " << endl;

  // Create the temp parent cube
  Isis::FileList cubes;
  cubes.Read("unitTest.lis");

  std::cout << "Testing Mosaic 1" << std::endl;
  Isis::ProcessMapMosaic m1;
  Isis::CubeAttributeOutput oAtt;
  MosaicPriority priority = input;
  m1.SetBandBinMatch(false);
  m1.SetOutputCube(cubes, oAtt, "./unitTest.cub");

  //set priority
  m1.SetPriority(priority);

  for(unsigned int i = 0; i < cubes.size(); i++) {
    if(m1.StartProcess(cubes[i])) {
      std::cout << cubes[i] << " is inside the mosaic" << std::endl;
    }
    else {
      std::cout << cubes[i] << " is outside the mosaic" << std::endl;
    }
  }

  m1.EndProcess();
  std::cout << "Mosaic label: " << std::endl;
  Isis::iApp->Exec("catlab", "FROM=./unitTest.cub");

  remove("./unitTest.cub");

  std::cout << "Testing Mosaic 2" << std::endl;
  Isis::ProcessMapMosaic m2;
  m2.SetBandBinMatch(false);
  m2.SetOutputCube(cubes, -6, -4, 29, 31, oAtt, "./unitTest.cub");

  //set priority
  m2.SetPriority(priority);

  for(unsigned int i = 0; i < cubes.size(); i++) {
    if(m2.StartProcess(cubes[i])) {
      std::cout << cubes[i] << " is inside the mosaic" << std::endl;
    }
    else {
      std::cout << cubes[i] << " is outside the mosaic" << std::endl;
    }
  }

  m2.EndProcess();
  std::cout << "Mosaic label: " << std::endl;

  Isis::iApp->Exec("catlab", "FROM=./unitTest.cub");


  Isis::Cube tmp;
  tmp.Open("./unitTest.cub");
  Isis::LineManager lm(tmp);
  lm.SetLine(1, 1);

  while(!lm.end()) {
    tmp.Read(lm);
    std::cout << "Mosaic Data: " << lm[lm.SampleDimension()/4] << '\t' <<
              lm[lm.SampleDimension()/2] << '\t' <<
              lm[(3*lm.SampleDimension())/4] << std::endl;
    lm++;
  }

  tmp.Close();
  remove("./unitTest.cub");
}



