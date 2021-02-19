/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>
#include "IException.h"
#include "LeastSquares.h"
#include "Preference.h"

using namespace std;

int main() {
  Isis::Preference::Preferences(true);
  try {
    cerr << "Unit Test for LeastSquares:" << endl;
    cerr << endl;
    Isis::BasisFunction b("Linear", 2, 2);

    Isis::LeastSquares lsq(b);

    vector<double> one;
    one.push_back(1.0);
    one.push_back(1.0);

    vector<double> two;
    two.push_back(-2.0);
    two.push_back(3.0);

    vector<double> tre;
    tre.push_back(2.0);
    tre.push_back(-1.0);

    lsq.AddKnown(one, 3.0);
    lsq.AddKnown(two, 1.0);
    lsq.AddKnown(tre, 2.0);

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

    cerr << "*** TEST 1:  3 POINTS, NO WEIGHTS ***************************" << endl;
    cerr << "Number of Knowns = " << knowns << endl;
    cerr << "        SVD\tresidual\tQRD\tresidual" << endl;
    cerr << "  one = " << evalSVD1 << "\t" << resSVD1 << "\t\t" << evalQRD1 << "\t" << resQRD1 << endl;
    cerr << "  two = " << evalSVD2 << "\t" << resSVD2 << "\t\t" << evalQRD2 << "\t" << resQRD2 << endl;
    cerr << "  tre = " << evalSVD3 << "\t" << resSVD3 << "\t\t" << evalQRD3 << "\t" << resQRD3 << endl;

    cerr << "---" << endl;
    cerr << "Test from Linear Algebra with Applications, 2nd Edition" << endl;
    cerr << "Steven J. Leon, page 191, 83/50=1.66 71/50=1.42" << endl;
    cerr << xcoefSVD << endl;
    cerr << ycoefSVD << endl ;
    cerr << "---" << endl;

    cerr << "*** TEST 2:  SAME 3 POINTS, MIDDLE POINT HAS WEIGHT 5 *******" << endl;
    lsq.Weight(1, 5);
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
    cerr << "Number of Knowns = " << knowns << endl;
    cerr << "        SVD\t\tQRD" << endl;
    cerr << "  one = " << evalSVD1 << "\t\t" << evalQRD1 << endl;
    cerr << "  two = " << evalSVD2 << "\t"   << evalQRD2 << endl;
    cerr << "  tre = " << evalSVD3 << "\t\t" << evalQRD3 << endl;
    cerr << "  x =   " << xcoefSVD << "\t\t" << xcoefQRD << endl;
    cerr << "  y =   " << ycoefSVD << "\t\t" << ycoefQRD << endl ;
    cerr << "---" << endl;

    cerr << "*** TEST 3:  SAME 3 POINTS, MIDDLE POINT REPEATED 5 TIMES ***" << endl;
    lsq.Weight(1, 1);
    lsq.AddKnown(two, 1.0);
    lsq.AddKnown(two, 1.0);
    lsq.AddKnown(two, 1.0);
    lsq.AddKnown(two, 1.0);
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
    cerr << "Number of Knowns = " << knowns << endl;
    cerr << "        SVD\t\tQRD" << endl;
    cerr << "  one = " << evalSVD1 << "\t\t" << evalQRD1 << endl;
    cerr << "  two = " << evalSVD2 << "\t" << evalQRD2 << endl;
    cerr << "  tre = " << evalSVD3 << "\t\t" << evalQRD3 << endl;
    cerr << "  x =   " << xcoefSVD << "\t\t" << xcoefQRD << endl;
    cerr << "  y =   " << ycoefSVD << "\t\t" << ycoefQRD << endl ;
    cerr << "---" << endl;

    cerr << "*** TEST 4:  SAME 3 POINTS, SPARSE ***" << endl;
    Isis::LeastSquares sparse(b, true, 3, 2, false);
    sparse.AddKnown(one, 3.0);
    sparse.AddKnown(two, 1.0);
    sparse.AddKnown(tre, 2.0);
    int sparseKnowns = sparse.Knowns();;
    sparse.Solve(Isis::LeastSquares::SPARSE);
    double evalSPARSE1 = sparse.Evaluate(one);
    double evalSPARSE2 = sparse.Evaluate(two);
    double evalSPARSE3 = sparse.Evaluate(tre);
    double xcoefSPARSE = b.Coefficient(0);
    double ycoefSPARSE = b.Coefficient(1);
    cerr << "Number of Knowns = " << sparseKnowns << endl;
    cerr << "        SPARSE" << endl;
    cerr << "  one = " << evalSPARSE1 << endl;
    cerr << "  two = " << evalSPARSE2 << endl;
    cerr << "  tre = " << evalSPARSE3 << endl;
    cerr << "  x =   " << xcoefSPARSE << endl;
    cerr << "  y =   " << ycoefSPARSE << endl ;
  }
  catch(Isis::IException &e) {
    e.print();
  }

  cerr << endl;

  try {
    Isis::BasisFunction b("Linear", 2, 2);
    Isis::LeastSquares lsq2(b);
    lsq2.Solve();
  }
  catch(Isis::IException &e) {
    e.print();
  }

  return 0;
}
