#include <iostream>
#include <cstdlib>
#include "Photometry.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "Preference.h"

using namespace Isis;

// Function f(x) = \cos(x) + 1 
double fn1 (double x, void *params);

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for Isis::Photometry" <<
            std::endl << std::endl;

  Pvl lab;
  lab.AddObject(PvlObject("PhotometricModel"));
  lab.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Name", "Minnaert"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Wh", "0.52"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("B0", "0.0"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hh", "0.0"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Theta", "30.0"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hg1", "0.213"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hg2", "1.0"));

  lab.AddObject(PvlObject("AtmosphericModel"));
  lab.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Name", "Anisotropic1"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Tau", "0.28"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Tauref", "0.001"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Wha", "0.95"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Bha", "0.85"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hga", "0.68"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hnorm", "0.003"));
  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Nulneg", "NO"));

  lab.AddObject(PvlObject("NormalizationModel"));
  lab.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Name", "Albedo"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Incref", "30.0"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Albedo", "0.0690507"));
  lab.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Thresh", "30.0"));

  Pvl labdem;
  labdem.AddObject(PvlObject("PhotometricModel"));
  labdem.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Name", "Lambert"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Wh", "0.52"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("B0", "0.0"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hh", "0.0"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Theta", "30.0"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hg1", "0.213"));
  labdem.FindObject("PhotometricModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hg2", "1.0"));

  labdem.AddObject(PvlObject("AtmosphericModel"));
  labdem.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Name", "Anisotropic1"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Tau", "0.28"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Tauref", "0.001"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Wha", "0.95"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Bha", "0.85"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hga", "0.68"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Hnorm", "0.003"));
  labdem.FindObject("AtmosphericModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Nulneg", "NO"));

  labdem.AddObject(PvlObject("NormalizationModel"));
  labdem.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Name", "AlbedoAtm"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Incref", "0.0"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Albedo", "0.0690507"));
  labdem.FindObject("NormalizationModel").FindGroup("Algorithm").
  AddKeyword(PvlKeyword("Thresh", "30.0"));

  std::cout << "Testing creation of photometry object ..." << std::endl;
  try {
    double albedo, mult, base;
    Photometry *pho = new Photometry(lab);
    Photometry *phodem = new Photometry(labdem);
    std::cout << "Testing photometry method without dem ..." << std::endl;
    pho->Compute(86.722672229212051, 51.7002388445338, 38.94144389777756,
                 51.7002388445338, 38.94144389777756, 0.080061890184879303, albedo, mult, base);
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
              std::endl;
    pho->Compute(86.7207248, 51.7031305, 38.9372914, 51.7031305, 38.9372914, .0797334611, albedo, mult,
                 base);
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
              std::endl;
    std::cout << "Testing photometry method with dem ..." << std::endl;
    phodem->Compute(86.7226722, 51.7002388, 38.9414439, 51.7002388, 38.9414439,
                    .0800618902, albedo, mult, base);
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
              std::endl;
    phodem->Compute(86.7207248, 51.7031305, 38.9372914, 51.7031305, 38.9372914,
                    .0797334611, albedo, mult, base);
    std::cout << "Photometric brightness value = " << albedo << std::endl <<
              std::endl;

    std::cerr << "\n***** Testing One dimensional Minimizations using GSL's brentminimizer *****\n";
    
    gsl_function F;
    
    F.function = &fn1;
    F.params = 0;
  
    double xa = 0, xb = 6;
    std::cerr << "xa = " << xa << " xb = " << xb << "\n\n";
    double x_minimum = 2;
    std::cerr << "Without using minbracket, Starting Minimum\nTest Minimum=" << x_minimum << "\n";
    Photometry::brentminimizer(xa, xb, &F, x_minimum);
    std::cerr << "brentminimizer's Converged Minimum = " << x_minimum << std::endl;
    
    std::cerr << "\nUsing minbracket for Starting Minimum\n";
    double xc = 0;
    double fxa, fxb, fxc;
    Photometry::minbracket(xa, xb, xc, fxa, fxb, fxc, F.function, F.params);
    std::cerr << "minbracket Minimum=" << xb << "\n";
    Photometry::brentminimizer(xa, xc, &F, xb);
    std::cerr << "brentminimizer's Converged Minimum = " << xb << std::endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  return 0;
}

// Function f(x) = \cos(x) + 1
// r8Brent's Minimization finds the minimum for this function
double fn1 (double x, void *params) {
  
  return cos (x) + 1.0;
}

