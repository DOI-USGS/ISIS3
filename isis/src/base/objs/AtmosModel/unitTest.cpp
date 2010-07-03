#include <iostream>
#include <cmath>
#include "NumericalApproximation.h"
#include "NumericalAtmosApprox.h"
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Anisotropic1.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;
void doit(Pvl &lab, PhotoModel &pm);

int main () {
  Isis::Preference::Preferences(true);
  cout << "UNIT TEST for Isis::AtmosModel" << endl << endl; 

  Pvl lab;
  lab.AddObject(PvlObject("PhotometricModel"));
  lab.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
  lab.FindObject("PhotometricModel").FindGroup("Algorithm").AddKeyword(PvlKeyword("Name","Lambert"));// HapkeHen
  PhotoModel *pm = PhotoModelFactory::Create(lab);
  
  cout << "Testing missing AtmosphericModel object ..." << endl;
  doit(lab,*pm);

  lab.AddObject(PvlObject("AtmosphericModel"));
  cout << "Testing missing Algorithm group ..." << endl;
  doit(lab,*pm);

  lab.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
  cout << "Testing missing Name keyword ..." << endl;
  doit(lab,*pm);

  lab.FindObject("AtmosphericModel").FindGroup("Algorithm").AddKeyword(PvlKeyword("Name","Anisotropic1"), Pvl::Replace);
  cout << "Testing supported atmospheric model ..." << endl;
  doit(lab,*pm);

  AtmosModel *am = AtmosModelFactory::Create(lab,*pm);

  try {
    am->SetAtmosWha(0.98);
    cout << "Testing atmospheric model get methods ..." << endl;
    cout << "AlgorithmName = " << am->AlgorithmName() << endl;
    cout << "AtmosTau = " << am->AtmosTau() << endl;
    cout << "AtmosWha = " << am->AtmosWha() << endl;
    cout << "AtmosHga = " << am->AtmosHga() << endl;
    cout << "AtmosNulneg = " << am->AtmosNulneg() << endl;
    cout << "AtmosNinc = " << am->AtmosNinc() << endl;

    cout << endl;

    am->SetStandardConditions(true);
    cout << "Testing atmospheric model get methods in standard conditions..." << endl;
    cout << "AlgorithmName = " << am->AlgorithmName() << endl;
    cout << "AtmosTau = " << am->AtmosTau() << endl;
    cout << "AtmosWha = " << am->AtmosWha() << endl;
    cout << "AtmosHga = " << am->AtmosHga() << endl;
    cout << "AtmosNulneg = " << am->AtmosNulneg() << endl;
    cout << "AtmosNinc = " << am->AtmosNinc() << endl;
    am->SetStandardConditions(false);

    am->SetAtmosWha(0.95);
  } 
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing boundary conditions of atmospheric model set methods ..." << endl;
  try {
    am->SetAtmosTau(-1.0);
  }
  catch (iException &e) {
    e.Report(false);
  }

  try {
    am->SetAtmosWha(0.0);
  }
  catch (iException &e) {
    e.Report(false);
  }

  try {
    am->SetAtmosWha(2.0);
  }
  catch (iException &e) {
    e.Report(false);
  }

  try {
    am->SetAtmosHga(-1.0);
  }
  catch (iException &e) {
    e.Report(false);
  }

  try {
    am->SetAtmosHga(1.0);
  }
  catch (iException &e) {
    e.Report(false);
  }

  cout << endl;

  cout << "Testing atmospheric model InrFunc2Bint method ..." 
      << endl;
  try {
    am->SetAtmosAtmSwitch(1);
    am->SetAtmosInc(0.0);
    am->SetAtmosPhi(0.0);
    am->SetAtmosHga(.68);
    am->SetAtmosTau(.28);
    double result = NumericalAtmosApprox::InrFunc2Bint(am,1.0e-6);
    cout << "Results from InrFunc2Bint = " << result <<
        endl << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  try {
    am->SetAtmosAtmSwitch(2);
    am->SetAtmosInc(3.0);
    am->SetAtmosPhi(78.75);
    am->SetAtmosHga(.68);
    am->SetAtmosTau(.28);
    double result = NumericalAtmosApprox::InrFunc2Bint(am,.75000025);
    cout << "Results from InrFunc2Bint = " << result <<
        endl << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing atmospheric model r8trapzd method ..." 
      << endl;
  try {
    am->SetAtmosAtmSwitch(1);
    am->SetAtmosInc(0.0);
    am->SetAtmosPhi(0.0);
    am->SetAtmosHga(.68);
    am->SetAtmosTau(.28);
    double ss = 0;
    NumericalAtmosApprox nam;
    for (int i = 1; i < 10; i++) {
      ss = nam.RefineExtendedTrap(am,NumericalAtmosApprox::OuterFunction,0.0,180.0,ss,i);
      cout << "Results from r8trapzd = " << ss << " for i = " << i << endl;
      ss = 0;
    }
    cout  << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }

  cout << "Testing atmospheric model OutrFunc2Bint method ..." 
      << endl;
  try {
    am->SetAtmosAtmSwitch(1);
    am->SetAtmosInc(0.0);
    am->SetAtmosPhi(0.0);
    am->SetAtmosHga(.68);
    am->SetAtmosTau(.28);
    double result = NumericalAtmosApprox::OutrFunc2Bint(am,0.0);
    cout << "Results from OutrFunc2Bint = " << result <<
        endl << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing atmospheric model r8qromb method ..." 
      << endl;
  try {
    am->SetAtmosAtmSwitch(1);
    am->SetAtmosInc(0.0);
    am->SetAtmosPhi(0.0);
    am->SetAtmosHga(.68);
    am->SetAtmosTau(.28);
    double ss;
    NumericalAtmosApprox nam;
    ss = nam.RombergsMethod(am,NumericalAtmosApprox::OuterFunction,0.,180.);
    cout << "Results from r8qromb = " << ss <<
        endl << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing atmospheric model GenerateAhTable method ..." 
      << endl;
  try {
    am->GenerateAhTable();

    cout << "Results from GenerateAhTable = " << endl;
    cout << "Ab = " << am->AtmosAb() << endl;
    int ninc = am->AtmosNinc();
    vector <double> inctable;
    inctable = am->AtmosIncTable();
    for (int i=0; i<ninc; i++) {
      cout << "i IncTable(i) = " << i << " " << inctable[i] <<
          endl;
    }
    vector <double> ahtable;
    ahtable = am->AtmosAhTable();
    for (int i=0; i<ninc; i++) {
      cout << "i AhTable(i) = " << i << " " << ahtable[i] <<
          endl;
    }
  }
  catch (iException &e) {
    e.Report(false);
  }
  cout << endl;

  cout << "Testing atmospheric model GenerateHahgTables method ..." 
      << endl;
  try {
    am->SetAtmosWha(.95);
    am->SetAtmosInc(0.0);
    am->SetAtmosPhi(0.0);
    am->SetAtmosHga(.68);
    am->SetAtmosTau(.28);
    am->GenerateHahgTables();
    cout << "Results from GenerateHahgTables = " << endl;
    cout << "Hahgsb = " << am->AtmosHahgsb() << endl;
    int ninc = am->AtmosNinc();
    vector <double> inctable;
    inctable = am->AtmosIncTable();
    for (int i=0; i<ninc; i++) {
      cout << "i IncTable(i) = " << i << " " << inctable[i] <<
          endl;
    }
    vector <double> hahgttable;
    hahgttable = am->AtmosHahgtTable();
    for (int i=0; i<ninc; i++) {
      cout << "i HahgtTable(i) = " << i << " " << hahgttable[i] <<
          endl;
    }
    vector <double> hahgt0table;
    hahgt0table = am->AtmosHahgt0Table();
    for (int i=0; i<ninc; i++) {
      // neglect rounding error.
      if(fabs(hahgt0table[i]) < 1E-16) {
        hahgt0table[i] = 0.0;
      }
      cout << "i Hahgt0Table(i) = " << i << " " << hahgt0table[i] <<
          endl;
    }
    cout << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }
  try{
      am->SetAtmosAtmSwitch(1);
      am->SetAtmosInc(0.0);
      am->SetAtmosPhi(0.0);
      am->SetAtmosHga(.68);
      am->SetAtmosTau(.28);
      double ss = 0;

      NumericalAtmosApprox nam;
      for(double a = 0.0; a < 0.8; a+=.3) {
        for(double b = 0.8; b > a; b-=.3) {
          ss = 0;
          ss = nam.RefineExtendedTrap(am,NumericalAtmosApprox::OuterFunction,a,b,ss,10);
          cout << "Results from r8trapzd = " << ss << " for interval (a,b) = (" 
            << a << "," << b << ")" << endl << endl;
        }
      }


  }catch (iException &error){
    error.Report(false);
  }
  // Exponential integrals and G11Prime
  cout << "Test En ..." << endl;
  try {
  int n = 1;
  double x = .28;
  double result;

  result = AtmosModel::En(n,x);
  cout << "Results from En(1,0.28) = " << result   << endl;
  cout << "           Actual value = " << 0.957308 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  try {
  int n = 1;
  double x = .733615937;
  double result;

  result = AtmosModel::En(n,x);
  cout << "Results from En(1,0.733615937) = " << result  << endl;
  cout << "                  Actual value = " << 0.35086 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  cout << "Test Ei ..." << endl;
  try {
  double x = .234;
  double result;

  result = AtmosModel::Ei(x);
  cout << "Results from Ei(0.234) = " << result    << endl;
  cout << "          Actual value = " << -0.626785 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  try {
  double x = 1.5;
  double result;

  result = AtmosModel::Ei(x);
  cout << "          Results from Ei(1.5) = " << result  << endl;
  cout << "                  Actual value = " << 3.30129 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  try {
  double x = 2.6;
  double result;

  result = AtmosModel::Ei(x);
  cout << "Results from Ei(2.6) = " << result  << endl;
  cout << "        Actual value = " << 7.57611 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  try {
  double x = .01583;
  double result;

  result = AtmosModel::Ei(x);
  cout << "Results from Ei(0.01583) = " << result << endl;
  cout << "            Actual value = " << -3.55274 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  cout << "Test G11Prime ..." << endl;
  try {
  double tau = .28;
  double result;

  result = AtmosModel::G11Prime(tau);
  cout << "Results from G11Prime(0.28) = " << result  << endl;
  cout << "               Actual value = " << 0.79134 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }

  try {
  double tau = 1.5836;
  double result;

  result = AtmosModel::G11Prime(tau);
  cout << "Results from G11Prime(1.5836) = " << result   << endl;
  cout << "                 Actual value = " << 0.217167 << endl
      << endl;
  }
  catch (iException &e) {
    e.Report();
  }
  cout << endl;
  cout << "x\tn\tG11Prime(x)\tEi(x)\tEn(x)" << endl;
  for(double x = .5; x < 1.75; x+=.5) {
    for(int n = 0; n < 3; n++) {
      cout << x << "\t" << n << "\t";
      cout << AtmosModel::G11Prime(x) << "\t";
      if(x == 1) cout << "\t";
      cout << AtmosModel::Ei(x) << "\t";
      cout << AtmosModel::En(n, x) << endl;
    }
  }
  cout << "EXCEPTIONS:" << endl;
  try{AtmosModel::Ei(0.0);} // require x > 0
  catch(iException e){ e.Report(); }
  try{AtmosModel::En(1,0.0);} // require (n>=0 & x>0) or (n>1 & x>=0)
  catch(iException e){ e.Report(); }
  try{AtmosModel::En(0,-1.0);}// require (n>=0 & x>0) or (n>1 & x>=0)
  catch(iException e){ e.Report(); }
  
  cout << "\t************************************************" << endl;
  cout << endl;
}

void doit(Pvl &lab, PhotoModel &pm) {
  try {
    //AtmosModel *am = AtmosModelFactory::Create(lab,pm);
    AtmosModelFactory::Create(lab,pm);
  }
  catch (iException &error) {
    error.Report(false);
  }

  cout << endl;
}
