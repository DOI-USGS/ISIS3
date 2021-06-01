#ifndef LineEquation_h
#define LineEquation_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>

namespace Isis {
  /**
   * @brief Utility class for creating and using cartesean line equations
   *
   * This class contains utility methods for creating and using cartesean
   * line equations.  If both points have the same value for the independent
   * variable (vertical line) an error is thrown.
   *
   * @ingroup Utility
   *
   * @author 2006-10-19 Debbie A. Cook
   *
   * @todo Add constructor with arguments (double slope, double intercept)
   *       Add method to double y = EvaluateLine( double x )
   *
   * @internal
   */
  class LineEquation {
    public:
//! Constructors
      LineEquation();
      LineEquation(double x1, double y1, double x2, double y2);

//! Destroys the LineEquation object
      ~LineEquation() {}
      void AddPoint(double x, double y);
      double Slope();
      double Intercept();
      int Points() {
        return p_x.size();
      };
      bool HaveSlope() {
        return p_slopeDefined;
      };
      bool HaveIntercept() {
        return p_interceptDefined;
      };
      bool Defined() {
        return p_defined;
      };

    private:
      std::vector<double> p_x;       //!< Independent variables
      std::vector<double> p_y;       //!< Dependent variables
      bool p_defined;           //!< Variable indicating if line is defined yet
      bool p_slopeDefined;      //!< Variable indicating if slope is defined yet
      bool p_interceptDefined;  //!< Variable indicating if intercept is defined yet
      double p_slope;
      double p_intercept;

  }; // end of LineEquation class
}
#endif
