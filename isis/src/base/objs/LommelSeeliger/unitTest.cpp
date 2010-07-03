#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;

int main () {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for LommelSeeliger photometric function" << 
      std::endl << std::endl;

  try {
  PvlGroup alg("Algorithm");
  alg += PvlKeyword("Name","LommelSeeliger");

  PvlObject o("PhotometricModel");
  o.AddGroup(alg);

  Pvl pvl;
  pvl.AddObject(o);
  std::cout << pvl << std::endl << std::endl;

  PhotoModel *pm = PhotoModelFactory::Create(pvl);

  std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0,0.0,0.0) <<
      std::endl << std::endl;
  std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0,45.0,30.0) <<
      std::endl << std::endl;
  std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0,90.0,90.0) <<
      std::endl << std::endl;
  }
  catch (iException &e) {
    e.Report();
  }

  return 0;
}
