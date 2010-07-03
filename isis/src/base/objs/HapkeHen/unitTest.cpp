#include <iostream>
#include <cstdlib>
#include "HapkeHen.h"
#include "PhotoModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"

using namespace Isis;

int main () {
  std::cout << "UNIT TEST for HapkeHen photometric function" << 
      std::endl << std::endl;

  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name","HapkeHen");
  algp += PvlKeyword("Wh",0.52);
  algp += PvlKeyword("B0",0.0);
  algp += PvlKeyword("Hh",0.0);
  algp += PvlKeyword("Theta",30.0);
  algp += PvlKeyword("Hg1",0.213);
  algp += PvlKeyword("Hg2",1.0);

  PvlObject op("PhotometricModel");
  op.AddGroup(algp);

  Pvl pvl;
  pvl.AddObject(op);
  std::cout << pvl << std::endl << std::endl;

  try {
    PhotoModel *pm = PhotoModelFactory::Create(pvl);
  
    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
        std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0,0.0,0.0) << std::endl << std::endl;
  
    std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
        std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0,45.0,30.0) << std::endl << std::endl;
  
    std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
        std::endl;
    std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0,90.0,90.0) << std::endl << std::endl;
  }
  catch (iException &e) {
    e.Report();
  }

  return 0;
}
