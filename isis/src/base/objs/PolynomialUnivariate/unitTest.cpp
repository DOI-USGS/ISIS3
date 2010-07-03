#include "PolynomialUnivariate.h"
#include <iostream>

using namespace std;

int main () {
  Isis::PolynomialUnivariate b(1);
  vector<double> coefs;
  coefs.push_back(0.5);
  coefs.push_back(0.5);
  b.SetCoefficients(coefs);

  cout << "Name   = " << b.Name() << endl;
  cout << "Ncoefs = " << b.Coefficients() << endl;
  cout << "Vars   = " << b.Variables() << endl;
  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Coefficient(i) << endl;
  }

  cout << "---" << endl;
  vector<double> vars;
  vars.push_back(2.0);
  cout << b.Evaluate(vars) << endl;
  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }

  cout << "---" << endl;
  vars[0] = -1.0;
  cout << b.Evaluate(vars) << endl;

  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }

  // Test 2nd order
  // 0.5 + 0.5*v1 + 1.0*v1*v1
  Isis::PolynomialUnivariate c(2);
  cout << "---- 2nd order ----" << endl;
  cout << "Name   = " << c.Name() << endl;
  cout << "Ncoefs = " << c.Coefficients() << endl;
  cout << "Vars   = " << c.Variables() << endl;
  coefs.push_back(1.0);
  c.SetCoefficients(coefs);
  cout << c.Evaluate(vars) << endl;
  for (int i=0; i<c.Coefficients(); i++) {
    cout << c.Term(i) << endl;
  }

  //  Test Derivative methods
  cout << "---- 2nd order ----" << endl;
  cout << "Name   = " << c.Name() << endl;
  cout << "Ncoefs = " << c.Coefficients() << endl;
  cout << "Vars   = " << c.Variables() << endl;
  cout << "Variable Derivative = " << c.DerivativeVar(2.0) << endl;
  cout << "Coefficient 2 Derivative = " << c.DerivativeCoef(2.0,2) << endl;
}
