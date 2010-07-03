#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "MoonAlbedo.h"
#include "NormModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;

int main () {
  Isis::Preference::Preferences(true);
  double result;
  double mult;
  double base;

  std::cout << "UNIT TEST for MoonAlbedo normalization function" << 
      std::endl << std::endl;

  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name","LunarLambertMcEwen");

  PvlObject op("PhotometricModel");
  op.AddGroup(algp);

  PvlGroup algn("Algorithm");
  algn += PvlKeyword("Name","MoonAlbedo");
  algn += PvlKeyword("D","0.0");
  algn += PvlKeyword("E","-0.218");
  algn += PvlKeyword("F","0.5");
  algn += PvlKeyword("G2","0.4");
  algn += PvlKeyword("H","0.054");
  algn += PvlKeyword("Bsh1","1.6");

  PvlObject on("NormalizationModel");
  on.AddGroup(algn);

  Pvl pvl;
  pvl.AddObject(op);
  pvl.AddObject(on);

  std::cout << pvl << std::endl << std::endl;

  try {
    PhotoModel *pm = PhotoModelFactory::Create(pvl);
    MoonAlbedo *nm = (MoonAlbedo*)NormModelFactory::Create(pvl,*pm);
  
    std::cout << "Test phase=86.7207248, incidence=51.7031305, emission=38.9372914, " <<
        "dn=.0800618902 ..." << std::endl;
    nm->CalcNrmAlbedo(86.7207248,51.7031305,38.9372914,.0800618902,result,mult,base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  
    std::cout << "Test phase=75.7207248, incidence=41.7031305, emission=28.9372914, " <<
        "dn=.0697334611 ..." << std::endl;
    nm->CalcNrmAlbedo(75.7207248,41.7031305,28.9372914,.0697334611,result,mult,base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  
    std::cout << "Test phase=53.7187773, incidence=31.7060221, emission=18.9331391, " <<
        "dn=.0194225037 ..." << std::endl;
    nm->CalcNrmAlbedo(53.7187773,31.7060221,18.9331391,.0194225037,result,mult,base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  }
  catch (iException &e) {
    e.Report();
  }

  return 0;
}
