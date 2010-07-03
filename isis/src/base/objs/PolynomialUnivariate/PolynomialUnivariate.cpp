/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2008/02/07 18:53:03 $                                                                 
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

#include <iostream>
#include <cmath>

#include "PolynomialUnivariate.h"
#include "iString.h"
#include "iException.h"

namespace Isis {
 /** 
  * Create a PolynomialUnivariate object    
  * 
  * @param degree The order/degree of the polynomial
  * 
  */
  PolynomialUnivariate::PolynomialUnivariate(int degree) : 
           Isis::Basis1VariableFunction("PolynomialUnivariate",(degree + 1)) {
    p_degree = degree;
  }

 /** 
  * This is the the overriding virtual function that provides the expansion of 
  * the two input variables into the polynomial equation. 
  * See BasisFunction for more information.
  *
  * @param vars A vector of double values to use for the expansion.
  */
  void PolynomialUnivariate::Expand(const std::vector<double> &vars) {
    p_terms.clear();
    p_terms.push_back(1.0);

    for (int i=1; i<=p_degree; i++) {
        p_terms.push_back(p_terms[i-1]*vars[0]);
    }
  }


  /**
   * This will take the Derivative with respect to the variable and evaluate at 
   * given value. 
   *  
   * @param [in] value   (const double)  value at which to evaluate derivative 
   *  
   * @history  2008-01-09  Tracie Sucharski, Original Version 
   *  
   */
  double PolynomialUnivariate::DerivativeVar(const double value) {

    double derivative = 0;

    for (int i=1; i<Coefficients(); i++) {
      derivative += i * Coefficient(i) * pow(value,i-1);
    }
    return derivative;
  }



  /**
   *  Evaluate the derivative of the polynomial defined by the given coefficients
   *  with respect to the coefficient at the given index, at the current value.
   *
   * @param [in]  value      (const double) value at which to evaluate derivative
   * @param [in]  coefIndex  (const int)    The index of the coefficient to 
   *                                          differentiate with respect to
   *  
   * @return    (double) The derivative evaluated at given value
   *
   */
  double PolynomialUnivariate::DerivativeCoef (const double value,
                                              const int coefIndex) {
    double derivative;

    if (coefIndex > 0  && coefIndex <= Coefficients()) {
      derivative = pow(value, coefIndex);
    }
    else if (coefIndex == 0) {
      derivative = 1;
    }
    else {
      Isis::iString msg = "Coeff index, " + Isis::iString(coefIndex) + " exceeds degree of polynomial";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
    return derivative;
  }


} // end namespace isis

