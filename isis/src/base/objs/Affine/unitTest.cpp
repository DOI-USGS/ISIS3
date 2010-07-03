#include <iostream>
#include <iomanip>

#include "Affine.h"
#include "Preference.h"
#include "tnt/tnt_array2d_utils.h"

using namespace std;
using namespace TNT;

int main () {
  typedef Isis::Affine::AMatrix AMatrix;
  Isis::Preference::Preferences(true);

  Isis::Affine a;

  // Test translate
  a.Translate (1.0,-1.0);
  a.Compute(0.0,0.0);
  std::cout << setprecision(1) << std::endl;
  std::cout <<  std::fixed << a.xp() << std::endl;
  std::cout <<  std::fixed << a.yp() << std::endl;
  a.ComputeInverse(a.xp(),a.yp());
  std::cout <<  std::fixed << a.x() << std::endl;
  std::cout <<  std::fixed << a.y() << std::endl;
  std::cout << "---" << std::endl;

  // Test rotate
  a.Identity();
  a.Rotate(90.0);
  a.Compute(0.0,1.0);
  std::cout <<  std::fixed << a.xp() << std::endl;
  std::cout <<  std::fixed << a.yp() << std::endl;
  a.ComputeInverse(a.xp(),a.yp());
  std::cout <<  std::fixed << a.x() << std::endl;
  std::cout <<  std::fixed << a.y() << std::endl;
  std::cout << "---" << std::endl;

  // Test solve (1,1)->(3,3) (3,3)->(1,1), (1,3)->(3,1)
  double x[] = { 1.0, 3.0, 1.0 };
  double y[] = { 1.0, 3.0, 3.0 };
  double xp[] = { 3.0, 1.0, 3.0 };
  double yp[] = { 3.0, 1.0, 1.0 };

  a.Solve(x,y,xp,yp,3);
  std::cout << "Forward: 1.0, 1.0\n";
  a.Compute(1.0,1.0);
  std::cout <<  std::fixed << a.xp() << std::endl;
  std::cout <<  std::fixed << a.yp() << std::endl;
  std::cout << "Inverse: " << a.xp() << "," << a.yp() << std::endl;
  a.ComputeInverse(a.xp(),a.yp());
  std::cout <<  std::fixed << a.x() << std::endl;
  std::cout <<  std::fixed << a.y() << std::endl;
  std::cout << "---" << std::endl;

  std::cout << "Forward: 3.0, 3.0\n";
  a.Compute(3.0,3.0);
  std::cout <<  std::fixed << a.xp() << std::endl;
  std::cout <<  std::fixed << a.yp() << std::endl;
  std::cout << "Inverse: " << a.xp() << "," << a.yp() << std::endl;
  a.ComputeInverse(a.xp(),a.yp());
  std::cout <<  std::fixed << a.x() << std::endl;
  std::cout <<  std::fixed << a.y() << std::endl;
  std::cout << "---" << std::endl;

  std::cout << "Forward: 1.0, 3.0\n";
  a.Compute(1.0,3.0);
  std::cout <<  std::fixed << a.xp() << std::endl;
  std::cout <<  std::fixed << a.yp() << std::endl;
  std::cout << "Inverse: " << a.xp() << "," << a.yp() << std::endl;
  a.ComputeInverse(a.xp(),a.yp());
  std::cout <<  std::fixed << a.x() << std::endl;
  std::cout <<  std::fixed << a.y() << std::endl;
  std::cout << "---" << std::endl;

  std::cout << "Forward: 3.0, 1.0\n";
  a.Compute(3.0,1.0);
  std::cout <<  std::fixed << a.xp() << std::endl;
  std::cout <<  std::fixed << a.yp() << std::endl;
  std::cout << "Inverse: " << a.xp() << "," << a.yp() << std::endl;
  a.ComputeInverse(a.xp(),a.yp());
  std::cout <<  std::fixed << a.x() << std::endl;
  std::cout <<  std::fixed << a.y() << std::endl;
  std::cout << "---" << std::endl;

  // Test Coefficients
  vector<double> xcoef = a.Coefficients(1);
  std::cout << std::fixed << xcoef[0] << std::endl;
  std::cout << std::fixed << xcoef[1] << std::endl;
  std::cout << std::fixed << xcoef[2] << std::endl;
  std::cout << "---" << std::endl;

  vector<double> ycoef = a.Coefficients(2);
  std::cout << std::fixed << ycoef[0] << std::endl;
  std::cout << std::fixed << ycoef[1] << std::endl;
  std::cout << std::fixed << ycoef[2] << std::endl;
  std::cout << "---" << std::endl;

  // Test InverseCoefficients
  vector<double> xpcoef = a.InverseCoefficients(1);
  std::cout << std::fixed << xpcoef[0] << std::endl;
  std::cout << std::fixed << xpcoef[1] << std::endl;
  std::cout << std::fixed << xpcoef[2] << std::endl;
  std::cout << "---" << std::endl;

  vector<double> ypcoef = a.InverseCoefficients(2);
  std::cout << std::fixed << ypcoef[0] << std::endl;
  std::cout << std::fixed << ypcoef[1] << std::endl;
  std::cout << std::fixed << ypcoef[2] << std::endl;
  std::cout << "---" << std::endl;

  //  Test AMatrix elements
  AMatrix forward = a.Forward();
  std::cout << "Forward Matrix\n" << forward << std::endl;
  AMatrix inverse = a.Inverse();
  std::cout << "\nInverse Matrix\n" << inverse << std::endl;

  // Matrix constructor
  std::cout << "\nMatrix Constructor\n";
  Isis::Affine b(forward);
  std::cout << "Forward: 1.0, 3.0\n";
  b.Compute(1.0,3.0);
  std::cout <<  std::fixed << b.xp() << std::endl;
  std::cout <<  std::fixed << b.yp() << std::endl;
  std::cout << "Inverse: " << b.xp() << "," << b.yp() << std::endl;
  a.ComputeInverse(b.xp(),b.yp());
  std::cout <<  std::fixed << b.x() << std::endl;
  std::cout <<  std::fixed << b.y() << std::endl;
  std::cout << "---" << std::endl;


}
