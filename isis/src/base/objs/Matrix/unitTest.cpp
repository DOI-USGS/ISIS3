#include "Matrix.h"
#include <iostream>
#include <iomanip>
#include "Preference.h"

using namespace std;
using namespace Isis;

int main () {
  Isis::Preference::Preferences(true);

  Matrix I = Matrix::Identity(2);
  cout << fixed << setprecision(12) << "Identity:" << endl << I << endl;
  Matrix A(2, 2);

  A[0][0] = 1; A[0][1] = 2;
  A[1][0] = 3; A[1][1] = 4;

  cout << "A:" << endl << A << endl;
  Matrix ApI = A+I,
            AmI = A-I,
            A2 = A*2.0;
  cout << "A+I:" << endl << ApI << endl;
  cout << "A-I:" << endl << AmI << endl;
  cout << "2.0*A:" << endl << A2 << endl;
  Matrix At = A.Transpose();
  cout << "A~ (transpose):" << endl << At << endl;
  Matrix AI = A*I;
  cout << "AI:" << endl << AI << endl;
  cout << "det(A)=" << A.Determinant() << endl;
  cout << "trace(A)=" << A.Trace() << endl;
  Matrix Ai = A.Inverse();
  cout << "A':" << endl << Ai << endl;
  Matrix AAi = A*Ai;
  cout << "AA':" << endl << AAi << endl;
  vector<double> evals = A.Eigenvalues();
  cout << "Eigenvalues: " << evals[0] << ", " << evals[1] << endl;
  Matrix E = A.Eigenvectors();
  cout << "EigenVectors:" << endl << E << endl;

  Matrix B(2, 3, 1);
  cout << "B:" << endl << B << endl;
  Matrix Bt = B.Transpose(),
            AB = A*B,
            BtA = Bt*A;

  cout << "AB:" << endl << AB << endl;
  cout << "B~A:" << endl << BtA << endl;
}
