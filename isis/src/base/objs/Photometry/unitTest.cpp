#include <iostream>
#include <cstdlib>
#include "Photometry.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;

int main () {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for Isis::Photometry" << 
      std::endl << std::endl;

  Pvl lab;
  lab.AddObject(PvlObject("PhotometricModel"));
  lab.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Name","Minnaert"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Wh","0.52"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("B0","0.0"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hh","0.0"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Theta","30.0"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hg1","0.213"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hg2","1.0"));
  
  lab.AddObject(PvlObject("AtmosphericModel"));
  lab.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Name","Anisotropic1"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Tau","0.28"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Tauref","0.001"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Wha","0.95"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Bha","0.85"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hga","0.68"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hnorm","0.003"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Nulneg","NO"));

  lab.AddObject(PvlObject("NormalizationModel"));
  lab.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Name","Albedo"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Incref","30.0"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Albedo","0.0690507"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Thresh","30.0"));

  Pvl labdem;
  labdem.AddObject(PvlObject("PhotometricModel"));
  labdem.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Name","Lambert"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Wh","0.52"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("B0","0.0"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hh","0.0"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Theta","30.0"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hg1","0.213"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hg2","1.0"));
  
  labdem.AddObject(PvlObject("AtmosphericModel"));
  labdem.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Name","Anisotropic1"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Tau","0.28"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Tauref","0.001"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Wha","0.95"));
//  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
//      AddKeyword(PvlKeyword("Wharef","0.95"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Bha","0.85"));
//  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
//      AddKeyword(PvlKeyword("Bharef","0.85"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hga","0.68"));
//  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
//      AddKeyword(PvlKeyword("Hgaref","0.68"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Hnorm","0.003"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Nulneg","NO"));

  labdem.AddObject(PvlObject("NormalizationModel"));
  labdem.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Name","AlbedoAtm"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Incref","0.0"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Albedo","0.0690507"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
      AddKeyword(PvlKeyword("Thresh","30.0"));

  std::cout << "Testing creation of photometry object ..." << std::endl;
  try {
//    double result;
    double albedo,mult,base;
    Photometry *pho = new Photometry(lab);
    Photometry *phodem = new Photometry(labdem);
    std::cout << "Testing photometry method without dem ..." << std::endl;
//    result = pho->Compute(86.722672229212051,51.7002388445338,38.94144389777756,
//        0.080061890184879303);
    pho->Compute(86.722672229212051,51.7002388445338,38.94144389777756,
        0.080061890184879303,albedo,mult,base);
//    std::cout << "Photometric brightness value = " << result << std::endl <<
//        std::endl;
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
        std::endl;
//    result = pho->Compute(86.7207248,51.7031305,38.9372914,.0797334611);
    pho->Compute(86.7207248,51.7031305,38.9372914,.0797334611,albedo,mult,
        base);
//    std::cout << "Photometric brightness value = " << result << std::endl <<
//        std::endl;
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
        std::endl;
    std::cout << "Testing photometry method with dem ..." << std::endl;
//    result = phodem->Compute(86.7226722,51.7002388,38.9414439,51.7910076,
//        39.0176048,.0800618902);
    phodem->Compute(86.7226722,51.7002388,38.9414439,
        .0800618902,albedo,mult,base);
//    std::cout << "Photometric brightness value = " << result << std::endl <<
//        std::endl;
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
        std::endl;
//    result = phodem->Compute(86.7207248,51.7031305,38.9372914,51.8776595,
//        38.9719125,.0797334611);
    phodem->Compute(86.7207248,51.7031305,38.9372914,
        .0797334611,albedo,mult,base);
//    std::cout << "Photometric brightness value = " << result << std::endl <<
//        std::endl;
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
        std::endl;
  } 
  catch (iException &e) {
    e.Report(false);
  }

  return 0;
}
