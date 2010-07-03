#ifndef PolynomialBivariate_h
#define PolynomialBivariate_h
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

#include <vector>
#include "BasisFunction.h"

namespace Isis {
/**                                                                       
 * @brief Nth degree Polynomial with two variables                
 *                                                                        
 * This is a derived class from BasisFunction which provides the capabilities 
 * of a polynomial equation in two variables with degree n, where n is specified 
 * during the object construction. For example, Degree = 1 z = a + b*x + c*y
 * Degree = 2 z = a + b*x + c*y + d*x**2 + e*x*y + f*y**2 
 * Degree = 3 z = a + b*x + c*y + d*x**2 + e*x*y + f*y**2 + g*x**3 + h*x**2*y 
 * + i*x*y**2 + j*y**3 In general the number of coefficients will be 
 * ((degree+1)*(degree+2))/2.                                    
 *                                                                        
 * @ingroup Math                                                  
 *                                                                        
 * @author  2004-06-24 Jeff Anderson                                    
 *                                                                        
 * @internal                                                              
 *  @history 2005-03-11 Elizabeth Ribelin - Modified file to support Doxygen
 *                                          documentation
 *                                                                        
 *  @todo 2005-03-11 Jeff Anderson - add coded and implementation examples to 
 *                                   class documentation
 *  @history 2008-02-05 Jeannie Walldren, Renamed from Poly2D.
 */                                                                       

  class PolynomialBivariate : public Isis::BasisFunction {
    public:
      PolynomialBivariate(int degree);

      //! Destroys the PolynomialBivariate object
      ~PolynomialBivariate() {};
  
      void Expand(const std::vector<double> &vars);
  
    private:
      int p_degree;   //!< The order/degree of the polynomial
  };
};

#endif

