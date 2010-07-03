#include <iostream>
#include <iomanip>
#include "LineEquation.h"
#include "Filename.h"
#include "Preference.h"
#include "Table.h"



using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << setprecision(8);
  cout << "Unit test for LineEquation" << endl;

  cout << "Testing first constructor..." << endl;
  Isis::LineEquation line1;
  line1.AddPoint(1.,1.);
  line1.AddPoint(3.,6.);
  cout << "     Slope = " << line1.Slope() << endl;
  cout << "     Intercept = " << line1.Intercept() << endl;

  cout << "Testing second constructor..." << endl;
  Isis::LineEquation line2(-1.,1.,-3.,2.);
  cout << "     Slope = " << line2.Slope() << endl;
  cout << "     Intercept = " << line2.Intercept() << endl;

}
