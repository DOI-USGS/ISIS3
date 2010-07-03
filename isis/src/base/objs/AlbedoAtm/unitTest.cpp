#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "AlbedoAtm.h"
#include "AtmosModel.h"
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

  std::cout << "UNIT TEST for AlbedoAtm normalization function" << 
      std::endl << std::endl;

  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name","Lambert");

  PvlObject op("PhotometricModel");
  op.AddGroup(algp);

  PvlGroup alga("Algorithm");
  alga += PvlKeyword("Name","Anisotropic1");
  alga += PvlKeyword("Bha","0.85");
  alga += PvlKeyword("Tau","0.28");
  alga += PvlKeyword("Wha","0.95");
  alga += PvlKeyword("Hga","0.68");
  alga += PvlKeyword("Tauref","0.0");
  alga += PvlKeyword("Hnorm","0.003");

  PvlObject oa("AtmosphericModel");
  oa.AddGroup(alga);

  PvlGroup algn("Algorithm");
  algn += PvlKeyword("Name","AlbedoAtm");
  algn += PvlKeyword("Incref","0.0");
  algn += PvlKeyword("Thresh","30.0");

  PvlObject on("NormalizationModel");
  on.AddGroup(algn);

  Pvl pvl;
  pvl.AddObject(op);
  pvl.AddObject(oa);
  pvl.AddObject(on);

  std::cout << pvl << std::endl << std::endl;

  try {
    PhotoModel *pm = PhotoModelFactory::Create(pvl);
    AtmosModel *am = AtmosModelFactory::Create(pvl,*pm);
    AlbedoAtm *nm = (AlbedoAtm*)NormModelFactory::Create(pvl,*pm,*am);
  
    std::cout << "Test phase=86.7207248, incidence=51.7031305, emission=38.9372914, " <<
        "dn=.0800618902 ..." << std::endl;
    nm->CalcNrmAlbedo(86.7207248,51.7031305,38.9372914,.0800618902,result,mult,base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  
    std::cout << "Test phase=86.7207248, incidence=51.7031305, emission=38.9372914, " <<
        "dn=.0797334611 ..." << std::endl;
    nm->CalcNrmAlbedo(86.7207248,51.7031305,38.9372914,.0797334611,result,mult,base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  
    std::cout << "Test phase=86.7187773, incidence=51.7060221, emission=38.9331391, " <<
        "dn=.0794225037 ..." << std::endl;
    nm->CalcNrmAlbedo(86.7187773,51.7060221,38.9331391,.0794225037,result,mult,base);
    std::cout << "Normalization value = " << result << std::endl << std::endl;
  }
  catch (iException &e) {
    e.Report();
  }

  return 0;
}
