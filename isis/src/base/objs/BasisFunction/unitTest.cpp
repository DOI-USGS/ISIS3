#include "BasisFunction.h"
#include "Preference.h"
#include <iostream>

using namespace Isis;
using namespace std;

int main() {
  Preference::Preferences(true);
  BasisFunction b("Basis", 2, 2);
  vector<double> coefs;
  coefs.push_back(0.5);
  coefs.push_back(-0.5);
  b.SetCoefficients(coefs);

  cout << "Name   = " << b.Name() << endl;
  cout << "Ncoefs = " << b.Coefficients() << endl;
  cout << "Vars   = " << b.Variables() << endl;
  for (int i = 0; i < b.Coefficients(); i++) {
    cout << b.Coefficient(i) << endl;
  }

  cout << "---" << endl;
  vector<double> vars = coefs;
  cout << b.Evaluate(vars) << endl;
  for (int i = 0; i < b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }

  cout << "---" << endl;
  vars[0] = 1.0;
  vars[1] = 2.0;
  cout << b.Evaluate(vars) << endl;
  for (int i = 0; i < b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }
  
  cout << "---" << endl;
  BasisFunction b1("Basis1", 1, 1);
  vector<double> coefs1;
  coefs1.push_back(5.0);
  b1.SetCoefficients(coefs1);
  
  cout << "Name   = " << b1.Name() << endl;
  cout << "Ncoefs = " << b1.Coefficients() << endl;
  cout << "Vars   = " << b1.Variables() << endl;
  for (int i = 0; i < b1.Coefficients(); i++) {
    cout << b1.Coefficient(i) << endl;
  }
  
  cout << "---" << endl;
  double var1 = 2.0;
  cout << b1.Evaluate(var1) << endl;
}
