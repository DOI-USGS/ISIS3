#ifndef PrincipalComponentAnalysis_h
#define PrincipalComponentAnalysis_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>
#include "tnt/tnt_array2d.h"
#include "MultivariateStatistics.h"
#include "IException.h"
#include "Constants.h"

namespace Isis {
  /**
   * @brief Principal Component Analysis class
   *
   *     This class is used to apply Principal Component Analysis
   * to transform multivariate data into its principal components
   * as well as invert it from component space.
   *
   * If you would like to see PrincipalComponentAnalysis being used
   *         in implementation, see pca.cpp or decorstretch.cpp
   *
   * @ingroup Math and Statistics
   *
   * @author 2006-05-18 Jacob Danton
   *
   * @internal
   *   @history 2016-08-28 Kelvin Rodriguez - Added string concatinations to properly convert
   *                                      numbers using QString::number to squash conversion
   *                                      warnings. Part of porting to OS X 10.11.
   */
  class PrincipalComponentAnalysis {
    public:
      PrincipalComponentAnalysis(const int n);
      PrincipalComponentAnalysis(TNT::Array2D<double> transform);
      ~PrincipalComponentAnalysis() {};
      void AddData(const double *data, const unsigned int count);
      void ComputeTransform();
      TNT::Array2D<double> Transform(TNT::Array2D<double> data);
      TNT::Array2D<double> Inverse(TNT::Array2D<double> data);
      TNT::Array2D<double> TransformMatrix() {
        return p_transform;
      };
      int Dimensions() {
        return p_dimensions;
      };

    private:
      void ComputeInverse();
      bool p_hasTransform;
      int p_dimensions;

      TNT::Array2D<double> p_transform, p_inverse;
      std::vector<Isis::MultivariateStatistics *> p_statistics;
  };
}

#endif
