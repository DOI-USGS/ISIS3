#if !defined(LineEquation_h)
#define LineEquation_h
/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:08 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
   */
  class LineEquation {
  public:
//! Constructors
    LineEquation();
    LineEquation( double x1, double y1, double x2, double y2 );

//! Destroys the LineEquation object
    ~LineEquation() {}
    void AddPoint(double x, double y);
    double Slope();
    double Intercept();
    int Points() { return p_x.size(); };
    bool HaveSlope() { return p_slopeDefined; };
    bool HaveIntercept() { return p_interceptDefined; };
    bool Defined() { return p_defined; };

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
