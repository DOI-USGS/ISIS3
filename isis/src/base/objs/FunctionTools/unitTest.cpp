#include "FunctionTools.h"
#include <iostream>
#include "Preference.h"
#include "IException.h"
#include "iString.h"



using namespace std;
using namespace Isis;

class CubicFunction :
public std::unary_function <double, double> {
  public:
   double operator()(double x) {
     return -4.0+5.0*x-8.0*x*x+6.0*x*x*x;
   }
};

class ErrorFunction :
public std::unary_function <double, double> {
  public:
   double operator()(double x) {
     iString msg = "This functor always throws an error\n";
     throw IException(IException::Programmer, msg, _FILEINFO_);
     return 0.0;
   }
};


int main() {
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
  catch (IException e) {
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
  catch (IException e) {
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
  catch (IException e) {
    e.print();
  }
  cerr << "Root Found: " << root << endl;



  return 0;


}
