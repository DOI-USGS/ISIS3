#include "BasisFunction.h"
#include "Preference.h"
#include <iostream>

using namespace std;

int main () {
  Isis::Preference::Preferences(true);
  Isis::BasisFunction b("Basis",2,2);
  vector<double> coefs;
  coefs.push_back(0.5);
  coefs.push_back(-0.5);
  b.SetCoefficients(coefs);

  cout << "Name   = " << b.Name() << endl;
  cout << "Ncoefs = " << b.Coefficients() << endl;
  cout << "Vars   = " << b.Variables() << endl;
  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Coefficient(i) << endl;
  }

  cout << "---" << endl;
  vector<double> vars = coefs;
  cout << b.Evaluate(vars) << endl;
  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }

  cout << "---" << endl;
  vars[0] = 1.0;
  vars[1] = 2.0;
  cout << b.Evaluate(vars) << endl;
  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }

  
  

}
