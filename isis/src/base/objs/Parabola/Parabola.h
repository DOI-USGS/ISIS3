
#ifndef Parabola_h
#define Parabola_h
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
#include "BasisFunction.h"

namespace Isis {

/**                                                                       
 * @brief Parabola basis function
 * 
 * This is a derived class from the BasisFunction class which creates a parabola
 * (second degree equation in 1 variable).  The parabolic function has the 
 * following form:
 * 
 * @f[
 *   x = A + B*y + C*y**2
 * @f]
 * 
 * @ingroup Math                                                  
 *                                                                        
 * @author  2005-06-09 Kris Becker
 *                                                                        
 * @internal
 *  @history 2006-04-15 Debbie A. Cook - Imported from ISIS2 to Isis 3                                                              
 */                                                                       

  class Parabola : public Isis::BasisFunction {
    public: 
      //! Create a Parabola object  

      // Get help to figure out why I have to pass the name in even with the
      // default set  
      Parabola(const std::string &bname = "Parabola") : 
               Isis::BasisFunction(bname, 1, 3) { }

      //! Destroys the Parabola object
      ~Parabola() {}
  
      void Expand(const std::vector<double> &vars);
  
  };

}
#endif

