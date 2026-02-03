#include "EigenUtilities.h"

#include <Error.h>
#include <Eigen/Dense>

#include "IException.h"

#include <iostream>
#include <vector>

// Keep these utilities separate as using Eigen in an existing source
// file results in a 50% increase in compilation time.

// Compute the best-fitting projective transform that maps a set of 3D points
// to 2D points.
// A best-fit linear transform could be created by eliminating the denominators below.
void Isis::computeBestFitProjectiveTransform(std::vector<std::vector<double>> const& imagePts,
                                             std::vector<std::vector<double>> const& groundPts,
                                             std::vector<double> & transformCoeffs) {
  
  if (imagePts.size() != groundPts.size()) {
    QString msg = "The number of inputs and outputs must agree.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }
  
  int numPts = imagePts.size();
  if (numPts < 8) {
    QString msg = "At least 8 points are needed to fit a 3D-to-2D projective transform. "
                  "Ideally more are preferred.";
    throw IException(IException::Programmer, msg, _FILEINFO_);
  }

  int numMatRows = 2 * numPts; // there exist x and y coords for each point
  int numMatCols = 14; // Number of variables in the projective transform
  
  Eigen::MatrixXd M = Eigen::MatrixXd::Zero(numMatRows, numMatCols);
  Eigen::VectorXd b = Eigen::VectorXd::Zero(numMatRows);
  
  for (int it = 0; it < numPts; it++) {

    double x = groundPts[it][0], y = groundPts[it][1], z = groundPts[it][2];
    double r = imagePts[it][0], c = imagePts[it][1];

    // If the solution coefficients are u0, u1, ..., must have:
 
    // (u0 + u1 * x + u2 * y + u3  * z) / (1 + u4  * x + u5  * y + u6  * z) = r
    // (u7 + u8 * x + u9 * y + u10 * z) / (1 + u11 * x + u12 * y + u13 * z) = c

    // Multiply by the denominators. Get a linear regression in the coefficients.
    
    M.row(2 * it + 0) << 1, x, y, z, -x * r, -y * r, -z * r, 0, 0, 0, 0, 0, 0, 0;
    M.row(2 * it + 1) << 0, 0, 0, 0, 0, 0, 0, 1, x, y, z, -x * c, -y * c, -z * c;

    b[2 * it + 0] = r;
    b[2 * it + 1] = c;
  }

  // Solve the over-determined system, per:
  // https://eigen.tuxfamily.org/dox/group__LeastSquares.html
  Eigen::VectorXd u = M.colPivHouseholderQr().solve(b);

  // Copy back the result to a standard vector (to avoid using Eigen too much as
  // that is slow to compile).
  transformCoeffs.resize(numMatCols);
  for (int it = 0; it < numMatCols; it++)
    transformCoeffs[it] = u[it];

  return;
} 
