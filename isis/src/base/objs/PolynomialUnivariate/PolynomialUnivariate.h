#ifndef PolynomialUnivariate_h
#define PolynomialUnivariate_h
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
#include "Basis1VariableFunction.h"

namespace Isis {
/**                                                                       
 * @brief Nth degree Polynomial with one variable                
 *                                                                        
 * This is a derived class from Basis1VariableFunction which provides the 
 * capabilities of a polynomial equation in one variable with degree n, where n 
 * is specified during the object construction. For example, Degree = 1 z = a + 
 * b*x Degree = 2 z = a + b*x + c*x**2 Degree = 3 z = a + b*x + c*x**2 + d*x**3 
 * In general the number of coefficients will be degree + 1. 
 *                                                                        
 * @ingroup Math                                                  
 *                                                                        
 * @author  Debbie A. Cook 2007-11-19 (modified from Poly2D, now
 *          named PolynominalBivariate, 2004-06-24 Jeff
 *          Anderson)
 *                                                                        
 * @internal                                                              
 *                                                                        
 *  @history 2008-01-11 Tracie Sucharski,  Renamed from Poly1D, add derivative
 *           methods.
 *  @history 2008-02-05 Jeannie Walldren,Renamed from
 *           Polynomial1Variable.
 */                                                                       

  class PolynomialUnivariate : public Isis::Basis1VariableFunction {
    public:
      PolynomialUnivariate(int degree);

      //! Destroys the PolynomialUnivariate object
      ~PolynomialUnivariate() {};
  
      void Expand(const std::vector<double> &vars);

      double DerivativeVar ( const double value );
      double DerivativeCoef ( const double value,const int coefIndex);
  
    private:
      int p_degree;   //!< The order/degree of the polynomial
  };
};

#endif

