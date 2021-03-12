/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include <cstdlib>
#include "Photometry.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;

// Function f(x) = \cos(x) + 1 
double fn1 (double x, void *params);

// Test brentsolver to find the root of the quadratic equation
struct quadratic_params {
  double a, b, c;
};
double quadratic (double x, void *params);

int main() {
  Isis::Preference::Preferences(true);

  std::cout << "UNIT TEST for Isis::Photometry" <<
            std::endl << std::endl;

  Pvl lab;
  lab.addObject(PvlObject("PhotometricModel"));
  lab.findObject("PhotometricModel").addGroup(PvlGroup("Algorithm"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Minnaert"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Wh", "0.52"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("B0", "0.0"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hh", "0.0"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Theta", "30.0"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hg1", "0.213"));
  lab.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hg2", "1.0"));

  lab.addObject(PvlObject("AtmosphericModel"));
  lab.findObject("AtmosphericModel").addGroup(PvlGroup("Algorithm"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Anisotropic1"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Tau", "0.28"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Tauref", "0.001"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Wha", "0.95"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Bha", "0.85"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hga", "0.68"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hnorm", "0.003"));
  lab.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Nulneg", "NO"));

  lab.addObject(PvlObject("NormalizationModel"));
  lab.findObject("NormalizationModel").addGroup(PvlGroup("Algorithm"));
  lab.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Albedo"));
  lab.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Incref", "30.0"));
  lab.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Albedo", "0.0690507"));
  lab.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Thresh", "30.0"));

  Pvl labdem;
  labdem.addObject(PvlObject("PhotometricModel"));
  labdem.findObject("PhotometricModel").addGroup(PvlGroup("Algorithm"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Lambert"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Wh", "0.52"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("B0", "0.0"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hh", "0.0"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Theta", "30.0"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hg1", "0.213"));
  labdem.findObject("PhotometricModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hg2", "1.0"));

  labdem.addObject(PvlObject("AtmosphericModel"));
  labdem.findObject("AtmosphericModel").addGroup(PvlGroup("Algorithm"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "Anisotropic1"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Tau", "0.28"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Tauref", "0.001"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Wha", "0.95"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Bha", "0.85"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hga", "0.68"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Hnorm", "0.003"));
  labdem.findObject("AtmosphericModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Nulneg", "NO"));

  labdem.addObject(PvlObject("NormalizationModel"));
  labdem.findObject("NormalizationModel").addGroup(PvlGroup("Algorithm"));
  labdem.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Name", "AlbedoAtm"));
  labdem.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Incref", "0.0"));
  labdem.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Albedo", "0.0690507"));
  labdem.findObject("NormalizationModel").findGroup("Algorithm").
  addKeyword(PvlKeyword("Thresh", "30.0"));

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
    
    gsl_function Func;
    
    Func.function = &fn1;
    Func.params   = 0;
  
    double xa = 0, xb = 6;
    std::cerr << "xa = " << xa << " xb = " << xb << "\n\n";
    double x_minimum = 2;
    std::cerr << "Without using minbracket, Starting Minimum\nTest Minimum=" << x_minimum << "\n";
    Photometry::brentminimizer(xa, xb, &Func, x_minimum, .001);
    std::cerr << "brentminimizer's Converged Minimum = " << x_minimum << std::endl;
    
    std::cerr << "\nUsing minbracket for Starting Minimum\n";
    double xc = 0;
    double fxa, fxb, fxc;
    Photometry::minbracket(xa, xb, xc, fxa, fxb, fxc, Func.function, Func.params);
    std::cerr << "minbracket Minimum=" << xb << "\n";
    Photometry::brentminimizer(xa, xc, &Func, xb, .001);
    std::cerr << "brentminimizer's Converged Minimum = " << xb << std::endl;
    
    
    std::cerr << "\n***** Testing Brent's Root Bracketing Algorithm *****\n";
    struct quadratic_params qparams;
    qparams.a = 1;
    qparams.b = 0;
    qparams.c = -5;
    Func.function = &quadratic;
    Func.params   = &qparams;
    
    double x_lo = 0;
    double x_hi = 5;
    double root = 0;
    double tolerance = 0.001;
    Photometry::brentsolver(x_lo, x_hi, &Func, tolerance, root);
    std::cerr << "Initial lower search interval = " << x_lo << "\n";
    std::cerr << "Initial higher search interval = " << x_hi << "\n";
    std::cerr << "Tolerance = " << tolerance << "\n";
    std::cerr << "brentsolvers Root = " << root << "\n";
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}

// Function f(x) = \cos(x) + 1
// brentminimizer Algorithm finds the minimum for this function
double fn1 (double x, void *params) {
  
  return cos (x) + 1.0;
}


// Function to test Brent's Root Bracketing Algorithm
double quadratic (double x, void *params) {
  struct quadratic_params *p = (struct quadratic_params *) params;
  
  double a = p->a;
  double b = p->b;
  double c = p->c;
  
  return (a * x + b) * x + c;
}
