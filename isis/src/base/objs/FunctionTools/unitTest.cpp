/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "FunctionTools.h"
#include <iostream>
#include <QSet>
#include "Preference.h"
#include "IException.h"
#include "IString.h"



using namespace std;
using namespace Isis;

class CubicFunction :
public std::function <double(double)> {
  public:
   double operator()(double x) {
     return -4.0+5.0*x-8.0*x*x+6.0*x*x*x;
   }
};

class ErrorFunction :
public std::function <double(double)> {
  public:
   double operator()(double x) {
     IString msg = "This functor always throws an error\n";
     throw IException(IException::Programmer, msg, _FILEINFO_);
     return 0.0;
   }
};


int main() {

  QList<double> roots;

  /******************Testing realLinearRoots**************************/
  cerr << "Testing realLinearRoots\n";
  cerr << "Equation: 3*X - 2 = 0.0  One real root\n";
  roots = FunctionTools::realLinearRoots(3.0,-2.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 0*X - 2 = 0.0  No roots\n";
  roots = FunctionTools::realLinearRoots(0,2);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 0*X - 0.0 = 0.0  Infinite roots (should return empty set)\n";
  roots = FunctionTools::realLinearRoots(0,2);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";



  /******************Testing realQuadraticRoots**************************/
  cerr << endl << endl;
  cerr << "Testing realQuadraticRoots\n";
  cerr << "Equation: 1.0*X^2 + -1.0*X - 2.0 = 0.0  Two real root\n";
  roots = FunctionTools::realQuadraticRoots(1.0,-1.0,-2.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 1.0*X^2 + -4.0*X + 4.0 = 0.0  one double root\n";
  roots = FunctionTools::realQuadraticRoots(1.0,-4.0,4.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";


  cerr << endl;
  cerr << "Equation: 0.0*X^2 + -4.0*X + 4.0 = 0.0  linear equation, one real root\n";
  roots = FunctionTools::realQuadraticRoots(0.0,-4.0,4.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 3.0*X^2 + 0.0*X - 12.0 = 0.0  zero linear coeff, two real roots\n";
  roots = FunctionTools::realQuadraticRoots(3.0,0.0,-12.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 3.0*X^2 + 0.0*X + 0.0 = 0.0  zero linear and const coeff, one double root\n";
  roots = FunctionTools::realQuadraticRoots(3.0,0.0,0.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 3.0*X^2 + -3.0*X + 0.0 = 0.0  zero const coeff, two real roots\n";
  roots = FunctionTools::realQuadraticRoots(3.0,-3.0,0.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";


  /******************Testing realCubicRoots**************************/
  cerr << endl << endl;
  cerr << "Testing realCubicRoots\n";
  cerr << "Equation: 1.0*x^3 - 3.0*X^2 + 0.0*X + 4.0 = 0.0"
           "  zero linear coeff, two real roots (one double)\n";
  roots = FunctionTools::realCubicRoots(1.0,-3.0,0.0,4.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";


  cerr << endl;
  cerr << "Equation: 1.0*x^3 - 4.0*X^2 + -7.0*X + 10.0 = 0.0"
           "  three real roots\n";
  roots = FunctionTools::realCubicRoots(1.0,-4.0,-7.0,10.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 1.0*x^3 + 1.0*X^2 + -2.0*X - 30.0 = 0.0"
           "  one real root\n";
  roots = FunctionTools::realCubicRoots(1.0,1.0,-2.0,-30.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 1.0*x^3 + 0.0*X^2 + 0.0*X - 8.0 = 0.0"
           "  zero quad and linear coeffs, one real root\n";
  roots = FunctionTools::realCubicRoots(1.0,0.0,0.0,-8.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  //repeating some tests with non-one leading coefficients
  cerr << endl;
  cerr << "Equation: 2.0*x^3 - 8.0*X^2 + -14.0*X + 20.0 = 0.0"
           "  three real roots\n";
  roots = FunctionTools::realCubicRoots(2.0,-8.0,-14.0,20.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: -2.0*x^3 + -2.0*X^2 + 4.0*X + 60.0 = 0.0"
           "  one real root\n";
  roots = FunctionTools::realCubicRoots(-2.0,-2.0,4.0,60.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 3.0*x^3 + 0.0*X^2 + 0.0*X - 24.0 = 0.0"
           "  zero quad and linear coeffs, one real root\n";
  roots = FunctionTools::realCubicRoots(3.0,0.0,0.0,-24.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: -3.0*x^3 + 0.0*X^2 + 0.0*X - 24.0 = 0.0"
           "  zero quad and linear coeffs, one real root\n";
  roots = FunctionTools::realCubicRoots(-3.0,0.0,0.0,24.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";



  //fall backs to less complicated polys
  cerr << endl;
  cerr << "Equation: 0.0*x^3 + 1.0*X^2 + 0.0*X - 4.0 = 0.0"
           "  fall back to quadratic math, two real roots\n";
  roots = FunctionTools::realCubicRoots(0.0,1.0,0.0,-4.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";

  cerr << endl;
  cerr << "Equation: 0.0*x^3 + 0.0*X^2 + 1.0*X - 4.0 = 0.0"
           "  fall back to linear math, one real root\n";
  roots = FunctionTools::realCubicRoots(0.0,0.0,1.0,-4.0);
  cerr << "solutions: ";
  if (roots.size() == 0.0) cerr << "Empty Set";
  else
    foreach (const double root, roots)
      cerr << root << "  ";


  /******************Testing BrentsRootFinder**************************/
  cerr << endl << endl << "Testing Brent's root finder\n";
  Isis::Preference::Preferences(true);
  QList <double> point1, point2;
  CubicFunction func;
  double root=0.0;
  //setting up two points that do not bound a root
  point1 << 0.0;
  point1 << func(0.0);
  point2 << 0.5;
  point2 << func(0.5);
  cerr << "Passing brentsRootFinder two points that don't bound a root "
          "(this should throw an error)\n";
  try {
    FunctionTools::brentsRootFinder(func,point1,point2,1e-6,100,root);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << endl;

  //setting up two points that do bound a root
  ErrorFunction errorFunc;
  point1.clear();
  point2.clear();
  point1 << -1.0 << -1.0;
  point2 <<  1.0 <<  1.0;
  cerr << "Catching an error thrown by the fucntor.\n";
  try {
    FunctionTools::brentsRootFinder(errorFunc,point1,point2,1e-6,100,root);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << endl;

  //actually finding a root now
  point1.clear();
  point2.clear();
  //setting up two points that do not bound a root
  point1 << 1.0;
  point1 << func(1.0);
  point2 << 1.5;
  point2 << func(1.5);
  cerr << "Passing brentsRootFinder two points that do bound a root\n";
  try {
    FunctionTools::brentsRootFinder(func,point1,point2,1e-6,100,root);
  }
  catch (IException &e) {
    e.print();
  }
  cerr << "Root Found: " << root << endl;



  return 0;


}
