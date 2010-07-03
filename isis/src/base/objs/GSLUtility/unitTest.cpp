//  $Id: unitTest.cpp,v 1.1 2009/08/21 01:04:57 kbecker Exp $
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "GSLUtility.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>

using namespace Isis;
using namespace Isis::GSL;
using namespace std;

typedef GSLUtility::GSLMatrix GSLMatrix;
typedef GSLUtility::GSLVector GSLVector;

int main(int argc, char *argv[]) {

  cout << "\n*** Test GSLUtility Functionality ***\n";
  cout << "Tests use GSL Cholesky and Eigen Value Solutions\n";
  cout << "Data provided by a Gruen Test case\n";
  cout << "Expected result for Fit = 0.02782\n";

  // Setup with initialization of parameters
  string ata_str = "8 8\n"
"0.00394087 -0.00134017 0.000698616 -0.00118582 5.08835e-05 -0.000191336 0.269915 0.0100344\n"
"-0.00134017 0.00261999 -0.000314756 5.08835e-05 -0.000710193 4.11822e-05 -0.168693 -0.00628949\n"
"0.000698616 -0.000314756 0.000219502 -0.000191336 4.11822e-05 -5.97796e-05 0.0939977 0.00337862\n"
"-0.00118582 5.08835e-05 -0.000191336 0.00382973 0.000141468 0.000632668 -0.0836021 -0.00315921\n"
"5.08835e-05 -0.000710193 4.11822e-05 0.000141468 0.00140484 -7.57756e-05 0.0812913 0.00282839\n" 
"-0.000191336 4.11822e-05 -5.97796e-05 0.000632668 -7.57756e-05 0.000169357 -0.0542839 -0.00190863\n" 
"0.269915 -0.168693 0.0939977 -0.0836021 0.0812913 -0.0542839 169 5.81505\n"
"0.0100344 -0.00628949 0.00337862 -0.00315921 0.00282839 -0.00190863 5.81505 0.200532\n";

  istringstream data_inp(ata_str);
  GSLMatrix ata;
  data_inp >> ata;

  double resid = 0.000167867;
  int npts = 169;

  string str_atl = "8\n-0.00031961\n9.03622e-05\n-7.49718e-05\n0.0003005\n"
                      "-5.42596e-05\n8.0876e-05\n-0.0431598\n-0.00160416\n";
  data_inp.str(str_atl.c_str());
  GSLVector atl;
  data_inp >> atl;


  //  Get instance of GSLUtility
  GSLUtility *gsl = GSLUtility::getInstance();

  size_t nRows = gsl->Rows(ata);
  size_t nCols = gsl->Columns(ata);

  // Compute variance
  double variance = resid / (double) (npts - nRows);
  gsl_matrix *A, *atai;
  try {
    cout << "\nSolve using GSL Cholesky\n";
    A = gsl->GSLTogsl(ata);  // Convert GSLMatrix to gsl_matrix
    gsl->check(gsl_linalg_cholesky_decomp(A));  // Compute Choleksy decomp
    atai = gsl->identity(gsl->Rows(ata), gsl->Columns(ata));  // GSL identity


    // Solve each variable independantly
    for (size_t i = 0 ; i < nRows ; i++ ) {
      gsl_vector_view x = gsl_vector_view_array(gsl_matrix_ptr(atai, i, 0), 
                                                nCols); 
      gsl->check(gsl_linalg_cholesky_svx(A, &x.vector));
    }

  //  Solve Gruen affine parameter contributions
    cout << "Solve Affine translation\n";
    GSLMatrix covar(8,8);
    GSLVector alpha(8);
    for ( size_t r = 0 ; r < nRows ; r++ ) {
      alpha[r] = 0.0;
      for (size_t c = 0 ; c < nCols ; c++ ) {
        double ataiV = gsl_matrix_get(atai, r, c);
        alpha[r] += ataiV * atl[c];
        covar[r][c] = variance * ataiV;
      }
    }

    //  Compute eigen vector solution for shift parameters
    GSLVector eigen(2);
    GSLMatrix subvar(2,2);
    subvar[0][0] = covar[2][2];
    subvar[0][1] = covar[2][5];
    subvar[1][0] = covar[5][2];
    subvar[1][1] = covar[5][5];
    gsl_matrix_view skmat  = gsl_matrix_view_array(&subvar[0][0],2,2);
    gsl_vector_view evals = gsl_vector_view_array(&eigen[0], 2);
    gsl_eigen_symm_workspace *w = gsl_eigen_symm_alloc(2);
    gsl->check(gsl_eigen_symm(&skmat.matrix, &evals.vector, w));
    gsl_eigen_symm_free(w);
    gsl_eigen_symmv_sort(&evals.vector, &skmat.matrix, GSL_EIGEN_SORT_VAL_DESC);
    gsl->free(A);
    gsl->free(atai);

//  Report results
    GSLMatrix affine(2,3,&alpha[0]);
    cout << "Affine parameters = " << setprecision(4) << affine;
    cout << "Eigen Vectors: " << eigen[0] << " " << eigen[1] << endl;
    cout << "Fit:  " << setprecision(4) 
         << sqrt(eigen[0]*eigen[0] + eigen[1]*eigen[1]) << endl;
  }
  catch (iException &ie) {
    ie.Report();
  }

  return (0);

}
