/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:06 $                                                                 
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
#ifndef BasisFunction_h
#define BasisFunction_h

#include <vector>
#include <string>

namespace Isis {
/**                                                   
 * @brief Generic linear equation class                                                                                                                                              
 *  
 * This is a base class for generating "generic" equations for the 
 * Isis least squares fitting algorithm (IsisLSQ). It allows the programmer
 * to set up equations in the form of:
 * @f[
 * x = C1*T1 + C2*T2 + ... + CN*TN;
 * @f]
 * where C1-CN are coefficients and T1-TN are terms.  Note that terms can
 * be comprised of multiple variables and/or functions.  For example,
 * @f[
 * x = C1 + C2*y + C3*y**2;
 * @f]
 * @f[
 * x = C1 + C2*y + C3*z + C4*y*z;
 * @f]
 * By deriving different functions off of this base class this allows the 
 * least squares class to be generalized.
 *                                                                  
 * @ingroup Math                                       
 *                                                    
 * @author 2004-06-24 Jeff Anderson                    
 *                                                    
 * @internal
 *   @todo Add coded example
 *   @history 2005-03-16 Leah Dahmer modified file to support Doxygen 
 *   documentation.
 */                                                   
  class BasisFunction {
    public:
      BasisFunction(const std::string &name, int numVars, int numCoefs);
      //! Destroys the BasisFunction object.
      virtual ~BasisFunction() {};
  
      void SetCoefficients(const std::vector<double> &coefs);
      double Evaluate (const std::vector<double> &vars);
      virtual void Expand(const std::vector<double> &vars);

/**                                                                       
 * Returns the number of coefficients for the equation.                                                                           
 *                                                                        
 * @return The number of coefficients.                                       
 */                                                                       
      int Coefficients() const { return p_numCoefs; };
/**                                                                       
 * Returns the number of variables in the equation.                                                         
 *                                                                                                                                                
 * @return The number of variables.                                       
 */                                                                       
      int Variables() const { return p_numVars; };
/**                                                                       
 * Returns the name of the equation.                                                         
 *                                                                                                                                                
 * @return The name of the equation.                                       
 */                                                                       
      std::string Name() const { return p_name; };
/**                                                                       
 * Returns the cth term. This is only valid after a Evalute/Expand has been 
 * invoked.  It represents the expansion of the variables into the ith term.  
 * For example, 
 * @f[
 * x = C1 + C2*x + C3*y + C4*x*y 
 * @f]
 * would return x*y for the 3rd 
 * term (zero-based)                                                          
 *                                                                        
 * @param c The index for the desired coefficient.                  
 *                                                                        
 * @return The cth term.                                       
 */                                                                       
      double Term(int c) const { return p_terms[c]; };
/**                                                                       
 * Returns the ith coefficient                                                          
 *                                                                        
 * @param i The index for the desired coefficient.                     
 *                                                                        
 * @return The ith coefficient                                      
 */                                                                       
      double Coefficient(int i) const { return p_coefs[i]; };
  
    protected:
      //! The name of the equation. Call it by using Name()
      std::string p_name;
      //! The number of variables in the equation. Call it by using Variables()
      int p_numVars;
      /** The number of coefficients in the equation. Call it by using
       * Coefficients()
       */
      int p_numCoefs;
      /** A vector of the coefficients in the equation. Call it by using
       * Coefficient()
       */
      std::vector<double> p_coefs;
      /** A vector of the terms in the equation. Call it by using
       * Term()
       */
      std::vector<double> p_terms;
     
  };
};

#endif
