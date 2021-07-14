/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "LineEquation.h"
#include "IException.h"
#include "IString.h"
#include <iostream>
#include <iomanip>

using namespace std;

namespace Isis {
  /**
   * Construct an empty LineEquation object
   */
  LineEquation::LineEquation() {
    p_defined = false;
    p_slopeDefined = false;
    p_interceptDefined = false;
  }

  /**
   * Construct and fill LineEquation object
   * @param x1 Double First independent variable
   * @param y1 Double First dependent variable
   * @param x2 Double Second independent variable
   * @param y2 Double Second dependent variable
   */
  LineEquation::LineEquation(double x1, double y1, double x2, double y2) {
    p_defined = false;
    p_slopeDefined = false;
    p_interceptDefined = false;
    AddPoint(x1, y1);
    AddPoint(x2, y2);
    p_defined = true;
    p_slope = Slope();
    p_intercept = Intercept();
  }

  /**
   * Add a point to the object.  The object is considered filled
   * once 2 points have been added (the line is defined).
   *
   * @param x Double Independent variable
   * @param y Double Dependent variable
   *
   */
  void LineEquation::AddPoint(double x, double y) {
    if(p_defined) {
      std::string msg = "Line equation is already defined with 2 points";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    p_x.push_back(x);
    p_y.push_back(y);
    if(Points() == 2) p_defined = true;
  }

  /**
   * Compute the slope of the line
   *
   * @return double The slope of the line if it exists
   */
  double LineEquation::Slope() {
    if(!p_defined) {
      std::string msg = "Line equation undefined:  2 points are required";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    else if(p_x[0] == p_x[1]) {
      std::string msg = "Points have identical independent variables -- no slope";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    else if(!p_slopeDefined) {
      p_slope = (p_y[0] - p_y[1]) / (p_x[0] - p_x[1]);
      p_slopeDefined = true;
    }
    return p_slope;
  }

  /**
   * Compute the intercept of the line
   *
   * @return double The y-intercept of the line if it exists
   */
  double LineEquation::Intercept() {
    if(!p_defined) {
      std::string msg = "Line equation undefined:  2 points are required";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    else if(p_x[0] == p_x[1]) {
      std::string msg = "Points have identical independent variables -- no intercept";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
    else if(!p_interceptDefined) {
      p_intercept = p_y[0] - Slope() * p_x[0];
      p_interceptDefined = true;
    }

    return p_intercept;
  }

}
