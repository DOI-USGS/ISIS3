#ifndef EigenUtilities_h
#define EigenUtilities_h

// Do not include Eigen header files here as those will slow down the compilation
// whereever this header file is included.

#include <vector>

namespace Isis {
  
// Compute the best-fitting projective transform that maps a set of 3D points
// to 2D points.
void computeBestFitProjectiveTransform(std::vector<std::vector<double>> const& imagePts,
                                       std::vector<std::vector<double>> const& groundPts,
                                       std::vector<double> & transformCoeffs);
}
#endif
