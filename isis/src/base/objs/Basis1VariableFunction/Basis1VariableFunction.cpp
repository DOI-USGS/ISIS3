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
#include <iostream>

#include "Basis1VariableFunction.h"
#include "iString.h"
#include "iException.h"

namespace Isis {

/**                                                                       
 * Creates a Basis Function with a single variable.
 *                                                                        
 * @param name Name of the Basis1VariableFunction. For example, "affine".
 * @param numCoefs Number of coefficients in the equation. For example:
 * @f[
 * x = C1 + C2 * x + C3 * x**2
 * @f]
 * has three coefficients: C1, C2 & C3.                                       
 */                                                                       
  Basis1VariableFunction::Basis1VariableFunction(const std::string &name, int numCoefs) :
    Isis::BasisFunction(name,1,numCoefs) {
  }

}
