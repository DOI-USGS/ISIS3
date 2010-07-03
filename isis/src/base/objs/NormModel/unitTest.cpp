#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "NormModel.h"
#include "NormModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;

void doit(Pvl &lab,PhotoModel &pm);
void doit(Pvl &lab,PhotoModel &pm,AtmosModel &am);

int main () {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for Isis::NormModel" << std::endl << std::endl; 

  Pvl lab;
  lab.AddObject(PvlObject("PhotometricModel"));
  lab.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").AddKeyword(PvlKeyword("Name","Lambert"));
  PhotoModel *pm = PhotoModelFactory::Create(lab);
  
  lab.AddObject(PvlObject("AtmosphericModel"));
  lab.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").AddKeyword(PvlKeyword("Name","Anisotropic1"));
  AtmosModel *am = AtmosModelFactory::Create(lab,*pm);

  std::cout << "Testing missing NormalizationModel object ..." << std::endl;
  
  doit(lab,*pm);
  doit(lab,*pm,*am);

  lab.AddObject(PvlObject("NormalizationModel"));
  std::cout << "Testing missing Algorithm group ..." << std::endl;
  
  doit(lab,*pm);
  doit(lab,*pm,*am);

  lab.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));

  std::cout << "Testing missing Name keyword ..." << std::endl;
  doit(lab,*pm);
  doit(lab,*pm,*am);

  lab.FindObject("NormalizationModel").FindGroup("Algorithm").AddKeyword(PvlKeyword("Name","Albedo"), Pvl::Replace);
  std::cout << "Testing supported normalization model ..." << std::endl;
  doit(lab,*pm);
  doit(lab,*pm,*am);

  NormModel *nm = NormModelFactory::Create(lab,*pm);

  try {
    std::cout << "Testing normalization model get methods ..." << std::endl;
    std::cout << "AlgorithmName = " << nm->AlgorithmName() << std::endl;
  } 
  catch (iException &e) {
    e.Report(false);
  }
  
  std::cout << std::endl;

  return 0;
}

void doit(Pvl &lab,PhotoModel &pm) {
  try {
    NormModelFactory::Create(lab,pm);
  }
  catch (iException &error) {
    error.Report(false);
  }
  std::cout << std::endl;
}

void doit(Pvl &lab,PhotoModel &pm,AtmosModel &am) {
  try {
    NormModelFactory::Create(lab,pm,am);
  }
  catch (iException &error) {
    error.Report(false);
  }
  std::cout << std::endl;
}
