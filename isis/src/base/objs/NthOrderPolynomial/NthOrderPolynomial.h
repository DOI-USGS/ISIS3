
#ifndef NthOrderPolynomial_h
#define NthOrderPolynomial_h
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
   * @brief NthOrderPolynomial basis function
   *
   * This is a derived class from the BasisFunction class which creates an nth order polynomial.
   *  
   * @ingroup Math
   *
   * @author  2018-01-01 Unknown
   *
   * @internal
   *  @history 2018-01-01 Unknown - Initial Version
   *  @history 2020-01-08 Kristin Berry - Update documentation prior to checkin to dev.
   */

  class NthOrderPolynomial : public Isis::BasisFunction {
    public:
      NthOrderPolynomial(int degree);

      //! Destroys the NthOrderPolynomial object
      ~NthOrderPolynomial() {}

      void Expand(const std::vector<double> &vars);
    
  private:
    int p_degree;
  };

}
#endif

