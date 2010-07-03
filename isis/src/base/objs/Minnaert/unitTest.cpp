#include <iostream>
#include <cstdlib>
#include "Minnaert.h"
#include "PhotoModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;

int main () {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for Minnaert photometric function" << 
      std::endl << std::endl;

  PvlGroup alg("Algorithm");
  alg += PvlKeyword("Name","Minnaert");

  PvlObject o("PhotometricModel");
  o.AddGroup(alg);

  Pvl pvl;
  pvl.AddObject(o);
  std::cout << pvl << std::endl << std::endl;

  try {
  Minnaert *pm = (Minnaert*)PhotoModelFactory::Create(pvl);

  std::cout << "PhotoK = " << pm->PhotoK() << std::endl;

  std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0,0.0,0.0) <<
      std::endl;
  std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0,45.0,30.0) <<
      std::endl;
  std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0,90.0,90.0) <<
      std::endl << std::endl;

  std::cout << "Test negative K value ..." << std::endl;
  try {
    pm->SetPhotoK(-1.0);
  } 
  catch (Isis::iException &e) {
    e.Report(false);
  }
  std::cout << std::endl;

  pm->SetPhotoK(0.0);
  std::cout << "PhotoK = " << pm->PhotoK() << std::endl;

  std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0,0.0,0.0) <<
      std::endl;
  std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0,45.0,30.0) <<
      std::endl;
  std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0,90.0,90.0) <<
      std::endl << std::endl;

  pm->SetPhotoK(0.5);
  std::cout << "PhotoK = " << pm->PhotoK() << std::endl;

  std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0,0.0,0.0) <<
      std::endl;
  std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0,45.0,30.0) <<
      std::endl;
  std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(180.0,90.0,90.0) <<
      std::endl << std::endl;

  pm->SetPhotoK(2.0);
  std::cout << "PhotoK = " << pm->PhotoK() << std::endl;

  std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(0.0,0.0,0.0) <<
      std::endl;
  std::cout << "Test phase=60.0, incidence=45.0, emission=30.0 ..." <<
      std::endl;
  std::cout << "Albedo = " << pm->CalcSurfAlbedo(60.0,45.0,30.0) <<
      std::endl;
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
