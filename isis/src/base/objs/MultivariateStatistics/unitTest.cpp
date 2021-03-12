/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include "MultivariateStatistics.h"
#include "IException.h"
#include "Preference.h"
#include "PvlObject.h"

using namespace std;
int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  Isis::MultivariateStatistics s;

  double x[10] = { 3, 4, 4, 2, 5, 3, 4, 5, 3, 2 };
  double y[10] = {57, 78, 72, 58, 89, 63, 73, 84, 75, 48};

  s.AddData(x, y, 10);
  cout << "SumX            = " << s.X().Sum() << endl;
  cout << "SumY            = " << s.Y().Sum() << endl;
  cout << "SumXY           = " << s.SumXY() << endl;
  cout << "SumXX           = " << s.X().SumSquare() << endl;
  cout << "SumYY           = " << s.Y().SumSquare() << endl;
  cout << endl;
  cout << "Covariance      = " << s.Covariance() << endl;
  cout << "VarianceX       = " << s.X().Variance() << endl;
  cout << "VarianceY       = " << s.Y().Variance() << endl;
  cout << "Correlation     = " << s.Correlation() << endl;
  cout << endl;
  cout << "Valid Pixels    = " << s.ValidPixels() << endl;
  cout << "Invalid Pixels  = " << s.InvalidPixels() << endl;
  cout << "Total Pixels    = " << s.TotalPixels() << endl;
  cout << endl;
  double a, b;
  s.LinearRegression(a, b);
  cout << "Linear Regression Y = aX + b" << endl;
  cout << "a = " << a << endl;
  cout << "b = " << b << endl;
  cout << endl;

  cout << endl << "Testing Pvl serialization methods..." << endl;
  Isis::PvlObject toStats = s.toPvl();
  Isis::MultivariateStatistics fromStats(toStats);
  cout << "SumX            = " << fromStats.X().Sum() << endl;
  cout << "SumY            = " << fromStats.Y().Sum() << endl;
  cout << "SumXY           = " << fromStats.SumXY() << endl;
  cout << "SumXX           = " << fromStats.X().SumSquare() << endl;
  cout << "SumYY           = " << fromStats.Y().SumSquare() << endl;
  cout << endl;
  cout << "Covariance      = " << fromStats.Covariance() << endl;
  cout << "VarianceX       = " << fromStats.X().Variance() << endl;
  cout << "VarianceY       = " << fromStats.Y().Variance() << endl;
  cout << "Correlation     = " << fromStats.Correlation() << endl;
  cout << endl;
  cout << "Valid Pixels    = " << fromStats.ValidPixels() << endl;
  cout << "Invalid Pixels  = " << fromStats.InvalidPixels() << endl;
  cout << "Total Pixels    = " << fromStats.TotalPixels() << endl;
  cout << endl;
  fromStats.LinearRegression(a, b);
  cout << "Linear Regression Y = aX + b" << endl;
  cout << "a = " << a << endl;
  cout << "b = " << b << endl;

}
