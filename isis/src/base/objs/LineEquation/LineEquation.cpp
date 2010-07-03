/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/06/19 14:33:02 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */    
#include "LineEquation.h"
#include "iException.h"
#include "iString.h"
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
    if (p_defined) {
      std::string msg = "Line equation is already defined with 2 points";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);   
    }
    p_x.push_back(x);
    p_y.push_back(y);
    if ( Points() == 2) p_defined = true;
  }

  /**
   * Compute the slope of the line
   *
   * @return double The slope of the line if it exists
   */
  double LineEquation::Slope() {
    if (!p_defined) {
      std::string msg = "Line equation undefined:  2 points are required";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);   
    }
    else if (p_x[0] == p_x[1]) {
      std::string msg = "Points have identical independent variables -- no slope";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);   
     }
    else if (!p_slopeDefined) {
      p_slope = (p_y[0] - p_y[1]) / (p_x[0] - p_x[1]);
    }
    return p_slope;
  }

  /**
   * Compute the intercept of the line
   *
   * @return double The y-intercept of the line if it exists
   */
  double LineEquation::Intercept() {
    if (!p_defined) {
       std::string msg = "Line equation undefined:  2 points are required";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);   
    }
    else if (p_x[0] == p_x[1]) {
       std::string msg = "Points have identical independent variables -- no intercept";
         throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);   
    }
    else if (!p_interceptDefined) {
       p_intercept = p_y[0] - Slope()*p_x[0];
    }

    return p_intercept;
   }

}
