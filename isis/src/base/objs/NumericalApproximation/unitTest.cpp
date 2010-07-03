#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "NumericalApproximation.h"
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Anisotropic1.h"
#include "Preference.h"
#include "iException.h"

using namespace std;
using namespace Isis;

//create a cubic polynomial to test interpolation, extrapolation
inline double f(double x) { return x*x*x + x*x + x; }
inline vector <double> f(vector <double> x) { 
  vector <double> y(x.size()); 
  for(unsigned int i=0;i<x.size();i++){
    y[i]=f(x[i]);
  } 
  return y; 
}
//first derivative of the function f (above) to test differentiation
inline double fstderiv(double x) { return 3*x*x + 2*x + 1; }
inline vector <double> fstderiv(vector <double> x) { 
  vector <double> y(x.size()); 
  for(unsigned int i=0;i<x.size();i++){
    y[i]=fstderiv(x[i]);
  } 
  return y; 
}
//second derivative of the function f (above) to test differentiation
inline double scndderiv(double x) { return 6*x + 2; }
//anitderivative of the function f (above) to test integration
inline double integral(double a, double b) { return (b*b*b*b/4 + b*b*b/3 + b*b/2) - (a*a*a*a/4 + a*a*a/3 + a*a/2); }//integral of f


int main (int argc, char *argv[]) {
  // include isis preferences so that line numbers will not be printed with errors
  Isis::Preference::Preferences(true);

  cout << endl << "Unit test for NumericalApproximation" << endl;
  //------------------------------------------------------------
  cout << "Using the function f(x) = x^3 + x^2 + x" << endl << endl;
  vector <double> x, y;
  x.push_back(0.00);     x.push_back(0.07);     x.push_back(0.12);     x.push_back(0.19); 
  x.push_back(0.22);     x.push_back(0.23);     x.push_back(0.32);     x.push_back(0.34); 
  x.push_back(0.36);     x.push_back(0.39);     x.push_back(0.40);     x.push_back(0.41); 
  x.push_back(0.44);     x.push_back(0.49);     x.push_back(0.54);     x.push_back(0.59); 
  x.push_back(0.64);     x.push_back(0.68);     x.push_back(0.70);     x.push_back(0.79); 
  x.push_back(0.80); 
  y = f(x);
  //------------------------------------------------------------
  try{
    cout << "1) Creating objects, accessing properties" << endl;
    cout << "Valid interpolation types include: " << endl;
    cout << "type \t name \t\t   minpoints" << endl;
    NumericalApproximation interp;
    for(int i = 0; i < 10; i++) {
      interp.SetInterpType(NumericalApproximation::InterpType(i));
      cout << interp.InterpolationType() << "\t" << interp.Name() << "\t";
      if(interp.Name().length() < 16){ 
        cout << "\t";
        if(interp.Name().length() < 8) cout << "\t";
      }
      cout << interp.MinPoints() << endl;
    }
    cout << "EXCEPTIONS:" << endl;
    try{ interp.SetInterpType(NumericalApproximation::InterpType(11));}
    catch(iException e){ e.Report(); }
    cout << "\t************************************************" << endl;
  
    cout << endl;
    cout << "Test constructors: " << endl;
    // Create NumericalApproximation objects 
    NumericalApproximation interp1;                                                // CubicNatural by default
    NumericalApproximation interp2(interp1);                                       // CubicNatural by copy
    NumericalApproximation interp3 = interp1;                                      // CubicNatural by assignment operator
    NumericalApproximation interp4(NumericalApproximation::CubicClamped);          // CubicClamped by interpType parameter
    NumericalApproximation interp5(x,y,NumericalApproximation::PolynomialNeville); // PolynomialNeville by x,y,interpType parameters
    NumericalApproximation interp6(x,y,NumericalApproximation::CubicHermite);      // CubicHermite by x,y,interpType parameters
                        
    // Check properties of objects created
    cout << "object \t type \t name" << endl;
    cout << "interp1  "  << interp1.InterpolationType() << " \t " << interp1.Name() << endl;
    cout << "interp2  "  << interp2.InterpolationType() << " \t " << interp2.Name() << endl;
    cout << "interp3  "  << interp3.InterpolationType() << " \t " << interp3.Name() << endl;
    cout << "interp4  "  << interp4.InterpolationType() << " \t " << interp4.Name() << endl;
    cout << "interp5  "  << interp5.InterpolationType() << " \t " << interp5.Name() << endl;
    cout << "interp6  "  << interp6.InterpolationType() << " \t " << interp6.Name() << endl;
    cout << endl;
    cout << "EXCEPTIONS:" << endl;
    try{// SIZE OF X IS NOT EQUAL TO THE SIZE OF Y
      y.pop_back();
      NumericalApproximation uneven(x,y);
    }catch(iException e){ e.Report(); }
    cout << "\t************************************************" << endl;
    cout << endl;
    // restore y-vector
    y.push_back(f(.8));

  //------------------------------------------------------------
    cout << "Add data, set properties:  " << endl;
    // Adding Data...
      //  by individual evenly spaced points
    for(double d = 0.0; d < 0.84; d+=.04) {
      interp1.AddData(d,f(d));
    }
      // by pointer containing irregularly spaced points
    interp2.AddData(x.size(), &x[0], &y[0]);
      // by vectors containing irregularly spaced points
    interp3.AddData(x,y);
    interp4.AddData(x,y);
    // interp5 and interp6 already contain data from constructors

    // Test Contains() method
    for(double d = 0.0; d < 0.05; d+=.01) {
      cout << "d = " << d << " is an x element of dataset? " << interp1.Contains(d) << endl;
    }
    cout << endl;

    // Setting interpolation...
    interp3.SetInterpType(NumericalApproximation::CubicNeighborhood);

    cout << "description \t\t\t\t\t size \t domain" << endl;
    cout << interp1.Name() << ", regularly spaced\t\t " << interp1.Size()   <<  " \t[" << interp1.DomainMinimum() << ", " << interp1.DomainMaximum() << "]" << endl;
    cout << interp2.Name() << ", irregularly spaced\t\t " << interp2.Size() <<  " \t[" << interp2.DomainMinimum() << ", " << interp2.DomainMaximum() << "]" << endl;
    cout << interp3.Name() << ", irregularly spaced\t " << interp3.Size()   <<  " \t[" << interp3.DomainMinimum() << ", " << interp3.DomainMaximum() << "]" << endl;
    cout << interp4.Name() << ", irregularly spaced\t\t " << interp4.Size() <<  " \t[" << interp4.DomainMinimum() << ", " << interp4.DomainMaximum() << "]" << endl;
    cout << interp5.Name() << ", irregularly spaced\t " << interp5.Size()   <<  " \t[" << interp5.DomainMinimum() << ", " << interp5.DomainMaximum() << "]" << endl;
    cout << endl;
                      
    // Computing interpolations
    cout << "EXCEPTIONS:" << endl;
    // Test ValidateDataSet(), MinPoints() exceptions
    try{ // DATA SET IS NOT IN UNIQUE
      interp2.AddData(0,.8);
      interp2.DomainMinimum();
    }catch(iException e){ e.Report(); }
    interp2.Reset();
    try{ // DATA SET IS LESS THAN MINPOINTS
      interp2.DomainMaximum();
    }catch(iException e){ e.Report(); }
    interp2.AddData(x,y);

      // Test CubicClamped exceptions
    // ENDPOINT DERIVATIVES ARE NOT SET YET
    try{ interp4.Evaluate(.5);}catch(iException e){ e.Report(); }
    interp4.SetCubicClampedEndptDeriv(fstderiv(x[0]),fstderiv(x[x.size()-1]));
    try{ // NEW DATA HAS BEEN ADDED TO SET CAUSES ENDPT DERIVATIVES TO BE RESET
      interp4.AddData(.9,f(.9));
      interp4.Evaluate(.5);
    }catch(iException e){ e.Report(); }
    try{ // CubicClamped DATA SET IS NOT IN ASCENDING ORDER
      interp4.AddData(.5,f(.5));
      interp4.DomainMinimum();
    }catch(iException e){ e.Report(); }
    // restore interp4 object
    interp4.Reset();
    interp4.AddData(x,y);
    interp4.SetCubicClampedEndptDeriv(fstderiv(x[0]),fstderiv(x[x.size()-1]));

      // Test PolynomialNeville exceptions
    try{ //  CubicPeriodic DATA SET DOESN'T HAVE FIRST AND LAST Y-VALUES EQUAL
      interp5.SetInterpType(NumericalApproximation::CubicNatPeriodic);
      interp5.DomainMinimum();
    }catch(iException e){ e.Report(); }
    interp5.Reset(NumericalApproximation::PolynomialNeville);
    interp5.AddData(x,y);

      // Test CubicHermite exceptions
    try{ //  DATA SET SIZE DOESN'T MATCH NUMBER OF DERIVATIVES ADDED
      interp6.AddCubicHermiteDeriv(fstderiv(3));
    }catch(iException e){ e.Report(); }
    interp6.Reset();
    interp6.AddData(x,y);
    interp6.AddCubicHermiteDeriv(fstderiv(x));
    cout << "\t************************************************" << endl;
    cout << endl;
  //------------------------------------------------------------
    // Evaluating the interpolations
    cout << endl;
    cout << "2) Dataset Interpolation:  " << endl;
    vector <double> points,interppts1,interppts2,interppts3,interppts4,interppts5,interppts6;
    for(double p = .05; p < .8; p+=.1) {
      points.push_back(p);
      //Evaluate individual points
      interppts1.push_back(interp1.Evaluate(p)); //natural, reg
    }
    // Evaluate vectors
    interppts2 = interp2.Evaluate(points); //  natural, irreg
    interppts3 = interp3.Evaluate(points); //  neighbor, irreg
    interppts4 = interp4.Evaluate(points); //  clamped, irreg
    interppts5 = interp5.Evaluate(points); //  neville, irreg
    interppts6 = interp6.Evaluate(points); //  hermite, irreg
    cout << "Evaluated points:" << endl;
      cout << "x \t y \t\t c-nat-regular \t c-nat-irreg \t c-neigh \t c-clamped \t p-neville \t c-hermite" << endl;
    for(unsigned int i = 0; i<interppts2.size(); i++) {
      cout << points[i] << " \t" << f(points[i]) << " \t " 
           << interppts1[i] << " \t " 
           << interppts2[i] << " \t " 
           << interppts3[i] << " \t " 
           << interppts4[i] << " \t " 
           << interppts5[i] << " \t " 
           << interppts6[i] << endl;
    }
    vector <double> error = interp5.PolynomialNevilleErrorEstimate();
    cout << "Errors: (difference from y-value)" << endl;
    cout << "x \tc-nat-regular\tc-nat-irreg\tc-neigh\t\tc-clamped\tp-neville-trueErr\tp-neville-estimErr\tc-herm" << endl;
    for(unsigned int i = 0; i<interppts2.size(); i++) {
      cout << points[i] << "\t" << interppts1[i]-f(points[i]) << "\t";
      if(interppts1[i]-f(points[i]) == 0) cout << "\t";
      cout << interppts2[i]-f(points[i]) << "\t";
      if(interppts2[i]-f(points[i]) == 0) cout << "\t";
      cout << interppts3[i]-f(points[i]) << "\t";
      if(interppts3[i]-f(points[i]) == 0) cout << "\t";
      cout << interppts4[i]-f(points[i]) << "\t";
      if(interppts4[i]-f(points[i]) == 0) cout << "\t";
      cout << interppts5[i]-f(points[i]) << "\t\t";
      if(interppts5[i]-f(points[i]) == 0) cout << "\t";
      cout << error[i] << "\t\t";
      cout << interppts6[i]-f(points[i]) << endl;
    }
    cout << endl;
    cout << "3) Dataset Extrapolation: " << endl;
    cout << "   Notice: Clamped cubic spline accurate if extrapolating cubic function." << endl;
    cout << "type\t\t\ta\tDomain\t{ymin,ymax}\tNearestEndpoint\t\tExtrapolate\ty(a)" << endl;
    for(double a = -1.5; a < 0; a+=.2) {
      double b = interp4.Evaluate(a,NumericalApproximation::NearestEndpoint);
      double c = interp4.Evaluate(a,NumericalApproximation::Extrapolate);
      cout << interp4.Name() << "\t\t" << a << "\t[" << interp4.DomainMinimum() << "," 
           << interp4.DomainMaximum() << "]\t{" << y[0] << "," << y[y.size()-1] << "}\t\t" 
           << b << "\t\t" << c << "\t\t" << f(a) << endl;
    }
    for(double a = .801; a < 1.5; a+=.1) {
      double b = interp5.Evaluate(a,NumericalApproximation::NearestEndpoint);
      double c = interp5.Evaluate(a,NumericalApproximation::Extrapolate);
      cout << interp5.Name() << "\t" << a << "\t[" << interp5.DomainMinimum() << "," 
           << interp5.DomainMaximum() << "]\t{" << y[0] << "," << y[y.size()-1] << "}\t\t" 
           << b << "\t\t" << c << "\t\t" << f(a) << endl;
    }
    cout << "   Notice: In general, accuracy declines farther from domain endpoints." << endl;
    cout << "\t************************************************" << endl;

    // Test Evaluate() exceptions:
    cout << "EXCEPTIONS:" << endl;
    // Test outside domain default throws exception
    try{ interp2.Evaluate(.9);}catch(iException e){ e.Report(); }
    // Test outside domain Extrapolate throws exception for GSL
    try{ interp2.Evaluate(.9,NumericalApproximation::Extrapolate);}catch(iException e){ e.Report(); }
    // Test outside domain Extrapolate throws exception for CubicNeighborhood
    try{ interp3.Evaluate(.9,NumericalApproximation::Extrapolate);}catch(iException e){ e.Report(); }
    // interp2 IS NOT OF INTERP TYPE polynomial-Neville
    try{ interp2.PolynomialNevilleErrorEstimate(); }catch(iException e){ e.Report(); }
    try{ // Evaluate() HAS NOT BEEN CALLED
      interp2.SetInterpType(NumericalApproximation::PolynomialNeville);
      interp2.PolynomialNevilleErrorEstimate();
    }catch(iException e){ e.Report(); }
    // Test Reset() and restore the state of interp2
    interp2.Reset(NumericalApproximation::CubicNatural);
    interp2.AddData(x,y);
    cout << "\t************************************************" << endl;
    
    // Differentiation Methods
    cout << endl;
    cout << "4) Differentiation Approximation:  " << endl;
    cout << "GSL First derivative method errors:  " << endl;
    cout << "a \ty'(a)\tErr:natural-reg\t\tErr:natural-irreg" << endl;
    for(double a = .05; a < .8; a+=.3) {
      cout << a << " \t" << fstderiv(a) << " \t";
      cout << interp1.GslFirstDerivative(a)-fstderiv(a) << "\t\t";
      cout << interp2.GslFirstDerivative(a)-fstderiv(a) << endl;
    }
    cout << "First difference method errors:  " << endl;
    cout << "a \ty'(a)\t2ptBack-Neigh\t3ptBack-Neigh\t2ptFor-Clamp\t3ptFor-Clamp\t3ptCtr-Neville\t5ptCtr-Neville" << endl;
    for(double a = .15; a < .8; a+=.3) {
      cout << a << " \t" << fstderiv(a) << " \t";
      cout << interp3.BackwardFirstDifference(a,2,.01)-fstderiv(a) << "\t";
      cout << interp3.BackwardFirstDifference(a,3,.01)-fstderiv(a) << "\t";
      cout << interp4.ForwardFirstDifference(a,2,.01) -fstderiv(a) << "\t\t";
      cout << interp4.ForwardFirstDifference(a,3,.01) -fstderiv(a) << "\t\t";
      cout << interp5.CenterFirstDifference(a,3,.01)  -fstderiv(a) << "\t\t";
      cout << interp5.CenterFirstDifference(a,5,.01)  -fstderiv(a) << endl;
    }
    cout << endl;
    cout << "\t--------------------------" << endl;
    
    cout << "GSL Second derivative method errors:  " << endl;
    cout << "a \ty''(a)\tErr:natural-reg\t\tErr:natural-irreg" << endl;
    for(double a = .15; a < .8; a+=.3) {
      cout << a << " \t" << scndderiv(a) << " \t";
      cout << interp1.GslSecondDerivative(a)-scndderiv(a) << "\t\t";
      cout << interp2.GslSecondDerivative(a)-scndderiv(a) << endl;
    }
    cout << "Second difference method errors:  " << endl;
    cout << "a \ty'(a)\t3ptBack-Neigh\t3ptFor-Clamp\t3ptCtr-Neville\t5ptCtr-Neville" << endl;
    for(double a = .15; a < .8; a+=.3) {
      cout << a << " \t" << scndderiv(a) << " \t";
      cout << interp3.BackwardSecondDifference(a,3,.01)-scndderiv(a) << "\t";
      cout << interp4.ForwardSecondDifference(a,3,.01) -scndderiv(a) << "\t\t";
      cout << interp5.CenterSecondDifference(a,3,.01)  -scndderiv(a) << "\t";
      cout << interp5.CenterSecondDifference(a,5,.01)  -scndderiv(a) << endl;
    }
    cout << "CubicClampedSecondDerivatives(): " << endl;
    vector <double> yDoublePrime = interp4.CubicClampedSecondDerivatives();
    cout << "xi \t actual2ndDeriv \t calculated2ndDeriv" << endl;
    for(unsigned int i = 0; i < x.size(); i++) {
      cout << x[i] << " \t " << scndderiv(x[i]) << " \t\t\t " << yDoublePrime[i] << endl;
    }
    cout << endl;
    cout << "EXCEPTIONS:" << endl;
    try{interp2.GslFirstDerivative(1.0);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.GslFirstDerivative(0.2);} // not GSL supported
    catch(iException e){ e.Report(); }
    try{interp3.BackwardFirstDifference(-1.0);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.BackwardFirstDifference(0.01,2,.1);} // steps outside domain
    catch(iException e){ e.Report(); }
    try{interp3.BackwardFirstDifference(0.1,4,.01);} // no 4pt formula
    catch(iException e){ e.Report(); }
    try{interp3.ForwardFirstDifference(-0.01);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.ForwardFirstDifference(0.75,3,.1);} // steps outside domain
    catch(iException e){ e.Report(); }
    try{interp3.ForwardFirstDifference(0.1,4,.01);} // no 4pt formula
    catch(iException e){ e.Report(); }
    try{interp3.CenterFirstDifference(.81);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.CenterFirstDifference(0.01,5,.1);} // steps outside domain
    catch(iException e){ e.Report(); }
    try{interp3.CenterFirstDifference(0.1,4,.01);} // no 4pt formula
    catch(iException e){ e.Report(); }
    try{interp1.GslSecondDerivative(1.0);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.GslSecondDerivative(0.8);} // not GSL supported
    catch(iException e){ e.Report(); }
    try{interp3.BackwardSecondDifference(-1.0);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.BackwardSecondDifference(0.01,3,.1);} // steps outside domain
    catch(iException e){ e.Report(); }
    try{interp3.BackwardSecondDifference(0.1,2,.01);} // no 2pt formula
    catch(iException e){ e.Report(); }
    try{interp3.ForwardSecondDifference(-0.01);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.ForwardSecondDifference(0.75,3,.1);} // steps outside domain
    catch(iException e){ e.Report(); }
    try{interp3.ForwardSecondDifference(0.1,2,.01);} // no 2pt formula
    catch(iException e){ e.Report(); }
    try{interp3.CenterSecondDifference(.81);} // outside domain
    catch(iException e){ e.Report(); }
    try{interp3.CenterSecondDifference(0.01,5,.1);} // steps outside domain
    catch(iException e){ e.Report(); }
    try{interp3.CenterSecondDifference(0.1,4,.01);} // no 4pt formula
    catch(iException e){ e.Report(); }
    //interp2.AddData(x,y);
    cout << "\t************************************************" << endl;
    
    // Integration Methods
    cout << endl;
    cout << "5) Integration Approximation:  " << endl;
    cout << "GSL Integral method errors:  " << endl;
    cout << "(a,b)\t\tY(b)-Y(a)\tErr:c-nat-reg\t\tErr:c-nat-irreg" << endl;
    for(double a = 0.0; a < 0.8; a+=.3) {
      for(double b = 0.8; b > a; b-=.3) {
        cout << "(" << a << "," << b << ")\t";
        if(a == 0) cout << "\t";
        cout << integral(a,b) << " \t";
        cout << interp1.GslIntegral(a,b)-integral(a,b) << "\t\t";
        cout << interp2.GslIntegral(a,b)-integral(a,b) << endl;
      }
    }
    cout << "Quadrature method errors:  " << endl;
    cout << "(a,b)\t\tY(b)-Y(a)\tTrap-Neigh\tSimp3pt-Neigh\tSimp4pt-Clamp\tBoole-Clamp\tExtTrap-Nev\tRomberg-Nev" << endl;
    double s;
    for(double a = 0.0; a < 0.8; a+=.3) {
      for(double b = 0.8; b > a; b-=.3) {
        s=0;
        s = interp5.RefineExtendedTrap(a,b,s,10);
        cout << "(" << a << "," << b << ")\t";
        if(a == 0) cout << "\t";
        cout << integral(a,b) << " \t";
        cout << interp3.TrapezoidalRule(a,b) - integral(a,b) << "\t";
        if (interp3.TrapezoidalRule(a,b) - integral(a,b) == 0) cout <<"\t";
        cout << interp3.Simpsons3PointRule(a,b) - integral(a,b) << "\t";
        if (interp3.Simpsons3PointRule(a,b) - integral(a,b) == 0) cout <<"\t";
        cout << interp4.Simpsons4PointRule(a,b) - integral(a,b) << "\t";
        if (interp4.Simpsons4PointRule(a,b) - integral(a,b) == 0) cout <<"\t";
        cout <<      interp4.BoolesRule(a,b) - integral(a,b) << "\t";
        if (     interp4.BoolesRule(a,b) - integral(a,b) == 0) cout <<"\t";
        cout <<                       s - integral(a,b) << "\t";
        if (                      s - integral(a,b) == 0) cout <<"\t";
        cout << interp5.RombergsMethod(a,b) - integral(a,b) << endl;
      }
    }
    cout << endl;
    cout << "EXCEPTIONS:" << endl;
    try{cout << interp1.GslIntegral(1.1,1.0) << endl;} // invalid interval  
    catch(iException e){ e.Report(); }
    try{cout << interp1.GslIntegral(1.0,1.1) << endl;} // outside domain
    catch(iException e){ e.Report(); }
    try{cout << interp3.GslIntegral(0.0,0.8) << endl;} // not GSL supported
    catch(iException e){ e.Report(); }
    try{cout << interp1.TrapezoidalRule(.81,.70) << endl;} // invalid interval
    catch(iException e){ e.Report(); }
    try{cout << interp1.TrapezoidalRule(-.1,.70) << endl;} // outside domain
    catch(iException e){ e.Report(); }
    cout << "\t************************************************" << endl;
  }
  catch (iException &e) {
    e.Report(false);
  }
  return 0;
}
