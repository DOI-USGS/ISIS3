#include "HapkePhotoModel.h"
#include "Preference.h"
#include "iException.h"

using namespace Isis;

class TestHapke : public HapkePhotoModel {
  public:
    TestHapke(Pvl &alg) : HapkePhotoModel(alg) {}
  
  protected:
    virtual double PhotoModelAlgorithm(double,double,double) {
      return 0.0;
    }
};

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "HapkePhotoModel unit test" << std::endl << std::endl;

  std::cout << "Test No Model" << std::endl;
  Pvl alg;
  try {
    TestHapke model(alg);
    std::cout << "SUCCESS" << std::endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }

  alg += PvlObject("PhotometricModel");
  alg.FindObject("PhotometricModel") += PvlGroup("Algorithm");

  std::cout << std::endl << "Test Defaults" << std::endl;
  try {
    std::cout << alg << std::endl;
    TestHapke model(alg);
    std::cout << "SUCCESS" << std::endl;
    std::cout << std::endl;
    std::cout << model.PhotoWh() << "," 
              << model.PhotoHh() << "," 
              << model.PhotoB0() << "," 
              << model.PhotoTheta() << "," 
              << model.Hfunc(1,2) << std::endl; 
    std::cout << std::endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }

  alg.FindObject("PhotometricModel").FindGroup("Algorithm") += PvlKeyword("Name", "Test");
  alg.FindObject("PhotometricModel").FindGroup("Algorithm") += PvlKeyword("Wh", 1);
  alg.FindObject("PhotometricModel").FindGroup("Algorithm") += PvlKeyword("Hh", 2);
  alg.FindObject("PhotometricModel").FindGroup("Algorithm") += PvlKeyword("B0", 3);
  alg.FindObject("PhotometricModel").FindGroup("Algorithm") += PvlKeyword("Theta", 4);

  std::cout << "Test Initialized" << std::endl;
  try {
    std::cout << alg << std::endl;
    TestHapke model(alg);
    std::cout << "SUCCESS" << std::endl;
    std::cout << std::endl;
    std::cout << model.PhotoWh() << "," 
              << model.PhotoHh() << "," 
              << model.PhotoB0() << "," 
              << model.PhotoTheta() << "," 
              << model.Hfunc(1,2) << std::endl; 
    std::cout << std::endl;
  }
  catch (iException &e) {
    e.Report(false);
    e.Clear();
  }

  return 0;
}
