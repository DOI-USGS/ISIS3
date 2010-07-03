#include <iostream>
#include "iException.h"
#include "LeastSquares.h"
#include "Preference.h"

int main () {
  Isis::Preference::Preferences(true);
  try {
  std::cout << "Unit Test for LeastSquares:" << std::endl;
  std::cout << std::endl;
  Isis::BasisFunction b("Linear",2,2);

  Isis::LeastSquares lsq(b);

  std::vector<double> one;
  one.push_back(1.0);
  one.push_back(1.0);

  std::vector<double> two;
  two.push_back(-2.0);
  two.push_back(3.0);

  std::vector<double> tre;
  tre.push_back(2.0);
  tre.push_back(-1.0);

  lsq.AddKnown(one,3.0);
  lsq.AddKnown(two,1.0);
  lsq.AddKnown(tre,2.0);

  int knowns = lsq.Knowns();;
  double evalSVD1, evalSVD2, evalSVD3, resSVD1, resSVD2, resSVD3;
  double evalQRD1, evalQRD2, evalQRD3, resQRD1, resQRD2, resQRD3;
  double xcoefSVD, ycoefSVD, xcoefQRD, ycoefQRD;
  lsq.Solve();
  evalSVD1 = lsq.Evaluate(one);
  evalSVD2 = lsq.Evaluate(two);
  evalSVD3 = lsq.Evaluate(tre);
  resSVD1 = lsq.Residual(0);
  resSVD2 = lsq.Residual(1);
  resSVD3 = lsq.Residual(2);
  xcoefSVD = b.Coefficient(0);
  ycoefSVD = b.Coefficient(1);
  lsq.Solve(Isis::LeastSquares::QRD);
  evalQRD1 = lsq.Evaluate(one);
  evalQRD2 = lsq.Evaluate(two);
  evalQRD3 = lsq.Evaluate(tre);
  resQRD1 = lsq.Residual(0);
  resQRD2 = lsq.Residual(1);
  resQRD3 = lsq.Residual(2);
  xcoefQRD = b.Coefficient(0);
  ycoefQRD = b.Coefficient(1);

  std::cout << "*** TEST 1:  3 POINTS, NO WEIGHTS ***************************" << std::endl;
  std::cout << "Number of Knowns = " << knowns << std::endl;
  std::cout << "        SVD\tresidual\tQRD\tresidual" << std::endl;
  std::cout << "  one = " << evalSVD1 << "\t" << resSVD1 << "\t\t" << evalQRD1 <<"\t" << resQRD1 << std::endl;
  std::cout << "  two = " << evalSVD2 << "\t" << resSVD2 << "\t\t" << evalQRD2 <<"\t" << resQRD2 << std::endl;
  std::cout << "  tre = " << evalSVD3 << "\t" << resSVD3 << "\t\t" << evalQRD3 <<"\t" << resQRD3 << std::endl;

  std::cout << "---" << std::endl;
  std::cout << "Test from Linear Algebra with Applications, 2nd Edition" << std::endl;
  std::cout << "Steven J. Leon, page 191, 83/50=1.66 71/50=1.42" << std::endl;
  std::cout << xcoefSVD << std::endl;
  std::cout << ycoefSVD << std::endl ; 
  std::cout << "---" << std::endl;

  std::cout << "*** TEST 2:  SAME 3 POINTS, MIDDLE POINT HAS WEIGHT 5 *******" << std::endl;
  lsq.Weight(1,5);
  knowns = lsq.Knowns();;
  lsq.Solve(Isis::LeastSquares::SVD);
  evalSVD1 = lsq.Evaluate(one);
  evalSVD2 = lsq.Evaluate(two);
  evalSVD3 = lsq.Evaluate(tre);
  xcoefSVD = b.Coefficient(0);
  ycoefSVD = b.Coefficient(1);
  lsq.Solve(Isis::LeastSquares::QRD);
  evalQRD1 = lsq.Evaluate(one);
  evalQRD2 = lsq.Evaluate(two);
  evalQRD3 = lsq.Evaluate(tre);
  xcoefQRD = b.Coefficient(0);
  ycoefQRD = b.Coefficient(1);
  std::cout << "Number of Knowns = " << knowns << std::endl;
  std::cout << "        SVD\t\tQRD" << std::endl;
  std::cout << "  one = " << evalSVD1 << "\t\t" << evalQRD1 << std::endl;
  std::cout << "  two = " << evalSVD2 << "\t"   << evalQRD2 << std::endl;
  std::cout << "  tre = " << evalSVD3 << "\t\t" << evalQRD3 << std::endl;
  std::cout << "  x =   " << xcoefSVD << "\t\t" << xcoefQRD << std::endl;
  std::cout << "  y =   " << ycoefSVD << "\t\t" << ycoefQRD << std::endl ; 
  std::cout << "---" << std::endl;

  std::cout << "*** TEST 3:  SAME 3 POINTS, MIDDLE POINT REPEATED 5 TIMES ***" << std::endl;
  lsq.Weight(1,1);
  lsq.AddKnown(two,1.0);
  lsq.AddKnown(two,1.0);
  lsq.AddKnown(two,1.0);
  lsq.AddKnown(two,1.0);
  knowns = lsq.Knowns();;
  lsq.Solve(Isis::LeastSquares::SVD);
  evalSVD1 = lsq.Evaluate(one);
  evalSVD2 = lsq.Evaluate(two);
  evalSVD3 = lsq.Evaluate(tre);
  xcoefSVD = b.Coefficient(0);
  ycoefSVD = b.Coefficient(1);
  lsq.Solve(Isis::LeastSquares::QRD);
  evalQRD1 = lsq.Evaluate(one);
  evalQRD2 = lsq.Evaluate(two);
  evalQRD3 = lsq.Evaluate(tre);
  xcoefQRD = b.Coefficient(0);
  ycoefQRD = b.Coefficient(1);
  std::cout << "Number of Knowns = " << knowns << std::endl;
  std::cout << "        SVD\t\tQRD" << std::endl;
  std::cout << "  one = " << evalSVD1 << "\t\t" << evalQRD1 << std::endl;
  std::cout << "  two = " << evalSVD2 << "\t" << evalQRD2 << std::endl;
  std::cout << "  tre = " << evalSVD3 << "\t\t" << evalQRD3 << std::endl;
  std::cout << "  x =   " << xcoefSVD << "\t\t" << xcoefQRD << std::endl;
  std::cout << "  y =   " << ycoefSVD << "\t\t" << ycoefQRD << std::endl ; 
  }
  catch (Isis::iException &e) {
    e.Report();
  }

  return 0;
}
