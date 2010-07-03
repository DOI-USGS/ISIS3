/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2008/01/11 23:18:25 $                                                                 
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
#ifndef Basis1VariableFunction_h
#define Basis1VariableFunction_h

#include <vector>
#include <string>

#include "BasisFunction.h"

namespace Isis {
/**                                                   
 * @brief Time based linear equation class                                      
 *  
 * This is a class for generating a general one-variable equation for the Isis 
 * least squares fitting algorithm (IsisLSQ). It allows the programmer to set up
 * equations in the form of: 
 * @f[
 * x = C1*T1 + C2*T2 + ... + CN*TN;
 * @f]
 * where C1-CN are coefficients and T1-TN are terms with a single variable. 
 * For example, 
 * @f[ 
 * x = C1 + C2*t + C3*t**2 
 * @f] 
 *                                                                  
 * @ingroup Math                                       
 *                                                    
 * @author 2004-06-24 Jeff Anderson                    
 *                                                    
 * @internal
 *   @todo Add coded example
 *   @history 2005-03-16 Leah Dahmer modified file to support Doxygen
 *                           documentation.
 *   @history 2008-01-08 Tracie Sucharski, Derived from BasisFunction class for
 *                          a single variable function.  Added Derivative
 *                          methods as pure virtuals.  This class was
 *                          developed as a convenience to simplify the
 *                          Derivative methods and any other methods that
 *                          might need to be developed in the future.
 *  
 */                                                   
  class Basis1VariableFunction : public Isis::BasisFunction {
    public:
      Basis1VariableFunction(const std::string &name, int numCoefs);
      //! Destroys the Basis1VariableFunction object.
      virtual ~Basis1VariableFunction() {};

      virtual double DerivativeVar (const double value) = 0;
      virtual double DerivativeCoef (const double value, const int coefIndex) = 0;
  
    protected:
     
  };
};

#endif
