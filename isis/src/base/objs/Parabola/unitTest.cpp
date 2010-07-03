#include "Parabola.h"
#include <iostream>
#include "Preference.h"

using namespace std;

int main () {
  Isis::Preference::Preferences(true);

  Isis::Parabola b("Parabola");
  vector<double> coefs;
  coefs.push_back(-6.0);
  coefs.push_back(5.0);
  coefs.push_back(1.0);
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
  vars[0] = -2.0;
  cout << b.Evaluate(vars) << endl;
  for (int i=0; i<b.Coefficients(); i++) {
    cout << b.Term(i) << endl;
  }
}
